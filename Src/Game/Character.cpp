#include "Character.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "Input.h"
#include "FHelper.h"
#include "BLAS.h"
#include "DriftParticleMgr.h"
#include "Camera.h"
#include "HitGroupMgr.h"
#include "OBB.h"
#include "CircuitStage.h"
#include "TextureManager.h"
#include "PlayerTire.h"
#include "BoostItem.h"
#include "ShellItem.h"
#include "PlayerOperationObject.h"
#include "FirstAIOperationObject.h"
#include "FirstAIWaypointMgr.h"
#include "BaseStage.h"
#include "StageObjectMgr.h"
#include "ShellObjectMgr.h"
#include "GameSceneMode.h"
#include "GhostOperationObject.h"
#include "DriftParticleMgr.h"

Character::Character(CHARA_ID CharaID)
{

	/*===== 初期化処理 =====*/

	// キャラのIDを保存
	charaID_ = CharaID;

	// キャラIDに応じて操作オブジェクトを生成。
	if (charaID_ == CHARA_ID::P1) {

		operationObject_ = std::make_shared<PlayerOperationObject>(0, false, this);

		// 車のモデルをロード
		playerModel_.Load(PlayerModel::COLOR::RED, false);

	}
	else if (charaID_ == CHARA_ID::P1_WGHOST) {

		operationObject_ = std::make_shared<PlayerOperationObject>(0, true, this);

		// 車のモデルをロード
		playerModel_.Load(PlayerModel::COLOR::RED, false);

	}
	else if (charaID_ == CHARA_ID::AI1) {

		operationObject_ = std::make_shared<FirstAIOperationObject>(static_cast<int>(FirstAIWayPointMgr::WAYPOINT_OFFSET::LEFT_LEARNING));

		// 車のモデルをロード
		playerModel_.Load(PlayerModel::COLOR::BLUE, false);

	}
	else if (charaID_ == CHARA_ID::GHOST) {

		operationObject_ = std::make_shared<GhostOperationObject>("Resource/Game/CharaGhostData/Dev_0.txt");

		// 車のモデルをロード
		playerModel_.Load(PlayerModel::COLOR::BLUE, true);

	}

	// タイヤをセット。
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carRightTireFrameInsIndex_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carRightTireInsIndex_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carLeftTireFrameInsIndex_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carLeftTireInsIndex_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carBehindTireFrameInsIndex_, true));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carBehindTireInsIndex_, true));

	pos_ = PLAYER_DEF_POS;
	prevPos_ = pos_;
	forwardVec_ = Vec3(0, 0, -1);
	bottomVec = Vec3(0, -1, 0);
	upVec_ = Vec3(0, 1, 0);
	size_ = Vec3(30, 30, 30);
	rotY_ = 0;
	speed_ = 0;
	gravity_ = 0;
	boostSpeed_ = 0;
	returnDefPosTimer_ = 0;
	boostTimer_ = 0;
	isDrift_ = false;
	onGround_ = true;
	onGrass_ = false;
	isGetItem_ = false;
	isTireMask_ = false;
	IsTurningIndicatorRed_ = false;
	isRotRightSide_ = false;
	turningIndicatorTimer_ = 0;
	tireLollingAmount_ = 0;
	canNotMoveTimer_ = 0;
	boostRotQ_ = DirectX::XMVECTOR();
	handleAmount_ = 0;
	handleRotQ_ = DirectX::XMVECTOR();
	nowBoostRotQ_ = DirectX::XMVECTOR();
	nowHandleRotQ_ = DirectX::XMVECTOR();
	defBodyMatRot_ = DirectX::XMMatrixIdentity();

	baseDriftRot_ = 0;
	nowDriftRot_ = 0;
	driftRotTimer_ = 0;
	baseBoostRot_ = 0;
	nowBoostRot_ = 0;

	// OBBを生成。
	obb_ = std::make_shared<OBB>();
	obb_->Setting(playerModel_.carBodyBlasIndex_, playerModel_.carBodyInsIndex_);

}

void Character::Init()
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
	boostTimer_ = 0;
	turningIndicatorTimer_ = 0;
	canNotMoveTimer_ = 0;
	isDrift_ = false;
	onGround_ = true;
	onGrass_ = false;
	isGetItem_ = false;
	IsTurningIndicatorRed_ = false;
	isTireMask_ = false;
	PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));

}

