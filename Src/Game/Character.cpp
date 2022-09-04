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
#include "DriftParticle.h"

Character::Character(CHARA_ID CharaID, const int& CharaIndex, const int& Param)
{

	/*===== 初期化処理 =====*/

	// キャラのIDを保存
	charaID_ = CharaID;

	// キャラの番号を保存。
	charaIndex_ = CharaIndex;

	// キャラIDに応じて操作オブジェクトを生成。
	if (charaID_ == CHARA_ID::P1) {

		operationObject_ = std::make_shared<PlayerOperationObject>(0, false, this);

		// 初期位置を設定。
		DEF_POS = PLAYER_DEF_POS;

		// 車のモデルをロード
		playerModel_.Load(PlayerModel::COLOR::RED, false);

	}
	else if (charaID_ == CHARA_ID::P1_WGHOST) {

		operationObject_ = std::make_shared<PlayerOperationObject>(0, true, this);

		// 初期位置を設定。
		DEF_POS = GHOST_DEF_POS;

		// 車のモデルをロード
		playerModel_.Load(PlayerModel::COLOR::RED, false);

	}
	else if (charaID_ == CHARA_ID::AI1) {

		operationObject_ = std::make_shared<FirstAIOperationObject>(static_cast<int>(FirstAIWayPointMgr::WAYPOINT_OFFSET::LEFT_LEARNING));

		// 初期位置を設定。
		DEF_POS = GHOST_DEF_POS;

		// 車のモデルをロード
		playerModel_.Load(PlayerModel::COLOR::BLUE, false);

	}
	else if (charaID_ == CHARA_ID::GHOST) {

		std::string filePath = "Resource/Game/CharaGhostData/Dev_";
		std::string dottxt = ".txt";
		std::string number = std::to_string(Param);

		operationObject_ = std::make_shared<GhostOperationObject>(filePath + number + dottxt);

		// 初期位置を設定。
		DEF_POS = GHOST_DEF_POS;

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

	rapCount_ = 0;
	isPassedMiddlePoint_ = 0;
	pos_ = DEF_POS;
	prevPos_ = pos_;
	forwardVec_ = Vec3(0, 0, -1);
	bottomVec = Vec3(0, -1, 0);
	upVec_ = Vec3(0, 1, 0);
	size_ = Vec3(50, 30, 50);
	rotY_ = DEF_ROTY;
	speed_ = 0;
	gravity_ = 0;
	beforeStartSmokeTimer_ = 0;
	jumpBoostSpeed_ = 0;
	engineWaveTimer_ = 0;
	engineWaveAmount_ = 0;
	boostSpeed_ = 0;
	returnDefPosTimer_ = 0;
	boostTimer_ = 0;
	isDrift_ = false;
	onGround_ = true;
	isBeforeStartPrev_ = false;
	onGroundPrev_ = false;
	isPrevGameFinish_ = false;
	onGrass_ = false;
	isGameFinish_ = false;
	isGetItem_ = false;
	isTireMask_ = false;
	isUseItem_ = false;
	IsTurningIndicatorRed_ = false;
	isRotRightSide_ = false;
	isDriftJumpPrev_ = false;
	turningIndicatorTimer_ = 0;
	tireLollingAmount_ = 0;
	canNotMoveTimer_ = 0;
	driftTimer_ = 0;
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

		// 初期位置を設定。
	if (charaID_ == CHARA_ID::P1) {
		DEF_POS = PLAYER_DEF_POS;
	}
	else if (charaID_ == CHARA_ID::P1_WGHOST) {
		DEF_POS = GHOST_DEF_POS;
	}
	else if (charaID_ == CHARA_ID::AI1) {
		DEF_POS = GHOST_DEF_POS;
	}
	else if (charaID_ == CHARA_ID::GHOST) {
		DEF_POS = GHOST_DEF_POS;
	}

	rapCount_ = 0;
	isPassedMiddlePoint_ = 0;
	pos_ = DEF_POS;
	prevPos_ = pos_;
	forwardVec_ = Vec3(0, 0, -1);
	bottomVec = Vec3(0, -1, 0);
	upVec_ = Vec3(0, 1, 0);
	size_ = Vec3(50, 30, 50);
	returnDefPosTimer_ = 0;
	rotY_ = DEF_ROTY;
	speed_ = 0;
	gravity_ = 0;
	boostSpeed_ = 0;
	boostTimer_ = 0;
	jumpBoostSpeed_ = 0;
	turningIndicatorTimer_ = 0;
	canNotMoveTimer_ = 0;
	driftJumpVec_ = Vec3();
	engineWaveTimer_ = 0;
	engineWaveAmount_ = 0;
	beforeStartSmokeTimer_ = 0;
	driftJumpSpeed_ = 0;
	isDriftJump_ = false;;
	isDrift_ = false;
	isPrevGameFinish_ = false;
	isUseItem_ = false;
	onGround_ = true;
	onGrass_ = false;
	isGetItem_ = false;
	isGameFinish_ = false;
	isInputLTPrev_ = false;
	isInputLT_ = false;
	isBeforeStartPrev_ = false;
	IsTurningIndicatorRed_ = false;
	isTireMask_ = false;
	isConcentrationLine_ = false;
	isJumpActionTrigger_ = false;
	PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));


	// 臨時のバグ対策です。 最初の一回目のドリフトのときのみオーラが出ないので、ここで一回生成しておく。
	RayConstBufferData constBuffer;
	DriftParticleMgr::Ins()->GenerateAura(charaIndex_, playerModel_.carBehindTireInsIndex_, static_cast<int>(DriftParticle::ID::AURA_BIG), isDriftRight_, 2 <= 0, constBuffer);
	DriftParticleMgr::Ins()->GenerateAura(charaIndex_, playerModel_.carBehindTireInsIndex_, static_cast<int>(DriftParticle::ID::AURA_SMALL), isDriftRight_, 2 <= 0, constBuffer);

	DriftParticleMgr::Ins()->DestroyAura(charaIndex_);

}

