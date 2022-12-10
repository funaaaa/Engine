#pragma once
#include "Vec.h"
#include "PlayerModel.h"
#include <memory>

class CharacterInclineBody;

// キャラクターのゲーム終了時に行う処理をまとめたクラス
class CharacterGameFinish {

private:

	/*===== メンバ変数 =====*/

	DirectX::XMMATRIX gameFinishTriggerMatRot_;
	Vec3 gameFinishTruggerForardVec_;// ゲーム終了時の正面ベクトル
	float gameFinishTriggerRotY_;	// ゲームが終わった瞬間の回転量
	float gameFinishEasingTimer_;	// ゲーム終了時演出に使用するイージングタイマー
	float gameFinishRotStopAmount_;	// 完全に停止した際の回転量。
	float gameFinishRotStopReturnAmount_;	// 完全に底した際に元の位置に戻すための回転量。
	const float GAME_FINISH_EASING_TIMER = 0.01f;
	const float GAME_FINISH_STOP_ROT = 2.0f;
	const float GAME_FINISH_STOP_ROT_LIMIT = 0.3f;
	bool isGameFinish_;				// ゲームが終わったフラグ
	bool isPrevGameFinish_;			// 前フレームのゲームが終わったフラグ
	bool isGameFinishDriftLeft_;	// ゲーム終了演出でどちらにドリフトするか。 t:左 f:右

public:

	// ゲーム終了時の基本的な更新処理に必要なデータ
	struct UpdateInputData {
		float maxSpeed_;			// キャラクターの最大移動速度
		PlayerModel playerModel_;	// キャラクターのモデルデータ
		std::weak_ptr<CharacterInclineBody> inclineBody_;	// キャラクターの回転に使用する処理をまとめたクラス
	};


public:

	/*===== メンバ関数 =====*/

	CharacterGameFinish();
	void Init();

	// ゲーム終了時の基本的な更新処理 主にタイマーの更新
	void Update(const UpdateInputData& Input, int& BoostTimer, float& Speed, float& RotY, Vec3& ForwardVec);

	// ゲーム終了時のAKIRAのドリフトをイメージした演出の処理 主に回転の更新
	void AKIRAERotation(PlayerModel& Model);

	// ゲーム終了時のフラグを更新。
	void UpdateGameFinishFlag(bool IsGameFinish);

	// ゲーム終了時のトリガー判定を取得。
	inline bool GetIsGameFinishTrigger() { return isGameFinish_ && !isPrevGameFinish_; }

	// ゲームが終了しているかをチェックする。ゲーム終了時フラグとは別。こっちでは前フレームでもゲームが終了してるかを確認する。
	inline bool CheckGameFinish() { return isGameFinish_ && isPrevGameFinish_; }

	// 各種ゲッタ
	inline bool GetIsGameFinish() { return isGameFinish_; }
	inline bool GetIsPrevGameFinish() { return isPrevGameFinish_; }
	bool GetIsGameFinishDriftLeft() { return isGameFinishDriftLeft_; }
	float GetGameFinishEasingTimer() { return gameFinishEasingTimer_; }
	DirectX::XMMATRIX GetGameFinishTriggerMatRot() { return gameFinishTriggerMatRot_; }

};