#pragma once
#include "Vec.h"
#include "ConstBuffers.h"
#include "PlayerModel.h"
#include "CharacterTireMask.h"
#include "FHelper.h"
#include <vector>
#include <memory>

class BaseStage;
class OBB;
class PlayerTire;
class BaseItem;
class BaseOperationObject;
class PolygonMeshInstance;
class BaseStage;
class CharacterInclineBody;
class CharacterGameFinish;
class CharacterDrift;
class CharacterRocket;
class CharacterTireMask;
struct CharacterFlags;

// キャラクタークラス。操作を管理する操作オブジェクトを入れ替えることによってAIとプレイヤーを切り替える。
class Character {

private:

	/*===== メンバ変数 =====*/
public:

	Vec3 pos_;								// 車の座標
	Vec3 prevPos_;							// 車の前フレームの座標
	Vec3 forwardVec_;						// 進行方向ベクトル
	Vec3 cameraForwardVec_;					// 甲羅にあたった瞬間の正面ベクトル
	Vec3 bottomVec;							// 下方向ベクトル
	Vec3 upVec_;							// 上方向ベクトル
	Vec3 size_;								// サイズ
	float boostSpeed_;						// ブーストされているときの移動速度
	int boostTimer_;						// ブーストするフレーム数
	const int BOOST_GIMMICK_BOOST_TIMER = 20;
	float speed_;							// 移動速度
	float gravity_;							// 重力
	float rotY_;							// ハンドル操作によって変わるY軸の回転量
	const float DEF_ROTY = -0.367411435f;
	float shellHitRot_;						// 甲羅に当たったときの回転。
	DirectX::XMMATRIX defBodyMatRot_;		// そのフレームのデフォルトの回転行列
	int returnDefPosTimer_;					// デフォルトの位置に戻るまでの時間 奈落に落ちた時用
	int charaIndex_;
	int canNotMoveTimer_;					// 操作不能のタイマー
	int shellIndex_;
	int timerToSkipShellCollider_;			// 甲羅の当たり判定を無効化するタイマー(甲羅を射った後に直ぐに自分に当たらないようにするため)
	std::shared_ptr<CharacterFlags> flags_;	// キャラクターで使用するフラグをまとめた構造体
	const int TIMER_TO_SKIP_SHELL_COLLIDER = 60;
	const int CAN_NOT_MOVE_TIMER_SHELL_HIT = 60;
	const int RETURN_DEFPOS_TIMER = 600;

	// 当たり判定用OBB
	std::shared_ptr<OBB> obb_;

	// 別な車との当たり判定用のヒットボックス
	std::weak_ptr<BLAS> hitBoxBlas_;
	std::weak_ptr<PolygonMeshInstance> hitBox_;

	std::vector<std::shared_ptr<PlayerTire>> tires_;


	// 操作オブジェクト
	std::shared_ptr<BaseOperationObject> operationObject_;

	// アイテムクラス
	std::shared_ptr<BaseItem> item_;

	const float MAX_SPEED = 16.0f;			// 移動速度の最大値
	const float MAX_SPEED_ON_GRASS = 12.0f;	// 草の上にいるときの最大速度
	const float ADD_SPEED = 2.0f;			// 移動速度の加算量
	const float HANDLE_DRIFT = 0.05f;		// ドリフト時のハンドリングの角度
	const float MAX_BOOST_SPEED = 15.0f;	// ブーストの移動量の最大値
	const float SUB_BOOST_SPEED = 0.2f;		// ブーストの移動量の現残量
	const float HANDLE_NORMAL = 0.03f;		// 通常時のハンドリングの角度
	const float MAX_GRAV = 8.0f;			// 重力の最大量
	const float ADD_GRAV = 0.4f;			// 重力の加算量
	Vec3 defPos_ = Vec3(0, 30, -30);
	const Vec3 PLAYER_DEF_POS = Vec3(-141.943f, 30.0f, 100.0f);
	const std::array<Vec3, 3> GHOST_DEF_POS = { Vec3(-59, 30, -106), Vec3(99, 30, -34), Vec3(23.775f, 30.0f, 166.0f) };


	/*-- ドリフト、加速時の車体の回転に関する変数 --*/
	std::shared_ptr<CharacterInclineBody> inclineBody_;	// キャラクターを回転させる際に使用する処理をまとめたクラス


