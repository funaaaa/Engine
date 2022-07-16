#pragma once
#include "BaseSprite.h"

class Sprite : public BaseSprite {

private:

	// ‹¤’Ê‚Ì¶¬ˆ—
	void CommonGenerate(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID);

public:

	// ¶¬ˆ—
	void GenerateForTexture(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID, LPCWSTR FileName);
	void GenerateForColor(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID, DirectX::XMFLOAT4 Color);
	void GenerateSpecifyTextureID(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID, int TextureID);

	// •`‰æˆ—
	void Draw();

	// F‚ğ•Ï‚¦‚éˆ—
	inline void SetColor(const DirectX::XMFLOAT4& Color) { constBufferDataB0_.color = Color; }

};