void Character::Update(std::weak_ptr<BaseStage> StageData, RayConstBufferData& ConstBufferData, bool& IsPassedMiddlePoint, int& RapCount)
{


	/*===== 更新処理 =====*/

	// ブースト時の回転を打ち消す。
	PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, defBodyMatRot_);

	// 入力処理
	Input(ConstBufferData);

	// 移動処理
	Move();

	// 当たり判定
	CheckHit(StageData, IsPassedMiddlePoint, RapCount);

	// 座標を更新。
	PolygonInstanceRegister::Ins()->ChangeTrans(playerModel_.carBodyInsIndex_, pos_);

	// 車体を傾ける処理。
	InclineCarBody();


	// 空中にいるときは初期地点まで戻るタイマーを更新。地上に要るときはタイマーを初期化。
	if (onGround_) {

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
	obb_->SetMat(playerModel_.carBodyInsIndex_);

	// 座標を保存。
	prevPos_ = pos_;

	// タイヤを更新する。
	for (auto& index : tires_) {

		index->Update();

	}


	// ブーストするタイマーが一定以上だったら加速し続ける。
	if (0 < boostTimer_) {

		--boostTimer_;

		boostSpeed_ = MAX_BOOST_SPEED;


		// 加速の割合を求める。
		float boostRate = boostSpeed_ / MAX_BOOST_SPEED;

		// 煙の大きさを決める。
		bool IsSmokeBIg = 0.5f < boostRate;

		// 設置していたら煙を生成。
		if (onGround_) {
			Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
			DriftParticleMgr::Ins()->Generate(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * 30.0f, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, IsSmokeBIg, true);
			driftVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
			DriftParticleMgr::Ins()->Generate(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * 30.0f, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, IsSmokeBIg, true);
		}

	}

	// 移動できないタイマーを更新する。
	if (0 < canNotMoveTimer_) {

		--canNotMoveTimer_;

		// イージング量を求める。
		float easingAmount = static_cast<float>(canNotMoveTimer_) / static_cast<float>(CAN_NOT_MOVE_TIMER_SHELL_HIT);

		easingAmount = easingAmount * easingAmount * easingAmount * easingAmount * easingAmount;

		rotY_ = shellHitRot_ + easingAmount * DirectX::XM_2PI;

	}

}

void Character::Draw()
{

	/*===== 描画処理 =====*/

}

