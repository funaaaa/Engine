#include "DriftParticleMgr.h"
#include "DriftParticle.h"
#include "BLASRegister.h"
#include "HitGroupMgr.h"
#include "FHelper.h"
#include "BLAS.h"
#include "TextureManager.h"

void DriftParticleMgr::Setting()
{

	/*===== セッティング処理 =====*/

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


	// パーティクル用のスフィアのBLASを生成する。
	smokeBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/DriftParticle/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], false, true);
	smokeBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/DriftParticle/smoke.dds"));
	fireBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/DriftParticle/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], false, true);
	fireBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/DriftParticle/fireBall.dds"));
	bigAuraBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/DriftParticle/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], false, true);
	bigAuraBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/DriftParticle/bigAura.dds"));
	smallAuraBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/DriftParticle/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], false, true);
	smallAuraBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/DriftParticle/smallAura.dds"));
	driftParticleBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/DriftParticle/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], false, true);
	driftParticleBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/DriftParticle/driftParticle.dds"));
	bigAuraOrangeBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/DriftParticle/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], false, true);
	bigAuraOrangeBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/DriftParticle/bigAuraOrange.dds"));
	smallAuraOrangeBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/DriftParticle/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], false, true);
	smallAuraOrangeBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/DriftParticle/smallAuraOrange.dds"));
	driftParticleOrangeBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/DriftParticle/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], false, true);
	driftParticleOrangeBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/DriftParticle/driftParticleOrange.dds"));
	jumpEffectBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/DriftParticle/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], false, true);
	jumpEffectBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/DriftParticle/jumpEffect.dds"));


	for (auto& index : driftParticle_) {

		index = std::make_shared<DriftParticle>();
		index->Setting(smokeBlas_, static_cast<int>(&index - &driftParticle_[0]));

	}
}

