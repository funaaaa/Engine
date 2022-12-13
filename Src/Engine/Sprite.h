#pragma once
#include "BaseSprite.h"

// �X�v���C�g�`��N���X
class Sprite : public BaseSprite {

private:

	// ���ʂ̐�������
	void CommonGenerate(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID);

public:

	// ��������
	void GenerateForTexture(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID, LPCWSTR FileName);
	void GenerateForColor(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID, DirectX::XMFLOAT4 Color);
	void GenerateSpecifyTextureID(Vec3 CenterPos, Vec2 Size, int ProjectionID, int PiplineID, int TextureID);

	// �`�揈��
	void Draw();

	// �F��ς��鏈��
	inline void SetColor(const DirectX::XMFLOAT4& Color) { constBufferDataB0_.color = Color; }

};