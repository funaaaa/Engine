#include "Player.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "Input.h"
#include "FHelper.h"
#include "BLAS.h"
#include "DriftParticleMgr.h"
#include "Camera.h"
#include "HitGroupMgr.h"
#include "GimmickMgr.h"

Player::Player(const StageData& StageObjectData)
{

	/*===== 初期化処理 =====*/

	// 車のボディーをロード
	carBodyBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBody.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" }, true);
	carBodyInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carBodyBlasIndex, PolygonInstanceRegister::SHADER_ID::REFLECTION);
	PolygonInstanceRegister::Ins()->AddScale(carBodyInsIndex, Vec3(12, 12, 12));

	// 車のタイヤをロード
	carTireBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carTire.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carTireInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carTireBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carTireInsIndex, carBodyInsIndex);

	// 車の鏡をロード
	carMirrorBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carMirror.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carMirrorInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlasIndex, PolygonInstanceRegister::SHADER_ID::COMPLETE_REFLECTION);
	PolygonInstanceRegister::Ins()->SetParentInstance(carMirrorInsIndex, carBodyInsIndex);

	// 車の鏡のカバーをロード
	carMirrorCoverBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carMirrorCover.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carMirrorCoverInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlasIndex, PolygonInstanceRegister::SHADER_ID::COMPLETE_REFLECTION);
	PolygonInstanceRegister::Ins()->SetParentInstance(carMirrorCoverInsIndex, carBodyInsIndex);

	// 車のウィンドウをロード
	carWindowBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carWindow.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/windowColor.png" }, true);
	carWindowInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carWindowBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carWindowInsIndex, carBodyInsIndex);


	stageModelData = StageObjectData;

	pos = PLAYER_DEF_POS;
	prevPos = pos;
	forwardVec = Vec3(0, 0, -1);
	bottomVec = Vec3(0, -1, 0);
	upVec = Vec3(0, 1, 0);
	size = Vec3(15, 15, 15);
	rotY = 0;
	speed = 0;
	gravity = 0;
	boostSpeed = 0;
	returnDefPosTimer = 0;
	isDrift = false;
	isGround = true;
	isGrass = false;

	// OBBを生成。
	obb.Setting(carBodyBlasIndex, carBodyInsIndex);

}

void Player::Init()
{

	/*===== 初期化処理 =====*/

	pos = PLAYER_DEF_POS;
	prevPos = pos;
	forwardVec = Vec3(0, 0, -1);
	bottomVec = Vec3(0, -1, 0);
	upVec = Vec3(0, 1, 0);
	size = Vec3(15, 15, 15);
	returnDefPosTimer = 0;
	rotY = 0;
	speed = 0;
	gravity = 0;
	boostSpeed = 0;
	isDrift = false;
	isGround = true;
	isGrass = false;
	PolygonInstanceRegister::Ins()->ChangeRotate(carBodyInsIndex, Vec3(0, 0, 0));

}