void Character::Update(std::weak_ptr<BaseStage> StageData, RayConstBufferData& ConstBufferData, const bool& IsBeforeStart, const bool& IsGameFinish)
{


	/*===== 更新処理 =====*/


	// ゲーム終了フラグを更新。
	isPrevGameFinish_ = isGameFinish_;
	isGameFinish_ = IsGameFinish;

	// エンジンの動きで動いた分を打ち消す。
	pos_.y_ -= engineWaveAmount_;

	// ブースト時の回転を打ち消す。
	PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, defBodyMatRot_);

	// 入力処理
	Input(ConstBufferData, IsBeforeStart);

	// ドリフトに関する更新処理
	UpdateDrift(IsBeforeStart);

	// 移動処理
	Move(IsBeforeStart);

	// 当たり判定
	CheckHit(StageData);

	// 開始前だったら
	if (IsBeforeStart) {

		// プレイヤーの位置を初期位置に戻す。
		pos_.x_ = DEF_POS.x_;
		pos_.z_ = DEF_POS.z_;

	}

	// エンジンの動き
	EngineSineWave();

	// ゲーム終了時の更新処理
	UpdateGameFinish();

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

			pos_ = DEF_POS;
			PolygonInstanceRegister::Ins()->ChangeTrans(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));
			forwardVec_ = Vec3(0, 0, -1);
			rotY_ = -0.367411435;
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

	// ドリフトパーティクルの更新処理。
	UpdateDriftParticle(ConstBufferData, IsGameFinish, IsBeforeStart);

	// 移動できないタイマーを更新する。
	if (0 < canNotMoveTimer_) {

		--canNotMoveTimer_;

		// イージング量を求める。
		float easingAmount = static_cast<float>(canNotMoveTimer_) / static_cast<float>(CAN_NOT_MOVE_TIMER_SHELL_HIT);

		easingAmount = easingAmount * easingAmount * easingAmount * easingAmount * easingAmount;

		rotY_ = shellHitRot_ + easingAmount * DirectX::XM_2PI;

	}

	// ブースト量が一定以上だったら集中線を出す。
	isConcentrationLine_ = (MAX_BOOST_SPEED / 2.0f < boostSpeed_) || (JUMP_BOOST_SPEED / 2.0f < jumpBoostSpeed_);

	// 接地フラグを保存。
	onGroundPrev_ = onGround_;

	// ゲーム開始前フラグを保存。
	isBeforeStartPrev_ = IsBeforeStart;

}

void Character::Draw()
{

	/*===== 描画処理 =====*/

}

