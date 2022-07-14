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
	for (auto& index : driftParticle_) {

		index = std::make_shared<DriftParticle>();
		index->Setting(particleSphereBlas, counter);
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
		for (auto& index : driftParticle_) {

			if (index->GetIsActive()) continue;

			index->Generate(Pos, DriftVec, CarMatRot, ConstBufferData);

			break;

		}

	}

}

void DriftParticleMgr::Update(RayConstBufferData& ConstBufferData)
{

	/*===== �X�V���� =====*/

	for (auto& index : driftParticle_) {

		if (!index->GetIsActive()) continue;

		index->Update(ConstBufferData);

	}

}
