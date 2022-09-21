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

	for (auto& index : smallAuraIndex_) {
		index.charaIndex_ = -1;
		index.insIndex_ = -1;
		index.isActive_ = false;
	}
	for (auto& index : bigAuraIndex_) {
		index.charaIndex_ = -1;
		index.insIndex_ = -1;
		index.isActive_ = false;
	}


	// �p�[�e�B�N���p�̃X�t�B�A��BLAS�𐶐�����B
	smokeBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/smoke.png" }, false, false);
	fireBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/fireBall.png" }, false, false);
	bigAuraBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/bigAura.png" }, false, false);
	smallAuraBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/smallAura.png" }, false, false);
	driftParticleBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/driftParticle.png" }, false, false);
	bigAuraOrangeBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/bigAuraOrange.png" }, false, false);
	smallAuraOrangeBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/smallAuraOrange.png" }, false, false);
	driftParticleOrangeBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/driftParticleOrange.png" }, false, false);
	jumpEffectBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/jumpEffect.png" }, false, false);


	for (auto& index : driftParticle_) {

		index = std::make_shared<DriftParticle>();
		index->Setting(smokeBlas_, static_cast<int>(&index - &driftParticle_[0]));

	}
}

void DriftParticleMgr::Init()
{

	/*===== ������ =====*/

	smokeGenerateDelay_ = 0;
	fireGenerateDelay_ = 0;

	for (auto& index : smallAuraIndex_) {
		index.charaIndex_ = -1;
		index.insIndex_ = -1;
		index.isActive_ = false;
	}
	for (auto& index : bigAuraIndex_) {
		index.charaIndex_ = -1;
		index.insIndex_ = -1;
		index.isActive_ = false;
	}

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

		Vec3 random = FHelper::GetRandVec3(-500, 500) / 100.0f;

		index_->GenerateFire(fireBlas_, Pos + random, MatRot, ConstBufferData);

		++generateCount;
		if (GENERATE_COUNT_FIRE < generateCount) {

			break;

		}


	}

	//}

}

void DriftParticleMgr::GenerateAura(const int& CharaIndex, std::weak_ptr<PolygonMeshInstance> TireInstance, const int& Id, const bool& IsBoostRight, const bool& IsOrange, RayConstBufferData& ConstBufferData)
{

	/*===== �I�[���𐶐����� =====*/

	// ��������B
	for (auto& index : driftParticle_) {

		if (index->GetIsActive()) continue;

		DriftParticle::ID id = static_cast<DriftParticle::ID>(Id);
		bool isAuraBig = id == DriftParticle::ID::AURA_BIG;
		if (IsOrange) {
			index->GenerateAura(isAuraBig ? bigAuraOrangeBlas_ : smallAuraOrangeBlas_, TireInstance, id, IsBoostRight, ConstBufferData);
		}
		else {
			index->GenerateAura(isAuraBig ? bigAuraBlas_ : smallAuraBlas_, TireInstance, id, IsBoostRight, ConstBufferData);
		}

		if (isAuraBig) {

			// �I�[���f�[�^�̋󂫂𒲂ׂ�B
			for (auto& auraIndex : bigAuraIndex_) {

				if (auraIndex.isActive_) continue;

				auraIndex.charaIndex_ = CharaIndex;
				auraIndex.insIndex_ = static_cast<int>(&index - &driftParticle_[0]);
				auraIndex.isActive_ = true;

				break;

			}

		}
		else {

			// �I�[���f�[�^�̋󂫂𒲂ׂ�B
			for (auto& auraIndex : smallAuraIndex_) {

				if (auraIndex.isActive_) continue;

				auraIndex.charaIndex_ = CharaIndex;
				auraIndex.insIndex_ = static_cast<int>(&index - &driftParticle_[0]);
				auraIndex.isActive_ = true;

				break;

			}

		}

		break;

	}

}

void DriftParticleMgr::GenerateDriftParticle(std::weak_ptr<PolygonMeshInstance> TireInstance, const bool& IsBoostRight, const bool& IsOrange, const int& Id, const float& DriftRate, const bool& IsLevelChange, DriftParticleMgr::DELAY_ID DelayID, RayConstBufferData& ConstBufferData)
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
		const int GENERATE_COUNT = static_cast<int>(5 * DriftRate);
		int generateCounter = 0;

		// ��������B
		for (auto& index_ : driftParticle_) {

			if (index_->GetIsActive()) continue;

			DriftParticle::ID id = static_cast<DriftParticle::ID>(Id);
			index_->GenerateDriftParticle(IsOrange ? driftParticleOrangeBlas_ : driftParticleBlas_, TireInstance, id, IsBoostRight, IsLevelChange, ConstBufferData);

			++generateCounter;
			if (GENERATE_COUNT < generateCounter) {

				break;

			}

		}

	}

}

void DriftParticleMgr::GenerateJumpEffect(std::weak_ptr<PolygonMeshInstance> CarBodyInstance, RayConstBufferData& ConstBufferData)
{

	/*===== �W�����v�A�N�V�������̃G�t�F�N�g�𐶐� =====*/

	// ��������B
	for (auto& index : driftParticle_) {

		if (index->GetIsActive()) continue;

		index->GenerateJumpEffect(jumpEffectBlas_, CarBodyInstance, ConstBufferData);

		break;

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

bool DriftParticleMgr::IsAuraGenerated(const int& CharaIndex)
{

	// �r�b�O�I�[������������Ă��邩�B
	bool isGenerateBigAura = false;
	for (auto& index : bigAuraIndex_) {

		if (!index.isActive_) continue;
		if (index.charaIndex_ != CharaIndex) continue;

		isGenerateBigAura = true;

		break;

	}

	// �X���[���I�[������������Ă��邩�B
	bool isGenerateSmallAura = false;
	for (auto& index : smallAuraIndex_) {

		if (!index.isActive_) continue;
		if (index.charaIndex_ != CharaIndex) continue;

		isGenerateSmallAura = true;

		break;

	}

	return isGenerateBigAura && isGenerateSmallAura;
}

void DriftParticleMgr::DestroyAura(const int& CharaIndex)
{

	/*===== �I�[����j�� =====*/

	// �I�[���f�[�^��j������B
	for (auto& index : bigAuraIndex_) {

		if (!index.isActive_) continue;
		if (index.charaIndex_ != CharaIndex) continue;

		driftParticle_[index.insIndex_]->Init();

		index.charaIndex_ = -1;
		index.insIndex_ = -1;
		index.isActive_ = false;

		break;

	}
	for (auto& index : smallAuraIndex_) {

		if (!index.isActive_) continue;
		if (index.charaIndex_ != CharaIndex) continue;

		driftParticle_[index.insIndex_]->Init();

		index.charaIndex_ = -1;
		index.insIndex_ = -1;
		index.isActive_ = false;

		break;

	}

}