bool Character::CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, TireMaskUV& TireMaskUVData)
{

	/*===== タイヤ痕を検出 =====*/

	// ゲームが終了したトリガー判定だったら。
	if (isGameFinish_ && !isPrevGameFinish_) {

		tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

	}

	// 空中にいたら、ドリフトジャンプ中だったら。
	if (!onGround_ || isDriftJump_) {

		tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

	}

	const float RAY_LENGTH = 80.0f;

	// ゲームが終了していたら。
	if (isGameFinish_) {

		// 左側にドリフトしていたら。
		if (!isGameFinishDriftLeft_) {

			FHelper::RayToModelCollisionData InputRayData;
			InputRayData.targetPolygonData_ = PolygonInstanceRegister::Ins()->GetMeshCollisionData(BaseStageData.lock()->stageObjectMgr_->GetInstanceIndex(0));

			// 戻り地保存用
			Vec3 ImpactPos;
			Vec3 HitNormal;
			Vec2 HitUV;
			float HitDistance;

			// 左前タイヤ
			InputRayData.rayPos_ = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carRightTireInsIndex_);
			InputRayData.rayDir_ = bottomVec;

			// タイヤ痕の位置を検出
			bool isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

			// 当たり判定が正しいかどうかをチェック。
			if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

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
			InputRayData.rayPos_ = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_);
			InputRayData.rayPos_ += FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_)) * 20.0f;
			InputRayData.rayDir_ = bottomVec;

			// タイヤ痕の位置を検出
			isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

			if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

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

		}
		else {

			FHelper::RayToModelCollisionData InputRayData;
			InputRayData.targetPolygonData_ = PolygonInstanceRegister::Ins()->GetMeshCollisionData(BaseStageData.lock()->stageObjectMgr_->GetInstanceIndex(0));

			// 戻り地保存用
			Vec3 ImpactPos;
			Vec3 HitNormal;
			Vec2 HitUV;
			float HitDistance;

			// 左前タイヤ
			InputRayData.rayPos_ = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carLeftTireInsIndex_);
			InputRayData.rayDir_ = bottomVec;

			// タイヤ痕の位置を検出
			bool isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

			// 当たり判定が正しいかどうかをチェック。
			if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

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
			InputRayData.rayPos_ = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_);
			InputRayData.rayPos_ += FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_)) * 20.0f;
			InputRayData.rayDir_ = bottomVec;

			// タイヤ痕の位置を検出
			isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

			if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

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


		}


		return true;

	}

	if (!isTireMask_) {

		tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

		return false;

	}

	FHelper::RayToModelCollisionData InputRayData;
	InputRayData.targetPolygonData_ = PolygonInstanceRegister::Ins()->GetMeshCollisionData(BaseStageData.lock()->stageObjectMgr_->GetInstanceIndex(0));

	// 戻り地保存用
	Vec3 ImpactPos;
	Vec3 HitNormal;
	Vec2 HitUV;
	float HitDistance;
	bool isHit = false;

	// 右ドリフトしていたらこの処理は通さない。
	if (!(isDrift_ && !isDriftRight_)) {

		// 左前タイヤ
		InputRayData.rayPos_ = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carLeftTireInsIndex_);
		InputRayData.rayDir_ = bottomVec;

		// タイヤ痕の位置を検出
		isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

		// 当たり判定が正しいかどうかをチェック。
		if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

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

	}


	// 左ドリフトしていたらこの処理は通さない。
	if (!(isDrift_ && isDriftRight_)) {

		// 右前タイヤ
		InputRayData.rayPos_ = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carRightTireInsIndex_);
		InputRayData.rayDir_ = bottomVec;

		// タイヤ痕の位置を検出
		isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

		if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

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

	}

	// 右後ろタイヤ
	InputRayData.rayPos_ = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carRightTireInsIndex_);
	InputRayData.rayDir_ = bottomVec;

	// 回転した後ろベクトルを求める。
	Vec3 behindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
	InputRayData.rayPos_ += behindVec * 70.0f;

	// タイヤ痕の位置を検出
	isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

	if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

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
	InputRayData.rayDir_ = bottomVec;

	// 回転した後ろベクトルを求める。
	behindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
	InputRayData.rayPos_ += behindVec * 70.0f;

	// タイヤ痕の位置を検出
	isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

	if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

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

Vec3 Character::GetCameraForwardVec()
{
	/*===== カメラ用正面ベクトル取得関数 =====*/

	//Vec3 movedVec = pos_ - prevPos_;

	return forwardVec_;

	//return FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

	//// 移動していなかったら。
	//if (movedVec.Length() == 0) {

	//	return forwardVec_;

	//}

	//// 移動していたら。
	//return movedVec.GetNormal();

}

