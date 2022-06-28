#pragma once
#include "BaseSprite.h"

class Sprite : public BaseSprite {

private:

	// ‹¤’Ê‚Ì¶¬ˆ—
	void CommonGenerate(Vec3 centerPos, DirectX::XMFLOAT2 size, int projectionID, int piplineID);

public:

	// ¶¬ˆ—
	void GenerateForTexture(Vec3 centerPos, DirectX::XMFLOAT2 size, int projectionID, int piplineID, LPCWSTR fileName);
	void GenerateForColor(Vec3 centerPos, DirectX::XMFLOAT2 size, int projectionID, int piplineID, DirectX::XMFLOAT4 color);
	void GenerateSpecifyTextureID(Vec3 centerPos, DirectX::XMFLOAT2 size, int projectionID, int piplineID, int textureID);

	// •`‰æˆ—
	void Draw();

	// F‚ğ•Ï‚¦‚éˆ—
	inline void SetColor(const DirectX::XMFLOAT4& color) { constBufferDataB0.color = color; }

};