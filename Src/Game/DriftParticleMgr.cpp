#include "DriftParticleMgr.h"
#include "DriftParticle.h"
#include "BLASRegister.h"
#include "HitGroupMgr.h"
#include "FHelper.h"

void DriftParticleMgr::Setting()
{

	/*===== �Z�b�e�B���O���� =====*/

	smokeGenerateDelay_ = 0;
	particleGenerateDelay_ = 0;
	fireGenerateDelay_ = 0;
	bigAuraIndex_ = -1;
	smallAuraIndex_ = -1;

	// �p�[�e�B�N���p�̃X�t�B�A��BLAS�𐶐�����B
	smokeBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/smoke.png" }, false, false);
	fireBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/fireBall.png" }, false, false);
	bigAuraBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/bigAura.png" }, false, false);
	smallAuraBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/smallAura.png" }, false, false);
	driftParticleBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/driftParticle.png" }, false, false);
	bigAuraOrangeBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/bigAuraOrange.png" }, false, false);
	smallAuraOrangeBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/smallAuraOrange.png" }, false, false);
	driftParticleOrangeBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/driftParticleOrange.png" }, false, false);


	for (auto& index : driftParticle_) {

		index = std::make_shared<DriftParticle>();
		index->Setting(smokeBlas_, &index - &driftParticle_[0]);

	}
}

void DriftParticleMgr::Init()
{

	/*===== ������ =====*/

	smokeGenerateDelay_ = 0;
	fireGenerateDelay_ = 0;
	bigAuraIndex_ = -1;
	smallAuraIndex_ = -1;

	for (auto& index : driftParticle_) {

		index->Init();

	}

}

void DriftParticleMgr::GenerateSmoke(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData, const bool& IsBoost, DriftParticleMgr::DELAY_ID DelayID, Vec3 ForwardVec)
{

	/*===== �������� =====*/

	// ��������x���^�C�}�[���X�V���A��莞�Ԍo�߂��Ă�����p�[�e�B�N���𐶐�����B
	++smokeGenerateDelay_;

	// �x���̏������B
	bool canGenerate = false;
	canGenerate = static_cast<int>(DelayID) < smokeGenerateDelay_;

	if (canGenerate) {

		smokeGenerateDelay_ = 0;

		// �������鐔
		const int GCOUNT_DASH = 2;
		const int GCOUNT_DEF = 1;
		int generateCounter = 0;

		// ��������B
		for (auto& index_ : driftParticle_) {

			if (index_->GetIsActive()) continue;

			index_->GenerateSmoke(smokeBlas_, Pos, MatRot, ConstBufferData, IsBoost, ForwardVec);

			++generateCounter;
			bool isDashOverTheAmount = (DelayID == DELAY_ID::DASH && GCOUNT_DASH <= generateCounter);
			bool isDefOverTheAmount = (DelayID == DELAY_ID::DEF && GCOUNT_DEF <= generateCounter);
			bool isNoneDelayOverTheAmount = (DelayID == DELAY_ID::NONE_DELAY && GCOUNT_DEF <= generateCounter);
			if (isDefOverTheAmount || isDashOverTheAmount || isNoneDelayOverTheAmount) {

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

		index_->GenerateFire(fireBlas_, Pos + random, MatRot, ConstBufferData);

		++generateCount;
		if (GENERATE_COUNT_FIRE < generateCount) {

			break;

		}


	}

	//}

}

void DriftParticleMgr::GenerateAura(const int& TireInsIndex_, const int& Id, const bool& IsBoostRight, const bool& IsOrange, RayConstBufferData& ConstBufferData)
{

	/*===== �I�[���𐶐����� =====*/

	// ��������B
	for (auto& index : driftParticle_) {

		if (index->GetIsActive()) continue;

		DriftParticle::ID id = static_cast<DriftParticle::ID>(Id);
		bool isAuraBig = id == DriftParticle::ID::AURA_BIG;
		if (IsOrange) {
			index->GenerateAura(isAuraBig ? bigAuraOrangeBlas_ : smallAuraOrangeBlas_, TireInsIndex_, id, IsBoostRight, ConstBufferData);
		}
		else {
			index->GenerateAura(isAuraBig ? bigAuraBlas_ : smallAuraBlas_, TireInsIndex_, id, IsBoostRight, ConstBufferData);
		}

		if (isAuraBig) {
			bigAuraIndex_ = static_cast<int>(&index - &driftParticle_[0]);
		}
		else {
			smallAuraIndex_ = static_cast<int>(&index - &driftParticle_[0]);
		}

		break;

	}

}

void DriftParticleMgr::GenerateDriftParticle(const int& TireInsIndex_, const bool& IsBoostRight, const bool& IsOrange, const int& Id, const float& DriftRate, const bool& IsLevelChange, DriftParticleMgr::DELAY_ID DelayID, RayConstBufferData& ConstBufferData)
{

	/*===== �p�[�e�B�N���𐶐� =====*/

	// ��������x���^�C�}�[���X�V���A��莞�Ԍo�߂��Ă�����p�[�e�B�N���𐶐�����B
	++particleGenerateDelay_;

	// �x���̏������B
	bool canGenerate = false;
	canGenerate = static_cast<int>(DelayID) < particleGenerateDelay_;

	if (canGenerate) {

		particleGenerateDelay_ = 0;

		// �������鐔
		const int MAX = 5;
		const int GENERATE_COUNT = 5 * DriftRate;
		int generateCounter = 0;

		// ��������B
		for (auto& index_ : driftParticle_) {

			if (index_->GetIsActive()) continue;

			DriftParticle::ID id = static_cast<DriftParticle::ID>(Id);
			index_->GenerateDriftParticle(IsOrange ? driftParticleOrangeBlas_ : driftParticleBlas_, TireInsIndex_, id, IsBoostRight, IsLevelChange, ConstBufferData);

			++generateCounter;
			if (GENERATE_COUNT < generateCounter) {

				break;

			}

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

void DriftParticleMgr::DestroyAura()
{

	/*===== �I�[����j�� =====*/

	// -1��������I�[���͐�������Ă��Ȃ��B
	if (bigAuraIndex_ != -1) {

		// �I�[����j���B
		driftParticle_[bigAuraIndex_]->Init();

		bigAuraIndex_ = -1;

	}
	if (smallAuraIndex_ != -1) {

		// �I�[����j���B
		driftParticle_[smallAuraIndex_]->Init();

		smallAuraIndex_ = -1;

	}

}
