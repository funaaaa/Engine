#include "CharacterDrift.h"
#include "PolygonInstance.h"
#include "TextureManager.h"
#include "BLAS.h"
#include "CharacterInclineBody.h"
#include "DriftParticleMgr.h"
#include "DriftParticle.h"
#include "CharacterGameFinish.h"

CharacterDrift::CharacterDrift()
{

	/*===== コンストラクタ =====*/

	Init();

}

void CharacterDrift::Init()
{

	/*===== 初期化処理 =====*/

	driftJumpVec_ = Vec3();
	driftJumpSpeed_ = 0.0f;
	driftTimer_ = 0;
	turningIndicatorTimer_ = 0;
	beforeStartSmokeTimer_ = 0;
	isDriftRight_ = false;
	isInputLTPrev_ = false;
	isInputLT_ = false;
	isDriftJump_ = false;
	isDriftJumpPrev_ = false;
	isDrift_ = false;
	isTireMask_ = false;
	IsTurningIndicatorRed_ = false;

}

void CharacterDrift::Input(const InputData& Input, BaseOperationObject::Operation& Operation, std::vector<std::shared_ptr<PlayerTire>>& Tires, PlayerModel& PlayerModel, float& RotY, float& BoostSpeed, Vec3& ForwardVec)
{

	/*===== 入力処理 =====*/

	// タイヤのマスクのフラグを初期化する。
	isTireMask_ = false;

	// 右スティックの横の傾き量でキャラを回転させる。
	if ((Operation.handleDriveRate_ != 0 || isDrift_) && !Input.isBeforeStart_) {

		// 回転量の最大値 通常状態とドリフト状態で違う。
		float handleAmount = Input.handleNormal_;

		// [ドリフト状態] [開始前じゃない] だったら回転量を多い方にする。
		if (isDrift_ && !Input.isBeforeStart_) {

			handleAmount = Input.handleNormal_;

			// ドリフトの向きによって回転量を変える。
			if (isDriftRight_ && Operation.handleDriveRate_ < 0.1f) {
				Operation.handleDriveRate_ = 0.1f;
			}
			if (!isDriftRight_ && -0.1f < Operation.handleDriveRate_) {
				Operation.handleDriveRate_ = -0.1f;
			}

		}

		// タイヤ痕を出す。
		if (0.95f < fabs(Operation.handleDriveRate_) || Operation.isDrift_) {
			isTireMask_ = true;
		}

		// タイヤを回転させる。
		for (auto& index : Tires) {

			index->Rot(true, static_cast<float>(Operation.handleDriveRate_));

		}

		// クォータニオンを求める。
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(Vec3(Input.upVec_).ConvertXMVECTOR(), handleAmount * Operation.handleDriveRate_);

		// 求めたクォータニオンを行列に治す。
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// 回転を加算する。
		PlayerModel.carBodyInstance_.lock()->AddRotate(quaternionMat);
		RotY += handleAmount * Operation.handleDriveRate_;

		// 正面ベクトルを車の回転行列分回転させる。
		ForwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PlayerModel.carBodyInstance_.lock()->GetRotate());


		// ウインカーの色を変えるタイマーを更新。
		++turningIndicatorTimer_;
		if (TURNING_INDICATOR_TIMER < turningIndicatorTimer_) {

			turningIndicatorTimer_ = 0;
			IsTurningIndicatorRed_ = IsTurningIndicatorRed_ ? false : true;

		}

		// ウインカーの色を変える。
		if (IsTurningIndicatorRed_) {

			// 曲がっているのが右だったら。
			if (0 < Operation.handleDriveRate_) {

				PlayerModel.carRightLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/blackRed.png"));
				PlayerModel.carLeftLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));

			}
			else {

				PlayerModel.carLeftLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/blackRed.png"));
				PlayerModel.carRightLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));

			}

		}
		else {

			PlayerModel.carRightLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));
			PlayerModel.carLeftLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));

		}

	}
	else {

		// 車のライトの色を元に戻す。
		PlayerModel.carRightLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));
		PlayerModel.carLeftLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));

		// 各変数を初期化。
		IsTurningIndicatorRed_ = false;
		turningIndicatorTimer_ = 100;	// チカチカするタイマーを初期化。

	}

	// ドリフトの入力情報を保存する。
	isInputLTPrev_ = isInputLT_;
	isInputLT_ = Operation.isDrift_;

	// ドリフトジャンプのフラグを保存しておく。
	isDriftJumpPrev_ = isDriftJump_;

	// ドリフトボタンの入力がトリガーだったら。
	bool triggerDriftBottom = !isInputLTPrev_ && isInputLT_;
	bool notJump = !isDriftJump_ && driftJumpSpeed_ <= 0.1f;
	bool isOnGround = Input.onGround_ || Input.isBeforeStart_;	// 設置していたら ゲームが始まっていない場合、キャラは空中に浮いているので、接地判定を取る。
	if (triggerDriftBottom && notJump && isOnGround && Input.canNotMoveTimer_ <= 0) {

		isDriftJump_ = true;
		driftJumpVec_ = Input.upVec_;
		driftJumpSpeed_ = DRIFT_JUMP_SPEED;

	}

	// ドリフト状態でタイマーがカウントされていたら。
	bool releaseDriftBottom = isInputLTPrev_ && !isInputLT_;
	if (isDrift_ && 0 < driftTimer_ && releaseDriftBottom) {

		isDrift_ = false;

		// タイマーに応じて加速させる。
		int driftLevel = -1;
		for (auto& index : DRIFT_TIMER) {

			if (driftTimer_ < index) break;

			driftLevel = static_cast<int>(&index - &DRIFT_TIMER[0]);

		}

		// ドリフトレベルが0以上だったら加速する。
		if (driftLevel != -1) {
			BoostSpeed = DRIFT_BOOST[driftLevel];
		}

	}

	// バグ回避でLTが押されていなかったらドリフトを解除する。 ドリフト解除の正規の処理はこの処理の上にあるので通常は問題ない。
	// 開始前の状態でもドリフトを解除する。
	if (!isInputLT_ || Input.isBeforeStart_) {

		isDrift_ = false;
		driftTimer_ = 0;

	}

}

