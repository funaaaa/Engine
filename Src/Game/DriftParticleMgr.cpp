#include "DriftParticleMgr.h"
#include "DriftParticle.h"
#include "BLASRegister.h"
#include "HitGroupMgr.h"
#include "FHelper.h"

void DriftParticleMgr::Setting()
{

	/*===== セッティング処理 =====*/

	smokeGenerateDelay_ = 0;
	particleGenerateDelay_ = 0;
	fireGenerateDelay_ = 0;
	bigAuraIndex_ = -1;
	smallAuraIndex_ = -1;

	// パーティクル用のスフィアのBLASを生成する。
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

	/*===== 初期化 =====*/

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

	/*===== オーラを生成する =====*/

	// 生成する。
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
		const int GENERATE_COUNT = 5 * DriftRate;
		int generateCounter = 0;

		// 生成する。
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

	/*===== 更新処理 =====*/

	for (auto& index_ : driftParticle_) {

		if (!index_->GetIsActive()) continue;

		index_->Update(ConstBufferData);

	}

}

void DriftParticleMgr::DestroyAura()
{

	/*===== オーラを破棄 =====*/

	// -1だったらオーラは生成されていない。
	if (bigAuraIndex_ != -1) {

		// オーラを破棄。
		driftParticle_[bigAuraIndex_]->Init();

		bigAuraIndex_ = -1;

	}
	if (smallAuraIndex_ != -1) {

		// オーラを破棄。
		driftParticle_[smallAuraIndex_]->Init();

		smallAuraIndex_ = -1;

	}

}
