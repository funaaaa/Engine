#pragma once
#include "Singleton.h"
#include <memory>
#include <array>

class RaytracingOutput;
class RayComputeShader;
class DynamicConstBuffer;

class RoughnessBlur : public Singleton<RoughnessBlur> {

private:

	/*===== メンバ変数 =====*/

	// ブラー出力用
	std::shared_ptr<RaytracingOutput> blurXOutput_;
	std::shared_ptr<RaytracingOutput> blurYOutput_;

	// ブラーをかけるコンピュートシェーダー
	std::shared_ptr<RayComputeShader> blurX_;
	std::shared_ptr<RayComputeShader> blurY_;

	// デノイズ時に使用する一時保蔵要テクスチャ
	std::shared_ptr<RaytracingOutput> denoiseOutput_;

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
	void ApplyGaussianBlur(const int& InputReflectionIndex, const int& InputRoughnessIndex, const int& DenoiseMaskIndex, const int& OutputUAVIndex, const int& BlurPower);

	// デノイズ
	void Denoise(const int& InputReflectionIndex, const int& InputRoughnessIndex, const int& DenoiseMaskIndex, const int& OutputUAVIndex, const int& DenoisePower, const int& DenoiseCount);

private:

	// ガウシアンブラーの重みを計算する。
	void CalcWeightsTableFromGaussian(float Power);

};
