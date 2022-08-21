#pragma once
#include "Vec.h"
#include "ConstBuffers.h"
#include <DirectXPackedVector.h>

// �h���t�g���̃p�[�e�B�N��
class DriftParticle {

private:

	int particleIns_;		// �p�[�e�B�N���̃C���X�^���XID
	int blasIndex_;			// BLAS�̃C���f�b�N�X
	int constBufferIndex_;	// ���̃p�[�e�B�N���̃��C�e�B���O�݂̂��s���萔�o�b�t�@�̃C���f�b�N�X�ԍ�
	Vec3 pos_;				// ���W
	bool isActive_;			// �L�����t���O


public:

	// �R���X�g���N�^
	DriftParticle();

	// �Z�b�e�B���O����
	void Setting(const int& BlasIndex, const int ConstBufferIndex);

	// ����������
	void Init();

	// ��������
	void Generate(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData);

	// �X�V����
	void Update(RayConstBufferData& ConstBufferData);

	// �A�N�Z�b�T
	const bool& GetIsActive() { return isActive_; }

};