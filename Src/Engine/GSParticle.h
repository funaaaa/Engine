#pragma once
#include "BaseGSParticle.h"
#include <vector>

class GSParticle : public BaseGSParticle {

private:

	// ‹¤’Ê‚Ì¶¬ˆ—
	void CommonGenerate(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, int instanceCount = 1);

public:

	// ¶¬ˆ—
	void GenerateForTexture(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, LPCWSTR fileName, int instanceCount = 1);
	void GenerateForColor(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, XMFLOAT4 color, int instanceCount = 1);
	void GenerateSpecifyTextureID(XMFLOAT3 centerPos, XMFLOAT2 size, int projectionID, int piplineID, int textureID, int instanceCount = 1);

	// •`‰æˆ—
	void Draw();
	void DrawInstance(vector<GSConstBufferDataB0> instanceData);

	// F‚ğ•Ï‚¦‚éˆ—
	inline void ChangeColor(const XMFLOAT4& color) {

		constBufferDataB0.color = color;

	}
	// F‚ğæ“¾‚·‚éˆ—
	inline XMFLOAT4 GetColor() { return constBufferDataB0.color; }

};