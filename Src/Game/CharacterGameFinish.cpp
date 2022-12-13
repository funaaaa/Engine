#include "CharacterGameFinish.h"
#include "FHelper.h"
#include "PolygonInstance.h"
#include "CharacterInclineBody.h"

CharacterGameFinish::CharacterGameFinish()
{

	/*===== コンストラクタ =====*/

	Init();

}

void CharacterGameFinish::Init()
{

	/*===== 初期化処理 =====*/

	isPrevGameFinish_ = false;
	isGameFinish_ = false;
	gameFinishTriggerMatRot_ = DirectX::XMMatrixIdentity();
	gameFinishTriggerRotY_ = 0;
	gameFinishEasingTimer_ = 0;
	gameFinishRotStopAmount_ = 0;
	gameFinishRotStopReturnAmount_ = 0;
	isGameFinish_ = false;
	isPrevGameFinish_ = false;
	isGameFinishDriftLeft_ = false;

}

void CharacterGameFinish::Update(const UpdateInputData& Input, int& BoostTimer, float& Speed, float& RotY, Vec3& ForwardVec)
{

	/*===== ゲーム終了時のタイマー等の基本的な更新処理 =====*/

	// ゲームが終わっていなかったら処理を飛ばす。
	if (!isGameFinish_) return;

	// ゲーム終了トリガーの場合。
	bool isFinishTrigger = !isPrevGameFinish_ && isGameFinish_;
	if (isFinishTrigger) {

		// 各変数を設定。
		BoostTimer = 0;
		Speed = Input.maxSpeed_;
		gameFinishTriggerRotY_ = RotY;
		gameFinishEasingTimer_ = 0;
		gameFinishTruggerForardVec_ = ForwardVec;
		gameFinishTriggerMatRot_ = Input.playerModel_.carBodyInstance_.lock()->GetRotate();

		// 演出でどちらにドリフトさせるかを取得。
		isGameFinishDriftLeft_ = Input.inclineBody_.lock()->GetHandleAmount() < 0;
		if (Input.inclineBody_.lock()->GetHandleAmount() == 0) {

			isGameFinishDriftLeft_ = true;

		}

		return;

	}


	/*-- ゲーム終了時の更新処理本編 --*/

	// 正面ベクトルを更新する。
	ForwardVec = gameFinishTruggerForardVec_;

	// ゲーム終了時のイージングタイマーを更新してオーバーフローしないようにする。
	gameFinishEasingTimer_ = FHelper::Saturate(gameFinishEasingTimer_ + GAME_FINISH_EASING_TIMER);

	// イージング量を求める。
	float easingAmount = FEasing::EaseOutQuint(gameFinishEasingTimer_);

	// 移動量を更新する。
	Speed = Input.maxSpeed_ * (1.0f - easingAmount);

	// 回転を更新する。
	const float ADD_ROT_Y = DirectX::XM_PIDIV2 * (isGameFinishDriftLeft_ ? -1.0f : 1.0f);
	RotY = gameFinishTriggerRotY_;
	RotY += ADD_ROT_Y * easingAmount;

}

void CharacterGameFinish::AKIRAERotation(PlayerModel& Model)
{

	/*===== ゲーム終了時のAKIRA処理を風演出の更新処理 =====*/

	if (isGameFinish_) {

		// イージングが終わったら。
		if (0.7f <= gameFinishEasingTimer_) {

			// 回転軸を求める。
			Vec3 axisOfRevolution = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), Model.carBodyInstance_.lock()->GetRotate());

			// クォータニオンを求める。
			DirectX::XMVECTOR gameFinishRotQ = DirectX::XMQuaternionRotationAxis(axisOfRevolution.ConvertXMVECTOR(), gameFinishRotStopAmount_);

			// クォータニオンから回転行列を求める。
			DirectX::XMMATRIX gameFinishRotMat = DirectX::XMMatrixRotationQuaternion(gameFinishRotQ);

			// 回転させる。
			Model.carBodyInstance_.lock()->AddRotate(gameFinishRotMat);

			// 回転量を減らす。
			if (isGameFinishDriftLeft_) {

				gameFinishRotStopReturnAmount_ -= -0.02f;
				gameFinishRotStopAmount_ += gameFinishRotStopReturnAmount_;
				if (0 <= gameFinishRotStopAmount_) {

					gameFinishRotStopAmount_ = 0.0f;

				}

			}
			else {

				gameFinishRotStopReturnAmount_ -= 0.02f;
				gameFinishRotStopAmount_ += gameFinishRotStopReturnAmount_;
				if (gameFinishRotStopAmount_ <= 0) {

					gameFinishRotStopAmount_ = 0.0f;

				}

			}

		}
		else {

			// イージング量を求める。
			float easingAmount = FEasing::EaseInSine(gameFinishEasingTimer_) * (isGameFinishDriftLeft_ ? -1.0f : 1.0f);

			if (GAME_FINISH_STOP_ROT_LIMIT < fabs(easingAmount)) easingAmount = GAME_FINISH_STOP_ROT_LIMIT * (easingAmount < 0 ? -1.0f : 1.0f);

			// 回転軸を求める。
			Vec3 axisOfRevolution = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), Model.carBodyInstance_.lock()->GetRotate());

			// クォータニオンを求める。
			DirectX::XMVECTOR gameFinishRotQ = DirectX::XMQuaternionRotationAxis(axisOfRevolution.ConvertXMVECTOR(), easingAmount);

			// クォータニオンから回転行列を求める。
			DirectX::XMMATRIX gameFinishRotMat = DirectX::XMMatrixRotationQuaternion(gameFinishRotQ);

			// 回転させる。
			Model.carBodyInstance_.lock()->AddRotate(gameFinishRotMat);

			gameFinishRotStopAmount_ = easingAmount;
			gameFinishRotStopReturnAmount_ = 0;

		}

	}

}

void CharacterGameFinish::UpdateGameFinishFlag(bool IsGameFinish)
{

	/*===== ゲーム終了時のフラグを更新 =====*/

	isPrevGameFinish_ = isGameFinish_;
	isGameFinish_ = IsGameFinish;

}
