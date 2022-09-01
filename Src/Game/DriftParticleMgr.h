#pragma once
#include <memory>
#include <array>
#include "Singleton.h"
#include "Vec.h"
#include "ConstBuffers.h"

class DriftParticle;

// �h���t�g���̃p�[�e�B�N���Ǘ��N���X
class DriftParticleMgr : public Singleton<DriftParticleMgr> {

private:

	static const int DRIFT_PARTICLE_COUNT = 120;
	std::array<std::shared_ptr<DriftParticle>, DRIFT_PARTICLE_COUNT> driftParticle_;	// �h���t�g���̃p�[�e�B�N��

	int smokeBlasIndex_;
	int fireBlasIndex_;

	const int GENERATE_COUNT_FIRE = 2;

	int smokeGenerateDelay_;		// �p�[�e�B�N���𐶐�����Ƃ��̒x��
	int fireGenerateDelay_;

public:

	enum DELAY_ID {

		NONE_DELAY = 0,
		DEF = 3,
		DASH = 12,

	};


public:

	// �Z�b�e�B���O
	void Setting();

	// ������
	void Init();

	// ��������
	void GenerateSmoke(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData, const bool& IsBoost, DriftParticleMgr::DELAY_ID DelayID);
	void GenerateFire(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData);

	// �X�V����
	void Update(RayConstBufferData& ConstBufferData);

};