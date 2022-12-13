#pragma once
#include <memory>
#include <array>
#include "Singleton.h"
#include "Vec.h"
#include "ConstBuffers.h"

class DriftParticle;
class BLAS;
class PolygonMeshInstance;

// ドリフト時のパーティクル管理クラス
class DriftParticleMgr : public Singleton<DriftParticleMgr> {

private:

	static const int DRIFT_PARTICLE_COUNT = 60;
	std::array<std::shared_ptr<DriftParticle>, DRIFT_PARTICLE_COUNT> driftParticle_;	// ドリフト時のパーティクル

	std::weak_ptr<BLAS> smokeBlas_;					// 煙のパーティクルのモデル情報
	std::weak_ptr<BLAS> fireBlas_;					// 炎のパーティクルのモデル情報
	std::weak_ptr<BLAS> smallAuraBlas_;				// ドリフト時に出る小さいオーラのモデル情報
	std::weak_ptr<BLAS> bigAuraBlas_;				// ドリフト時に出る大きいオーラのモデル情報
	std::weak_ptr<BLAS> driftParticleBlas_;			// ドリフトのパーティクルのモデル情報
	std::weak_ptr<BLAS> smallAuraOrangeBlas_;		// ドリフト二段階目に出る小さいオーラのモデル情報
	std::weak_ptr<BLAS> bigAuraOrangeBlas_;			// ドリフト二段階目に出る大きいオーラのモデル情報
	std::weak_ptr<BLAS> driftParticleOrangeBlas_;	// ドリフト二段階目に出るパーティクルのモデル情報
	std::weak_ptr<BLAS> jumpEffectBlas_;			// ジャンプ時に出るエフェクトのモデル情報

	// オーラがどのキャラに出るかを保存する構造体
	struct AuraData {
		int insIndex_;		// オーラを出すキャラの描画情報
		int charaIndex_;	// オーラを出すキャラのCharacterMgrでの番号
		bool isActive_;		// オーラが有効化されているかどうか。
	};
	std::array<AuraData, 10> bigAuraIndex_;		// ドリフト二段階目以降に発生するドリフト演出の大きい方
	std::array<AuraData, 10> smallAuraIndex_;	// ドリフト二段階目以降二発生するドリフト演出の小さい方

	const int GENERATE_COUNT_FIRE = 2;			// 炎のパーティクルを生成する数

	int smokeGenerateDelay_;					// パーティクルを生成するときの遅延
	int fireGenerateDelay_;						// 炎のパーティクルを生成する遅延
	int particleGenerateDelay_;					// ドリフトのパーティクルを生成する遅延

public:

	// パーティクルを生成する際の遅延
	enum DELAY_ID {

		NONE_DELAY = 0,
		DELAY1 = 1,
		DEF = 5,
		BEFORE_START = 4,
		DASH = 12,

	};


public:

	// セッティング
	void Setting();

	// 初期化
	void Init();

	// 生成処理
	void GenerateSmoke(const Vec3& Pos, const DirectX::XMMATRIX MatRot , bool IsBoost, DriftParticleMgr::DELAY_ID DelayID, Vec3 ForwardVec = Vec3());
	void GenerateFire(const Vec3& Pos, const DirectX::XMMATRIX MatRot );
	void GenerateAura(int BlasIndex, std::weak_ptr<PolygonMeshInstance> TireInstance, int Id, bool IsBoostRight, bool IsOrange );
	void GenerateDriftParticle(std::weak_ptr<PolygonMeshInstance> TireInstance, bool IsBoostRight, bool IsOrange, int Id, float DriftRate, bool IsLevelChange, DriftParticleMgr::DELAY_ID DelayID );
	void GenerateJumpEffect(std::weak_ptr<PolygonMeshInstance> CarBodyInstance );

	// 更新処理
	void Update();

	// オーラを破棄。
	void DestroyAura(int CharaIndex);
	// オーラは生成済みか
	bool IsAuraGenerated(int CharaIndex);

};