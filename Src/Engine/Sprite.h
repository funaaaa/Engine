#pragma once
#include "BaseSprite.h"

// スプライト描画クラス
class Sprite : public BaseSprite {

private:

	// 共通の生成処理
	void CommonGenerate(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID);

public:

	// 生成処理
	void GenerateForTexture(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID, LPCWSTR FileName);
	void GenerateForColor(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID, DirectX::XMFLOAT4 Color);
	void GenerateSpecifyTextureID(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID, int TextureID);

	// 描画処理
	void Draw();

	// 色を変える処理
	inline void SetColor(const DirectX::XMFLOAT4& Color) { constBufferDataB0_.color = Color; }

};