#pragma once
#include "Vec.h"
#include "BaseOperationObject.h"
#include "PlayerModel.h"
#include "PlayerTire.h"
#include <array>
#include <vector>
#include <memory>

class CharacterInclineBody;
class CharacterGameFinish;

// キャラクターのドリフトに関する処理をまとめたクラス
class CharacterDrift {

private:

	/*===== メンバ変数 =====*/

	Vec3 driftJumpVec_;					// ドリフトジャンプをする方向 基本的に床の法線になる。
	float driftJumpSpeed_;				// ドリフトジャンプをする際の移動速度。最初だけ大きくしてだんだん小さくすることでジャンプする。
	const float DRIFT_JUMP_SPEED = 6.0f;
	const float SUB_DRIFT_JUMP_SPEED = 0.6f;
	int driftTimer_;					// ドリフトの経過時間
	int turningIndicatorTimer_;			// ウインカーがチカチカするタイマー
	int beforeStartSmokeTimer_;			// ゲーム開始前でアクセルの入力があった際に出る煙の生成スパン。
	const int BEFORE_START_SMOKE_TIMER = 5;
	const int TURNING_INDICATOR_TIMER = 30;
	bool isDriftRight_;					// ドリフトが右側かどうか。
	bool isInputLTPrev_;				// 前フレームにLTが押されたかどうか
	bool isInputLT_;					// LTが押されたかどうか。
	bool isDriftJump_;					// ドリフト前のジャンプ中かどうか。
	bool isDriftJumpPrev_;				// ドリフト前のジャンプ中かどうかの前Fフラグ。
	bool isDrift_;						// ドリフト状態かどうか。
	bool isTireMask_;					// タイヤ痕を書き込むかどうかのフラグ。
	bool IsTurningIndicatorRed_;		// ウインカーの色が赤かどうか。
	const std::array<int, 3> DRIFT_TIMER = { 30,90,160 };			// レベルごとのドリフトレベルが上がるまでのタイマー
	const std::array<float, 3> DRIFT_BOOST = { 10.0f,20.0f,30.0f };	// レベルごとのドリフトブーストの量

public:

	// 入力の更新処理に必要なデータ 入力用
	struct InputData {
		Vec3 upVec_;
		int canNotMoveTimer_;
		float handleNormal_;
		bool isBeforeStart_;
		bool onGround_;
	};

	struct DriftParticleInData {
		Vec3 forwardVec_;
		int charaIndex_;
		float maxBoostSpeed_;
		bool isGameFinish_;
		bool isBeforeStart_;
		bool onGround_;
		bool onGroundPrev_;
		bool isAccel_;
		bool IsJumpAction_;
	};
	struct DriftParticleOutData {
		int& boostTimer_;
		float& boostSpeed_;
		PlayerModel& playerData_;
		std::weak_ptr<CharacterGameFinish> gameFinish_;
		DriftParticleOutData(int& BoostTimer, float& BoostSpeed, PlayerModel& PlayerData, std::weak_ptr<CharacterGameFinish> GameFinish)
			: boostTimer_(BoostTimer), boostSpeed_(BoostSpeed), playerData_(PlayerData), gameFinish_(GameFinish) {};
	};

public:

	/*===== メンバ関数 =====*/

	CharacterDrift();
	void Init();
	void Input(const InputData& Input, BaseOperationObject::Operation& Operation, std::vector<std::shared_ptr<PlayerTire>>& Tires, PlayerModel& PlayerModel, float& RotY, float& BoostSpeed, Vec3& ForwardVec);
	void Update(std::weak_ptr<CharacterInclineBody> InclineBody, Vec3& Pos, Vec3& PrevPos, bool OnGround);


	// ドリフト時に出るパーティクルの更新処理 Characterの更新処理の最後に持ってくるためにUpdateと分離。
	void UpdateDriftParticle(const DriftParticleInData& InData, DriftParticleOutData& OutData);

	// ゲッタ
	inline bool GetIsDrift() { return isDrift_; }
	inline bool GetIsTireMask() { return isTireMask_; }
	bool GetIsDriftRight() { return isDriftRight_; }
	bool GetIsDriftJump() { return isDriftJump_; }


};