void CharacterDrift::Update(std::weak_ptr<CharacterInclineBody> InclineBody, Vec3& Pos, Vec3& PrevPos, bool OnGround)
{

	/*===== 更新処理 =====*/

		// ドリフトジャンプしていたら。
	if (isDriftJump_) {

		// 座標を移動させる。
		Pos += driftJumpVec_ * driftJumpSpeed_;
		PrevPos += driftJumpVec_ * driftJumpSpeed_;

		// ドリフト量を減らす。
		driftJumpSpeed_ -= SUB_DRIFT_JUMP_SPEED;
		if (driftJumpSpeed_ < 0.0f) {

			driftJumpSpeed_ = 0.0f;

		}

		// スティック入力があったら。
		if (0.1f < std::fabs(InclineBody.lock()->GetHandleAmount())) {

			isDrift_ = true;
			isDriftRight_ = 0.0f < InclineBody.lock()->GetHandleAmount();

		}



	}

	// ドリフト中だったら。
	if (isDrift_) {

		// ドリフトの経過時間のタイマーを更新。
		++driftTimer_;

	}
	else {

		driftTimer_ = 0;

	}

	// ジャンプしたFでこの処理が通らないようにするための条件
	bool isNotTriggerLT = !(!isInputLTPrev_ && isInputLT_);
	// 設置したらドリフトジャンプを解除する。
	if (isDriftJump_ && OnGround && isNotTriggerLT) {

		isDriftJump_ = false;
		driftJumpSpeed_ = 0.0f;

	}

}

