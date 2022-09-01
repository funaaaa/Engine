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

	const int GENERATE_COUNT_FIRE = 2;

	int smokeGenerateDelay_;		// パーティクルを生成するときの遅延
	int fireGenerateDelay_;

public:

	enum DELAY_ID {

		NONE_DELAY = 0,
		DEF = 3,
		DASH = 12,

	};


public:

	// セッティング
	void Setting();

	// 初期化
	void Init();

	// 生成処理
	void GenerateSmoke(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData, const bool& IsBoost, DriftParticleMgr::DELAY_ID DelayID);
	void GenerateFire(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData);

	// 更新処理
	void Update(RayConstBufferData& ConstBufferData);

};