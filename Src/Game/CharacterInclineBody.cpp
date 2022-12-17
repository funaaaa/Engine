#include "CharacterInclineBody.h"
#include "FHelper.h"
#include "Character.h"
#include "PolygonInstance.h"

CharacterInclineBody::CharacterInclineBody()
{

	/*===== コンストラクタ =====*/

	boostRotQ_ = DirectX::XMVECTOR();
	handleRotQ_ = DirectX::XMVECTOR();
	nowBoostRotQ_ = DirectX::XMVECTOR();
	nowHandleRotQ_ = DirectX::XMVECTOR();
	handleAmount_ = 0;
	baseHandleAmount_ = 0;
	tireLollingAmount_ = 0;
	isRotRightSide_ = false;
	nowDriftRot_ = 0;
	baseDriftRot_ = 0;
	driftRotTimer_ = 0;
	baseBoostRot_ = 0;
	nowBoostRot_ = 0;
	nowFrameHandleAmount_ = 0;

}

void CharacterInclineBody::Init()
{

	/*===== 初期化処理 =====*/

	boostRotQ_ = DirectX::XMVECTOR();
	handleRotQ_ = DirectX::XMVECTOR();
	nowBoostRotQ_ = DirectX::XMVECTOR();
	nowHandleRotQ_ = DirectX::XMVECTOR();
	handleAmount_ = 0;
	baseHandleAmount_ = 0;
	tireLollingAmount_ = 0;
	isRotRightSide_ = false;
	nowDriftRot_ = 0;
	baseDriftRot_ = 0;
	driftRotTimer_ = 0;
	baseBoostRot_ = 0;
	nowBoostRot_ = 0;
	nowFrameHandleAmount_ = 0;

}

