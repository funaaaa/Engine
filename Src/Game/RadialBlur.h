#pragma once
#include "Singleton.h"
#include "Engine.h"
#include <memory>
#include <array>

class RaytracingOutput;
class RayComputeShader;
class DynamicConstBuffer;

class RadialBlur : public Singleton<RadialBlur> {

private:

	/*===== メンバ変数 =====*/

	// ブラーをかけるコンピュートシェーダー
	std::shared_ptr<RayComputeShader> blur_;

	// 影テクスチャと色テクスチャを混ぜ合わせるシェーダー
	std::shared_ptr<RayComputeShader> mixColorAndLuminance_;

	// 重みテーブル
	static const int WEIGHTS_COUNT = 4;
	std::array<float, WEIGHTS_COUNT> blurWeight_;

	// 重み定数バッファ
	std::shared_ptr<DynamicConstBuffer> weight_;

	// ブラーの強さ
	float blurPower_;


public:

	/*===== メンバ関数 =====*/

	// セッティング処理。
	void Setting();

	// デノイズ
	void Blur(int InImg, int OutImg);

	inline void SetBlurPower(float BlurPower) { blurPower_ = BlurPower; }

private:

	// ラジアルブラーをかける。
	void ApplyRadialBlur(int InImg, int OutImg);

};