#pragma once
#include "Singleton.h"
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
	static const int GAUSSIAN_WEIGHTS_COUNT = 8;
	std::array<float, GAUSSIAN_WEIGHTS_COUNT> gaussianWeights_;

	// 重み定数バッファ
	std::shared_ptr<DynamicConstBuffer> weightTableCBX_;
	std::shared_ptr<DynamicConstBuffer> weightTableCBY_;


public:

	/*===== メンバ関数 =====*/

	// セッティング処理。
	void Setting();

	// ガウシアンブラーをかける。
	void ApplyGaussianBlur(const int& InputUAVIndex, const int& DenoiseMaskIndex, const int& OutputUAVIndex, const int& BlurPower);

	// 色情報と明るさ情報をかける。
	void MixColorAndLuminance(const int& InputColorIndex, const int& InputLuminanceIndex, const int& InputLightLuminanceIndex, const int& InputGIIndex, const int& OutputUAVIndex);

	// デノイズ
	void Denoise(const int& InImg, const int& OutImg, const int& DenoiseMaskIndex, const int& DenoisePower, const int& DenoiseCount);

private:

	// ガウシアンブラーの重みを計算する。
	void CalcWeightsTableFromGaussian(float Power);

};