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
	mixColorAndLuminance_->Setting(L"Resource/ShaderFiles/RayTracing/MixColorAndLuminance.hlsl", 0, 0, 4, { 0,0 });

	// ガウシアンブラーの重みテーブルを計算。
	CalcWeightsTableFromGaussian(10);

	// 定数バッファを生成。
	weightTableCBX_ = std::make_shared<DynamicConstBuffer>();
	weightTableCBX_->Generate(sizeof(float) * GAUSSIAN_WEIGHTS_COUNT, L"GaussianWeightCBX");
	weightTableCBY_ = std::make_shared<DynamicConstBuffer>();
	weightTableCBY_->Generate(sizeof(float) * GAUSSIAN_WEIGHTS_COUNT, L"GaussianWeightCBY");

}

void Denoiser::ApplyGaussianBlur(int InputUAVIndex, int DenoiseMaskIndex, int OutputUAVIndex, int BlurPower)
{

	/*===== デノイズ処理 =====*/

	// 重みテーブルを計算。
	CalcWeightsTableFromGaussian(static_cast<float>(BlurPower));

	// 重みテーブルを書き込む。
	weightTableCBX_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), gaussianWeights_.data(), sizeof(float) * GAUSSIAN_WEIGHTS_COUNT);
	weightTableCBY_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), gaussianWeights_.data(), sizeof(float) * GAUSSIAN_WEIGHTS_COUNT);

	// コンピュートシェーダーを実行。
	blurX_->ChangeInputUAVIndex({ InputUAVIndex, DenoiseMaskIndex });
	blurY_->ChangeInputUAVIndex({ blurXOutput_->GetUAVIndex(), DenoiseMaskIndex });
	blurX_->Dispatch(static_cast<UINT>(WINDOW_WIDTH / 32) + 1, static_cast<UINT>(WINDOW_HEIGHT / 32) + 1, static_cast<UINT>(1), blurXOutput_->GetUAVIndex(), { weightTableCBX_->GetBuffer(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() }, Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]);
	blurY_->Dispatch(static_cast<UINT>((WINDOW_WIDTH / 1.0f) / 32) + 1, static_cast<UINT>((WINDOW_HEIGHT / 1.0f) / 32) + 1, static_cast<UINT>(1), OutputUAVIndex, { weightTableCBY_->GetBuffer(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() }, Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]);

}

void Denoiser::MixColorAndLuminance(int InputColorIndex, int InputLuminanceIndex, int InputLightLuminanceIndex, int InputGIIndex, int OutputUAVIndex)
{

	/*===== 色情報と明るさ情報の乗算 =====*/

	// コンピュートシェーダーを実行。
	mixColorAndLuminance_->ChangeInputUAVIndex({ InputColorIndex,InputLuminanceIndex, InputLightLuminanceIndex, InputGIIndex });
	mixColorAndLuminance_->Dispatch(WINDOW_WIDTH / 32, WINDOW_HEIGHT / 32, 1, OutputUAVIndex, {}, Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]);

}

void Denoiser::Denoise(int InImg, int OutImg, int DenoiseMaskIndex, int DenoisePower, int DenoiseCount)
{

	/*===== デノイズ =====*/

	// デノイズする数が1回だったら。
	if (DenoiseCount == 1) {

		// ガウシアンブラーをかける。
		ApplyGaussianBlur(InImg, DenoiseMaskIndex, OutImg, DenoisePower);

	}
	// デノイズする数が2回だったら。
	else if (DenoiseCount == 2) {

		// ガウシアンブラーをかける。
		ApplyGaussianBlur(InImg, DenoiseMaskIndex, denoiseOutput_->GetUAVIndex(), DenoisePower);
		ApplyGaussianBlur(denoiseOutput_->GetUAVIndex(), DenoiseMaskIndex, OutImg, DenoisePower);

	}
	else {

		for (int index = 0; index < DenoiseCount; ++index) {

			// デノイズが最初の一回だったら。
			if (index == 0) {

				ApplyGaussianBlur(InImg, DenoiseMaskIndex, denoiseOutput_->GetUAVIndex(), DenoisePower);

			}
			// デノイズの最終段階だったら。
			else if (index == DenoiseCount - 1) {

				ApplyGaussianBlur(denoiseOutput_->GetUAVIndex(), DenoiseMaskIndex, OutImg, DenoisePower);

			}
			else {

				ApplyGaussianBlur(denoiseOutput_->GetUAVIndex(), DenoiseMaskIndex, denoiseOutput_->GetUAVIndex(), DenoisePower);

			}

		}

	}


}

void Denoiser::AfterDraw()
{

	/*===== 描画後処理 =====*/

	////グラフィックコマンドリストの実行
	//ID3D12CommandList* cmdLists[] = { cmdList_.Get() }; // コマンドリストの配列
	//cmdQueue_->ExecuteCommandLists(1, cmdLists);

	// グラフィックコマンドリストの完了待ち
	//computeCmdQueue_->Signal(computeFence_.Get(), ++computeFenceVal_);
	//if (fence_->GetCompletedValue() != fenceVal_) {
	//	HANDLE event = CreateEvent(nullptr, false, false, nullptr);
	//	fence_->SetEventOnCompletion(fenceVal_, event);
	//	WaitForSingleObject(event, INFINITE);
	//	CloseHandle(event);
	//}

	//// コマンドアロケータのリセット
	//denoiseCmdAllocator_->Reset();							// キューをクリア

	//// コマンドリストのリセット
	//denoiseCmdList_->Reset(denoiseCmdAllocator_.Get(), nullptr);	// 再びコマンドリストを貯める準備

	assert(0);		// この関数は呼んじゃダメ！！！！！

}

void Denoiser::BeforeDraw()
{

	/*===== 描画前処理 =====*/

	ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

}

void Denoiser::CloseCommandList()
{

	/*===== コマンドリストを閉める =====*/

	// グラフィックコマンドリストのクローズ
	Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->Close();

}

void Denoiser::CalcWeightsTableFromGaussian(float Power)
{

	/*===== ガウシアンブラーの重みを計算 =====*/

	// 重みの合計を記録する変数を定義する。
	float total = 0;

	// ここからガウス関数を用いて重みを計算している。
	// ループ変数のxが基準テクセルからの距離。
	for (int x_ = 0; x_ < GAUSSIAN_WEIGHTS_COUNT; x_++)
	{
		gaussianWeights_[x_] = expf(-0.5f * static_cast<float>(x_ * x_) / Power);
		total += 2.0f * gaussianWeights_.at(x_);
	}

	// 重みの合計で除算することで、重みの合計を1にしている。
	for (int i = 0; i < GAUSSIAN_WEIGHTS_COUNT; i++)
	{
		gaussianWeights_[i] /= total;
	}

}