void CharacterInclineBody::Update(const InputData& Input)
{

	/*===== 更新処理 =====*/

	// ハンドルの量を補間する。
	handleAmount_ += (baseHandleAmount_ - handleAmount_) / 30.0f;

	// 空中に居たら。
	if (!Input.onGround_ && !Input.isDriftJump_) {

		// 移動した方向ベクトル
		Vec3 movedVec = (Input.pos_ - Input.prevPos_).GetNormal();

		if (!std::isnan(movedVec.x_) && Input.isPlayer_) {

			// デフォルトの上ベクトル。
			Vec3 defUpVec = Vec3(0, -1, 0);

			// 回転軸を求める。
			Vec3 axisOfRevolution = defUpVec.Cross(movedVec);

			// 回転軸を正規化する。
			if (axisOfRevolution.Length() != 0) {
				axisOfRevolution.Normalize();
			}

			/*-- 内積から回転量を取得 --*/

			// 回転量を求める。
			float amountOfRotation = defUpVec.Dot(movedVec);

			// 逆余弦を求める関数を使用して求めたcosθをラジアンに変換。
			amountOfRotation = acosf(amountOfRotation) - DirectX::XM_PIDIV2;


			/*-- クォータニオンを使って回転 --*/

			// 回転軸が{0,0,0}だったら処理を飛ばす。
			if (axisOfRevolution.Length() != 0 && amountOfRotation != 0) {

				// クォータニオンを求める。
				DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(axisOfRevolution.ConvertXMVECTOR(), amountOfRotation);

				// 求めたクォータニオンを行列に治す。
				DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

				// プレイヤーを回転させる。
				Input.playerModel_.carBodyInstance_.lock()->ChangeRotate(quaternionMat);

				// 上ベクトルを基準としたクォータニオンを求める。
				Vec3 normal_ = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), Input.playerModel_.carBodyInstance_.lock()->GetRotate());
				DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), Input.rotY_);

				// クォータニオンを行列に治す。
				DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

				// プレイヤーを回転させる。
				Input.playerModel_.carBodyInstance_.lock()->AddRotate(upQuaternionMat);

			}
		}

	}

	// ブースト時の回転。
	if (0 < Input.boostSpeed_) {

		float boostRate = Input.boostSpeed_ / Input.maxBoostSpeed_;
		const float MAX_ROT = 0.1f;

		baseBoostRot_ = MAX_ROT * boostRate;

		// バランスを取るタイマーを更新。
		++forwardTireLollingTimer_;
		if (FORWARD_TIMER_LOLLING_TIMER < forwardTireLollingTimer_) {

			forwardTireLollingTimer_ = 0;

			// バランスを取っている風にするためにランダムで回転量を変更する。
			const float MAX_RANDOM = 0.4f;
			baseBoostRot_ += FHelper::GetRand(-MAX_RANDOM * boostRate, MAX_RANDOM * boostRate);

		}

	}
	else {

		boostRotQ_ = DirectX::XMVECTOR();
		forwardTireLollingTimer_ = 0;
		baseBoostRot_ = 0;

	}

	{

		nowBoostRot_ += (baseBoostRot_ - nowBoostRot_) / 5.0f;

		Vec3 horiVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), Input.playerModel_.carBodyInstance_.lock()->GetRotate());

		// クォータニオンを求める。
		boostRotQ_ = DirectX::XMQuaternionRotationAxis(horiVec.ConvertXMVECTOR(), nowBoostRot_);

		// ブーストの車体のクォータニオンを補間。
		nowBoostRotQ_ = DirectX::XMQuaternionSlerp(nowBoostRotQ_, boostRotQ_, 0.2f);
		// 回転行列を求める。
		DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(nowBoostRotQ_);
		Input.playerModel_.carBodyInstance_.lock()->AddRotate(mat);

	}


	// ドリフト時の回転。
	if (Input.isDrift_ && 0 != handleAmount_) {

		// 前フレームと回転方向が違かったら初期化する。
		if (isRotRightSide_ && handleAmount_ < 0) {
			handleRotQ_ = DirectX::XMVECTOR();
			baseDriftRot_ = 0;
			driftRotTimer_ = 0;
		}

		++driftRotTimer_;
		if (MAX_DRIFT_ROT_TIMER < driftRotTimer_) driftRotTimer_ = MAX_DRIFT_ROT_TIMER;

		float timerRate = static_cast<float>(driftRotTimer_) / static_cast<float>(MAX_DRIFT_ROT_TIMER);

		// 回転量にイージングをかける。 マジックナンバーは傾きの微調整用。
		if (0.35f < fabs(handleAmount_)) {
			baseDriftRot_ = 0.6f * (0 < handleAmount_ ? 1.0f : -1.0f);
		}
		else {
			baseDriftRot_ = std::fabs(FEasing::EaseOutCubic(timerRate) * MAX_DRIFT_ROT) * handleAmount_;
		}

		// 回転方向を保存。
		isRotRightSide_ = 0 < handleAmount_;

		// バランスを取っている風にするためにランダムで回転量を変更する。
		const float MAX_RANDOM = 0.1f;
		tireLollingAmount_ = FHelper::GetRand(-MAX_RANDOM * timerRate, MAX_RANDOM * timerRate);
		baseDriftRot_ += tireLollingAmount_;

	}
	else {

		handleRotQ_ = DirectX::XMVECTOR();

		baseDriftRot_ = 0;
		driftRotTimer_ = 0;

	}

	// 回転させる。
	{
		nowDriftRot_ += (baseDriftRot_ - nowDriftRot_) / 10.0f;

		Vec3 horiVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), Input.playerModel_.carBodyInstance_.lock()->GetRotate());
		Vec3 forwardVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), Input.playerModel_.carBodyInstance_.lock()->GetRotate());
		Vec3 upVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), Input.playerModel_.carBodyInstance_.lock()->GetRotate());

		// 横クォータニオンを求める。
		handleRotQ_ = DirectX::XMQuaternionRotationAxis(horiVec.ConvertXMVECTOR(), nowDriftRot_);
		// 正面クォータニオンを求める。 横だけだと回転がちょっとだけずれているため。
		DirectX::XMVECTOR forwardQ = DirectX::XMQuaternionRotationAxis(forwardVec.ConvertXMVECTOR(), 0.01f);
		// 上クォータニオンを求める。
		DirectX::XMVECTOR upQ = DirectX::XMQuaternionRotationAxis(upVec.ConvertXMVECTOR(), -0.2f * handleAmount_);

		// クォータニオンをかける。
		handleRotQ_ = DirectX::XMQuaternionMultiply(handleRotQ_, forwardQ);
		handleRotQ_ = DirectX::XMQuaternionMultiply(handleRotQ_, upQ);
	}

	// ブーストの車体のクォータニオンを補間。
	nowHandleRotQ_ = DirectX::XMQuaternionSlerp(nowHandleRotQ_, handleRotQ_, 0.2f);
	// 回転行列を求める。
	DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(nowHandleRotQ_);
	Input.playerModel_.carBodyInstance_.lock()->AddRotate(mat);

}

void CharacterInclineBody::InitQuaternion()
{

	/*===== 車体の回転に使用しているクォータニオンを初期化する =====*/

	boostRotQ_ = DirectX::XMVECTOR();
	handleRotQ_ = DirectX::XMVECTOR();
	nowBoostRotQ_ = DirectX::XMVECTOR();
	nowHandleRotQ_ = DirectX::XMVECTOR();

}
