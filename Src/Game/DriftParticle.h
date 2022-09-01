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
	bool isAppearingNow_;	// �o�����t���O �A���t�@�l��Z������B
	int appearingTimer_;	// �o�����Ă��������܂ł̃^�C�}�[

	const int APPEARING_TIMER = 5;

	const float APPEARING_ALPHA = 0.1f;
	const float EXIT_ALPHA = 0.1f;
	const float FIRE_ALPHA = 0.1f;

public:

	enum class ID {
		SMOKE,
		FIRE,
	};

private:

	ID id_;


public:

	// �R���X�g���N�^
	DriftParticle();

	// �Z�b�e�B���O����
	void Setting(const int& BlasIndex, const int ConstBufferIndex);

	// ����������
	void Init();

	// ��������
	void GenerateSmoke(const int& BlasIndex, const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData, const bool& IsBoost);
	void GenerateFire(const int& BlasIndex, const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData);

	// �X�V����
	void Update(RayConstBufferData& ConstBufferData);

	// �A�N�Z�b�T
	const bool& GetIsActive() { return isActive_; }

};