void Player::Update(RayConstBufferData& ConstBufferData, bool& IsPassedMiddlePoint, int& RapCount)
{

	/*===== 更新処理 =====*/

	// 入力処理
	Input(ConstBufferData);

	// 移動処理
	Move();

	// 当たり判定
	CheckHit(IsPassedMiddlePoint, RapCount);

	// 座標を更新。
	PolygonInstanceRegister::Ins()->ChangeTrans(carBodyInsIndex, pos);

	// 座標を保存。
	prevPos = pos;

	// 空中にいるときは初期地点まで戻るタイマーを更新。地上に要るときはタイマーを初期化。
	if (isGround) {

		returnDefPosTimer = 0;

	}
	else {

		++returnDefPosTimer;

		if (RETURN_DEFPOS_TIMER < returnDefPosTimer) {

			pos = PLAYER_DEF_POS;
			PolygonInstanceRegister::Ins()->ChangeTrans(carBodyInsIndex, Vec3(0, 0, 0));
			PolygonInstanceRegister::Ins()->ChangeRotate(carBodyInsIndex, Vec3(0, 0, 0));
			forwardVec = Vec3(0, 0, -1);
			rotY = 0;
			upVec = Vec3(0, 1, 0);
			returnDefPosTimer = 0;

		}

	}

	// OBBを更新。
	obb.SetMat(carBodyInsIndex);

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
	float inputRightTriValue = Input::Ins()->PadTrigger(XINPUT_TRIGGER_RIGHT);
	if ((INPUT_DEADLINE_TRI < inputRightTriValue) && isGround) {

		speed += inputRightTriValue * ADD_SPEED;

	}
	else if (Input::Ins()->IsKey(DIK_W) && isGround) {

		speed += ADD_SPEED;

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
	float inputLeftStickHori = Input::Ins()->PadStick(XINPUT_THUMB_LEFTSIDE);
	const float LEFT_STICK_INPUT_DEADLINE = 0.2f;
	int inputADKey = Input::Ins()->IsKey(DIK_D) - Input::Ins()->IsKey(DIK_A);
	if (LEFT_STICK_INPUT_DEADLINE < std::fabs(inputLeftStickHori) || inputADKey != 0) {

		// 回転量 通常状態とドリフト状態で違う。
		float handleAmount = HANDLE_NORMAL;

		// ドリフト状態だったら回転量を多い方にする。
		if (isDrift) {

			handleAmount = HANDLE_DRIFT;

			// ついでにドリフト状態の時のブーストするまでのタイマーを更新する。
			if (isGround) {
				++driftBoostTimer;
				if (DRIFT_BOOST_TIMER < driftBoostTimer) driftBoostTimer = DRIFT_BOOST_TIMER;
			}

		}

		// クォータニオンを求める。
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(upVec.ConvertXMVECTOR(), handleAmount * inputLeftStickHori + inputADKey * handleAmount);

		// 求めたクォータニオンを行列に治す。
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// 回転を加算する。
		PolygonInstanceRegister::Ins()->AddRotate(carBodyInsIndex, quaternionMat);
		rotY += handleAmount * inputLeftStickHori + inputADKey * handleAmount;
		nowFrameInputLeftStickHori = inputLeftStickHori + inputADKey * handleAmount;

		// 正面ベクトルを車の回転行列分回転させる。
		forwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(carBodyInsIndex));

	}

	// LTが引かれていたらドリフト状態にする。
	const float INPUT_DEADLINE_DRIFT = 0.9f;
	float inputLeftTriValue = Input::Ins()->PadTrigger(XINPUT_TRIGGER_LEFT);
	bool isInputNowFrameLeftStrick = LEFT_STICK_INPUT_DEADLINE < fabs(nowFrameInputLeftStickHori) || inputADKey != 0;
	bool isInputLShift = Input::Ins()->IsKey(DIK_LSHIFT);
	if ((INPUT_DEADLINE_DRIFT < inputLeftTriValue || isInputLShift) && isInputNowFrameLeftStrick) {

		isDrift = true;

		// ドリフトのベクトルを求める。
		Vec3 driftVec = Vec3();
		if (nowFrameInputLeftStickHori < 0) {

			driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(carBodyInsIndex));

		}
		else {

			driftVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(carBodyInsIndex));

		}

		// ドリフト時のパーティクルを生成。
		//DriftParticleMgr::Ins()->Generate(pos, driftVec, PolygonInstanceRegister::Ins()->GetRotate(carInstanceIndex), ConstBufferData);
		ConstBufferData;

	}
	// すでにドリフト中だったら勝手に解除しないようにする。
	else if ((INPUT_DEADLINE_DRIFT < inputLeftTriValue || isInputLShift) && isDrift) {
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
	if (Input::Ins()->IsPadBottom(XINPUT_GAMEPAD_B) || Input::Ins()->IsKeyTrigger(DIK_SPACE)) {

		pos = PLAYER_DEF_POS;
		PolygonInstanceRegister::Ins()->ChangeTrans(carBodyInsIndex, Vec3(0, 0, 0));
		PolygonInstanceRegister::Ins()->ChangeRotate(carBodyInsIndex, Vec3(0, 0, 0));
		forwardVec = Vec3(0, 0, -1);
		rotY = 0;
		upVec = Vec3(0, 1, 0);

	}

}

