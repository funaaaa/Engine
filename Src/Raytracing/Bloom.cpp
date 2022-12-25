#include "Bloom.h"
#include "RaytracingOutput.h"
#include "RayComputeShader.h"
#include "DynamicConstBuffer.h"
#include "WindowsAPI.h"
#include "Engine.h"

void Bloom::Setting()
{

	/*===== 前準備処理 =====*/

	// ブルーム中間テクスチャを生成。
	for (auto& index : emissiveIntermediateTexture_) {

		index = std::make_shared<RaytracingOutput>();
		index->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"EmissiveIntermediateTexture", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	}

	// ブラー出力用クラスを生成。
	blurXOutput_ = std::make_shared<RaytracingOutput>();
	blurYOutput_ = std::make_shared<RaytracingOutput>();

	// ブラー出力テスト用クラスをセット。
	blurXOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"BloomBlurXOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	blurYOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"BloomBlurYOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// 使用するコンピュートシェーダーを生成。
	blurX_ = std::make_shared<RayComputeShader>();
	blurY_ = std::make_shared<RayComputeShader>();
	blurFinal_ = std::make_shared<RayComputeShader>();

	// ガウシアンブラーに使用するコンピュートシェーダーをセット。
	blurX_->Setting(L"Resource/ShaderFiles/RayTracing/BloomBlurX.hlsl", 0, 1, 1, { 0 });
	blurY_->Setting(L"Resource/ShaderFiles/RayTracing/BloomBlurY.hlsl", 0, 1, 1, { blurXOutput_->GetUAVIndex() });
	blurFinal_->Setting(L"Resource/ShaderFiles/RayTracing/BloomBlurFinal.hlsl", 0, 0, 1, { blurYOutput_->GetUAVIndex() });

	// エミッシブを合成するコンピュートシェーダーを生成。
	mixEmissive_ = std::make_shared<RayComputeShader>();
	mixEmissive_->Setting(L"Resource/ShaderFiles/RayTracing/BloomMix.hlsl", 0, 0, 4, { 0 });

	// ガウシアンブラーの重みテーブルを計算。
	CalcBloomWeightsTableFromGaussian(10);

	// 定数バッファを生成。
	bloomWeightTableCBX_ = std::make_shared<DynamicConstBuffer>();
	bloomWeightTableCBX_->Generate(sizeof(float) * BLOOM_GAUSSIAN_WEIGHTS_COUNT, L"BloomGaussianWeightCBX");
	bloomWeightTableCBY_ = std::make_shared<DynamicConstBuffer>();
	bloomWeightTableCBY_->Generate(sizeof(float) * BLOOM_GAUSSIAN_WEIGHTS_COUNT, L"BloomGaussianWeightCBY");

}

void Bloom::ApplyBloom(int InOutImg)
{

	/*===== ブルームをかける =====*/

	for (int index = 0; index < EMISSIVE_COUNT; ++index) {

		// 最初の一回目だったら
		if (index == 0) {
			ApplyBloomGaussianBlur(InOutImg, InOutImg, emissiveIntermediateTexture_[index]->GetUAVIndex());
		}
		else {
			ApplyBloomGaussianBlur(emissiveIntermediateTexture_[index - 1]->GetUAVIndex(), InOutImg, emissiveIntermediateTexture_[index]->GetUAVIndex());
		}

	}

	std::vector<int> inputUAV;
	for (auto& index : emissiveIntermediateTexture_) inputUAV.emplace_back(index->GetUAVIndex());
	mixEmissive_->ChangeInputUAVIndex(inputUAV);
	mixEmissive_->Dispatch(static_cast<UINT>(WINDOW_WIDTH / 32) + 1, static_cast<UINT>(WINDOW_HEIGHT / 32) + 1, static_cast<UINT>(1), InOutImg, { }, Engine::Ins()->mainGraphicsCmdList_);

}

void Bloom::ApplyBloomGaussianBlur(int InputUAVIndex, int SourceUAVIndex, int OutputUAVIndex)
{

	/*===== ブルーム用のガウシアンブラーを実行 =====*/

	// 重みテーブルを計算。
	CalcBloomWeightsTableFromGaussian(static_cast<float>(BLUR_POWER));

	// 重みテーブルを書き込む。
	bloomWeightTableCBX_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), bloomGaussianWeights_.data(), sizeof(float) * BLOOM_GAUSSIAN_WEIGHTS_COUNT);
	bloomWeightTableCBY_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), bloomGaussianWeights_.data(), sizeof(float) * BLOOM_GAUSSIAN_WEIGHTS_COUNT);

	// コンピュートシェーダーを実行。
	blurX_->ChangeInputUAVIndex({ InputUAVIndex });
	blurY_->ChangeInputUAVIndex({ blurXOutput_->GetUAVIndex() });
	blurFinal_->ChangeInputUAVIndex({ blurYOutput_->GetUAVIndex() });
	blurX_->Dispatch(static_cast<UINT>((WINDOW_WIDTH / 2.0f) / 32) + 1, static_cast<UINT>(WINDOW_HEIGHT / 32) + 1, static_cast<UINT>(1), blurXOutput_->GetUAVIndex(), { bloomWeightTableCBX_->GetBuffer(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() }, Engine::Ins()->mainGraphicsCmdList_);
	blurY_->Dispatch(static_cast<UINT>((WINDOW_WIDTH / 2.0f) / 32) + 1, static_cast<UINT>((WINDOW_HEIGHT / 2.0f) / 32) + 1, static_cast<UINT>(1), blurYOutput_->GetUAVIndex(), { bloomWeightTableCBY_->GetBuffer(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() }, Engine::Ins()->mainGraphicsCmdList_);
	blurFinal_->Dispatch(static_cast<UINT>(WINDOW_WIDTH / 32) + 1, static_cast<UINT>(WINDOW_HEIGHT / 32) + 1, static_cast<UINT>(1), OutputUAVIndex, {}, Engine::Ins()->mainGraphicsCmdList_);

}

void Bloom::CalcBloomWeightsTableFromGaussian(float Power)
{

	/*===== 重みのテーブルを計算 =====*/

	// 重みの合計を記録する変数を定義する。
	float total = 0;

	// ここからガウス関数を用いて重みを計算している。
	// ループ変数のxが基準テクセルからの距離。
	for (int x = 0; x < BLOOM_GAUSSIAN_WEIGHTS_COUNT; ++x)
	{
		bloomGaussianWeights_[x] = expf(-0.5f * static_cast<float>(x * x) / Power);
		total += 2.0f * bloomGaussianWeights_.at(x);
	}

	// 重みの合計で除算することで、重みの合計を1にしている。
	for (int i = 0; i < BLOOM_GAUSSIAN_WEIGHTS_COUNT; ++i)
	{
		bloomGaussianWeights_[i] /= total;
	}

}