float Character::GetNowSpeedPer()
{

	/*====== 移動速度の割合を計算 =====*/

	float per = FHelper::Saturate((speed_ + boostSpeed_ + jumpBoostSpeed_ * 2.0f) / (MAX_SPEED + MAX_BOOST_SPEED));

	return per;

}

bool Character::GetIsItem()
{
	return item_.operator bool();
}

void Character::DeleteInstance()
{

	/*===== モデルのインスタンスを削除 =====*/

	playerModel_.Delete();

}

void Character::Input(RayConstBufferData& ConstBufferData, const bool& IsBeforeStart)
{

	/*===== 入力処理 =====*/

	// ゲームが終了している場合は入力を無効化する。 ゲームが終了した最初のFでは入力を取る必要があるので、Prevも比較している。
	if (isGameFinish_ && isPrevGameFinish_) return;

	ConstBufferData;

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

	// 開始前のアクセルのフラグを更新。
	isAccel_ = 0 < operation.accelerationRate_;

	// RTが引かれていたら加速。
	if (0 < operation.accelerationRate_ && onGround_ && !IsBeforeStart) {

		speed_ += operation.accelerationRate_ * ADD_SPEED;

	}
	else if (onGround_) {

		// 移動していなくて地上にいたら移動量を0に近づける。
		speed_ -= speed_ / 10.0f;

	}
	else {

		// 移動していなくて空中にいたら移動量を0に近づける。
		speed_ -= speed_ / 100.0f;

	}

	// アイテムを後ろに投げるフラグを更新。
	isShotBehind_ = operation.isShotBehind_;

	// 現在のフレームの右スティックの傾き具合。
	float nowFrameInputLeftStickHori = 0;

	// 右スティックの横の傾き量でキャラを回転させる。
	if ((operation.handleDriveRate_ != 0 || isDrift_) && !IsBeforeStart) {

		// 回転量の最大値 通常状態とドリフト状態で違う。
		float handleAmount = HANDLE_NORMAL;

		// [ドリフト状態] [開始前じゃない] だったら回転量を多い方にする。
		if (isDrift_ && !IsBeforeStart) {

			handleAmount = HANDLE_DRIFT;

			// ドリフトの向きによって回転量を変える。
			if (isDriftRight_ && operation.handleDriveRate_ < 0.1f) {
				operation.handleDriveRate_ = 0.1f;
			}
			if (!isDriftRight_ && -0.1f < operation.handleDriveRate_) {
				operation.handleDriveRate_ = -0.1f;
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
		nowFrameInputLeftStickHori = handleAmount * operation.handleDriveRate_;
		rotY_ += handleAmount * operation.handleDriveRate_;

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

	// ドリフトの入力情報を保存する。
	isInputLTPrev_ = isInputLT_;
	isInputLT_ = operation.isDrift_;

	// ドリフトジャンプのフラグを保存しておく。
	isDriftJumpPrev_ = isDriftJump_;

	// ドリフトボタンの入力がトリガーだったら。
	bool triggerDriftBottom = !isInputLTPrev_ && isInputLT_;
	bool notJump = !isDriftJump_ && driftJumpSpeed_ <= 0.0f;
	bool isOnGround = onGround_ || IsBeforeStart;	// 設置していたら ゲームが始まっていない場合、キャラは空中に浮いているので、接地判定を取る。
	if (triggerDriftBottom && notJump && isOnGround) {

		isDriftJump_ = true;
		driftJumpVec_ = upVec_;
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
			boostTimer_ = DRIFT_BOOST[driftLevel];
		}

	}

	// バグ回避でLTが押されていなかったらドリフトを解除する。 ドリフト解除の正規の処理はこの処理の上にあるので通常は問題ない。
	// 開始前の状態でもドリフトを解除する。
	if (!isInputLT_ || IsBeforeStart) {

		isDrift_ = false;
		driftTimer_ = 0;

	}

	// このキャラがゴーストだった場合、アイテム取得命令が出たらアイテムを取得する。
	if (charaID_ == CHARA_ID::GHOST && operation.isGetItem_) {

		item_ = std::make_shared<BoostItem>();
		item_->Generate(playerModel_.carBodyInsIndex_);

	}

	// アイテムを操作
	isUseItem_ = false;
	if (operation.isUseItemTrigger_ && item_.operator bool()) {

		if (item_->GetItemID() == BaseItem::ItemID::BOOST) {

			boostSpeed_ = MAX_BOOST_SPEED;
			boostTimer_ = 30;
			isUseItem_ = true;
			item_.reset();

		}
		else {

			item_->Use(rotY_, static_cast<int>(ShellItem::PARAM_ID::BEHIND));

		}


	}

	if (operation.isUseItemRelease_ && item_.operator bool()) {

		isUseItem_ = true;

		if (isShotBehind_) {
			item_->Use(rotY_, static_cast<int>(ShellItem::PARAM_ID::BEHIND_THROW));
		}
		else {
			item_->Use(rotY_, static_cast<int>(ShellItem::PARAM_ID::FORWARD_THROW));
		}
		item_.reset();


	}

	// ジャンプアクションのトリガー判定。
	isJumpActionTrigger_ = operation.isJumpActionTrigger_;

	// 入力を保存する。
	handleAmount_ = operation.handleDriveRate_;

}

void Character::Move(const bool& IsBeforeStart)
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
	pos_ += forwardVec_ * (speed_ + boostSpeed_ + jumpBoostSpeed_);

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

	// ゲームが開始したトリガーのときにアクセルがふまれていたら加速。
	bool isGameStartRelease = isBeforeStartPrev_ && !IsBeforeStart;
	if (isGameStartRelease && isAccel_) {

		boostTimer_ = 20;
		boostSpeed_ = MAX_BOOST_SPEED;

	}

	// 座標に重力を加算する。
	pos_ += Vec3(0, -1, 0) * gravity_;

	// 下ベクトルを車の回転行列分回転させる。
	bottomVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

}

void Character::UpdateDrift(const bool& IsBeforeStart)
{

	/*===== ドリフトに関する更新処理 =====*/

	// ドリフトジャンプしていたら。
	if (isDriftJump_) {

		// 座標を移動させる。
		pos_ += driftJumpVec_ * driftJumpSpeed_;
		prevPos_ += driftJumpVec_ * driftJumpSpeed_;

		// ドリフト量を減らす。
		driftJumpSpeed_ -= SUB_DRIFT_JUMP_SPEED;
		if (driftJumpSpeed_ < 0.0f) {

			driftJumpSpeed_ = 0.0f;

		}

		// スティック入力があったら。
		if (0.1f < std::fabs(handleAmount_)) {

			isDrift_ = true;
			isDriftRight_ = 0.0f < handleAmount_;

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
	if (isDriftJump_ && onGround_ && isNotTriggerLT) {

		isDriftJump_ = false;
		driftJumpSpeed_ = 0.0f;

	}

}

void Character::CheckHit(std::weak_ptr<BaseStage> StageData)
{

	/*===== 当たり判定 =====*/

	// 当たり判定関数に入れる値を設定。
	BaseStage::ColliderInput input;
	input.targetInsIndex_ = playerModel_.carBodyInsIndex_;
	input.targetOBB_ = obb_;
	input.targetPos_ = pos_;
	input.targetUpVec_ = upVec_;
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
		boostTimer_ = 20;

	}
	if (output.isHitGoal_) {

		// ゴール
		if (isPassedMiddlePoint_) {
			isPassedMiddlePoint_ = false;
			++rapCount_;

			// 三周以上にならないようにする。
			if (3 <= rapCount_) rapCount_ = 3;

		}

		operationObject_->OnGoal();

	}
	if (output.isHitMiddlePoint_) {

		// 中間地点との当たり判定
		isPassedMiddlePoint_ = true;

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

		// 回転を更新。
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, output.matRot_);


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

		// 回転を更新。
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, output.matRot_);

	}
	// ゴーストだったら当たり判定を飛ばす。
	if (output.isHitItemBox_ && charaID_ != CHARA_ID::GHOST && !item_.operator bool()) {

		// アイテムを取得したフラグ
		isGetItem_ = true;

		if (GameSceneMode::Ins()->id_ == GameSceneMode::MODE_ID::WRITE_GHOST || GameSceneMode::Ins()->id_ == GameSceneMode::MODE_ID::GHOST) {

			item_ = std::make_shared<BoostItem>();
			item_->Generate(playerModel_.carBodyInsIndex_);

		}
		else {

			// ランダムでアイテムを生成する。
			int random = FHelper::GetRand(0, 1000);

			//if (random % 2 == 0) {

			item_ = std::make_shared<BoostItem>();
			item_->Generate(playerModel_.carBodyInsIndex_);

			/*}
			else {

				item_ = std::make_shared<ShellItem>();
				item_->Generate(playerModel_.carBodyInsIndex_);

			}*/

		}

	}
	else {

		// アイテムを取得しなかった
		isGetItem_ = false;

	}

	// 段差加速オブジェクトと当たっていて、ジャンプアクションボタンを押していたら。
	isJumpAction_ = false;
	if (output.isHitStepBoostGimmick_ && isJumpActionTrigger_) {

		// 加速させる。
		jumpBoostSpeed_ = JUMP_BOOST_SPEED;

		isJumpAction_ = true;

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



	jumpBoostSpeed_ -= SUB_JUMP_BOOST_SPEED;
	if (jumpBoostSpeed_ < 0) {

		jumpBoostSpeed_ = 0;

	}


	if (Input::Ins()->IsKeyTrigger(DIK_O)) {

		++rapCount_;

	}

}

void Character::InclineCarBody()
{

	/*===== 車体を傾ける処理 =====*/

	// BODYの回転行列を保存。
	defBodyMatRot_ = PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_);

	// 空中に居たら。
	if (!onGround_ && !isDriftJump_) {

		// 移動した方向ベクトル
		Vec3 movedVec = (pos_ - prevPos_).GetNormal();

		if (!std::isnan(movedVec.x_) && charaID_ == CHARA_ID::P1) {

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
				PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, quaternionMat);

				// 上ベクトルを基準としたクォータニオンを求める。
				Vec3 normal_ = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
				DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), rotY_);

				// クォータニオンを行列に治す。
				DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

				// プレイヤーを回転させる。
				PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, upQuaternionMat);

			}
		}

	}

	// ブースト時の回転。
	if (0 < boostSpeed_) {

		float boostRate = boostSpeed_ / MAX_BOOST_SPEED;
		const float MAX_ROT = 0.7f;

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

		Vec3 horiVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
		Vec3 forwardVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
		Vec3 upVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

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
	PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, mat);


	// ゲーム終了時演出用の回転
	if (isGameFinish_) {

		// イージングが終わったら。
		if (0.7f <= gameFinishEasingTimer_) {

			// 回転軸を求める。
			Vec3 axisOfRevolution = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

			// クォータニオンを求める。
			DirectX::XMVECTOR gameFinishRotQ = DirectX::XMQuaternionRotationAxis(axisOfRevolution.ConvertXMVECTOR(), gameFinishRotStopAmount_);

			// クォータニオンから回転行列を求める。
			DirectX::XMMATRIX gameFinishRotMat = DirectX::XMMatrixRotationQuaternion(gameFinishRotQ);

			// 回転させる。
			PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, gameFinishRotMat);

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
			Vec3 axisOfRevolution = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

			// クォータニオンを求める。
			DirectX::XMVECTOR gameFinishRotQ = DirectX::XMQuaternionRotationAxis(axisOfRevolution.ConvertXMVECTOR(), easingAmount);

			// クォータニオンから回転行列を求める。
			DirectX::XMMATRIX gameFinishRotMat = DirectX::XMMatrixRotationQuaternion(gameFinishRotQ);

			// 回転させる。
			PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, gameFinishRotMat);

			gameFinishRotStopAmount_ = easingAmount;
			gameFinishRotStopReturnAmount_ = 0;

		}

	}

}

