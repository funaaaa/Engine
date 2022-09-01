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
	int trackedID_;			// �ǐՑΏۂ�ID
	bool isTrackRight_;		// �ǂ��瑤�̃^�C���ɒǐՂ��邩 t:�E f:��

	const int APPEARING_TIMER = 5;

	const float APPEARING_ALPHA = 0.1f;
	const float EXIT_ALPHA = 0.1f;
	const float FIRE_ALPHA = 0.1f;


	// �I�[���Ɋւ���ϐ�
	int changeScaleTimer_;	// �X�P�[����ς���܂ł̃^�C�}�[
	const int CHANGE_SCALE_TIMER_BIG = 7.0f;
	const int CHANGE_SCALE_TIMER_SMALL = 3.0f;
	float nowScale_;
	float changeScale_;
	const float AURA_BIG_SCALE = 25.0f;
	const float AURA_SMALL_SCALE = 15.0f;

	// �I�[���̈ʒu�����E�ɂ��炷��
	const float AURA_SIDE_SIZE = 23.0f;

	// �p�[�e�B�N���Ɋւ���ϐ��B
	DirectX::XMVECTOR particleMatQ_;
	Vec3 particlePos_;	// ���̒l��pos�ɉ��Z����B
	Vec3 particlePrevPos_;
	Vec3 particlePrevMoveVec_;
	Vec3 particleVec_;
	Vec2 particleNowScale_;
	float particleSpeed_;
	const Vec2 PARTICLE_SCALE = Vec2(0.3f, 3.0f);
	const float MAX_PARTICLE_SPEED = 2.5f;
	float grav;
	const float ADD_GRAV = 0.4f;
	const float MAX_GRAV = 8.0f;


public:

	enum class ID {
		SMOKE,
		FIRE,
		AURA_BIG,	// �h���t�g�̓�i�K�ڈȍ~�Ń^�C���ɔ�������I�[���̑傫����
		AURA_SMALL,	// �h���t�g�̓�i�K�ڈȍ~�Ń^�C���ɔ�������I�[���̏�������
		PARTICLE,
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
	void GenerateAura(const int& BlasIndex, const int& TireInsIndex_, const ID& Id, const bool& IsBoostRight, RayConstBufferData& ConstBufferData);
	void GenerateDriftParticle(const int& BlasIndex, const int& TireInsIndex_, const ID& Id, const bool& IsBoostRight, RayConstBufferData& ConstBufferData);

	// �X�V����
	void Update(RayConstBufferData& ConstBufferData);

	// �A�N�Z�b�T
	const bool& GetIsActive() { return isActive_; }

};