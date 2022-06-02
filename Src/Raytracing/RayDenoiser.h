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
	std::shared_ptr<RaytracingOutput> blurXOutput;
	std::shared_ptr<RaytracingOutput> blurYOutput;

	// ブラーをかけるコンピュートシェーダー
	std::shared_ptr<RayComputeShader> blurX;
	std::shared_ptr<RayComputeShader> blurY;
	std::shared_ptr<RayComputeShader> blurFinal;

	// 重みテーブル
	static const int GAUSSIAN_WEIGHTS_COUNT = 8;
	std::array<float, GAUSSIAN_WEIGHTS_COUNT> gaussianWeights;

	// 重み定数バッファ
	std::shared_ptr<DynamicConstBuffer> weightTableCBX;
	std::shared_ptr<DynamicConstBuffer> weightTableCBY;


public:

	/*===== メンバ関数 =====*/

	// セッティング処理。
	void Setting();

	// ガウシアンブラーをかける。
	void ApplyDenoise(const int& InputUAVIndex, const int& OutputUAVIndex, const int& BlurPower);

private:

	// ガウシアンブラーの重みを計算する。
	void CalcWeightsTableFromGaussian(float Power);

};