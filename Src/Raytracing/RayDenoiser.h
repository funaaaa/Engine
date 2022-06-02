#pragma once
#include "Singleton.h"
#include "RaytracingOutput.h"
#include "RayComputeShader.h"

class Denoiser : public Singleton<Denoiser>{

private:

	/*===== メンバ変数 =====*/

	// ブラー出力用
	RaytracingOutput blurXOutput;
	RaytracingOutput blurYOutput;

	// ブラーをかけるコンピュートシェーダー
	RayComputeShader blurX;
	RayComputeShader blurY;
	RayComputeShader blurFinal;


public:

	/*===== メンバ関す =====*/

	// セッティング処理。
	void Setting();

	// ガウシアンブラーをかける。
	void ApplyDenoise(const int& InputUAVIndex, const int& OutputUAVIndex);

};