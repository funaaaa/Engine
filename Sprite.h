#pragma once
#include "BaseSprite.h"

class Sprite : public BaseSprite {

private:

	// ‹¤’Ê‚Ì¶¬ˆ—
	void CommonGenerate(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID);

public:

	// ¶¬ˆ—
	void GenerateForTexture(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, LPCWSTR fileName);
	void GenerateForColor(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, XMFLOAT4 color);
	void GenerateSpecifyTextureID(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, int textureID);

	// •`‰æˆ—
	void Draw();

	// F‚ğ•Ï‚¦‚éˆ—
	inline void SetColor(const XMFLOAT4& color) { constBufferDataB0.color = color; }

};