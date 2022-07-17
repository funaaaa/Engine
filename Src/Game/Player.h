#pragma once
#include "Vec.h"
#include "ConstBuffers.h"
#include <vector>
#include "PlayerModel.h"
#include <memory>

class BaseStage;
class OBB;
class PlayerTire;

class Player {

private:

	/*===== メンバ変数 =====*/
public:

	Vec3 pos_;				// 車の座標
	Vec3 prevPos_;			// 車の前フレームの座標
	Vec3 forwardVec_;		// 進行方向ベクトル
	Vec3 bottomVec;			// 下方向ベクトル
	Vec3 upVec_;			// 上方向ベクトル
	Vec3 size_;				// サイズ
	float speed_;			// 移動速度
	float gravity_;			// 重力
	float rotY_;				// ハンドル操作によって変わるY軸の回転量
	int returnDefPosTimer_;	// デフォルトの位置に戻るまでの時間 奈落に落ちた時用
	const int RETURN_DEFPOS_TIMER = 600;
	bool isGround_;			// 地上にいるか t=地上 f=空中
	bool isGrass_;			// 草の上にいるか t=草の上 f=草の上じゃない

	std::shared_ptr<OBB> obb_;	// 当たり判定用OBB

	std::vector<std::shared_ptr<PlayerTire>> tires_;

	bool IsTurningIndicatorRed_;// ウインカーの色が赤かどうか。
	int turningIndicatorTimer_;	// ウインカーがチカチカするタイマー
	const int TURNING_INDICATOR_TIMER = 30;

	const float MAX_SPEED = 16.0f;		// 移動速度の最大値
	const float MAX_SPEED_ON_GRASS = 8.0f;// 草の上にいるときの最大速度
	const float ADD_SPEED = 2.0f;		// 移動速度の加算量
	const float HANDLE_NORMAL = 0.03f;	// 通常時のハンドリングの角度
	const float MAX_GRAV = 7.0f;		// 重力の最大量
	const float ADD_GRAV = 0.05f;		// 重力の加算量
	const Vec3 PLAYER_DEF_POS = Vec3(0, 30, -30);


	/*-- モデルのデータに関する変数 --*/

	PlayerModel playerModel_;


	/*-- ドリフトに関する変数 --*/

	float boostSpeed_;					// ブーストされているときの移動速度
	int driftBoostTimer_;				// ドリフトでブーストするまでのタイマー
	bool isDrift_;						// ドリフト状態かどうか。

	const float HANDLE_DRIFT = 0.06f;	// ドリフト時のハンドリングの角度
	const float MAX_BOOST_SPEED = 20.0f;// ブーストの移動量の最大値
	const float SUB_BOOST_SPEED = 0.2f;	// ブーストの移動量の現残量
	const int DRIFT_BOOST_TIMER = 30;	// ドリフトでブーストするまでのタイマー


public:

	/*===== メンバ関数 =====*/

	// 初期化処理
	Player();
	void Init();

	// 更新処理
	void Update(std::weak_ptr<BaseStage> StageData, RayConstBufferData& ConstBufferData, bool& IsPassedMiddlePoint, int& RapCount);

	// 描画処理
	void Draw();

	const Vec3& GetPos() { return pos_; }
	const Vec3& GetForwardVec() { return forwardVec_; }
	Vec3 GetUpVec() { return upVec_; };
	float GetNowSpeedPer();

private:

	// 入力処理
	void Input(RayConstBufferData& ConstBufferData);

	// 移動処理
	void Move();

	// 当たり判定
	void CheckHit(std::weak_ptr<BaseStage> StageData, bool& IsPassedMiddlePoint, int& RapCount);

	// 斜め床の回転
	void RotObliqueFloor(const Vec3& HitNormal);

};