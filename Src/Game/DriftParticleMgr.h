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

	static const int DRIFT_PARTICLE_COUNT = 60;
	std::array<std::shared_ptr<DriftParticle>, DRIFT_PARTICLE_COUNT> driftParticle_;	// �h���t�g���̃p�[�e�B�N��

	std::weak_ptr<BLAS> smokeBlas_;					// ���̃p�[�e�B�N���̃��f�����
	std::weak_ptr<BLAS> fireBlas_;					// ���̃p�[�e�B�N���̃��f�����
	std::weak_ptr<BLAS> smallAuraBlas_;				// �h���t�g���ɏo�鏬�����I�[���̃��f�����
	std::weak_ptr<BLAS> bigAuraBlas_;				// �h���t�g���ɏo��傫���I�[���̃��f�����
	std::weak_ptr<BLAS> driftParticleBlas_;			// �h���t�g�̃p�[�e�B�N���̃��f�����
	std::weak_ptr<BLAS> smallAuraOrangeBlas_;		// �h���t�g��i�K�ڂɏo�鏬�����I�[���̃��f�����
	std::weak_ptr<BLAS> bigAuraOrangeBlas_;			// �h���t�g��i�K�ڂɏo��傫���I�[���̃��f�����
	std::weak_ptr<BLAS> driftParticleOrangeBlas_;	// �h���t�g��i�K�ڂɏo��p�[�e�B�N���̃��f�����
	std::weak_ptr<BLAS> jumpEffectBlas_;			// �W�����v���ɏo��G�t�F�N�g�̃��f�����

	// �I�[�����ǂ̃L�����ɏo�邩��ۑ�����\����
	struct AuraData {
		int insIndex_;		// �I�[�����o���L�����̕`����
		int charaIndex_;	// �I�[�����o���L������CharacterMgr�ł̔ԍ�
		bool isActive_;		// �I�[�����L��������Ă��邩�ǂ����B
	};
	std::array<AuraData, 10> bigAuraIndex_;		// �h���t�g��i�K�ڈȍ~�ɔ�������h���t�g���o�̑傫����
	std::array<AuraData, 10> smallAuraIndex_;	// �h���t�g��i�K�ڈȍ~�񔭐�����h���t�g���o�̏�������

	const int GENERATE_COUNT_FIRE = 2;			// ���̃p�[�e�B�N���𐶐����鐔

	int smokeGenerateDelay_;					// �p�[�e�B�N���𐶐�����Ƃ��̒x��
	int fireGenerateDelay_;						// ���̃p�[�e�B�N���𐶐�����x��
	int particleGenerateDelay_;					// �h���t�g�̃p�[�e�B�N���𐶐�����x��

public:

	// �p�[�e�B�N���𐶐�����ۂ̒x��
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
	void GenerateSmoke(const Vec3& Pos, const DirectX::XMMATRIX MatRot , bool IsBoost, DriftParticleMgr::DELAY_ID DelayID, Vec3 ForwardVec = Vec3());
	void GenerateFire(const Vec3& Pos, const DirectX::XMMATRIX MatRot );
	void GenerateAura(int BlasIndex, std::weak_ptr<PolygonMeshInstance> TireInstance, int Id, bool IsBoostRight, bool IsOrange );
	void GenerateDriftParticle(std::weak_ptr<PolygonMeshInstance> TireInstance, bool IsBoostRight, bool IsOrange, int Id, float DriftRate, bool IsLevelChange, DriftParticleMgr::DELAY_ID DelayID );
	void GenerateJumpEffect(std::weak_ptr<PolygonMeshInstance> CarBodyInstance );

	// �X�V����
	void Update();

	// �I�[����j���B
	void DestroyAura(int CharaIndex);
	// �I�[���͐����ς݂�
	bool IsAuraGenerated(int CharaIndex);

};