void DriftParticleMgr::Init()
{

	/*===== 初期化 =====*/

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

void DriftParticleMgr::GenerateSmoke(const Vec3& Pos, const DirectX::XMMATRIX MatRot, bool IsBoost, DriftParticleMgr::DELAY_ID DelayID, Vec3 ForwardVec)
{

	/*===== 生成処理 =====*/

	// 生成する遅延タイマーを更新し、一定時間経過していたらパーティクルを生成する。
	++smokeGenerateDelay_;

	// 遅延の条件式。
	bool canGenerate = false;
	canGenerate = static_cast<int>(DelayID) < smokeGenerateDelay_;

	if (canGenerate) {

		smokeGenerateDelay_ = 0;

		// 生成する数
		const int GCOUNT_DASH = 2;
		const int GCOUNT_DEF = 1;
		int generateCounter = 0;

		// 生成する。
		for (auto& index_ : driftParticle_) {

			if (index_->GetIsActive()) continue;

			index_->GenerateSmoke(smokeBlas_, Pos, MatRot, IsBoost, ForwardVec);

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

void DriftParticleMgr::GenerateFire(const Vec3& Pos, const DirectX::XMMATRIX MatRot)
{

	/*===== 生成処理 =====*/

	//// 生成する遅延タイマーを更新し、一定時間経過していたらパーティクルを生成する。
	//++fireGenerateDelay_;

	//// 遅延の条件式。
	//bool canGenerate = GENERATE_DELAY_FIRE < fireGenerateDelay_;

	//if (canGenerate) {

	//fireGenerateDelay_ = 0;

	int generateCount = 0;

	// 生成する。
	for (auto& index_ : driftParticle_) {

		if (index_->GetIsActive()) continue;

		Vec3 random = FHelper::GetRandVec3(-500, 500) / 100.0f;

		index_->GenerateFire(fireBlas_, Pos + random, MatRot);

		++generateCount;
		if (GENERATE_COUNT_FIRE < generateCount) {

			break;

		}


	}

	//}

}

void DriftParticleMgr::GenerateAura(int CharaIndex, std::weak_ptr<PolygonMeshInstance> TireInstance, int Id, bool IsBoostRight, bool IsOrange)
{

	/*===== オーラを生成する =====*/

	// 生成する。
	for (auto& index : driftParticle_) {

		if (index->GetIsActive()) continue;

		DriftParticle::ID id = static_cast<DriftParticle::ID>(Id);
		bool isAuraBig = id == DriftParticle::ID::AURA_BIG;
		if (IsOrange) {
			index->GenerateAura(isAuraBig ? bigAuraOrangeBlas_ : smallAuraOrangeBlas_, TireInstance, id, IsBoostRight);
		}
		else {
			index->GenerateAura(isAuraBig ? bigAuraBlas_ : smallAuraBlas_, TireInstance, id, IsBoostRight);
		}

		if (isAuraBig) {

			// オーラデータの空きを調べる。
			for (auto& auraIndex : bigAuraIndex_) {

				if (auraIndex.isActive_) continue;

				auraIndex.charaIndex_ = CharaIndex;
				auraIndex.insIndex_ = static_cast<int>(&index - &driftParticle_[0]);
				auraIndex.isActive_ = true;

				break;

			}

		}
		else {

			// オーラデータの空きを調べる。
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

void DriftParticleMgr::GenerateDriftParticle(std::weak_ptr<PolygonMeshInstance> TireInstance, bool IsBoostRight, bool IsOrange, int Id, float DriftRate, bool IsLevelChange, DriftParticleMgr::DELAY_ID DelayID)
{

	/*===== パーティクルを生成 =====*/

	// 生成する遅延タイマーを更新し、一定時間経過していたらパーティクルを生成する。
	++particleGenerateDelay_;

	// 遅延の条件式。
	bool canGenerate = false;
	canGenerate = static_cast<int>(DelayID) < particleGenerateDelay_;

	if (canGenerate) {

		particleGenerateDelay_ = 0;

		// 生成する数
		const int MAX = 5;
		const int GENERATE_COUNT = static_cast<int>(5 * DriftRate);
		int generateCounter = 0;

		// 生成する。
		for (auto& index_ : driftParticle_) {

			if (index_->GetIsActive()) continue;

			DriftParticle::ID id = static_cast<DriftParticle::ID>(Id);
			index_->GenerateDriftParticle(IsOrange ? driftParticleOrangeBlas_ : driftParticleBlas_, TireInstance, id, IsBoostRight, IsLevelChange);

			++generateCounter;
			if (GENERATE_COUNT < generateCounter) {

				break;

			}

		}

	}

}

void DriftParticleMgr::GenerateJumpEffect(std::weak_ptr<PolygonMeshInstance> CarBodyInstance)
{

	/*===== ジャンプアクション時のエフェクトを生成 =====*/

	// 生成する。
	for (auto& index : driftParticle_) {

		if (index->GetIsActive()) continue;

		index->GenerateJumpEffect(jumpEffectBlas_, CarBodyInstance);

		break;

	}

}

void DriftParticleMgr::Update()
{

	/*===== 更新処理 =====*/

	for (auto& index_ : driftParticle_) {

		if (!index_->GetIsActive()) continue;

		index_->Update();

	}

}

bool DriftParticleMgr::IsAuraGenerated(int CharaIndex)
{

	// ビッグオーラが生成されているか。
	bool isGenerateBigAura = false;
	for (auto& index : bigAuraIndex_) {

		if (!index.isActive_) continue;
		if (index.charaIndex_ != CharaIndex) continue;

		isGenerateBigAura = true;

		break;

	}

	// スモールオーラが生成されているか。
	bool isGenerateSmallAura = false;
	for (auto& index : smallAuraIndex_) {

		if (!index.isActive_) continue;
		if (index.charaIndex_ != CharaIndex) continue;

		isGenerateSmallAura = true;

		break;

	}

	return isGenerateBigAura && isGenerateSmallAura;
}

void DriftParticleMgr::DestroyAura(int CharaIndex)
{

	/*===== オーラを破棄 =====*/

	// オーラデータを破棄する。
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