void Player::Move()
{

	/*===== 移動処理 =====*/

	// 移動速度が限界を超えないようにする。
	if (MAX_SPEED < speed) {

		speed = MAX_SPEED;

	}

	// 草の上にいたら移動速度の限界値を下げる。
	if (isGrass && MAX_SPEED_ON_GRASS < speed) {

		speed = MAX_SPEED_ON_GRASS;

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
	bottomVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(carBodyInsIndex));

}

void Player::CheckHit(bool& IsPassedMiddlePoint, int& RapCount)
{

	/*===== 当たり判定 =====*/

	{

		/*-- ステージとの当たり判定 --*/

		// 当たり判定に使用するデータ
		FHelper::RayToModelCollisionData collistionData;

		// 当たり判定に必要なデータを埋めていく。
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageBlasIndex]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData.stageBlasIndex]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageBlasIndex]->GetVertexIndex();
		collistionData.rayPos = pos;
		collistionData.rayDir = bottomVec;
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData.stageInsIndex);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData.stageInsIndex);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData.stageInsIndex);

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


		/*-- 草との当たり判定 --*/

		// 当たっていなかったら当たり判定を行う。
		if (!isHit) {

			// 当たり判定に必要なデータを埋めていく。
			collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageGrassBlasIndex]->GetVertexPos();
			collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData.stageGrassBlasIndex]->GetVertexNormal();
			collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageGrassBlasIndex]->GetVertexIndex();
			collistionData.rayPos = pos;
			collistionData.rayDir = bottomVec;
			collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData.stageGrassInsIndex);
			collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData.stageGrassInsIndex);
			collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData.stageGrassInsIndex);

			// 当たり判定の結果保存用変数。
			isHit = false;
			impactPos = Vec3();
			hitDistance = 0;
			hitNormal = Vec3();

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

				// 草の上にいる判定。
				isGrass = true;

			}
			else {

				// 空中にいる判定。
				isGround = false;

				// 草の上にいない判定。
				isGrass = false;

			}

		}
		// 通常の床で当たっているということは草の上にはいないということなので、falseにする。
		else {

			isGrass = false;

		}


		// 正面方向の当たり判定を行うため、レイの飛ばす方向を変える。
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageBlasIndex]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData.stageBlasIndex]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageBlasIndex]->GetVertexIndex();
		collistionData.rayPos = prevPos;
		collistionData.rayDir = (pos - prevPos).GetNormal();
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData.stageInsIndex);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData.stageInsIndex);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData.stageInsIndex);

		// 当たり判定を行う。
		isHit = false;
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// 当たった距離がY軸のサイズよりも小さかったら。
		isHit &= fabs(hitDistance) < (pos - prevPos).Length();

		// 当たっていたら押し戻す。
		if (isHit) {

			// 法線方向に当たった分押し戻す。
			pos = impactPos + hitNormal * hitDistance;

		}


	}


	/*-- 中心地点とゴール地点との当たり判定 --*/

	// 中間地点に達していなかったら中間地点との当たり判定を行う。
	if (!IsPassedMiddlePoint) {

		// 当たり判定に使用するデータ
		FHelper::RayToModelCollisionData collistionData;

		// 当たり判定に必要なデータを埋めていく。
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData.middlePointBlasIndex]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData.middlePointBlasIndex]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData.middlePointBlasIndex]->GetVertexIndex();
		collistionData.rayPos = prevPos;
		collistionData.rayDir = forwardVec;
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData.middlePointInsIndex);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData.middlePointInsIndex);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData.middlePointInsIndex);

		// 当たり判定の結果保存用変数。
		bool isHit = false;
		Vec3 impactPos;
		float hitDistance;
		Vec3 hitNormal;

		// 当たり判定を行う。
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// 当たった距離がY軸のサイズよりも小さかったら。
		isHit &= hitDistance < (pos - prevPos).Length();
		isHit &= 0 < hitDistance;

		// 当たっていたら。
		if (isHit) {

			IsPassedMiddlePoint = true;

		}

	}
	else {

		// 当たり判定に使用するデータ
		FHelper::RayToModelCollisionData collistionData;

		// 当たり判定に必要なデータを埋めていく。
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData.goalBlasIndex]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData.goalBlasIndex]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData.goalBlasIndex]->GetVertexIndex();
		collistionData.rayPos = prevPos;
		collistionData.rayDir = forwardVec;
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData.goalInsIndex);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData.goalInsIndex);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData.goalInsIndex);

		// 当たり判定の結果保存用変数。
		bool isHit = false;
		Vec3 impactPos;
		float hitDistance;
		Vec3 hitNormal;

		// 当たり判定を行う。
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// 当たった距離がY軸のサイズよりも小さかったら。
		isHit &= hitDistance < (pos - prevPos).Length();
		isHit &= 0 < hitDistance;

		// 当たっていたら。
		if (isHit) {

			IsPassedMiddlePoint = false;
			++RapCount;

		}

	}


	/*===== 装飾オブジェクトとの当たり判定 =====*/

	{

		const int BLAS_COUNT = static_cast<int>(stageModelData.stageOrnamentInsIndex.size());
		for (int index = 0; index < BLAS_COUNT; ++index) {

			// 当たり判定に使用するデータ
			FHelper::RayToModelCollisionData collistionData;
			collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageOrnamentBlasIndex[index]]->GetVertexPos();
			collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData.stageOrnamentBlasIndex[index]]->GetVertexNormal();
			collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageOrnamentBlasIndex[index]]->GetVertexIndex();
			collistionData.rayPos = prevPos;
			collistionData.rayDir = (pos - prevPos).GetNormal();
			collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData.stageOrnamentInsIndex[index]);
			collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData.stageOrnamentInsIndex[index]);
			collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData.stageOrnamentInsIndex[index]);

			// 当たり判定を行う。
			bool isHit = false;
			Vec3 impactPos;
			float hitDistance;
			Vec3 hitNormal;
			isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

			// 当たった距離がY軸のサイズよりも小さかったら。
			isHit &= fabs(hitDistance) < (pos - prevPos).Length();

			// 当たっていたら押し戻す。
			if (isHit) {

				// 法線方向に当たった分押し戻す。
				pos = impactPos + hitNormal * hitDistance;
				speed = 0;
				boostSpeed = 0;

			}

		}

	}


	/*===== ギミックとの当たり判定 =====*/

	{

		std::vector<std::shared_ptr<BaseGimmick>> gimmics = GimmickMgr::Ins()->GetGimmickData();
		for (auto& index : gimmics) {

			// フラグが立っていなかったら処理を続ける。
			if (!index->GetIsActive()) continue;

			// 当たり判定を行う。
			bool isHit = obb.CheckHitOBB(index->GetOBB());

			// 当たっていなかったら処理を飛ばす。
			if (!isHit) continue;

			// ブーストをマックスにする。
			boostSpeed = MAX_BOOST_SPEED;

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
		PolygonInstanceRegister::Ins()->ChangeRotate(carBodyInsIndex, quaternionMat);

		// 上ベクトルを基準としたクォータニオンを求める。
		Vec3 normal = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal.ConvertXMVECTOR(), rotY);

		// クォータニオンを行列に治す。
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// プレイヤーを回転させる。
		PolygonInstanceRegister::Ins()->AddRotate(carBodyInsIndex, upQuaternionMat);


		/*-- プレイヤーの回転行列をもとに各ベクトルを回転 --*/

		// 回転行列を取得。
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(carBodyInsIndex);

		// 上ベクトルを更新。
		upVec = normal;

		//正面ベクトルを更新。
		forwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

	// 当たった法線が上だった場合は行列を初期化して回転させる。
	if (HitNormal == Vec3(0, 1, 0)) {

		// プレイヤーを回転させる。
		PolygonInstanceRegister::Ins()->ChangeRotate(carBodyInsIndex, DirectX::XMMatrixIdentity());

		// 上ベクトルを基準としたクォータニオンを求める。
		Vec3 normal = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal.ConvertXMVECTOR(), rotY);

		// クォータニオンを行列に治す。
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// プレイヤーを回転させる。
		PolygonInstanceRegister::Ins()->AddRotate(carBodyInsIndex, upQuaternionMat);


		/*-- プレイヤーの回転行列をもとに各ベクトルを回転 --*/

		// 回転行列を取得。
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(carBodyInsIndex);

		// 上ベクトルを更新。
		upVec = normal;

		//正面ベクトルを更新。
		forwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

}