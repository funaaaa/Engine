#pragma once
#include "Vec.h"
#include "ConstBuffers.h"
#include <DirectXPackedVector.h>
#include <memory>

class PolygonMeshInstance;
class BLAS;

// ドリフト時のパーティクル
class DriftParticle {

private:

	std::weak_ptr<PolygonMeshInstance> instance_;	// パーティクルのインスタンスID
	std::weak_ptr<BLAS> blas_;						// BLASのインデックス
	int constBufferIndex_;	// このパーティクルのライティングのみを行う定数バッファのインデックス番号
	Vec3 pos_;				// 座標
	Vec3 forardVec_;		// 移動方向ベクトル
	Vec3 scale_;
	bool isActive_;			// 有効化フラグ
	bool isAppearingNow_;	// 出現中フラグ アルファ値を濃くする。
	int appearingTimer_;	// 出現してから消えるまでのタイマー
	std::weak_ptr<PolygonMeshInstance> trackedInstance;			// 追跡対象のID
	bool isTrackRight_;		// どちら側のタイヤに追跡するか t:右 f:左

	const int APPEARING_TIMER = 5;

	const float APPEARING_ALPHA = 0.1f;
	const float EXIT_ALPHA = 0.1f;
	const float FIRE_ALPHA = 0.1f;
	const float SMOKE_SPEED = 0.5f;


	/*-- オーラに関する変数 --*/

	int changeScaleTimer_;	// スケールを変えるまでのタイマー
	const int CHANGE_SCALE_TIMER_BIG = 7;
	const int CHANGE_SCALE_TIMER_SMALL = 3;
	const int AURA_FIRST_BIG_COUNT = 3;
	bool isAuraBig_;
	float nowScale_;
	float changeScale_;
	const float AURA_BIG_SCALE = 25.0f;
	const float AURA_SMALL_SCALE = 15.0f;
	const float AURA_ADD_SCALE = 20.0f;

	// オーラの位置を左右にずらす量
	const float AURA_SIDE_SIZE = 23.0f;


	/*-- パーティクルに関する変数 --*/

	DirectX::XMVECTOR particleMatQ_;
	Vec3 particlePos_;	// この値をposに加算する。
	Vec3 particlePrevPos_;
	Vec3 particlePrevMoveVec_;
	Vec3 particleVec_;
	Vec2 particleNowScale_;
	float particleSpeed_;
	const Vec2 PARTICLE_SCALE = Vec2(0.3f, 3.0f);
	const Vec2 PARTICLE_SCALE_LEVEL_CHANGE = Vec2(0.35f, 3.5f);
	const float MAX_PARTICLE_SPEED = 2.5f;
	const float MAX_PARTICLE_SPEED_LEVEL_CHANGE = 5.0f;
	float grav;
	const float ADD_GRAV = 0.4f;
	const float MAX_GRAV = 8.0f;


	/*-- ジャンプアクションエフェクトに関する変数 --*/

	float jumpActionEasingTimer_;
	const float ADD_JUMP_ACTION_EASING_TIMER_APPE = 0.08f;
	const float ADD_JUMP_ACTION_EASING_TIMER_EXIT = 0.1f;
	const float JUMP_DEF_SCALE = 10.0f;
	const float JUMP_BIG_SCALE = 80.0f;
	const float JUMP_EXIT_SCALE = 120.0f;


public:

	enum class ID {
		SMOKE,
		FIRE,
		AURA_BIG,	// ドリフトの二段階目以降でタイヤに発生するオーラの大きい方
		AURA_SMALL,	// ドリフトの二段階目以降でタイヤに発生するオーラの小さい方
		PARTICLE,
		JUMP_EFFECT,
	};

private:

	ID mode_;


public:

	// コンストラクタ
	DriftParticle();

	// セッティング処理
	void Setting(std::weak_ptr<BLAS> Blas, const int ConstBufferIndex);

	// 初期化処理
	void Init();

	// 生成処理
	void GenerateSmoke(std::weak_ptr<BLAS> Blas, const Vec3& Pos, const DirectX::XMMATRIX MatRot, const bool& IsBoost, Vec3 ForwardVec);
	void GenerateFire(std::weak_ptr<BLAS> Blas, const Vec3& Pos, const DirectX::XMMATRIX MatRot);
	void GenerateAura(std::weak_ptr<BLAS> Blas, std::weak_ptr<PolygonMeshInstance> TireInstance, const ID& Id, const bool& IsBoostRight);
	void GenerateDriftParticle(std::weak_ptr<BLAS> Blas, std::weak_ptr<PolygonMeshInstance> TireInstance, const ID& Id, const bool& IsBoostRight, const bool& IsLevelChange);
	void GenerateJumpEffect(std::weak_ptr<BLAS> Blas, std::weak_ptr<PolygonMeshInstance> CarBodyInstance);

	// 更新処理
	void Update();

	// アクセッサ
	const bool& GetIsActive() { return isActive_; }

};