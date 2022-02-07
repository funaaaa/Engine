#pragma once
#include "SpriteGaussian.h"
#include "RenderTarget.h"

#define NUM_GAUSS_WEIGHT 8

class Gaussian {
public:
	RenderTarget gaussX;
	RenderTarget gaussY;
	SpriteGaussian gaussXSprite;
	SpriteGaussian gaussYSprite;

	//コンストラクタ
	Gaussian() {};

	void Init();
};

class MultiPathFunction : public Singleton<MultiPathFunction> {
private:
	//コンストラクタ
	friend Singleton<MultiPathFunction>;

public:

	static Gaussian gaussian;			//ガウシアンぼかしをする際に使用する変数

	MultiPathFunction() {
		gaussian.Init();
	};

public:

	//リソースバリア関係
	void ResourceBarrierBefore(RenderTarget renderTarget);
	void ResourceBarrierAfter(RenderTarget renderTarget);
	void ResourceBarriersAfter(UINT num, RenderTarget* renderTargets[]);
	//レンダーターゲットと深度バッファをクリア
	void ClearRtvDsv(RenderTarget renderTarget);
	//レンダーターゲットを設定
	void SetRenderTargetAndClear(RenderTarget renderTarget);
	void SetRenderTarget(RenderTarget renderTarget);
	//MRT用のレンダーターゲット設定
	void SetRenderTargetsAndClear(UINT num, RenderTarget* renderTargets[]);
	void SetRenderTargets(UINT num, RenderTarget* renderTargets[]);

	//ガウスぼかし
	//void GaussianBlur(SpriteMultiPath xBlur, RenderTarget xBlurTarget, SpriteMultiPath yBlur, RenderTarget yBlurTarget);
	void GaussianBlur(XMFLOAT2 textureSize, float blurAmount, int blurCount, int targetTextureID, RenderTarget& exportTarget);
	//ガウスぼかしのぼかし量を設定
	void SetGaussianAmount(SpriteGaussian& xBlur, SpriteGaussian& yBlur, float amount);
	//重みの計算
	void CalcWeightsTableFromGaussian(vector<float>& weightsTbl, int sizeOfWeightsTbl, float sigma);

};