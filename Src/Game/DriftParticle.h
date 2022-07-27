#pragma once
#include "Vec.h"
#include "ConstBuffers.h"
#include <DirectXPackedVector.h>

// �h���t�g���̃p�[�e�B�N��
class DriftParticle {

private:

	int particleIns_;		// �p�[�e�B�N���̃C���X�^���XID
	int constBufferIndex_;	// ���̃p�[�e�B�N���̃��C�e�B���O�݂̂��s���萔�o�b�t�@�̃C���f�b�N�X�ԍ�
	Vec3 pos_;				// ���W
	Vec3 forwardVec_;		// �i�s�����x�N�g��
	float speed_;			// �ړ����x
	float scale_;			// �傫��
	bool isActive_;			// �L�����t���O

	const float SPEED = 5.0f;
	const float SUB_SPEED = 0.1f;
	const float SCALE = 1.0f;
	const float SUB_SCALE = 0.05f;


public:

	// �R���X�g���N�^
	DriftParticle();

	// �Z�b�e�B���O����
	void Setting(const int& BlasIndex, const int ConstBufferIndex);

	// ����������
	void Init();

	// ��������
	void Generate(const Vec3& Pos, const Vec3& DriftVec, const DirectX::XMMATRIX& CarMatRot, RayConstBufferData& ConstBufferData);

	// �X�V����
	void Update(RayConstBufferData& ConstBufferData);

	// �A�N�Z�b�T
	const bool& GetIsActive() { return isActive_; }

};