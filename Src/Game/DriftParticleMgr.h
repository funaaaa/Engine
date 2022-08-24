#pragma once
#include <memory>
#include <array>
#include "Singleton.h"
#include "Vec.h"
#include "ConstBuffers.h"

class DriftParticle;

// ドリフト時のパーティクル管理クラス
class DriftParticleMgr : public Singleton<DriftParticleMgr> {

private:

	static const int DRIFT_PARTICLE_COUNT = 120;
	std::array<std::shared_ptr<DriftParticle>, DRIFT_PARTICLE_COUNT> driftParticle_;	// ドリフト時のパーティクル

	int smokeBlasIndex_;
	int fireBlasIndex_;

	const int GENERATE_DELAY = 3;
	const int GENERATE_DELAY_DASH = 12;
	const int GENERATE_COUNT_FIRE = 2;

	int smokeGenerateDelay_;		// パーティクルを生成するときの遅延
	int fireGenerateDelay_;


public:

	// セッティング
	void Setting();

	// 初期化
	void Init();

	// 生成処理
	void GenerateSmoke(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData, const bool& IsBoost, bool IsDash = false);
	void GenerateFire(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData);

	// 更新処理
	void Update(RayConstBufferData& ConstBufferData);

};