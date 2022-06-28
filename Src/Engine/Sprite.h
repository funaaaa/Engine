#pragma once
#include "BaseSprite.h"

class Sprite : public BaseSprite {

private:

	// ���ʂ̐�������
	void CommonGenerate(Vec3 centerPos, DirectX::XMFLOAT2 size, int projectionID, int piplineID);

public:

	// ��������
	void GenerateForTexture(Vec3 centerPos, DirectX::XMFLOAT2 size, int projectionID, int piplineID, LPCWSTR fileName);
	void GenerateForColor(Vec3 centerPos, DirectX::XMFLOAT2 size, int projectionID, int piplineID, DirectX::XMFLOAT4 color);
	void GenerateSpecifyTextureID(Vec3 centerPos, DirectX::XMFLOAT2 size, int projectionID, int piplineID, int textureID);

	// �`�揈��
	void Draw();

	// �F��ς��鏈��
	inline void SetColor(const DirectX::XMFLOAT4& color) { constBufferDataB0.color = color; }

};