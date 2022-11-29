#include "RayDenoiser.h"
#include "RaytracingOutput.h"
#include "RayComputeShader.h"
#include "DynamicConstBuffer.h"
#include "WindowsAPI.h"
#include "Engine.h"

void Denoiser::Setting()
{

	/*===== コンストラクタ =====*/

	// ブラー出力用クラスを生成。
	blurXOutput_ = std::make_shared<RaytracingOutput>();
	blurYOutput_ = std::make_shared<RaytracingOutput>();

	// ブラー出力テスト用クラスをセット。
	blurXOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseBlurXOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	blurYOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseBlurYOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// デノイズ時に排出する用クラスをセット。
	denoiseOutput_ = std::make_shared<RaytracingOutput>();
	denoiseOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseBlurBuffOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// 使用するコンピュートシェーダーを生成。
	blurX_ = std::make_shared<RayComputeShader>();
	blurY_ = std::make_shared<RayComputeShader>();
	blurFinal_ = std::make_shared<RayComputeShader>();
	mixColorAndLuminance_ = std::make_shared<RayComputeShader>();

	// ガウシアンブラーに使用するコンピュートシェーダーをセット。
	blurX_->Setting(L"Resource/ShaderFiles/RayTracing/DenoiseBlurX.hlsl", 0, 1, 2, { 0 });
	blurY_->Setting(L"Resource/ShaderFiles/RayTracing/DenoiseBlurY.hlsl", 0, 1, 2, { blurXOutput_->GetUAVIndex() });
	blurFinal_->Setting(L"Resource/ShaderFiles/RayTracing/DenoiseFinal.hlsl", 0, 0, 1, { blurYOutput_->GetUAVIndex() });
	mixColorAndLuminance_->Setting(L"Resource/ShaderFiles/RayTracing/MixColorAndLuminance.hlsl", 0, 0, 5, { 0,0 });

	// ガウシアンブラーの重みテーブルを計算。
	CalcDenoiseWeightsTableFromGaussian(10);

	// 定数バッファを生成。
	denoiseWeightTableCBX_ = std::make_shared<DynamicConstBuffer>();
	denoiseWeightTableCBX_->Generate(sizeof(float) * DENOISE_GAUSSIAN_WEIGHTS_COUNT, L"GaussianWeightCBX");
	denoiseWeightTableCBY_ = std::make_shared<DynamicConstBuffer>();
	denoiseWeightTableCBY_->Generate(sizeof(float) * DENOISE_GAUSSIAN_WEIGHTS_COUNT, L"GaussianWeightCBY");

}

void Denoiser::ApplyDenoiseGaussianBlur(int InputUAVIndex, int DenoiseMaskIndex, int OutputUAVIndex, int BlurPower)
{

	/*===== デノイズ処理 =====*/

	// 重みテーブルを計算。
	CalcDenoiseWeightsTableFromGaussian(static_cast<float>(BlurPower));

	// 重みテーブルを書き込む。
	denoiseWeightTableCBX_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), denoiseGaussianWeights_.data(), sizeof(float) * DENOISE_GAUSSIAN_WEIGHTS_COUNT);
	denoiseWeightTableCBY_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), denoiseGaussianWeights_.data(), sizeof(float) * DENOISE_GAUSSIAN_WEIGHTS_COUNT);

	// コンピュートシェーダーを実行。
	blurX_->ChangeInputUAVIndex({ InputUAVIndex, DenoiseMaskIndex });
	blurY_->ChangeInputUAVIndex({ blurXOutput_->GetUAVIndex(), DenoiseMaskIndex });
	blurX_->Dispatch(static_cast<UINT>(WINDOW_WIDTH / 32) + 1, static_cast<UINT>(WINDOW_HEIGHT / 32) + 1, static_cast<UINT>(1), blurXOutput_->GetUAVIndex(), { denoiseWeightTableCBX_->GetBuffer(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() }, Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]);
	blurY_->Dispatch(static_cast<UINT>((WINDOW_WIDTH / 1.0f) / 32) + 1, static_cast<UINT>((WINDOW_HEIGHT / 1.0f) / 32) + 1, static_cast<UINT>(1), OutputUAVIndex, { denoiseWeightTableCBY_->GetBuffer(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() }, Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]);

}

void Denoiser::MixColorAndLuminance(int InputColorIndex, int InputLuminanceIndex, int InputLightLuminanceIndex, int InputGIIndex, int InputEmissiveIndex, int OutputUAVIndex)
{

	/*===== 色情報と明るさ情報の乗算 =====*/

	// コンピュートシェーダーを実行。
	mixColorAndLuminance_->ChangeInputUAVIndex({ InputColorIndex,InputLuminanceIndex, InputLightLuminanceIndex, InputGIIndex, InputEmissiveIndex });
	mixColorAndLuminance_->Dispatch(WINDOW_WIDTH / 32, WINDOW_HEIGHT / 32 + 1, 1, OutputUAVIndex, {}, Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]);

}

void Denoiser::Denoise(int InImg, int OutImg, int DenoiseMaskIndex, int DenoisePower, int DenoiseCount)
{

	/*===== デノイズ =====*/

	// デノイズする数が1回だったら。
	if (DenoiseCount == 1) {

		// ガウシアンブラーをかける。
		ApplyDenoiseGaussianBlur(InImg, DenoiseMaskIndex, OutImg, DenoisePower);

	}
	// デノイズする数が2回だったら。
	else if (DenoiseCount == 2) {

		// ガウシアンブラーをかける。
		ApplyDenoiseGaussianBlur(InImg, DenoiseMaskIndex, denoiseOutput_->GetUAVIndex(), DenoisePower);
		ApplyDenoiseGaussianBlur(denoiseOutput_->GetUAVIndex(), DenoiseMaskIndex, OutImg, DenoisePower);

	}
	else {

		for (int index = 0; index < DenoiseCount; ++index) {

			// デノイズが最初の一回だったら。
			if (index == 0) {

				ApplyDenoiseGaussianBlur(InImg, DenoiseMaskIndex, denoiseOutput_->GetUAVIndex(), DenoisePower);

			}
			// デノイズの最終段階だったら。
			else if (index == DenoiseCount - 1) {

				ApplyDenoiseGaussianBlur(denoiseOutput_->GetUAVIndex(), DenoiseMaskIndex, OutImg, DenoisePower);

			}
			else {

				ApplyDenoiseGaussianBlur(denoiseOutput_->GetUAVIndex(), DenoiseMaskIndex, denoiseOutput_->GetUAVIndex(), DenoisePower);

			}

		}

	}


}

void Denoiser::CalcDenoiseWeightsTableFromGaussian(float Power)
{

	/*===== ガウシアンブラーの重みを計算 =====*/

	// 重みの合計を記録する変数を定義する。
	float total = 0;

	// ここからガウス関数を用いて重みを計算している。
	// ループ変数のxが基準テクセルからの距離。
	for (int x = 0; x < DENOISE_GAUSSIAN_WEIGHTS_COUNT; x++)
	{
		denoiseGaussianWeights_[x] = expf(-0.5f * static_cast<float>(x * x) / Power);
		total += 2.0f * denoiseGaussianWeights_.at(x);
	}

	// 重みの合計で除算することで、重みの合計を1にしている。
	for (int i = 0; i < DENOISE_GAUSSIAN_WEIGHTS_COUNT; i++)
	{
		denoiseGaussianWeights_[i] /= total;
	}

}
