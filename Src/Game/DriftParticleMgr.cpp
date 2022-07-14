#include "DriftParticleMgr.h"
#include "DriftParticle.h"
#include "BLASRegister.h"
#include "HitGroupMgr.h"

void DriftParticleMgr::Setting(const int& ConstBufferIndex)
{

	/*===== セッティング処理 =====*/

	particleGenerateDelay_ = 0;

	// パーティクル用のスフィアのBLASを生成する。
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

	/*===== 生成処理 =====*/

	// 生成する遅延タイマーを更新し、一定時間経過していたらパーティクルを生成する。
	++particleGenerateDelay_;
	if (GENERATE_DELAY < particleGenerateDelay_) {

		particleGenerateDelay_ = 0;

		// 生成する。
		for (auto& index : driftParticle_) {

			if (index->GetIsActive()) continue;

			index->Generate(Pos, DriftVec, CarMatRot, ConstBufferData);

			break;

		}

	}

}

void DriftParticleMgr::Update(RayConstBufferData& ConstBufferData)
{

	/*===== 更新処理 =====*/

	for (auto& index : driftParticle_) {

		if (!index->GetIsActive()) continue;

		index->Update(ConstBufferData);

	}

}
