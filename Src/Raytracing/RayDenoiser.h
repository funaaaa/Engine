#pragma once
#include "Singleton.h"
#include "Engine.h"
#include <memory>
#include <array>

class RaytracingOutput;
class RayComputeShader;
class DynamicConstBuffer;

class Denoiser : public Singleton<Denoiser> {

private:

	/*===== メンバ変数 =====*/

	// ブラー出力用
	std::shared_ptr<RaytracingOutput> blurXOutput_;
	std::shared_ptr<RaytracingOutput> blurYOutput_;

	// ブラーをかけるコンピュートシェーダー
	std::shared_ptr<RayComputeShader> blurX_;
	std::shared_ptr<RayComputeShader> blurY_;
	std::shared_ptr<RayComputeShader> blurFinal_;

	// デノイズ時に使用する一時保蔵要テクスチャ
	std::shared_ptr<RaytracingOutput> denoiseOutput_;

	// 影テクスチャと色テクスチャを混ぜ合わせるシェーダー
	std::shared_ptr<RayComputeShader> mixColorAndLuminance_;

	// 重みテーブル
	static const int DENOISE_GAUSSIAN_WEIGHTS_COUNT = 4;
	std::array<float, DENOISE_GAUSSIAN_WEIGHTS_COUNT> denoiseGaussianWeights_;

	// 重み定数バッファ
	std::shared_ptr<DynamicConstBuffer> denoiseWeightTableCBX_;
	std::shared_ptr<DynamicConstBuffer> denoiseWeightTableCBY_;

public:

	/*===== メンバ関数 =====*/

	// セッティング処理。
	void Setting();

	// ガウシアンブラーをかける。
	void ApplyDenoiseGaussianBlur(int InputUAVIndex, int DenoiseMaskIndex, int OutputUAVIndex, int BlurPower);

	// 色情報と明るさ情報をかける。
	void MixColorAndLuminance(int InputColorIndex, int InputLuminanceIndex, int InputLightLuminanceIndex, int InputEmissiveIndex, int OutputUAVIndex);

	// デノイズ
	void Denoise(int InImg, int OutImg, int DenoiseMaskIndex, int DenoisePower, int DenoiseCount);

private:

	// ガウシアンブラーの重みを計算する。
	void CalcDenoiseWeightsTableFromGaussian(float Power);

};