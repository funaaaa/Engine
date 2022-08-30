#pragma once
#include "Vec.h"
#include "ConstBuffers.h"
#include <vector>
#include "PlayerModel.h"
#include <memory>

class BaseStage;
class OBB;
class PlayerTire;
class BaseItem;
class BaseOperationObject;
class BaseStage;

class Character {

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
	float rotY_;			// ハンドル操作によって変わるY軸の回転量
	float shellHitRot_;		// 甲羅に当たったときの回転。
	DirectX::XMMATRIX defBodyMatRot_;	// そのフレームのデフォルトの回転行列
	int returnDefPosTimer_;	// デフォルトの位置に戻るまでの時間 奈落に落ちた時用
	int canNotMoveTimer_;	// 操作不能のタイマー
	const int CAN_NOT_MOVE_TIMER_SHELL_HIT = 60;
	const int RETURN_DEFPOS_TIMER = 600;
	bool isShotBehind_;		// 後ろ側に甲羅を投げるかのフラグ
	bool onGround_;			// 地上にいるか t=地上 f=空中
	bool onGroundPrev_;		// 前フレームのonGround_
	bool onGrass_;			// 草の上にいるか t=草の上 f=草の上じゃない
	bool isConcentrationLine_;	// 集中線を出すかフラグ。
	bool isUseItem_;	// アイテムを使った瞬間。
	bool isJumpActionTrigger_;	// ジャンプアクションのトリガー

	std::shared_ptr<OBB> obb_;	// 当たり判定用OBB

	std::vector<std::shared_ptr<PlayerTire>> tires_;

	bool isGetItem_;	// アイテムを取得したフレームの判定

	// 操作オブジェクト
	std::shared_ptr<BaseOperationObject> operationObject_;

	// アイテムクラス
	std::shared_ptr<BaseItem> item_;

	bool IsTurningIndicatorRed_;// ウインカーの色が赤かどうか。
	int turningIndicatorTimer_;	// ウインカーがチカチカするタイマー
	const int TURNING_INDICATOR_TIMER = 30;

	const float MAX_SPEED = 16.0f;		// 移動速度の最大値
	const float MAX_SPEED_ON_GRASS = 8.0f;// 草の上にいるときの最大速度
	const float ADD_SPEED = 2.0f;		// 移動速度の加算量
	const float HANDLE_NORMAL = 0.03f;	// 通常時のハンドリングの角度
	const float MAX_GRAV = 8.0f;		// 重力の最大量
	const float ADD_GRAV = 0.2f;		// 重力の加算量
	const Vec3 PLAYER_DEF_POS = Vec3(0, 30, -30);


	/*-- ドリフト、加速時の車体の回転に関する変数 --*/

	float handleAmount_;				// ハンドル量
	DirectX::XMVECTOR handleRotQ_;		// ハンドルの回転量
	DirectX::XMVECTOR nowHandleRotQ_;	// ハンドルの回転量
	DirectX::XMVECTOR boostRotQ_;		// 加速時の正面方向への回転の行列
	DirectX::XMVECTOR nowBoostRotQ_;	// 加速時の正面方向への回転の行列
	const float  MAX_DRIFT_ROT = 0.4f;
	float baseDriftRot_;
	float nowDriftRot_;
	float baseBoostRot_;
	float nowBoostRot_;
	float tireLollingAmount_;
	int forwardTireLollingTimer_;
	const int FORWARD_TIMER_LOLLING_TIMER = 20;
	int driftRotTimer_;
	const int MAX_DRIFT_ROT_TIMER = 10;
	bool isRotRightSide_ = false;


	/*-- モデルのデータに関する変数 --*/

	PlayerModel playerModel_;


	/*-- ドリフトに関する変数 --*/

	float boostSpeed_;					// ブーストされているときの移動速度
	int driftBoostTimer_;				// ドリフトでブーストするまでのタイマー
	int boostTimer_;					// ブーストするフレーム数
	bool isDrift_;						// ドリフト状態かどうか。
	bool isTireMask_;


	/*-- ジャンプのブーストに関する変数 --*/

	float jumpBoostSpeed_;
	const float JUMP_BOOST_SPEED = 10.0f;
	const float SUB_JUMP_BOOST_SPEED = 0.2f;


public:

	struct TireUVSet {
		Vec2 uv_;
		Vec2 prevuv_;
	};

	// タイヤ痕書き込み用
	struct TireMaskUV {
		TireUVSet forwardLeftUV_;
		TireUVSet forwardRightUV_;
		TireUVSet behindLeftUV_;
		TireUVSet behindRightUV_;
	};

private:

	TireMaskUV tireMaskUV_;				// タイヤ痕を出す際に仕様

	const float HANDLE_DRIFT = 0.05f;	// ドリフト時のハンドリングの角度
	const float MAX_BOOST_SPEED = 15.0f;// ブーストの移動量の最大値
	const float SUB_BOOST_SPEED = 0.2f;	// ブーストの移動量の現残量
	const int DRIFT_BOOST_TIMER = 30;	// ドリフトでブーストするまでのタイマー
	const int DRIFT_TIMER = 20;			// ドリフト時のタイマー


public:

	enum class CHARA_ID {

		P1,	// プレイヤー1
		P1_WGHOST,	// プレイヤー1ゴースト書き込み有り
		AI1,	// AI1
		GHOST,	// ゴースト

	};


private:

	CHARA_ID charaID_;


public:

	/*===== メンバ関数 =====*/

	// 初期化処理
	Character(CHARA_ID CharaID);
	void Init();

	// 更新処理
	void Update(std::weak_ptr<BaseStage> StageData, RayConstBufferData& ConstBufferData, bool& IsPassedMiddlePoint, int& RapCount);

	// 描画処理
	void Draw();

	// タイヤ痕検出
	bool CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, TireMaskUV& TireMaskUVData);


	const Vec3& GetPos() { return pos_; }
	const Vec3& GetPrevPos() { return prevPos_; }
	Vec3 GetCameraForwardVec();
	const Vec3& GetForwardVec() { return forwardVec_; }
	Vec3 GetUpVec() { return upVec_; };
	float GetNowSpeedPer();
	bool GetIsGetItem() { return isGetItem_; }	// アイテムを取得した瞬間
	bool GetIsItem();	// アイテムを持っているか。
	bool GetUseItem() { return isUseItem_; }	// アイテムを使った瞬間。
	bool GetIdConcentrationLine() { return isConcentrationLine_; }

	// デバッグ用
	bool IsP1() { return charaID_ == CHARA_ID::P1; }

private:

	// 入力処理
	void Input(RayConstBufferData& ConstBufferData);

	// 移動処理
	void Move();

	// 当たり判定
	void CheckHit(std::weak_ptr<BaseStage> StageData, bool& IsPassedMiddlePoint, int& RapCount);

	// 斜め床の回転
	void RotObliqueFloor(const Vec3& HitNormal);

	// 車体傾けの処理
	void InclineCarBody();

};