	/*-- タイヤ痕に関する処理 --*/
	std::shared_ptr<CharacterTireMask> tireMask_;		// タイヤ痕に関する処理をまとめたクラス


	/*-- モデルのデータに関する変数 --*/
	PlayerModel playerModel_;


	/*-- ドリフトに関する変数 --*/
	std::shared_ptr<CharacterDrift> drift_;				// ドリフトに関する処理をまとめたクラス


	/*-- ジャンプのブーストに関する変数 --*/
	float jumpBoostSpeed_;
	const float JUMP_BOOST_SPEED = 10.0f;
	const float SUB_JUMP_BOOST_SPEED = 0.2f;


	/*-- エンジン用変数 --*/
	float engineWaveTimer_;		// 開始前にサイン波の動きを指せるようのタイマー	
	float engineWaveAmount_;


	/*-- 開始前用変数 --*/
	const float BEFORE_START_WAVE_LENGTH_RUN = 0.05f;
	const float BEFORE_START_WAVE_LENGTH_DEF = 0.3f;
	const float BEFORE_START_WAVE_LENGTH_ACCELL = 1.0f;


	/*-- ゴール演出用変数 --*/
	std::shared_ptr<CharacterGameFinish> gameFinish_;	// キャラクターのゲーム終了時に行う処理をまとめたクラス


	/*-- ゴール用 --*/
	int rapCount_;
	bool isPassedMiddlePoint_;


	/*-- ロケットアイテム関係 --*/
	std::shared_ptr<CharacterRocket> rocket_;


public:

	enum class CHARA_ID {

		P1,	// プレイヤー1
		P1_WGHOST,	// プレイヤー1ゴースト書き込み有り
		AI1,	// AI1
		GHOST,	// ゴースト

	};


public:

	CHARA_ID charaID_;


public:

	/*===== メンバ関数 =====*/

	// 初期化処理
	Character(CHARA_ID CharaID, int CharaIndex, int Level, int CharaPersonality = 0);
	void Init();

	// 更新処理
	void Update(std::weak_ptr<BaseStage> StageData, std::vector<std::shared_ptr<Character>> CharaData, bool IsBeforeStart, bool IsGameFinish);

	// 描画処理
	void Draw();

	// タイヤ痕検出
	bool CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, CharacterTireMask::TireMaskUV& TireMaskUVData);


	const Vec3& GetPos() { return pos_; }
	const Vec3& GetPrevPos() { return prevPos_; }
	Vec3 GetCameraForwardVec();
	const Vec3& GetForwardVec() { return forwardVec_; }
	Vec3 GetUpVec() { return upVec_; };
	float GetNowSpeedPer();
	bool GetIsGetItem();	// アイテムを取得した瞬間
	bool GetIsItem();	// アイテムを持っているか。
	bool GetUseItem();	// アイテムを使った瞬間。
	bool GetIdConcentrationLine();
	int GetRapCount() { return rapCount_; }
	bool GetIsPassedMiddlePoint() { return isPassedMiddlePoint_; }

	// モデルを削除。
	void DeleteInstance();

	// デバッグ用
	bool IsP1() { return charaID_ == CHARA_ID::P1; }

private:

	// 入力処理
	void Input(bool IsBeforeStart);

	// 移動処理
	void Move(bool IsBeforeStart);

	// ドリフトに関する更新処理
	void UpdateDrift();

	// 当たり判定
	void CheckHit(std::weak_ptr<BaseStage> StageData, std::vector<std::shared_ptr<Character>> CharaData);

	// 車体傾けの処理
	void InclineCarBody();

	// エンジンの上下
	void EngineSineWave();

	// ゲーム終了時の更新処理
	void UpdateGameFinish();

	// ドリフトパーティクルの更新処理
	void UpdateDriftParticle(bool IsGameFinish, bool IsBeforeStart);

	// 車と車の押し戻し判定
	void CheckHitOtherCar(std::vector<std::shared_ptr<Character>> CharaData);

	// 車との当たり判定
	void CheckHitCar(const FHelper::RayToModelCollisionData& CollisionData, std::weak_ptr<Character> IndexCharacter, float CheckHitSize);

	// ロケットに関する更新処理
	void UpdateRocket();

};