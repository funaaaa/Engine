#pragma once
#include "Vec.h"

class Player {

private:

	/*===== メンバ変数 =====*/

	int carBlasIndex;		// 車のモデルのBLASのインデックス
	int carInstanceIndex;	// 車のモデルのインスタンスのインデックス
	int stageBlasIndex;		// ステージのBLASのインデックス
	int stageInstanceIndex;	// ステージのインスタンスのインデックス
	Vec3 pos;				// 車の座標
	Vec3 forwardVec;		// 進行方向ベクトル
	Vec3 bottomVec;			// 下方向ベクトル
	Vec3 upVec;				// 上方向ベクトル
	Vec3 size;				// サイズ
	float speed;			// 移動速度
	float gravity;			// 重力
	float rotY;				// ハンドル操作によって変わるY軸の回転量
	bool isGround;			// 地上にいるか t=地上 f=空中

	const float MAX_SPEED = 10.0f;		// 移動速度の最大値
	const float ADD_SPEED = 2.0f;		// 移動速度の加算量
	const float HANDLE_NORMAL = 0.03f;	// 通常時のハンドリングの角度
	const float MAX_GRAV = 5.0f;		// 重力の最大量
	const float ADD_GRAV = 0.05f;		// 重力の加算量



	/*-- ドリフトに関する変数 --*/

	float boostSpeed;					// ブーストされているときの移動速度
	int driftBoostTimer;				// ドリフトでブーストするまでのタイマー
	bool isDrift;						// ドリフト状態かどうか。

	const float HANDLE_DRIFT = 0.06f;	// ドリフト時のハンドリングの角度
	const float MAX_BOOST_SPEED = 20.0f;// ブーストの移動量の最大値
	const float SUB_BOOST_SPEED = 0.2f;	// ブーストの移動量の現残量
	const int DRIFT_BOOST_TIMER = 30;	// ドリフトでブーストするまでのタイマー


public:

	/*===== メンバ関数 =====*/

	// 初期化処理
	void Init(const int& StageBlasIndex, const int& StageInstanceIndex);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	const Vec3& GetPos() { return pos; }
	const Vec3& GetForwardVec() { return forwardVec; }
	Vec3 GetUpVec() { return upVec; };
	float GetNowSpeedPer();

private:

	// 入力処理
	void Input();

	// 移動処理
	void Move();

	// 当たり判定
	void CheckHit();

	// 斜め床の回転
	void RotObliqueFloor(const Vec3& HitNormal);

};