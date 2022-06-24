#include "Player.h"
#include "BLASRegister.h"
#include "PorygonInstanceRegister.h"
#include "HitGroupMgr.h"
#include "Input.h"
#include "FHelper.h"
#include "BLAS.h"
#include "DriftParticleMgr.h"

void Player::Init(const int& StageBlasIndex, const int& StageInstanceIndex)
{

	/*===== 初期化処理 =====*/

	carBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/", "car.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }, true);
	carInstanceIndex = PorygonInstanceRegister::Ins()->CreateInstance(carBlasIndex, PorygonInstanceRegister::SHADER_ID_REFLECTION);
	PorygonInstanceRegister::Ins()->AddScale(carInstanceIndex, Vec3(10, 10, 10));

	stageBlasIndex = StageBlasIndex;
	stageInstanceIndex = StageInstanceIndex;

	pos = Vec3(0, 30, 0);
	forwardVec = Vec3(0, 0, 1);
	bottomVec = Vec3(0, -1, 0);
	upVec = Vec3(0, 1, 0);
	size = Vec3(10, 10, 10);
	rotY = 0;
	speed = 0;
	gravity = 0;
	boostSpeed = 0;
	isDrift = false;
	isGround = true;

}

void Player::Update(RayConstBufferData& ConstBufferData)
{

	/*===== 更新処理 =====*/

	// 入力処理
	Input(ConstBufferData);

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

float Player::GetNowSpeedPer()
{

	/*====== 移動速度の割合を計算 =====*/

	float per = (speed + boostSpeed) / (MAX_SPEED + MAX_BOOST_SPEED);

	return per;

}

void Player::Input(RayConstBufferData& ConstBufferData)
{

	/*===== 入力処理 =====*/

	// RTが引かれていたら加速。
	const float INPUT_DEADLINE_TRI = 0.5f;
	float inputRightTriValue = Input::Ins()->isPadTri(XINPUT_TRIGGER_RIGHT);
	if (INPUT_DEADLINE_TRI < inputRightTriValue && isGround) {

		speed += inputRightTriValue * ADD_SPEED;

	}
	else if (isGround) {

		// 移動していなくて地上にいたら移動量を0に近づける。
		speed -= speed / 10.0f;

	}
	else {

		// 移動していなくて空中にいたら移動量を0に近づける。
		speed -= speed / 200.0f;

	}

	// 現在のフレームの右スティックの傾き具合。
	float nowFrameInputLeftStickHori = 0;

	// 右スティックの横の傾き量でキャラを回転させる。
	float inputLeftStickHori = Input::Ins()->isPadThumb(XINPUT_THUMB_LEFTSIDE);
	const float LEFT_STICK_INPUT_DEADLINE = 0.2f;
	if (LEFT_STICK_INPUT_DEADLINE < std::fabs(inputLeftStickHori)) {

		// 回転量 通常状態とドリフト状態で違う。
		float handleAmount = HANDLE_NORMAL;

		// ドリフト状態だったら回転量を多い方にする。
		if (isDrift) {

			handleAmount = HANDLE_DRIFT;

			// ついでにドリフト状態の時のブーストするまでのタイマーを更新する。
			++driftBoostTimer;
			if (DRIFT_BOOST_TIMER < driftBoostTimer) driftBoostTimer = DRIFT_BOOST_TIMER;

		}

		// クォータニオンを求める。
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(upVec.ConvertXMVECTOR(), handleAmount * inputLeftStickHori);

		// 求めたクォータニオンを行列に治す。
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// 回転を加算する。
		PorygonInstanceRegister::Ins()->AddRotate(carInstanceIndex, quaternionMat);
		rotY += handleAmount * inputLeftStickHori;
		nowFrameInputLeftStickHori = inputLeftStickHori;

		// 正面ベクトルを車の回転行列分回転させる。
		forwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex));

	}

	// LTが引かれていたらドリフト状態にする。
	const float INPUT_DEADLINE_DRIFT = 0.9f;
	float inputLeftTriValue = Input::Ins()->isPadTri(XINPUT_TRIGGER_LEFT);
	bool isInputNowFrameLeftStrick = LEFT_STICK_INPUT_DEADLINE < fabs(nowFrameInputLeftStickHori);
	if (INPUT_DEADLINE_DRIFT < inputLeftTriValue && isInputNowFrameLeftStrick) {

		isDrift = true;

		// ドリフトのベクトルを求める。
		Vec3 driftVec = Vec3();
		if (nowFrameInputLeftStickHori < 0) {

			driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex));

		}
		else {

			driftVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex));

		}

		// ドリフト時のパーティクルを生成。
		//DriftParticleMgr::Ins()->Generate(pos, driftVec, PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex), ConstBufferData);

	}
	// すでにドリフト中だったら勝手に解除しないようにする。
	else if (INPUT_DEADLINE_DRIFT < inputLeftTriValue && isDrift) {
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

			// ぴったり押し戻すと次のフレームで空中判定になってしまうので、若干オフセットを設ける。
			const float PUSH_BACK_OFFSET = 1.0f;

			// 法線方向に当たった分押し戻す。
			pos += hitNormal * (size.y - (hitDistance + PUSH_BACK_OFFSET));

			// 地上にいる判定。
			isGround = true;

			// 斜め床の回転処理。
			RotObliqueFloor(hitNormal);

		}
		else {

			// 空中にいる判定。
			isGround = false;

		}

		// 正面方向の当たり判定を行うため、レイの飛ばす方向を変える。
		collistionData.rayDir = forwardVec;

		// 当たり判定を行う。
		isHit = false;
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// 当たった距離がY軸のサイズよりも小さかったら。
		isHit &= (hitDistance - size.y) < 0;
		isHit &= 0 < hitDistance;

		// 当たっていたら押し戻す。
		if (isHit) {

			// 法線方向に当たった分押し戻す。
			pos += hitNormal * (size.x - hitDistance);

		}

	}

}

