#pragma once
#include "Singleton.h"
#include "Engine.h"
#include <memory>
#include <array>

class RaytracingOutput;
class RayComputeShader;
class DynamicConstBuffer;

// ブルームクラス 輝度のテクスチャを渡すことでブルームをかけて返す。
class Bloom : public Singleton<Bloom> {

private:

	/*===== メンバ変数 =====*/

	// ブラーをかけるコンピュートシェーダー
	std::shared_ptr<RayComputeShader> blurX_;
	std::shared_ptr<RayComputeShader> blurY_;
	std::shared_ptr<RayComputeShader> blurFinal_;

	// ブラー出力用
	std::shared_ptr<RaytracingOutput> blurXOutput_;
	std::shared_ptr<RaytracingOutput> blurYOutput_;

	// エミッシブを合成する。
	std::shared_ptr<RayComputeShader> mixEmissive_;

	// ブルーム用中間テクスチャ
	static const int EMISSIVE_COUNT = 4;
	std::array<std::shared_ptr<RaytracingOutput>, EMISSIVE_COUNT> emissiveIntermediateTexture_;

	// 重みテーブル
	static const int BLOOM_GAUSSIAN_WEIGHTS_COUNT = 8;
	std::array<float, BLOOM_GAUSSIAN_WEIGHTS_COUNT> bloomGaussianWeights_;

	// 重み定数バッファ
	std::shared_ptr<DynamicConstBuffer> bloomWeightTableCBX_;
	std::shared_ptr<DynamicConstBuffer> bloomWeightTableCBY_;

	// ガウシアンブラーの強さ
	const float BLUR_POWER = 1000.0f;


public:

	/*===== メンバ関数 =====*/

	// 前準備
	void Setting();

	// ブルームをかける。
	void ApplyBloom(int InOutImg);

private:

	// ガウシアンブラーをかける。
	void ApplyBloomGaussianBlur(int InputUAVIndex, int SourceUAVIndex, int OutputUAVIndex);

	// ガウシアンブラーの重みを計算する。
	void CalcBloomWeightsTableFromGaussian(float Power);


};