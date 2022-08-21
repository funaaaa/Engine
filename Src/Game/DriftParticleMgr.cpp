#include "DriftParticleMgr.h"
#include "DriftParticle.h"
#include "BLASRegister.h"
#include "HitGroupMgr.h"

void DriftParticleMgr::Setting()
{

	/*===== �Z�b�e�B���O���� =====*/

	particleGenerateDelay_ = 0;

	// �p�[�e�B�N���p�̃X�t�B�A��BLAS�𐶐�����B
	int particleSphereBlas = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/smoke.png" }, false, false);


	for (auto& index : driftParticle_) {

		index = std::make_shared<DriftParticle>();
		index->Setting(particleSphereBlas, &index - &driftParticle_[0]);

	}
}

void DriftParticleMgr::Init()
{

	/*===== ������ =====*/

	particleGenerateDelay_ = 0;

	for (auto& index : driftParticle_) {

		index->Init();

	}

}

void DriftParticleMgr::Generate(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData)
{

	/*===== �������� =====*/

	// ��������x���^�C�}�[���X�V���A��莞�Ԍo�߂��Ă�����p�[�e�B�N���𐶐�����B
	++particleGenerateDelay_;
	if (GENERATE_DELAY < particleGenerateDelay_) {

		particleGenerateDelay_ = 0;

		// ��������B
		for (auto& index_ : driftParticle_) {

			if (index_->GetIsActive()) continue;

			index_->Generate(Pos, MatRot, ConstBufferData);

			break;

		}

	}

}

void DriftParticleMgr::Update(RayConstBufferData& ConstBufferData)
{

	/*===== �X�V���� =====*/

	for (auto& index_ : driftParticle_) {

		if (!index_->GetIsActive()) continue;

		index_->Update(ConstBufferData);

	}

}
