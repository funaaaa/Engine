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

	// 車のモデルをロード
	playerModel_.Load();

	stageModelData_ = StageObjectData;

	pos_ = PLAYER_DEF_POS;
	prevPos_ = pos_;
	forwardVec_ = Vec3(0, 0, -1);
	bottomVec = Vec3(0, -1, 0);
	upVec_ = Vec3(0, 1, 0);
	size_ = Vec3(15, 15, 15);
	rotY_ = 0;
	speed_ = 0;
	gravity_ = 0;
	boostSpeed_ = 0;
	returnDefPosTimer_ = 0;
	isDrift_ = false;
	isGround_ = true;
	isGrass_ = false;

	// OBBを生成。
	obb_.Setting(playerModel_.carBodyBlasIndex_, playerModel_.carBodyInsIndex_);

}

void Player::Init()
{

	/*===== 初期化処理 =====*/

	pos_ = PLAYER_DEF_POS;
	prevPos_ = pos_;
	forwardVec_ = Vec3(0, 0, -1);
	bottomVec = Vec3(0, -1, 0);
	upVec_ = Vec3(0, 1, 0);
	size_ = Vec3(30, 30, 30);
	returnDefPosTimer_ = 0;
	rotY_ = 0;
	speed_ = 0;
	gravity_ = 0;
	boostSpeed_ = 0;
	isDrift_ = false;
	isGround_ = true;
	isGrass_ = false;
	PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));

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
	PolygonInstanceRegister::Ins()->ChangeTrans(playerModel_.carBodyInsIndex_, pos_);

	// 座標を保存。
	prevPos_ = pos_;

	// 空中にいるときは初期地点まで戻るタイマーを更新。地上に要るときはタイマーを初期化。
	if (isGround_) {

		returnDefPosTimer_ = 0;

	}
	else {

		++returnDefPosTimer_;

		if (RETURN_DEFPOS_TIMER < returnDefPosTimer_) {

			pos_ = PLAYER_DEF_POS;
			PolygonInstanceRegister::Ins()->ChangeTrans(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));
			forwardVec_ = Vec3(0, 0, -1);
			rotY_ = 0;
			upVec_ = Vec3(0, 1, 0);
			returnDefPosTimer_ = 0;

		}

	}

	// OBBを更新。
	obb_.SetMat(playerModel_.carBodyInsIndex_);

}

void Player::Draw()
{

	/*===== 描画処理 =====*/

}

float Player::GetNowSpeedPer()
{

	/*====== 移動速度の割合を計算 =====*/

	float per = (speed_ + boostSpeed_) / (MAX_SPEED + MAX_BOOST_SPEED);

	return per;

}