void Player::RotObliqueFloor(const Vec3& HitNormal)
{

	/*===== 斜め床の回転処理 =====*/

	/*-- 外積から回転軸を取得 --*/

	// デフォルトの上ベクトル。
	Vec3 defUpVec = Vec3(0, 1, 0);

	// 回転軸を求める。
	Vec3 axisOfRevolution = defUpVec.Cross(HitNormal);

	// 回転軸を正規化する。
	if (axisOfRevolution.Length() != 0) {
		axisOfRevolution.Normalize();
	}

	/*-- 内積から回転量を取得 --*/

	// 回転量を求める。
	float amountOfRotation = defUpVec.Dot(HitNormal);

	// 逆余弦を求める関数を使用して求めたcosθをラジアンに変換。
	amountOfRotation = acosf(amountOfRotation);


	/*-- クォータニオンを使って回転 --*/

	// 回転軸が{0,0,0}だったら処理を飛ばす。
	if (axisOfRevolution.Length() != 0 && amountOfRotation != 0) {

		// クォータニオンを求める。
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(axisOfRevolution.ConvertXMVECTOR(), amountOfRotation);

		// 求めたクォータニオンを行列に治す。
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// プレイヤーを回転させる。
		PorygonInstanceRegister::Ins()->ChangeRotate(carInstanceIndex, quaternionMat);

		// 上ベクトルを基準としたクォータニオンを求める。
		Vec3 normal = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal.ConvertXMVECTOR(), rotY);

		// クォータニオンを行列に治す。
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// プレイヤーを回転させる。
		PorygonInstanceRegister::Ins()->AddRotate(carInstanceIndex, upQuaternionMat);


		/*-- プレイヤーの回転行列をもとに各ベクトルを回転 --*/

		// 回転行列を取得。
		DirectX::XMMATRIX rotationMatBuff = PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex);

		// 上ベクトルを更新。
		upVec = normal;

		//正面ベクトルを更新。
		forwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), rotationMatBuff);

	}

	// 当たった法線が上だった場合は行列を初期化して回転させる。
	if (HitNormal == Vec3(0, 1, 0)) {

		// プレイヤーを回転させる。
		PorygonInstanceRegister::Ins()->ChangeRotate(carInstanceIndex, DirectX::XMMatrixIdentity());

		// 上ベクトルを基準としたクォータニオンを求める。
		Vec3 normal = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal.ConvertXMVECTOR(), rotY);

		// クォータニオンを行列に治す。
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// プレイヤーを回転させる。
		PorygonInstanceRegister::Ins()->AddRotate(carInstanceIndex, upQuaternionMat);


		/*-- プレイヤーの回転行列をもとに各ベクトルを回転 --*/

		// 回転行列を取得。
		DirectX::XMMATRIX rotationMatBuff = PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex);

		// 上ベクトルを更新。
		upVec = normal;

		//正面ベクトルを更新。
		forwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), rotationMatBuff);

	}

}