void CharacterDrift::UpdateDriftParticle(const DriftParticleInData& InData, DriftParticleOutData& OutData)
{

	/*===== ドリフトパーティクルの更新処理 =====*/

	// ブーストするタイマーが一定以上だったら加速し続ける。
	if (0 < OutData.boostTimer_) {

		--OutData.boostTimer_;

		OutData.boostSpeed_ = InData.maxBoostSpeed_;

		// 加速の割合を求める。
		float boostRate = OutData.boostSpeed_ / InData.maxBoostSpeed_;

		// 煙の大きさを決める。
		bool IsSmokeBig = 0.5f < boostRate;

		// 設置していたら煙を生成。
		if (InData.onGround_) {
			Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), OutData.playerData_.carBodyInstance_.lock()->GetRotate());
			DriftParticleMgr::Ins()->GenerateSmoke(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, OutData.playerData_.carBodyInstance_.lock()->GetRotate(), IsSmokeBig, DriftParticleMgr::DELAY_ID::DASH);
			driftVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), OutData.playerData_.carBodyInstance_.lock()->GetRotate());
			DriftParticleMgr::Ins()->GenerateSmoke(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, OutData.playerData_.carBodyInstance_.lock()->GetRotate(), IsSmokeBig, DriftParticleMgr::DELAY_ID::DASH);
		}

		// 設置していて移動速度が一定以上だったら炎を生成。
		if (15.0f < OutData.boostSpeed_) {
			DriftParticleMgr::Ins()->GenerateFire(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos(), OutData.playerData_.carBodyInstance_.lock()->GetRotate());
		}

	}

	// 開始前でアクセルを踏んでいたら。
	if (InData.isAccel_ && InData.isBeforeStart_) {

		// 煙を出すタイマーを更新。
		++beforeStartSmokeTimer_;
		if (BEFORE_START_SMOKE_TIMER < beforeStartSmokeTimer_) {

			beforeStartSmokeTimer_ = 0;

			Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), OutData.playerData_.carBodyInstance_.lock()->GetRotate());
			// 左右に散らす。
			Vec3 generatePos = OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * static_cast<float>(FHelper::GetRand(-2, 2));
			// 後ろ方向に持ってくる。
			generatePos += -InData.forwardVec_ * 20.0f;
			DriftParticleMgr::Ins()->GenerateSmoke(generatePos, OutData.playerData_.carBodyInstance_.lock()->GetRotate(), false, DriftParticleMgr::DELAY_ID::NONE_DELAY, -InData.forwardVec_);

		}

	}

	// 着地した瞬間だったら。
	bool isJumpDriftRelease = (isDriftJumpPrev_ && !isDriftJump_);
	bool onGroundTrigger = (!InData.onGroundPrev_ && InData.onGround_);
	if ((isJumpDriftRelease || onGroundTrigger) && !InData.isBeforeStart_ && !OutData.gameFinish_.lock()->GetIsGameFinish()) {

		// 三回ランダムに位置をずらして生成する。
		Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), OutData.playerData_.carBodyInstance_.lock()->GetRotate());
		for (int index = 0; index < 3; ++index) {

			DriftParticleMgr::Ins()->GenerateSmoke(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, OutData.playerData_.carBodyInstance_.lock()->GetRotate(), false, DriftParticleMgr::DELAY_ID::NONE_DELAY);

		}
		// 三回ランダムに位置をずらして生成する。
		for (int index = 0; index < 3; ++index) {
			DriftParticleMgr::Ins()->GenerateSmoke(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() - driftVec * 30.0f, OutData.playerData_.carBodyInstance_.lock()->GetRotate(), false, DriftParticleMgr::DELAY_ID::NONE_DELAY);

		}

	}

	// ドリフト中は煙を出す。
	if (isDrift_ && InData.onGround_) {


		// 現在のレベル。
		int nowLevel = 0;
		for (auto& index : DRIFT_TIMER) {

			if (driftTimer_ < index) continue;

			nowLevel = static_cast<int>(&index - &DRIFT_TIMER[0]);

		}

		// 現在のドリフトレベルが1以上だったらパーティクルとオーラを出す。
		if (1 <= nowLevel) {

			if (!DriftParticleMgr::Ins()->IsAuraGenerated(InData.charaIndex_)) {
				DriftParticleMgr::Ins()->GenerateAura(InData.charaIndex_, OutData.playerData_.carBehindTireInstance_, static_cast<int>(DriftParticle::ID::AURA_BIG), isDriftRight_, 2 <= nowLevel);
				DriftParticleMgr::Ins()->GenerateAura(InData.charaIndex_, OutData.playerData_.carBehindTireInstance_, static_cast<int>(DriftParticle::ID::AURA_SMALL), isDriftRight_, 2 <= nowLevel);
			}

			// レートを求める。
			float rate = 0;
			rate = FHelper::Saturate(static_cast<float>(driftTimer_) / static_cast<float>(DRIFT_TIMER[0]));

			// パーティクルを生成する。
			DriftParticleMgr::Ins()->GenerateDriftParticle(OutData.playerData_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), rate, false, DriftParticleMgr::DELAY_ID::DELAY1);

			// レベルが上った瞬間だったら一気にパーティクルを生成する。
			if (driftTimer_ == DRIFT_TIMER[0] || driftTimer_ == DRIFT_TIMER[1] || driftTimer_ == DRIFT_TIMER[2]) {

				DriftParticleMgr::Ins()->GenerateDriftParticle(OutData.playerData_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1);
				DriftParticleMgr::Ins()->GenerateDriftParticle(OutData.playerData_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1);
				DriftParticleMgr::Ins()->GenerateDriftParticle(OutData.playerData_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1);
				DriftParticleMgr::Ins()->GenerateDriftParticle(OutData.playerData_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1);
				DriftParticleMgr::Ins()->GenerateDriftParticle(OutData.playerData_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1);

				// オーラを一旦破棄
				DriftParticleMgr::Ins()->DestroyAura(InData.charaIndex_);

			}

		}

		// 煙を出す。
		Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(isDriftRight_ ? -1.0f : 1.0f, 0, 0), OutData.playerData_.carBodyInstance_.lock()->GetRotate());
		DriftParticleMgr::Ins()->GenerateSmoke(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, OutData.playerData_.carBodyInstance_.lock()->GetRotate(), nowLevel < 1, DriftParticleMgr::DELAY_ID::DEF);



	}
	else {

		// オーラを破棄 関数内に二重解放対策の条件式がある。
		DriftParticleMgr::Ins()->DestroyAura(InData.charaIndex_);

	}

	// ゲームが終了していたら イージングが終了していなかったら。
	if (InData.isGameFinish_ && OutData.gameFinish_.lock()->GetGameFinishEasingTimer() < 0.9f) {

		// 煙を出す。
		Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(OutData.gameFinish_.lock()->GetIsGameFinishDriftLeft() ? -1.0f : 1.0f, 0, 0), OutData.playerData_.carBodyInstance_.lock()->GetRotate());
		DriftParticleMgr::Ins()->GenerateSmoke(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, OutData.gameFinish_.lock()->GetGameFinishTriggerMatRot(), false, DriftParticleMgr::DELAY_ID::NONE_DELAY);
		DriftParticleMgr::Ins()->GenerateSmoke(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, OutData.gameFinish_.lock()->GetGameFinishTriggerMatRot(), false, DriftParticleMgr::DELAY_ID::NONE_DELAY);
		DriftParticleMgr::Ins()->GenerateSmoke(OutData.gameFinish_.lock()->GetIsGameFinishDriftLeft() ? OutData.playerData_.carLeftTireInstance_.lock()->GetWorldPos() : OutData.playerData_.carRightTireInstance_.lock()->GetWorldPos(), OutData.gameFinish_.lock()->GetGameFinishTriggerMatRot(), false, DriftParticleMgr::DELAY_ID::NONE_DELAY);


	}

	// ジャンプアクションのパーティクルを生成。
	if (InData.IsJumpAction_) {

		DriftParticleMgr::Ins()->GenerateJumpEffect(OutData.playerData_.carBodyInstance_);

	}

}
