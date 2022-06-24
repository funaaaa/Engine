#include "DriftParticleMgr.h"
#include "DriftParticle.h"
#include "BLASRegister.h"
#include "HitGroupMgr.h"

void DriftParticleMgr::Setting(const int& ConstBufferIndex)
{

	/*===== �Z�b�e�B���O���� =====*/

	particleGenerateDelay = 0;

	// �p�[�e�B�N���p�̃X�t�B�A��BLAS�𐶐�����B
	int particleSphereBlas = BLASRegister::Ins()->GenerateObj("Resource/", "sphere.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" });


	int counter = ConstBufferIndex;
	for (auto& index : driftParticle) {

		index = std::make_shared<DriftParticle>();
		index->Setting(particleSphereBlas, counter);
		++counter;

	}
}

void DriftParticleMgr::Generate(const Vec3& Pos, const Vec3& DriftVec, const DirectX::XMMATRIX& CarMatRot)
{

	/*===== �������� =====*/

	// ��������x���^�C�}�[���X�V���A��莞�Ԍo�߂��Ă�����p�[�e�B�N���𐶐�����B
	++particleGenerateDelay;
	if (GENERATE_DELAY < particleGenerateDelay) {

		particleGenerateDelay = 0;

		// ��������B
		for (auto& index : driftParticle) {

			if (index->GetIsActive()) continue;

			index->Generate(Pos, DriftVec, CarMatRot);

			break;

		}

	}

}

void DriftParticleMgr::Update(RayConstBufferData& ConstBufferData)
{

	/*===== �X�V���� =====*/

	for (auto& index : driftParticle) {

		if (!index->GetIsActive()) continue;

		index->Update(ConstBufferData);

	}

}