void Player::Input(RayConstBufferData& ConstBufferData)
{

	/*===== 入力処理 =====*/

	// RTが引かれていたら加速。
	const float INPUT_DEADLINE_TRI = 0.5f;
	float inputRightTriValue = Input::Ins()->PadTrigger(XINPUT_TRIGGER_RIGHT);
	if ((INPUT_DEADLINE_TRI < inputRightTriValue) && isGround_) {

		speed_ += inputRightTriValue * ADD_SPEED;

	}
	else if (Input::Ins()->IsKey(DIK_W) && isGround_) {

		speed_ += ADD_SPEED;

	}
	else if (isGround_) {

		// 移動していなくて地上にいたら移動量を0に近づける。
		speed_ -= speed_ / 10.0f;

	}
	else {

		// 移動していなくて空中にいたら移動量を0に近づける。
		speed_ -= speed_ / 200.0f;

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
		if (isDrift_) {

			handleAmount = HANDLE_DRIFT;

			// ついでにドリフト状態の時のブーストするまでのタイマーを更新する。
			if (isGround_) {
				++driftBoostTimer_;
				if (DRIFT_BOOST_TIMER < driftBoostTimer_) driftBoostTimer_ = DRIFT_BOOST_TIMER;
			}

			// タイヤを回転させる。
			Vec3 rot = Vec3(0.0f, 0.5f, 0.0f);
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carRightTireFrameInsIndex_, rot * static_cast<float>(inputADKey));
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carRightTireInsIndex_, rot * static_cast<float>(inputADKey));
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carLeftTireFrameInsIndex_, rot * static_cast<float>(inputADKey));
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carLeftTireInsIndex_, rot * static_cast<float>(inputADKey));

		}
		// ドリフト状態じゃなかったら。
		else {

			// タイヤを回転させる。
			Vec3 rot = Vec3(0.0f, 0.3f, 0.0f);
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carRightTireFrameInsIndex_, rot * static_cast<float>(inputADKey));
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carRightTireInsIndex_, rot * static_cast<float>(inputADKey));
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carLeftTireFrameInsIndex_, rot * static_cast<float>(inputADKey));
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carLeftTireInsIndex_, rot * static_cast<float>(inputADKey));

		}

		// クォータニオンを求める。
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(upVec_.ConvertXMVECTOR(), handleAmount * inputLeftStickHori + static_cast<float>(inputADKey) * handleAmount);

		// 求めたクォータニオンを行列に治す。
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// 回転を加算する。
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, quaternionMat);
		rotY_ += handleAmount * inputLeftStickHori + static_cast<float>(inputADKey) * handleAmount;
		nowFrameInputLeftStickHori = inputLeftStickHori + static_cast<float>(inputADKey) * handleAmount;

		// 正面ベクトルを車の回転行列分回転させる。
		forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

	}
	else {

		// タイヤの回転をデフォルトに戻す。
		Vec3 rot = Vec3(0.0f, 0.0f, 0.0f);
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carRightTireFrameInsIndex_, rot);
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carRightTireInsIndex_, rot);
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carLeftTireFrameInsIndex_, rot);
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carLeftTireInsIndex_, rot);

	}

	// LTが引かれていたらドリフト状態にする。
	const float INPUT_DEADLINE_DRIFT = 0.9f;
	float inputLeftTriValue = Input::Ins()->PadTrigger(XINPUT_TRIGGER_LEFT);
	bool isInputNowFrameLeftStrick = LEFT_STICK_INPUT_DEADLINE < fabs(nowFrameInputLeftStickHori) || inputADKey != 0;
	bool isInputLShift = Input::Ins()->IsKey(DIK_LSHIFT);
	if ((INPUT_DEADLINE_DRIFT < inputLeftTriValue || isInputLShift) && isInputNowFrameLeftStrick) {

		isDrift_ = true;

		// ドリフトのベクトルを求める。
		Vec3 driftVec = Vec3();
		if (nowFrameInputLeftStickHori < 0) {

			driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

		}
		else {

			driftVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

		}

		// ドリフト時のパーティクルを生成。
		//DriftParticleMgr::Ins()->Generate(pos, driftVec, PolygonInstanceRegister::Ins()->GetRotate(carInstanceIndex), ConstBufferData);
		ConstBufferData;

	}
	// すでにドリフト中だったら勝手に解除しないようにする。
	else if ((INPUT_DEADLINE_DRIFT < inputLeftTriValue || isInputLShift) && isDrift_) {
	}
	else {

		// ドリストのブーストするまでのタイマーが規定値以上だったらブーストする。
		if (DRIFT_BOOST_TIMER <= driftBoostTimer_) {

			boostSpeed_ = MAX_BOOST_SPEED;

		}

		// ドリフトのブーストするまでのタイマーを初期化する。
		driftBoostTimer_ = 0;

		isDrift_ = false;

	}

	// デバッグ用 Bボタンが押されたら初期位置に戻す。
	if (Input::Ins()->IsPadBottom(XINPUT_GAMEPAD_B) || Input::Ins()->IsKeyTrigger(DIK_SPACE)) {

		pos_ = PLAYER_DEF_POS;
		PolygonInstanceRegister::Ins()->ChangeTrans(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));
		forwardVec_ = Vec3(0, 0, -1);
		rotY_ = 0;
		upVec_ = Vec3(0, 1, 0);

	}

}

void Player::Move()
{

	/*===== 移動処理 =====*/

	// 移動速度が限界を超えないようにする。
	if (MAX_SPEED < speed_) {

		speed_ = MAX_SPEED;

	}

	// 草の上にいたら移動速度の限界値を下げる。
	if (isGrass_ && MAX_SPEED_ON_GRASS < speed_) {

		speed_ = MAX_SPEED_ON_GRASS;

	}

	// 座標移動させる。
	pos_ += forwardVec_ * (speed_ + boostSpeed_);

	// ドリフト時のブースト移動量を0に近づける。
	if (0 < boostSpeed_) {

		boostSpeed_ -= SUB_BOOST_SPEED;

	}
	else {

		boostSpeed_ = 0;

	}

	// 地上にいたら重力を無効化する。
	if (isGround_) {

		gravity_ = 0;

	}
	// 空中にいたら重力を加算する。
	else {

		gravity_ += ADD_GRAV;

		// 重力量が限界を超えないようにする。
		if (MAX_GRAV < gravity_) {

			gravity_ = MAX_GRAV;

		}

	}

	// 座標に重力を加算する。
	pos_ += Vec3(0, -1, 0) * gravity_;

	// 下ベクトルを車の回転行列分回転させる。
	bottomVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

}

