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

	static const int DRIFT_PARTICLE_COUNT = 30;
	std::array<std::shared_ptr<DriftParticle>, DRIFT_PARTICLE_COUNT> driftParticle_;	// ドリフト時のパーティクル
	
	const int GENERATE_DELAY = 3;
	int particleGenerateDelay_;		// パーティクルを生成するときの遅延


public:

	// セッティング
	void Setting();

	// 初期化
	void Init();

	// 生成処理
	void Generate(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData);

	// 更新処理
	void Update(RayConstBufferData& ConstBufferData);

};