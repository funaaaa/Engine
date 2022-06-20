#pragma once
#include "Vec.h"

class Player {

private:

	/*===== メンバ変数 =====*/

	int carInstanceIndex;	// 車のモデルのインスタンスのインデックス
	Vec3 pos;				// 車の座標
	Vec3 forwardVec;		// 進行方向ベクトル
	float speed;			// 移動速度

	const float MAX_SPEED = 10.0f;		// 移動速度の最大値
	const float ADD_SPEED = 2.0f;		// 移動速度の加算量
	const float HANDLE_NORMAL = 0.05f;	// 通常時のハンドリングの角度


public:

	/*===== メンバ関数 =====*/

	// 初期化処理
	void Init();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	const Vec3& GetPos() { return pos; }
	const Vec3& GetForwardVec() { return forwardVec; }
	Vec3 GetUpVec();
	float GetNowSpeedPer() { return speed / MAX_SPEED; }

private:

	// 入力処理
	void Input();

	// 移動処理
	void Move();

};