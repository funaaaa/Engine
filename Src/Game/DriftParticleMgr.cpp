#include "DriftParticleMgr.h"
#include "DriftParticle.h"
#include "BLASRegister.h"
#include "HitGroupMgr.h"
#include "FHelper.h"

void DriftParticleMgr::Setting()
{

	/*===== �Z�b�e�B���O���� =====*/

	smokeGenerateDelay_ = 0;
	fireGenerateDelay_ = 0;

	// �p�[�e�B�N���p�̃X�t�B�A��BLAS�𐶐�����B
	smokeBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/smoke.png" }, false, false);
	fireBlasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/fireBall.png" }, false, false);

	for (auto& index : driftParticle_) {

		index = std::make_shared<DriftParticle>();
		index->Setting(smokeBlasIndex_, &index - &driftParticle_[0]);

	}
}

void DriftParticleMgr::Init()
{

	/*===== ������ =====*/

	smokeGenerateDelay_ = 0;
	fireGenerateDelay_ = 0;

	for (auto& index : driftParticle_) {

		index->Init();

	}

}

void DriftParticleMgr::GenerateSmoke(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData, const bool& IsBoost, bool IsDash)
{

	/*===== �������� =====*/

	// ��������x���^�C�}�[���X�V���A��莞�Ԍo�߂��Ă�����p�[�e�B�N���𐶐�����B
	++smokeGenerateDelay_;

	// �x���̏������B
	bool canGenerate = false;
	if (IsDash) {
		canGenerate = GENERATE_DELAY_DASH < smokeGenerateDelay_;
	}
	else {
		canGenerate = GENERATE_DELAY < smokeGenerateDelay_;
	}

	if (canGenerate) {

		smokeGenerateDelay_ = 0;

		// �������鐔
		const int GCOUNT_DASH = 2;
		const int GCOUNT_DEF = 1;
		int generateCounter = 0;

		// ��������B
		for (auto& index_ : driftParticle_) {

			if (index_->GetIsActive()) continue;

			index_->GenerateSmoke(smokeBlasIndex_, Pos, MatRot, ConstBufferData, IsBoost, IsDash);

			++generateCounter;
			if ((IsDash && GCOUNT_DASH <= generateCounter) || (!IsDash && GCOUNT_DEF <= generateCounter)) {

				break;

			}

		}

	}

}

void DriftParticleMgr::GenerateFire(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData)
{

	/*===== �������� =====*/

	//// ��������x���^�C�}�[���X�V���A��莞�Ԍo�߂��Ă�����p�[�e�B�N���𐶐�����B
	//++fireGenerateDelay_;

	//// �x���̏������B
	//bool canGenerate = GENERATE_DELAY_FIRE < fireGenerateDelay_;

	//if (canGenerate) {

	//fireGenerateDelay_ = 0;

	int generateCount = 0;

	// ��������B
	for (auto& index_ : driftParticle_) {

		if (index_->GetIsActive()) continue;

		Vec3 random = FHelper::GetRandVec3(-500.0f, 500.0f) / 100.0f;

		index_->GenerateFire(fireBlasIndex_, Pos + random, MatRot, ConstBufferData);

		++generateCount;
		if (GENERATE_COUNT_FIRE < generateCount) {

			break;

		}


	}

	//}

}

void DriftParticleMgr::Update(RayConstBufferData& ConstBufferData)
{

	/*===== �X�V���� =====*/

	for (auto& index_ : driftParticle_) {

		if (!index_->GetIsActive()) continue;

		index_->Update(ConstBufferData);

	}

}