void Player::CheckHit(bool& IsPassedMiddlePoint, int& RapCount)
{

	/*===== 当たり判定 =====*/

	{

		/*-- ステージとの当たり判定 --*/

		// 当たり判定に使用するデータ
		FHelper::RayToModelCollisionData collistionData;

		// 当たり判定に必要なデータを埋めていく。
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageBlasIndex_]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageBlasIndex_]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageBlasIndex_]->GetVertexIndex();
		collistionData.rayPos = pos_;
		collistionData.rayDir = bottomVec;
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData_.stageInsIndex_);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData_.stageInsIndex_);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData_.stageInsIndex_);

		// 当たり判定の結果保存用変数。
		bool isHit = false;
		Vec3 impactPos;
		float hitDistance;
		Vec3 hitNormal;

		// 当たり判定を行う。
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// 当たった距離がY軸のサイズよりも小さかったら。
		isHit &= (hitDistance - size_.y_) < 0;
		isHit &= 0 < hitDistance;

		// 当たっていたら押し戻す。
		if (isHit) {

			// ぴったり押し戻すと次のフレームで空中判定になってしまうので、若干オフセットを設ける。
			const float PUSH_BACK_OFFSET = 1.0f;

			// 法線方向に当たった分押し戻す。
			pos_ += hitNormal * (size_.y_ - (hitDistance + PUSH_BACK_OFFSET));

			// 地上にいる判定。
			isGround_ = true;

			// 斜め床の回転処理。
			RotObliqueFloor(hitNormal);

		}
		else {

			// 空中にいる判定。
			isGround_ = false;

		}


		/*-- 草との当たり判定 --*/

		// 当たっていなかったら当たり判定を行う。
		if (!isHit) {

			// 当たり判定に必要なデータを埋めていく。
			collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageGrassBlasIndex_]->GetVertexPos();
			collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageGrassBlasIndex_]->GetVertexNormal();
			collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageGrassBlasIndex_]->GetVertexIndex();
			collistionData.rayPos = pos_;
			collistionData.rayDir = bottomVec;
			collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData_.stageGrassInsIndex_);
			collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData_.stageGrassInsIndex_);
			collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData_.stageGrassInsIndex_);

			// 当たり判定の結果保存用変数。
			isHit = false;
			impactPos = Vec3();
			hitDistance = 0;
			hitNormal = Vec3();

			// 当たり判定を行う。
			isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

			// 当たった距離がY軸のサイズよりも小さかったら。
			isHit &= (hitDistance - size_.y_) < 0;
			isHit &= 0 < hitDistance;

			// 当たっていたら押し戻す。
			if (isHit) {

				// ぴったり押し戻すと次のフレームで空中判定になってしまうので、若干オフセットを設ける。
				const float PUSH_BACK_OFFSET = 1.0f;

				// 法線方向に当たった分押し戻す。
				pos_ += hitNormal * (size_.y_ - (hitDistance + PUSH_BACK_OFFSET));

				// 地上にいる判定。
				isGround_ = true;

				// 斜め床の回転処理。
				RotObliqueFloor(hitNormal);

				// 草の上にいる判定。
				isGrass_ = true;

			}
			else {

				// 空中にいる判定。
				isGround_ = false;

				// 草の上にいない判定。
				isGrass_ = false;

			}

		}
		// 通常の床で当たっているということは草の上にはいないということなので、falseにする。
		else {

			isGrass_ = false;

		}


		// 正面方向の当たり判定を行うため、レイの飛ばす方向を変える。
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageBlasIndex_]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageBlasIndex_]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageBlasIndex_]->GetVertexIndex();
		collistionData.rayPos = prevPos_;
		collistionData.rayDir = (pos_ - prevPos_).GetNormal();
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData_.stageInsIndex_);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData_.stageInsIndex_);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData_.stageInsIndex_);

		// 当たり判定を行う。
		isHit = false;
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// 当たった距離がY軸のサイズよりも小さかったら。
		isHit &= fabs(hitDistance) < (pos_ - prevPos_).Length();

		// 当たっていたら押し戻す。
		if (isHit) {

			// 法線方向に当たった分押し戻す。
			pos_ = impactPos + hitNormal * hitDistance;

		}


	}


	/*-- 中心地点とゴール地点との当たり判定 --*/

	// 中間地点に達していなかったら中間地点との当たり判定を行う。
	if (!IsPassedMiddlePoint) {

		// 当たり判定に使用するデータ
		FHelper::RayToModelCollisionData collistionData;

		// 当たり判定に必要なデータを埋めていく。
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData_.middlePointBlasIndex_]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData_.middlePointBlasIndex_]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData_.middlePointBlasIndex_]->GetVertexIndex();
		collistionData.rayPos = prevPos_;
		collistionData.rayDir = forwardVec_;
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData_.middlePointInsIndex_);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData_.middlePointInsIndex_);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData_.middlePointInsIndex_);

		// 当たり判定の結果保存用変数。
		bool isHit = false;
		Vec3 impactPos;
		float hitDistance;
		Vec3 hitNormal;

		// 当たり判定を行う。
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// 当たった距離がY軸のサイズよりも小さかったら。
		isHit &= hitDistance < (pos_ - prevPos_).Length();
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
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData_.goalBlasIndex_]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData_.goalBlasIndex_]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData_.goalBlasIndex_]->GetVertexIndex();
		collistionData.rayPos = prevPos_;
		collistionData.rayDir = forwardVec_;
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData_.goalInsIndex_);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData_.goalInsIndex_);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData_.goalInsIndex_);

		// 当たり判定の結果保存用変数。
		bool isHit = false;
		Vec3 impactPos;
		float hitDistance;
		Vec3 hitNormal;

		// 当たり判定を行う。
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// 当たった距離がY軸のサイズよりも小さかったら。
		isHit &= hitDistance < (pos_ - prevPos_).Length();
		isHit &= 0 < hitDistance;

		// 当たっていたら。
		if (isHit) {

			IsPassedMiddlePoint = false;
			++RapCount;

		}

	}


	/*===== 装飾オブジェクトとの当たり判定 =====*/

	{

		const int BLAS_COUNT = static_cast<int>(stageModelData_.stageOrnamentInsIndex_.size());
		for (int index_ = 0; index_ < BLAS_COUNT; ++index_) {

			// 当たり判定に使用するデータ
			FHelper::RayToModelCollisionData collistionData;
			collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageOrnamentBlasIndex_[index_]]->GetVertexPos();
			collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageOrnamentBlasIndex_[index_]]->GetVertexNormal();
			collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageOrnamentBlasIndex_[index_]]->GetVertexIndex();
			collistionData.rayPos = prevPos_;
			collistionData.rayDir = (pos_ - prevPos_).GetNormal();
			collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData_.stageOrnamentInsIndex_[index_]);
			collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData_.stageOrnamentInsIndex_[index_]);
			collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData_.stageOrnamentInsIndex_[index_]);

			// 当たり判定を行う。
			bool isHit = false;
			Vec3 impactPos;
			float hitDistance;
			Vec3 hitNormal;
			isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

			// 当たった距離がY軸のサイズよりも小さかったら。
			isHit &= fabs(hitDistance) < (pos_ - prevPos_).Length();

			// 当たっていたら押し戻す。
			if (isHit) {

				// 法線方向に当たった分押し戻す。
				pos_ = impactPos + hitNormal * hitDistance;
				speed_ = 0;
				boostSpeed_ = 0;

			}

		}

	}


	/*===== ギミックとの当たり判定 =====*/

	{

		std::vector<std::shared_ptr<BaseGimmick>> gimmics = GimmickMgr::Ins()->GetGimmickData();
		for (auto& index_ : gimmics) {

			// フラグが立っていなかったら処理を続ける。
			if (!index_->GetIsActive()) continue;

			// 当たり判定を行う。
			bool isHit = obb_.CheckHitOBB(index_->GetOBB());

			// 当たっていなかったら処理を飛ばす。
			if (!isHit) continue;

			// ブーストをマックスにする。
			boostSpeed_ = MAX_BOOST_SPEED;

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
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, quaternionMat);

		// 上ベクトルを基準としたクォータニオンを求める。
		Vec3 normal_ = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), rotY_);

		// クォータニオンを行列に治す。
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// プレイヤーを回転させる。
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, upQuaternionMat);


		/*-- プレイヤーの回転行列をもとに各ベクトルを回転 --*/

		// 回転行列を取得。
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_);

		// 上ベクトルを更新。
		upVec_ = normal_;

		//正面ベクトルを更新。
		forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

	// 当たった法線が上だった場合は行列を初期化して回転させる。
	if (HitNormal == Vec3(0, 1, 0)) {

		// プレイヤーを回転させる。
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, DirectX::XMMatrixIdentity());

		// 上ベクトルを基準としたクォータニオンを求める。
		Vec3 normal_ = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), rotY_);

		// クォータニオンを行列に治す。
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// プレイヤーを回転させる。
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, upQuaternionMat);


		/*-- プレイヤーの回転行列をもとに各ベクトルを回転 --*/

		// 回転行列を取得。
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_);

		// 上ベクトルを更新。
		upVec_ = normal_;

		//正面ベクトルを更新。
		forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

}