bool Character::CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, TireMaskUV& TireMaskUVData)
{

	/*===== タイヤ痕を検出 =====*/

	if (!isTireMask_) {

		tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

		return false;

	}

	FHelper::RayToModelCollisionData InputRayData;
	InputRayData.targetVertex_ = BLASRegister::Ins()->GetBLAS()[BaseStageData.lock()->stageObjectMgr_->GetBlasIndex(0)]->GetVertexPos();
	InputRayData.targetNormal_ = BLASRegister::Ins()->GetBLAS()[BaseStageData.lock()->stageObjectMgr_->GetBlasIndex(0)]->GetVertexNormal();
	InputRayData.targetIndex_ = BLASRegister::Ins()->GetBLAS()[BaseStageData.lock()->stageObjectMgr_->GetBlasIndex(0)]->GetVertexIndex();
	InputRayData.targetUV_ = BLASRegister::Ins()->GetBLAS()[BaseStageData.lock()->stageObjectMgr_->GetBlasIndex(0)]->GetVertexUV();
	InputRayData.matTrans_ = PolygonInstanceRegister::Ins()->GetTrans(BaseStageData.lock()->stageObjectMgr_->GetBlasIndex(0));
	InputRayData.matScale_ = PolygonInstanceRegister::Ins()->GetScale(BaseStageData.lock()->stageObjectMgr_->GetBlasIndex(0));
	InputRayData.matRot_ = PolygonInstanceRegister::Ins()->GetRotate(BaseStageData.lock()->stageObjectMgr_->GetBlasIndex(0));

	// 戻り地保存用
	Vec3 ImpactPos;
	Vec3 HitNormal;
	Vec2 HitUV;
	float HitDistance;

	// 左前タイヤ
	InputRayData.rayPos_ = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carLeftTireInsIndex_);
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

	// タイヤ痕の位置を検出
	bool isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

	// 当たり判定が正しいかどうかをチェック。
	if (!isHit || HitDistance < 0 || 40 < HitDistance) {

		tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);

		return false;

	}
	else {

		tireMaskUV_.forwardLeftUV_.prevuv_ = tireMaskUV_.forwardLeftUV_.uv_;
		tireMaskUV_.forwardLeftUV_.uv_ = HitUV;
		TireMaskUVData.forwardLeftUV_.prevuv_ = tireMaskUV_.forwardLeftUV_.prevuv_;
		TireMaskUVData.forwardLeftUV_.uv_ = HitUV;

	}

	// 右前タイヤ
	InputRayData.rayPos_ = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carRightTireInsIndex_);
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

	// タイヤ痕の位置を検出
	isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

	if (!isHit || HitDistance < 0 || 40 < HitDistance) {

		tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

		return false;

	}
	else {

		tireMaskUV_.forwardRightUV_.prevuv_ = tireMaskUV_.forwardRightUV_.uv_;
		tireMaskUV_.forwardRightUV_.uv_ = HitUV;
		TireMaskUVData.forwardRightUV_.prevuv_ = tireMaskUV_.forwardRightUV_.prevuv_;
		TireMaskUVData.forwardRightUV_.uv_ = HitUV;

	}

	// 右後ろタイヤ
	InputRayData.rayPos_ = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carRightTireInsIndex_);
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

	// 回転した後ろベクトルを求める。
	Vec3 behindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
	InputRayData.rayPos_ += behindVec * 70.0f;

	// タイヤ痕の位置を検出
	isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

	if (!isHit || HitDistance < 0 || 40 < HitDistance) {

		tireMaskUV_.behindRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.behindRightUV_.uv_ = Vec2(0, 0);

		return false;

	}
	else {

		tireMaskUV_.behindRightUV_.prevuv_ = tireMaskUV_.behindRightUV_.uv_;
		tireMaskUV_.behindRightUV_.uv_ = HitUV;
		TireMaskUVData.behindRightUV_.prevuv_ = tireMaskUV_.behindRightUV_.prevuv_;
		TireMaskUVData.behindRightUV_.uv_ = HitUV;

	}

	// 左後ろタイヤ
	InputRayData.rayPos_ = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carLeftTireInsIndex_);
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

	// 回転した後ろベクトルを求める。
	behindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
	InputRayData.rayPos_ += behindVec * 70.0f;

	// タイヤ痕の位置を検出
	isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

	if (!isHit || HitDistance < 0 || 40 < HitDistance) {

		tireMaskUV_.behindLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.behindLeftUV_.uv_ = Vec2(0, 0);

		return false;

	}
	else {

		tireMaskUV_.behindLeftUV_.prevuv_ = tireMaskUV_.behindLeftUV_.uv_;
		tireMaskUV_.behindLeftUV_.uv_ = HitUV;
		TireMaskUVData.behindLeftUV_.prevuv_ = tireMaskUV_.behindLeftUV_.prevuv_;
		TireMaskUVData.behindLeftUV_.uv_ = HitUV;

	}

	return true;


}

float Character::GetNowSpeedPer()
{

	/*====== 移動速度の割合を計算 =====*/

	float per = (speed_ + boostSpeed_) / (MAX_SPEED + MAX_BOOST_SPEED);

	return per;

}

