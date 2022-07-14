#include "DriftParticleMgr.h"
#include "DriftParticle.h"
#include "BLASRegister.h"
#include "HitGroupMgr.h"

void DriftParticleMgr::Setting(const int& ConstBufferIndex)
{

	/*===== �Z�b�e�B���O���� =====*/

	particleGenerateDelay_ = 0;

	// �p�[�e�B�N���p�̃X�t�B�A��BLAS�𐶐�����B
	int particleSphereBlas = BLASRegister::Ins()->GenerateObj("Resource/", "sphere.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" });


	int counter = ConstBufferIndex;
	for (auto& index_ : driftParticle_) {

		index_ = std::make_shared<DriftParticle>();
		index_->Setting(particleSphereBlas, counter);
		++counter;

	}
}

void DriftParticleMgr::Generate(const Vec3& Pos, const Vec3& DriftVec, const DirectX::XMMATRIX& CarMatRot, RayConstBufferData& ConstBufferData)
{

	/*===== �������� =====*/

	// ��������x���^�C�}�[���X�V���A��莞�Ԍo�߂��Ă�����p�[�e�B�N���𐶐�����B
	++particleGenerateDelay_;
	if (GENERATE_DELAY < particleGenerateDelay_) {

		particleGenerateDelay_ = 0;

		// ��������B
		for (auto& index_ : driftParticle_) {

			if (index_->GetIsActive()) continue;

			index_->Generate(Pos, DriftVec, CarMatRot, ConstBufferData);

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