void Character::EngineSineWave()
{

	/*===== エンジンの上下 =====*/

	// サイン波用のタイマーを更新する。
	engineWaveTimer_ += 1.5f;

	// 変化量
	float waveLength = 0;

	// 開始前でアクセルがふまれていたら激しく動かす。
	if (isAccel_ && isBeforeStartPrev_) {

		waveLength = BEFORE_START_WAVE_LENGTH_ACCELL;

	}
	// [地上にいて止まっていたら] or [開始前だったら] エンジンを動かす。
	else if ((onGround_ && speed_ <= 0.1f && boostSpeed_ <= 0.1f) || isBeforeStartPrev_) {

		waveLength = BEFORE_START_WAVE_LENGTH_DEF;

	}
	else if (onGround_) {

		waveLength = BEFORE_START_WAVE_LENGTH_RUN;

	}

	// サイン波を計算する。
	engineWaveAmount_ = std::sin(engineWaveTimer_) * waveLength;

	// 動かす。
	pos_.y_ += engineWaveAmount_;

}

void Character::UpdateGameFinish()
{

	/*===== ゲーム終了時の更新処理 =====*/

	// ゲームが終わっていなかったら処理を飛ばす。
	if (!isGameFinish_) return;

	// ゲーム終了トリガーの場合。
	bool isFinishTrigger = !isPrevGameFinish_ && isGameFinish_;
	if (isFinishTrigger) {

		// 各変数を設定。
		boostTimer_ = 0;
		speed_ = MAX_SPEED;
		gameFinishTriggerRotY_ = rotY_;
		gameFinishEasingTimer_ = 0;
		gameFinishTruggerForardVec_ = forwardVec_;
		gameFinishTriggerMatRot_ = PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_);

		// 演出でどちらにドリフトさせるかを取得。
		isGameFinishDriftLeft_ = handleAmount_ < 0;
		if (handleAmount_ == 0) {

			isGameFinishDriftLeft_ = true;

		}

		return;

	}


	/*-- ゲーム終了時の更新処理本編 --*/

	// 正面ベクトルを更新する。
	forwardVec_ = gameFinishTruggerForardVec_;

	// ゲーム終了時のイージングタイマーを更新してオーバーフローしないようにする。
	gameFinishEasingTimer_ = FHelper::Saturate(gameFinishEasingTimer_ + GAME_FINISH_EASING_TIMER);

	// イージング量を求める。
	float easingAmount = FEasing::EaseOutQuint(gameFinishEasingTimer_);

	// 移動量を更新する。
	speed_ = MAX_SPEED * (1.0f - easingAmount);

	// 回転を更新する。
	const float ADD_ROT_Y = DirectX::XM_PIDIV2 * (isGameFinishDriftLeft_ ? -1.0f : 1.0f);
	rotY_ = gameFinishTriggerRotY_;
	rotY_ += ADD_ROT_Y * easingAmount;

}

