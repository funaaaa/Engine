#pragma once
#include "BaseSprite.h"

class SpriteGaussian : public BaseSprite {

public:

	// マルチパスレンダリング用の定数構造体
	struct ConstBufferDataGaussian {
		XMFLOAT4 gaussianWeight[2];				// ガウスフィルターに使用する重み
		float pad1;
	};

private:

	ConstBufferDataGaussian constBufferDataB1;	// 定数 マルチパスレンダリングに使用する各情報
	ComPtr<ID3D12Resource> constBuffB1;			// 定数バッファ

private:

	// 共通の生成処理
	void CommonGenerate(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID);

public:

	// 生成処理
	void GenerateForTexture(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, LPCWSTR fileName);
	void GenerateForColor(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, XMFLOAT4 color);
	void GenerateSpecifyTextureID(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, int textureID);

	// 描画処理
	void Draw();

	// 定数データを代入
	void ChangeConstBufferDataB1(vector<float> amount);

};