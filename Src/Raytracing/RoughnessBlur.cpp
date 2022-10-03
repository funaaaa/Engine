#include "RoughnessBlur.h"
#include "RaytracingOutput.h"
#include "RayComputeShader.h"
#include "DynamicConstBuffer.h"
#include "DirectXBase.h"
#include "WindowsAPI.h"

void RoughnessBlur::Setting()
{

	/*===== コンストラクタ =====*/

	// ブラー出力用クラスを生成。
	blurXOutput_ = std::make_shared<RaytracingOutput>();
	blurYOutput_ = std::make_shared<RaytracingOutput>();

	// ブラー出力テスト用クラスをセット。
	blurXOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseBlurXOutput");
	blurYOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseBlurYOutput");

	// デノイズ時に排出する用クラスをセット。
	denoiseOutput_ = std::make_shared<RaytracingOutput>();
	denoiseOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseBlurBuffOutput");

	// 使用するコンピュートシェーダーを生成。
	blurX_ = std::make_shared<RayComputeShader>();
	blurY_ = std::make_shared<RayComputeShader>();

	// ガウシアンブラーに使用するコンピュートシェーダーをセット。
	blurX_->Setting(L"Resource/ShaderFiles/RayTracing/RoughnessBlurX.hlsl", 0, 1, 3, { 0 });
	blurY_->Setting(L"Resource/ShaderFiles/RayTracing/RoughnessBlurY.hlsl", 0, 1, 3, { blurXOutput_->GetUAVIndex() });

	// ガウシアンブラーの重みテーブルを計算。
	CalcWeightsTableFromGaussian(10);

	// 定数バッファを生成。
	weightTableCBX_ = std::make_shared<DynamicConstBuffer>();
	weightTableCBX_->Generate(sizeof(float) * GAUSSIAN_WEIGHTS_COUNT, L"GaussianWeightCBX");
	weightTableCBY_ = std::make_shared<DynamicConstBuffer>();
	weightTableCBY_->Generate(sizeof(float) * GAUSSIAN_WEIGHTS_COUNT, L"GaussianWeightCBY");

}

void RoughnessBlur::ApplyGaussianBlur(const int& InputReflectionIndex, const int& InputRoughnessIndex, const int& DenoiseMaskIndex, const int& OutputUAVIndex, const int& BlurPower)
{

	/*===== デノイズ処理 =====*/

	// 重みテーブルを計算。
	CalcWeightsTableFromGaussian(static_cast<float>(BlurPower));

	// 重みテーブルを書き込む。
	weightTableCBX_->Write(DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex(), gaussianWeights_.data(), sizeof(float) * GAUSSIAN_WEIGHTS_COUNT);
	weightTableCBY_->Write(DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex(), gaussianWeights_.data(), sizeof(float) * GAUSSIAN_WEIGHTS_COUNT);

	// 出力用UAVの状態を変える。
	blurXOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	blurYOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// コンピュートシェーダーを実行。
	blurX_->ChangeInputUAVIndex({ InputReflectionIndex, InputRoughnessIndex, DenoiseMaskIndex });
	blurY_->ChangeInputUAVIndex({ blurXOutput_->GetUAVIndex(), InputRoughnessIndex, DenoiseMaskIndex });
	blurX_->Dispatch(static_cast<UINT>(WINDOW_WIDTH / 32) + 1, static_cast<UINT>(WINDOW_HEIGHT / 32) + 1, static_cast<UINT>(1), blurXOutput_->GetUAVIndex(), { weightTableCBX_->GetBuffer(DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() });
	blurY_->Dispatch(static_cast<UINT>((WINDOW_WIDTH / 1.0f) / 32) + 1, static_cast<UINT>((WINDOW_HEIGHT / 1.0f) / 32) + 1, static_cast<UINT>(1), OutputUAVIndex, { weightTableCBY_->GetBuffer(DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() });

	// 出力用UAVの状態を変える。
	blurXOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	blurYOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

}

void RoughnessBlur::Denoise(const int& InputReflectionIndex, const int& InputRoughnessIndex, const int& DenoiseMaskIndex, const int& OutputUAVIndex, const int& DenoisePower, const int& DenoiseCount)
{

	/*===== デノイズ =====*/

	// デノイズする数が1回だったら。
	if (DenoiseCount == 1) {

		// ガウシアンブラーをかける。
		ApplyGaussianBlur(InputReflectionIndex, InputRoughnessIndex, DenoiseMaskIndex, OutputUAVIndex, DenoisePower);

	}
	// デノイズする数が2回だったら。
	else if (DenoiseCount == 2) {

		// ガウシアンブラーをかける。
		ApplyGaussianBlur(InputReflectionIndex, InputRoughnessIndex, DenoiseMaskIndex, OutputUAVIndex, DenoisePower);
		ApplyGaussianBlur(denoiseOutput_->GetUAVIndex(), InputRoughnessIndex, DenoiseMaskIndex, OutputUAVIndex, DenoisePower);

	}
	else {

		for (int index = 0; index < DenoiseCount; ++index) {

			// デノイズが最初の一回だったら。
			if (index == 0) {

				ApplyGaussianBlur(InputReflectionIndex, InputRoughnessIndex, DenoiseMaskIndex, denoiseOutput_->GetUAVIndex(), DenoisePower);

			}
			// デノイズの最終段階だったら。
			else if (index == DenoiseCount - 1) {

				ApplyGaussianBlur(denoiseOutput_->GetUAVIndex(), InputRoughnessIndex, DenoiseMaskIndex, OutputUAVIndex, DenoisePower);

			}
			else {

				ApplyGaussianBlur(denoiseOutput_->GetUAVIndex(), InputRoughnessIndex, DenoiseMaskIndex, denoiseOutput_->GetUAVIndex(), DenoisePower);

			}

		}

	}

}

void RoughnessBlur::CalcWeightsTableFromGaussian(float Power)
{

	/*===== ガウシアンブラーの重みを計算 =====*/

	// 重みの合計を記録する変数を定義する。
	float total = 0;

	// ここからガウス関数を用いて重みを計算している。
	// ループ変数のxが基準テクセルからの距離。
	for (int x = 0; x < GAUSSIAN_WEIGHTS_COUNT; x++)
	{
		gaussianWeights_[x] = expf(-0.5f * static_cast<float>(x * x) / Power);
		total += 2.0f * gaussianWeights_.at(x);
	}

	// 重みの合計で除算することで、重みの合計を1にしている。
	for (int i = 0; i < GAUSSIAN_WEIGHTS_COUNT; i++)
	{
		gaussianWeights_[i] /= total;
	}

}
