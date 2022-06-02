#include "RayDenoiser.h"

void Denoiser::Setting()
{

	/*===== コンストラクタ =====*/

	// ブラー出力テスト用クラスをセット。
	blurXOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);
	blurYOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// ガウシアンブラーに使用するコンピュートシェーダーをセット。
	blurX.Setting(L"Resource/ShaderFiles/RayTracing/DenoiseBlurX.hlsl", 0, 0, 1, { 0 });
	blurY.Setting(L"Resource/ShaderFiles/RayTracing/DenoiseBlurY.hlsl", 0, 0, 1, { blurXOutput.GetUAVIndex() });
	blurFinal.Setting(L"Resource/ShaderFiles/RayTracing/DenoiseFinal.hlsl", 0, 0, 1, { blurYOutput.GetUAVIndex() });

}

void Denoiser::ApplyDenoise(const int& InputUAVIndex, const int& OutputUAVIndex)
{

	/*===== デノイズ処理 =====*/

	// 出力用UAVの状態を変える。
	blurXOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	blurYOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// コンピュートシェーダーを実行。
	blurX.ChangeInputUAVIndex({ InputUAVIndex });
	blurX.Dispatch((window_width / 2.0f) / 4, window_height / 4, 1, blurXOutput.GetUAVIndex(), {});
	blurY.Dispatch((window_width / 2.0f) / 4, (window_height / 2.0f) / 4, 1, blurYOutput.GetUAVIndex(), {});
	blurFinal.Dispatch(window_width / 4, window_height / 4, 1, OutputUAVIndex, {});

	// 出力用UAVの状態を変える。
	blurXOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	blurYOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

}
