#pragma once
#include "Vec.h"
#include "ConstBuffers.h"
#include <DirectXPackedVector.h>

// ドリフト時のパーティクル
class DriftParticle {

private:

	int particleIns_;		// パーティクルのインスタンスID
	int blasIndex_;			// BLASのインデックス
	int constBufferIndex_;	// このパーティクルのライティングのみを行う定数バッファのインデックス番号
	Vec3 pos_;				// 座標
	bool isActive_;			// 有効化フラグ
	bool isAppearingNow_;	// 出現中フラグ アルファ値を濃くする。
	int appearingTimer_;	// 出現してから消えるまでのタイマー
	int trackedID_;			// 追跡対象のID
	bool isTrackRight_;		// どちら側のタイヤに追跡するか t:右 f:左

	const int APPEARING_TIMER = 5;

	const float APPEARING_ALPHA = 0.1f;
	const float EXIT_ALPHA = 0.1f;
	const float FIRE_ALPHA = 0.1f;


	// オーラに関する変数
	int changeScaleTimer_;	// スケールを変えるまでのタイマー
	const int CHANGE_SCALE_TIMER_BIG = 7.0f;
	const int CHANGE_SCALE_TIMER_SMALL = 3.0f;
	float nowScale_;
	float changeScale_;
	const float AURA_BIG_SCALE = 25.0f;
	const float AURA_SMALL_SCALE = 15.0f;

	// オーラの位置を左右にずらす量
	const float AURA_SIDE_SIZE = 23.0f;

	// パーティクルに関する変数。
	DirectX::XMVECTOR particleMatQ_;
	Vec3 particlePos_;	// この値をposに加算する。
	Vec3 particlePrevPos_;
	Vec3 particlePrevMoveVec_;
	Vec3 particleVec_;
	Vec2 particleNowScale_;
	float particleSpeed_;
	const Vec2 PARTICLE_SCALE = Vec2(0.3f, 3.0f);
	const float MAX_PARTICLE_SPEED = 2.5f;
	float grav;
	const float ADD_GRAV = 0.4f;
	const float MAX_GRAV = 8.0f;


public:

	enum class ID {
		SMOKE,
		FIRE,
		AURA_BIG,	// ドリフトの二段階目以降でタイヤに発生するオーラの大きい方
		AURA_SMALL,	// ドリフトの二段階目以降でタイヤに発生するオーラの小さい方
		PARTICLE,
	};

private:

	ID id_;


public:

	// コンストラクタ
	DriftParticle();

	// セッティング処理
	void Setting(const int& BlasIndex, const int ConstBufferIndex);

	// 初期化処理
	void Init();

	// 生成処理
	void GenerateSmoke(const int& BlasIndex, const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData, const bool& IsBoost);
	void GenerateFire(const int& BlasIndex, const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData);
	void GenerateAura(const int& BlasIndex, const int& TireInsIndex_, const ID& Id, const bool& IsBoostRight, RayConstBufferData& ConstBufferData);
	void GenerateDriftParticle(const int& BlasIndex, const int& TireInsIndex_, const ID& Id, const bool& IsBoostRight, RayConstBufferData& ConstBufferData);

	// 更新処理
	void Update(RayConstBufferData& ConstBufferData);

	// アクセッサ
	const bool& GetIsActive() { return isActive_; }

};