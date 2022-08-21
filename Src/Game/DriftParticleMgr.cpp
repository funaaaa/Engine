#include "DriftParticleMgr.h"
#include "DriftParticle.h"
#include "BLASRegister.h"
#include "HitGroupMgr.h"

void DriftParticleMgr::Setting()
{

	/*===== セッティング処理 =====*/

	particleGenerateDelay_ = 0;

	// パーティクル用のスフィアのBLASを生成する。
	int particleSphereBlas = BLASRegister::Ins()->GenerateObj("Resource/Game/", "plane.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/smoke.png" }, false, false);


	for (auto& index : driftParticle_) {

		index = std::make_shared<DriftParticle>();
		index->Setting(particleSphereBlas, &index - &driftParticle_[0]);

	}
}

void DriftParticleMgr::Init()
{

	/*===== 初期化 =====*/

	particleGenerateDelay_ = 0;

	for (auto& index : driftParticle_) {

		index->Init();

	}

}

void DriftParticleMgr::Generate(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData)
{

	/*===== 生成処理 =====*/

	// 生成する遅延タイマーを更新し、一定時間経過していたらパーティクルを生成する。
	++particleGenerateDelay_;
	if (GENERATE_DELAY < particleGenerateDelay_) {

		particleGenerateDelay_ = 0;

		// 生成する。
		for (auto& index_ : driftParticle_) {

			if (index_->GetIsActive()) continue;

			index_->Generate(Pos, MatRot, ConstBufferData);

			break;

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