void Character::Input(RayConstBufferData& ConstBufferData)
{

	/*===== 入力処理 =====*/

	// 操作オブジェクトからの入力を受け取る。
	BaseOperationObject::OperationInputData operationInputData;
	operationInputData.pos_ = pos_;
	operationInputData.forwradVec_ = forwardVec_;
	BaseOperationObject::Operation operation = operationObject_->Input(operationInputData);

	// タイヤのマスクのフラグを初期化する。
	isTireMask_ = false;

	// 動けないタイマーが動いていたら動かさない。
	if (0 < canNotMoveTimer_) {

		operation.accelerationRate_ = 0;
		operation.handleDriveRate_ = 0;

	}

	// RTが引かれていたら加速。
	if (0 < operation.accelerationRate_ && onGround_) {

		speed_ += operation.accelerationRate_ * ADD_SPEED;

	}
	else if (onGround_) {

		// 移動していなくて地上にいたら移動量を0に近づける。
		speed_ -= speed_ / 10.0f;

	}
	else {

		// 移動していなくて空中にいたら移動量を0に近づける。
		speed_ -= speed_ / 200.0f;

	}

	// アイテムを後ろに投げるフラグを更新。
	isShotBehind_ = operation.isShotBehind_;

	// 現在のフレームの右スティックの傾き具合。
	float nowFrameInputLeftStickHori = 0;

	// 右スティックの横の傾き量でキャラを回転させる。
	if (operation.handleDriveRate_ != 0) {

		// 回転量 通常状態とドリフト状態で違う。
		float handleAmount = HANDLE_NORMAL;

		// ドリフト状態だったら回転量を多い方にする。
		if (isDrift_) {

			handleAmount = HANDLE_DRIFT;

			// ついでにドリフト状態の時のブーストするまでのタイマーを更新する。
			if (onGround_) {
				++driftBoostTimer_;
				if (DRIFT_BOOST_TIMER < driftBoostTimer_) driftBoostTimer_ = DRIFT_BOOST_TIMER;
			}

			// タイヤを回転させる。
			for (auto& index : tires_) {

				index->Rot(true, static_cast<float>(operation.handleDriveRate_));

			}

			isTireMask_ = true;


		}
		// ドリフト状態じゃなかったら。
		else {

			// タイヤを回転させる。
			for (auto& index : tires_) {

				index->Rot(true, static_cast<float>(operation.handleDriveRate_));

			}

		}

		// クォータニオンを求める。
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(upVec_.ConvertXMVECTOR(), handleAmount * operation.handleDriveRate_);

		// 求めたクォータニオンを行列に治す。
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// 回転を加算する。
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, quaternionMat);
		rotY_ += handleAmount * operation.handleDriveRate_;
		nowFrameInputLeftStickHori = handleAmount * operation.handleDriveRate_;

		// 正面ベクトルを車の回転行列分回転させる。
		forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));


		// ウインカーの色を変えるタイマーを更新。
		++turningIndicatorTimer_;
		if (TURNING_INDICATOR_TIMER < turningIndicatorTimer_) {

			turningIndicatorTimer_ = 0;
			IsTurningIndicatorRed_ = IsTurningIndicatorRed_ ? false : true;

		}

		// ウインカーの色を変える。
		if (IsTurningIndicatorRed_) {

			// 曲がっているのが右だったら。
			if (0 < operation.handleDriveRate_) {

				BLASRegister::Ins()->ChangeTex(playerModel_.carRightLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/blackRed.png"));
				BLASRegister::Ins()->ChangeTex(playerModel_.carLeftLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));

			}
			else {

				BLASRegister::Ins()->ChangeTex(playerModel_.carLeftLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/blackRed.png"));
				BLASRegister::Ins()->ChangeTex(playerModel_.carRightLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));

			}

		}
		else {

			BLASRegister::Ins()->ChangeTex(playerModel_.carRightLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));
			BLASRegister::Ins()->ChangeTex(playerModel_.carLeftLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));

		}

	}
	else {

		// 車のライトの色を元に戻す。
		BLASRegister::Ins()->ChangeTex(playerModel_.carRightLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));
		BLASRegister::Ins()->ChangeTex(playerModel_.carLeftLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));

		// 各変数を初期化。
		IsTurningIndicatorRed_ = false;
		turningIndicatorTimer_ = 100;	// チカチカするタイマーを初期化。

	}

	// LTが引かれていたらドリフト状態にする。
	if (operation.isDrift_ && operation.handleDriveRate_ != 0) {

		isDrift_ = true;

		// ドリフトのベクトルを求める。
		Vec3 driftVec = Vec3();
		if (nowFrameInputLeftStickHori < 0) {

			driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));


		}
		else {

			driftVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

		}

		// 更にブーストがかかっていたら煙を出す。
		Vec3 random = FHelper::GetRandVec3(-1, 1);
		if (0.0f < boostSpeed_ && onGround_) {
			DriftParticleMgr::Ins()->Generate(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * 30.0f + random, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, true);
		}
		else if (onGround_) {
			DriftParticleMgr::Ins()->Generate(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * 30.0f + random, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, false);
		}

	}
	// すでにドリフト中だったら勝手に解除しないようにする。
	else if (operation.isDrift_ && isDrift_) {
	}
	else {

		// ドリストのブーストするまでのタイマーが規定値以上だったらブーストする。
		if (DRIFT_BOOST_TIMER <= driftBoostTimer_) {

			boostSpeed_ = MAX_BOOST_SPEED;
			boostTimer_ = 10;

		}

		// ドリフトのブーストするまでのタイマーを初期化する。
		driftBoostTimer_ = 0;

		isDrift_ = false;

	}

	// このキャラがゴーストだった場合、アイテム取得命令が出たらアイテムを取得する。
	if (charaID_ == CHARA_ID::GHOST && operation.isGetItem_) {

		item_ = std::make_shared<BoostItem>();
		item_->Generate(playerModel_.carBodyInsIndex_);

	}

	// アイテムを操作
	if (operation.isUseItemTrigger_ && item_.operator bool()) {

		if (item_->GetItemID() == BaseItem::ItemID::BOOST) {

			boostSpeed_ = MAX_BOOST_SPEED;
			boostTimer_ = 30;
			item_.reset();

		}
		else {

			item_->Use(rotY_, static_cast<int>(ShellItem::PARAM_ID::BEHIND));

		}


	}

	if (operation.isUseItemRelease_ && item_.operator bool()) {

		if (isShotBehind_) {
			item_->Use(rotY_, static_cast<int>(ShellItem::PARAM_ID::BEHIND_THROW));
		}
		else {
			item_->Use(rotY_, static_cast<int>(ShellItem::PARAM_ID::FORWARD_THROW));
		}
		item_.reset();


	}

	// 入力を保存する。
	handleAmount_ = operation.handleDriveRate_;

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

