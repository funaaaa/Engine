#include "Player.h"
#include "BLASRegister.h"
#include "PorygonInstanceRegister.h"
#include "HitGroupMgr.h"
#include "Input.h"
#include "FHelper.h"
#include "BLAS.h"

void Player::Init(const int& StageBlasIndex, const int& StageInstanceIndex)
{

	/*===== 初期化処理 =====*/

	int carBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/", "car.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" });
	carInstanceIndex = PorygonInstanceRegister::Ins()->CreateInstance(carBlasIndex, PorygonInstanceRegister::SHADER_ID_COMPLETE_REFLECTION);
	PorygonInstanceRegister::Ins()->AddScale(carInstanceIndex, Vec3(10, 10, 10));

	stageBlasIndex = StageBlasIndex;
	stageInstanceIndex = StageInstanceIndex;

	pos = Vec3(0, 30, 0);
	forwardVec = Vec3(0, 0, 1);
	bottomVec = Vec3(0, -1, 0);
	size = Vec3(10, 10, 10);
	speed = 0;
	gravity = 0;
	boostSpeed = 0;
	isDrift = false;
	isGround = true;

}

void Player::Update()
{

	/*===== 更新処理 =====*/

	// 入力処理
	Input();

	// 移動処理
	Move();

	// 当たり判定
	CheckHit();

	// 座標を更新。
	PorygonInstanceRegister::Ins()->ChangeTrans(carInstanceIndex, pos);

}

void Player::Draw()
{

	/*===== 描画処理 =====*/

}

Vec3 Player::GetUpVec()
{

	/*===== 上ベクトルを取得 =====*/

	// 回転行列を取得。
	DirectX::XMMATRIX matRot = PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex);

	// デフォルトの上ベクトル。
	Vec3 defUpVec = Vec3(0, 1, 0);

	// デフォルトの上ベクトルに回転行列をかけた値を返す。
	return FHelper::MulRotationMatNormal(defUpVec, matRot);

}

float Player::GetNowSpeedPer()
{

	/*====== 移動速度の割合を計算 =====*/

	float per = (speed + boostSpeed) / (MAX_SPEED + MAX_BOOST_SPEED);

	return per;

}

void Player::Input()
{

	/*===== 入力処理 =====*/

	// RTが引かれていたら加速。
	const float INPUT_DEADLINE_TRI = 0.5f;
	float inputRightTriValue = Input::Ins()->isPadTri(XINPUT_TRIGGER_RIGHT);
	if (INPUT_DEADLINE_TRI < inputRightTriValue) {

		speed += inputRightTriValue * ADD_SPEED;

	}
	else {

		// 移動していなかったら移動量を0に近づける。
		speed -= speed / 10.0f;

	}

	// LTが引かれていたらドリフト状態にする。
	const float INPUT_DEADLINE_DRIFT = 0.9f;
	float inputLeftTriValue = Input::Ins()->isPadTri(XINPUT_TRIGGER_LEFT);
	if (INPUT_DEADLINE_DRIFT < inputLeftTriValue) {

		isDrift = true;

	}
	else {

		// ドリストのブーストするまでのタイマーが規定値以上だったらブーストする。
		if (DRIFT_BOOST_TIMER <= driftBoostTimer) {

			boostSpeed = MAX_BOOST_SPEED;

		}

		// ドリフトのブーストするまでのタイマーを初期化する。
		driftBoostTimer = 0;

		isDrift = false;

	}

	// 右スティックの横の傾き量でキャラを回転させる。
	float inputLeftStickHori = Input::Ins()->isPadThumb(XINPUT_THUMB_LEFTSIDE);
	if (0.2f < std::fabs(inputLeftStickHori)) {

		// 回転量 通常状態とドリフト状態で違う。
		float handleAmount = HANDLE_NORMAL;

		// ドリフト状態だったら回転量を多い方にする。
		if (isDrift) {

			handleAmount = HANDLE_DRIFT;

			// ついでにドリフト状態の時のブーストするまでのタイマーを更新する。
			++driftBoostTimer;
			if (DRIFT_BOOST_TIMER < driftBoostTimer) driftBoostTimer = DRIFT_BOOST_TIMER;

		}

		// 回転を加算する。
		PorygonInstanceRegister::Ins()->AddRotate(carInstanceIndex, Vec3(0, handleAmount * inputLeftStickHori, 0));

		// 正面ベクトルを車の回転行列分回転させる。
		forwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex));

	}

	// デバッグ用 Bボタンが押されたら初期位置に戻す。
	if (Input::Ins()->isPad(XINPUT_GAMEPAD_B)) {

		pos = Vec3(0, 30, 0);

	}

}

void Player::Move()
{

	/*===== 移動処理 =====*/

	// 移動速度が限界を超えないようにする。
	if (MAX_SPEED < speed) {

		speed = MAX_SPEED;

	}

	// 座標移動させる。
	pos += forwardVec * (speed + boostSpeed);

	// ドリフト時のブースト移動量を0に近づける。
	if (0 < boostSpeed) {

		boostSpeed -= SUB_BOOST_SPEED;

	}
	else {

		boostSpeed = 0;

	}

	// 地上にいたら重力を無効化する。
	if (isGround) {

		gravity = 0;

	}
	// 空中にいたら重力を加算する。
	else {

		gravity += ADD_GRAV;

		// 重力量が限界を超えないようにする。
		if (MAX_GRAV < gravity) {

			gravity = MAX_GRAV;

		}

	}

	// 座標に重力を加算する。
	pos += Vec3(0, -1, 0) * gravity;

	// 下ベクトルを車の回転行列分回転させる。
	bottomVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex));

}

void Player::CheckHit()
{

	/*===== 当たり判定 =====*/

	{

		/*-- ステージとの当たり判定 --*/

		// 当たり判定に使用するデータ
		FHelper::RayToModelCollisionData collistionData;

		// 当たり判定に必要なデータを埋めていく。
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageBlasIndex]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageBlasIndex]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageBlasIndex]->GetVertexIndex();
		collistionData.rayPos = pos;
		collistionData.rayDir = bottomVec;
		collistionData.matTrans = PorygonInstanceRegister::Ins()->GetTrans(stageInstanceIndex);
		collistionData.matScale = PorygonInstanceRegister::Ins()->GetScale(stageInstanceIndex);
		collistionData.matRot = PorygonInstanceRegister::Ins()->GetRotate(stageInstanceIndex);

		// 当たり判定の結果保存用変数。
		bool isHit = false;
		Vec3 impactPos;
		float hitDistance;
		Vec3 hitNormal;

		// 当たり判定を行う。
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// 当たった距離がY軸のサイズよりも小さかったら。
		isHit &= (hitDistance - size.y) < 0;
		isHit &= 0 < hitDistance;

		// 当たっていたら押し戻す。
		if (isHit) {

			// 法線方向に当たった分押し戻す。
			pos += hitNormal * (size.y - hitDistance);

			// 地上にいる判定。
			isGround = true;

		}
		else {

			// 空中にいる判定。
			isGround = false;

		}

	}

}