void Character::UpdateDriftParticle(RayConstBufferData& ConstBufferData, const bool& IsGameFinish, const bool& IsBeforeStart)
{

	/*===== ドリフトパーティクルの更新処理 =====*/

	// ブーストするタイマーが一定以上だったら加速し続ける。
	if (0 < boostTimer_) {

		--boostTimer_;

		boostSpeed_ = MAX_BOOST_SPEED;

		// 加速の割合を求める。
		float boostRate = boostSpeed_ / MAX_BOOST_SPEED;

		// 煙の大きさを決める。
		bool IsSmokeBig = 0.5f < boostRate;

		// 設置していたら煙を生成。
		if (onGround_) {
			Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
			DriftParticleMgr::Ins()->GenerateSmoke(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * 30.0f, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, IsSmokeBig, DriftParticleMgr::DELAY_ID::DASH);
			driftVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
			DriftParticleMgr::Ins()->GenerateSmoke(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * 30.0f, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, IsSmokeBig, DriftParticleMgr::DELAY_ID::DASH);
		}

		// 設置していて移動速度が一定以上だったら炎を生成。
		if (15.0f < boostSpeed_) {
			DriftParticleMgr::Ins()->GenerateFire(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData);
		}

	}

	// 開始前でアクセルを踏んでいたら。
	if (isAccel_ && IsBeforeStart) {

		// 煙を出すタイマーを更新。
		++beforeStartSmokeTimer_;
		if (BEFORE_START_SMOKE_TIMER < beforeStartSmokeTimer_) {

			beforeStartSmokeTimer_ = 0;

			Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
			// 左右に散らす。
			Vec3 generatePos = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * FHelper::GetRand(-2, 2);
			// 後ろ方向に持ってくる。
			generatePos += -forwardVec_ * 20.0f;
			DriftParticleMgr::Ins()->GenerateSmoke(generatePos, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, false, DriftParticleMgr::DELAY_ID::NONE_DELAY, -forwardVec_);

		}

	}

	// 着地した瞬間だったら。
	bool isJumpDriftRelease = (isDriftJumpPrev_ && !isDriftJump_);
	bool onGroundTrigger = (!onGroundPrev_ && onGround_);
	if ((isJumpDriftRelease || onGroundTrigger) && !IsBeforeStart && !isGameFinish_) {

		// 三回ランダムに位置をずらして生成する。
		Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
		for (int index = 0; index < 3; ++index) {

			DriftParticleMgr::Ins()->GenerateSmoke(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * 30.0f, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, false, DriftParticleMgr::DELAY_ID::NONE_DELAY);

		}
		// 三回ランダムに位置をずらして生成する。
		for (int index = 0; index < 3; ++index) {
			DriftParticleMgr::Ins()->GenerateSmoke(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) - driftVec * 30.0f, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, false, DriftParticleMgr::DELAY_ID::NONE_DELAY);

		}

	}

	// ドリフト中は煙を出す。
	if (isDrift_ && onGround_) {


		// 現在のレベル。
		int nowLevel = 0;
		for (auto& index : DRIFT_TIMER) {

			if (driftTimer_ < index) continue;

			nowLevel = static_cast<int>(&index - &DRIFT_TIMER[0]);

		}

		// 現在のドリフトレベルが1以上だったらパーティクルとオーラを出す。
		if (1 <= nowLevel) {

			if (!DriftParticleMgr::Ins()->IsAuraGenerated(charaIndex_)) {
				DriftParticleMgr::Ins()->GenerateAura(charaIndex_, playerModel_.carBehindTireInsIndex_, static_cast<int>(DriftParticle::ID::AURA_BIG), isDriftRight_, 2 <= nowLevel, ConstBufferData);
				DriftParticleMgr::Ins()->GenerateAura(charaIndex_, playerModel_.carBehindTireInsIndex_, static_cast<int>(DriftParticle::ID::AURA_SMALL), isDriftRight_, 2 <= nowLevel, ConstBufferData);
			}

			// レートを求める。
			float rate = 0;
			rate = FHelper::Saturate(static_cast<float>(driftTimer_) / static_cast<float>(DRIFT_TIMER[0]));

			// パーティクルを生成する。
			DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInsIndex_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), rate, false, DriftParticleMgr::DELAY_ID::DELAY1, ConstBufferData);

			// レベルが上った瞬間だったら一気にパーティクルを生成する。
			if (driftTimer_ == DRIFT_TIMER[0] || driftTimer_ == DRIFT_TIMER[1] || driftTimer_ == DRIFT_TIMER[2]) {

				DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInsIndex_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1, ConstBufferData);
				DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInsIndex_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1, ConstBufferData);
				DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInsIndex_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1, ConstBufferData);
				DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInsIndex_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1, ConstBufferData);
				DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInsIndex_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1, ConstBufferData);

				// オーラを一旦破棄
				DriftParticleMgr::Ins()->DestroyAura(charaIndex_);

			}

		}

		// 煙を出す。
		Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(isDriftRight_ ? -1.0f : 1.0f, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
		DriftParticleMgr::Ins()->GenerateSmoke(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * 30.0f, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, nowLevel < 1, DriftParticleMgr::DELAY_ID::DEF);



	}
	else {

		// オーラを破棄 関数内に二重解放対策の条件式がある。
		DriftParticleMgr::Ins()->DestroyAura(charaIndex_);

	}

	// ゲームが終了していたら イージングが終了していなかったら。
	if (IsGameFinish && gameFinishEasingTimer_ < 0.9f) {

		// 煙を出す。
		Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(isGameFinishDriftLeft_ ? -1.0f : 1.0f, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
		DriftParticleMgr::Ins()->GenerateSmoke(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * 30.0f, gameFinishTriggerMatRot_, ConstBufferData, false, DriftParticleMgr::DELAY_ID::NONE_DELAY);
		DriftParticleMgr::Ins()->GenerateSmoke(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * 30.0f, gameFinishTriggerMatRot_, ConstBufferData, false, DriftParticleMgr::DELAY_ID::NONE_DELAY);
		DriftParticleMgr::Ins()->GenerateSmoke(PolygonInstanceRegister::Ins()->GetWorldPos(isGameFinishDriftLeft_ ? playerModel_.carLeftTireInsIndex_ : playerModel_.carRightTireInsIndex_), gameFinishTriggerMatRot_, ConstBufferData, false, DriftParticleMgr::DELAY_ID::NONE_DELAY);


	}

	// ジャンプアクションのパーティクルを生成。
	if (isJumpAction_ || Input::Ins()->IsKeyTrigger(DIK_M)) {

		DriftParticleMgr::Ins()->GenerateJumpEffect(playerModel_.carBodyInsIndex_, ConstBufferData);

	}

}
