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

	const int APPEARING_TIMER = 5;

	const float APPEARING_ALPHA = 0.1f;
	const float EXIT_ALPHA = 0.1f;
	const float FIRE_ALPHA = 0.1f;

public:

	enum class ID {
		SMOKE,
		FIRE,
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

	// 更新処理
	void Update(RayConstBufferData& ConstBufferData);

	// アクセッサ
	const bool& GetIsActive() { return isActive_; }

};