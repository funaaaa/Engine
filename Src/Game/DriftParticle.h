#pragma once
#include "Vec.h"
#include "ConstBuffers.h"
#include <DirectXPackedVector.h>
#include <memory>

class PolygonMeshInstance;
class BLAS;

// �h���t�g���̃p�[�e�B�N��
class DriftParticle {

private:

	std::weak_ptr<PolygonMeshInstance> instance_;	// �p�[�e�B�N���̃C���X�^���XID
	std::weak_ptr<BLAS> blas_;						// BLAS�̃C���f�b�N�X
	int constBufferIndex_;	// ���̃p�[�e�B�N���̃��C�e�B���O�݂̂��s���萔�o�b�t�@�̃C���f�b�N�X�ԍ�
	Vec3 pos_;				// ���W
	Vec3 forardVec_;		// �ړ������x�N�g��
	Vec3 scale_;
	bool isActive_;			// �L�����t���O
	bool isAppearingNow_;	// �o�����t���O �A���t�@�l��Z������B
	int appearingTimer_;	// �o�����Ă��������܂ł̃^�C�}�[
	std::weak_ptr<PolygonMeshInstance> trackedInstance;			// �ǐՑΏۂ�ID
	bool isTrackRight_;		// �ǂ��瑤�̃^�C���ɒǐՂ��邩 t:�E f:��

	const int APPEARING_TIMER = 5;

	const float APPEARING_ALPHA = 0.1f;
	const float EXIT_ALPHA = 0.1f;
	const float FIRE_ALPHA = 0.1f;
	const float SMOKE_SPEED = 0.5f;


	/*-- �I�[���Ɋւ���ϐ� --*/

	int changeScaleTimer_;	// �X�P�[����ς���܂ł̃^�C�}�[
	const int CHANGE_SCALE_TIMER_BIG = 7;
	const int CHANGE_SCALE_TIMER_SMALL = 3;
	const int AURA_FIRST_BIG_COUNT = 3;
	bool isAuraBig_;
	float nowScale_;
	float changeScale_;
	const float AURA_BIG_SCALE = 25.0f;
	const float AURA_SMALL_SCALE = 15.0f;
	const float AURA_ADD_SCALE = 20.0f;

	// �I�[���̈ʒu�����E�ɂ��炷��
	const float AURA_SIDE_SIZE = 23.0f;


	/*-- �p�[�e�B�N���Ɋւ���ϐ� --*/

	DirectX::XMVECTOR particleMatQ_;
	Vec3 particlePos_;	// ���̒l��pos�ɉ��Z����B
	Vec3 particlePrevPos_;
	Vec3 particlePrevMoveVec_;
	Vec3 particleVec_;
	Vec2 particleNowScale_;
	float particleSpeed_;
	const Vec2 PARTICLE_SCALE = Vec2(0.3f, 3.0f);
	const Vec2 PARTICLE_SCALE_LEVEL_CHANGE = Vec2(0.35f, 3.5f);
	const float MAX_PARTICLE_SPEED = 2.5f;
	const float MAX_PARTICLE_SPEED_LEVEL_CHANGE = 5.0f;
	float grav;
	const float ADD_GRAV = 0.4f;
	const float MAX_GRAV = 8.0f;


	/*-- �W�����v�A�N�V�����G�t�F�N�g�Ɋւ���ϐ� --*/

	float jumpActionEasingTimer_;
	const float ADD_JUMP_ACTION_EASING_TIMER_APPE = 0.08f;
	const float ADD_JUMP_ACTION_EASING_TIMER_EXIT = 0.1f;
	const float JUMP_DEF_SCALE = 10.0f;
	const float JUMP_BIG_SCALE = 80.0f;
	const float JUMP_EXIT_SCALE = 120.0f;


public:

	enum class ID {
		SMOKE,
		FIRE,
		AURA_BIG,	// �h���t�g�̓�i�K�ڈȍ~�Ń^�C���ɔ�������I�[���̑傫����
		AURA_SMALL,	// �h���t�g�̓�i�K�ڈȍ~�Ń^�C���ɔ�������I�[���̏�������
		PARTICLE,
		JUMP_EFFECT,
	};

private:

	ID mode_;


public:

	// �R���X�g���N�^
	DriftParticle();

	// �Z�b�e�B���O����
	void Setting(std::weak_ptr<BLAS> Blas, const int ConstBufferIndex);

	// ����������
	void Init();

	// ��������
	void GenerateSmoke(std::weak_ptr<BLAS> Blas, const Vec3& Pos, const DirectX::XMMATRIX MatRot, const bool& IsBoost, Vec3 ForwardVec);
	void GenerateFire(std::weak_ptr<BLAS> Blas, const Vec3& Pos, const DirectX::XMMATRIX MatRot);
	void GenerateAura(std::weak_ptr<BLAS> Blas, std::weak_ptr<PolygonMeshInstance> TireInstance, const ID& Id, const bool& IsBoostRight);
	void GenerateDriftParticle(std::weak_ptr<BLAS> Blas, std::weak_ptr<PolygonMeshInstance> TireInstance, const ID& Id, const bool& IsBoostRight, const bool& IsLevelChange);
	void GenerateJumpEffect(std::weak_ptr<BLAS> Blas, std::weak_ptr<PolygonMeshInstance> CarBodyInstance);

	// �X�V����
	void Update();

	// �A�N�Z�b�T
	const bool& GetIsActive() { return isActive_; }

};