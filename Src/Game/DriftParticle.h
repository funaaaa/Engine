#pragma once
#include "Vec.h"
#include "ConstBuffers.h"
#include <DirectXPackedVector.h>

// ドリフト時のパーティクル
class DriftParticle {

private:

	int particleIns_;		// パーティクルのインスタンスID
	int constBufferIndex_;	// このパーティクルのライティングのみを行う定数バッファのインデックス番号
	Vec3 pos_;				// 座標
	Vec3 forwardVec_;		// 進行方向ベクトル
	float speed_;			// 移動速度
	float scale_;			// 大きさ
	bool isActive_;			// 有効化フラグ

	const float SPEED = 5.0f;
	const float SUB_SPEED = 0.1f;
	const float SCALE = 1.0f;
	const float SUB_SCALE = 0.05f;


public:

	// コンストラクタ
	DriftParticle();

	// セッティング処理
	void Setting(const int& BlasIndex, const int ConstBufferIndex);

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(const Vec3& Pos, const Vec3& DriftVec, const DirectX::XMMATRIX& CarMatRot, RayConstBufferData& ConstBufferData);

	// 更新処理
	void Update(RayConstBufferData& ConstBufferData);

	// アクセッサ
	const bool& GetIsActive() { return isActive_; }

};