void Character::Move()
{

	/*===== 移動処理 =====*/

	// 移動速度が限界を超えないようにする。
	if (MAX_SPEED < speed_) {

		speed_ = MAX_SPEED;

	}

	// 草の上にいたら移動速度の限界値を下げる。
	if (onGrass_ && MAX_SPEED_ON_GRASS < speed_) {

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
	if (onGround_) {

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

void Character::CheckHit(std::weak_ptr<BaseStage> StageData, bool& IsPassedMiddlePoint, int& RapCount)
{

	/*===== 当たり判定 =====*/

	// 当たり判定関数に入れる値を設定。
	BaseStage::ColliderInput input;
	input.targetInsIndex_ = playerModel_.carBodyInsIndex_;
	input.targetOBB_ = obb_;
	input.targetPos_ = pos_;
	input.targetPrevPos_ = prevPos_;
	input.targetRotY_ = rotY_;
	input.targetSize_ = size_;
	input.isInvalidateRotY_ = false;
	input.isPlayer_ = true;

	// 当たり判定関数から返ってくる値。
	BaseStage::ColliderOutput output;

	// 当たり判定を行う。
	output = StageData.lock()->Collider(input);

	// 当たり判定の結果から処理を行う。
	if (output.isHitBoostGimmick_) {

		// ブーストをマックスにする。
		boostSpeed_ = MAX_BOOST_SPEED;
		boostTimer_ = 10;

	}
	if (output.isHitGoal_) {

		// ゴール
		if (IsPassedMiddlePoint) {
			IsPassedMiddlePoint = false;
			++RapCount;

		}

		operationObject_->OnGoal();

	}
	if (output.isHitMiddlePoint_) {

		// 中間地点との当たり判定
		IsPassedMiddlePoint = true;

	}
	if (output.isHitOrnament_) {

		// 装飾オブジェクトとの当たった判定
		speed_ = 0;
		boostSpeed_ = 0;

	}
	// 設置判定を初期化。
	onGround_ = false;
	onGrass_ = false;
	if (output.isHitStage_) {

		// ステージとの当たり判定
		onGround_ = true;

		forwardVec_ = output.forwardVec_;
		upVec_ = output.upVec_;

	}
	if (output.isHitStageGrass_) {

		// ステージと当たっていなかったら
		if (!onGround_) {

			// 草とあたった判定
			onGround_ = true;
			onGrass_ = true;

			forwardVec_ = output.forwardVec_;
			upVec_ = output.upVec_;

		}

	}
	// ゴーストだったら当たり判定を飛ばす。
	if (output.isHitItemBox_ && charaID_ != CHARA_ID::GHOST) {

		// アイテムを取得したフラグ
		isGetItem_ = true;

		if (GameSceneMode::Ins()->id_ == GameSceneMode::MODE_ID::WRITE_GHOST || GameSceneMode::Ins()->id_ == GameSceneMode::MODE_ID::GHOST) {

			item_ = std::make_shared<BoostItem>();
			item_->Generate(playerModel_.carBodyInsIndex_);

		}
		else {

			// ランダムでアイテムを生成する。
			int random = FHelper::GetRand(0, 1000);

			if (random % 2 == 0) {

				item_ = std::make_shared<BoostItem>();
				item_->Generate(playerModel_.carBodyInsIndex_);

			}
			else {

				item_ = std::make_shared<ShellItem>();
				item_->Generate(playerModel_.carBodyInsIndex_);

			}

		}

	}
	else {

		// アイテムを取得しなかった
		isGetItem_ = false;

	}

	// その他の変数を初期化。
	pos_ = output.resultPos_;

	// ゴースト以外だったら。
	if (charaID_ != CHARA_ID::GHOST) {

		// 甲羅との当たり判定
		bool isHitShell = ShellObjectMgr::Ins()->Collider(obb_);

		if (isHitShell) {

			canNotMoveTimer_ = CAN_NOT_MOVE_TIMER_SHELL_HIT;
			shellHitRot_ = rotY_;

		}

	}

}

void Character::RotObliqueFloor(const Vec3& HitNormal)
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

void Character::InclineCarBody()
{

	/*===== 車体を傾ける処理 =====*/

		// BODYの回転行列を保存。
	defBodyMatRot_ = PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_);

	// ブースト時の回転。
	if (0 < boostSpeed_) {

		float boostRate = boostSpeed_ / MAX_BOOST_SPEED;
		const float MAX_ROT = 0.3f;

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

		Vec3 horiVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

		// クォータニオンを求める。
		boostRotQ_ = DirectX::XMQuaternionRotationAxis(horiVec.ConvertXMVECTOR(), nowBoostRot_);

		// ブーストの車体のクォータニオンを補間。
		nowBoostRotQ_ = DirectX::XMQuaternionSlerp(nowBoostRotQ_, boostRotQ_, 0.2f);
		// 回転行列を求める。
		DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(nowBoostRotQ_);
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, mat);

	}


	// ドリフト時の回転。
	if (isDrift_ && 0 != handleAmount_) {

		// 前フレームと回転方向が違かったら初期化する。
		if (isRotRightSide_ && handleAmount_ < 0) {
			handleRotQ_ = DirectX::XMVECTOR();
			baseDriftRot_ = 0;
			driftRotTimer_ = 0;
		}

		++driftRotTimer_;
		if (MAX_DRIFT_ROT_TIMER < driftRotTimer_) driftRotTimer_ = MAX_DRIFT_ROT_TIMER;

		float timerRate = static_cast<float>(driftRotTimer_) / static_cast<float>(MAX_DRIFT_ROT_TIMER);

		// 回転量にイージングをかける。
		baseDriftRot_ = std::fabs(FEasing::EaseInCubic(timerRate) * MAX_DRIFT_ROT) * (handleAmount_ < 0 ? -1 : 1);

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

		Vec3 horiVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
		Vec3 forwardVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

		// 横クォータニオンを求める。
		handleRotQ_ = DirectX::XMQuaternionRotationAxis(horiVec.ConvertXMVECTOR(), nowDriftRot_);
		// 正面クォータニオンを求める。 横だけだと回転がちょっとだけずれているため。
		DirectX::XMVECTOR forwardQ = DirectX::XMQuaternionRotationAxis(forwardVec.ConvertXMVECTOR(), 0.01f);

		// クォータニオンをかける。
		handleRotQ_ = DirectX::XMQuaternionMultiply(handleRotQ_, forwardQ);
	}

	// ブーストの車体のクォータニオンを補間。
	nowHandleRotQ_ = DirectX::XMQuaternionSlerp(nowHandleRotQ_, handleRotQ_, 0.2f);
	// 回転行列を求める。
	DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(nowHandleRotQ_);
	PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, mat);

}

/*

・ドリフトのときの回転の動きの研究

*/