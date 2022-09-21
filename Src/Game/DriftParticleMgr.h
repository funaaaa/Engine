#pragma once
#include <memory>
#include <array>
#include "Singleton.h"
#include "Vec.h"
#include "ConstBuffers.h"

class DriftParticle;
class BLAS;
class PolygonMeshInstance;

// �h���t�g���̃p�[�e�B�N���Ǘ��N���X
class DriftParticleMgr : public Singleton<DriftParticleMgr> {

private:

	static const int DRIFT_PARTICLE_COUNT = 120;
	std::array<std::shared_ptr<DriftParticle>, DRIFT_PARTICLE_COUNT> driftParticle_;	// �h���t�g���̃p�[�e�B�N��

	std::weak_ptr<BLAS> smokeBlas_;
	std::weak_ptr<BLAS> fireBlas_;
	std::weak_ptr<BLAS> smallAuraBlas_;
	std::weak_ptr<BLAS> bigAuraBlas_;
	std::weak_ptr<BLAS> driftParticleBlas_;
	std::weak_ptr<BLAS> smallAuraOrangeBlas_;
	std::weak_ptr<BLAS> bigAuraOrangeBlas_;
	std::weak_ptr<BLAS> driftParticleOrangeBlas_;
	std::weak_ptr<BLAS> jumpEffectBlas_;

	struct AuraData {
		int insIndex_;
		int charaIndex_;
		bool isActive_;
	};
	std::array<AuraData, 10> bigAuraIndex_;		// �h���t�g��i�K�ڈȍ~�ɔ�������h���t�g���o�̑傫����
	std::array<AuraData, 10> smallAuraIndex_;	// �h���t�g��i�K�ڈȍ~�񔭐�����h���t�g���o�̏�������

	const int GENERATE_COUNT_FIRE = 2;

	int smokeGenerateDelay_;		// �p�[�e�B�N���𐶐�����Ƃ��̒x��
	int fireGenerateDelay_;
	int particleGenerateDelay_;

public:

	enum DELAY_ID {

		NONE_DELAY = 0,
		DELAY1 = 1,
		DEF = 5,
		BEFORE_START = 4,
		DASH = 12,

	};


public:

	// �Z�b�e�B���O
	void Setting();

	// ������
	void Init();

	// ��������
	void GenerateSmoke(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData, const bool& IsBoost, DriftParticleMgr::DELAY_ID DelayID, Vec3 ForwardVec = Vec3());
	void GenerateFire(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData);
	void GenerateAura(const int& BlasIndex, std::weak_ptr<PolygonMeshInstance> TireInstance, const int& Id, const bool& IsBoostRight, const bool& IsOrange, RayConstBufferData& ConstBufferData);
	void GenerateDriftParticle(std::weak_ptr<PolygonMeshInstance> TireInstance, const bool& IsBoostRight, const bool& IsOrange, const int& Id, const float& DriftRate, const bool& IsLevelChange, DriftParticleMgr::DELAY_ID DelayID, RayConstBufferData& ConstBufferData);
	void GenerateJumpEffect(std::weak_ptr<PolygonMeshInstance> CarBodyInstance, RayConstBufferData& ConstBufferData);

	// �X�V����
	void Update(RayConstBufferData& ConstBufferData);

	// �I�[����j���B
	void DestroyAura(const int& CharaIndex);
	// �I�[���͐����ς݂�
	bool IsAuraGenerated(const int& CharaIndex);

};