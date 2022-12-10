#include "Character.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "Input.h"
#include "FHelper.h"
#include "BLAS.h"
#include "DriftParticleMgr.h"
#include "Camera.h"
#include "HitGroupMgr.h"
#include "OBB.h"
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
#include "MugenStage.h"
#include "RadialBlur.h"
#include "SceneMgr.h"
#include "BaseScene.h"
#include "CharacterInclineBody.h"

Character::Character(CHARA_ID CharaID, int CharaIndex, int Level, int CharaPersonality)
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
		defPos_ = PLAYER_DEF_POS;

		// 車のモデルをロード
		playerModel_.Load(PlayerModel::COLOR::RED, PlayerModel::CHARA::PLAYER);

	}
	else if (charaID_ == CHARA_ID::P1_WGHOST) {

		operationObject_ = std::make_shared<PlayerOperationObject>(0, true, this);

		// 初期位置を設定。
		defPos_ = GHOST_DEF_POS[0];

		// 車のモデルをロード
		playerModel_.Load(PlayerModel::COLOR::RED, PlayerModel::CHARA::PLAYER);

	}
	else if (charaID_ == CHARA_ID::AI1) {

		operationObject_ = std::make_shared<FirstAIOperationObject>(static_cast<int>(FirstAIWayPointMgr::WAYPOINT_OFFSET::CENTER), Level, CharaPersonality);

		// 初期位置を設定。
		defPos_ = GHOST_DEF_POS[0];

		// 車のモデルをロード
		playerModel_.Load(PlayerModel::COLOR::BLUE, PlayerModel::CHARA::AI);

	}
	else if (charaID_ == CHARA_ID::GHOST) {

		std::string filePath = "Resource/Game/CharaGhostData/Ghost_";
		std::string dottxt = ".txt";
		std::string number = std::to_string(Level);

		operationObject_ = std::make_shared<GhostOperationObject>(filePath + number + dottxt);

		// 初期位置を設定。
		defPos_ = GHOST_DEF_POS[0];

		// 車のモデルをロード
		playerModel_.Load(PlayerModel::COLOR::BLUE, PlayerModel::CHARA::GHOST);

	}

	// タイヤをセット。
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carRightTireFrameInstance_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carRightTireInstance_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carLeftTireFrameInstance_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carLeftTireInstance_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carBehindTireFrameInstance_, true));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carBehindTireInstance_, true));

	// ロケットを生成。
	rocketBlas_[0] = BLASRegister::Ins()->GenerateObj("Resource/Game/UI/", "RocketHead.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	rocketBlas_[1] = BLASRegister::Ins()->GenerateObj("Resource/Game/UI/", "RocketBody.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	rocketBlas_[2] = BLASRegister::Ins()->GenerateObj("Resource/Game/UI/", "RocketLegs.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	rocketBlas_[3] = BLASRegister::Ins()->GenerateObj("Resource/Game/UI/", "RocketWindow.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	for (int index = 0; index < 4; ++index) {
		rocketIns_[index] = PolygonInstanceRegister::Ins()->CreateInstance(rocketBlas_[index], PolygonInstanceRegister::DEF);
		rocketIns_[index].lock()->ChangeScale(Vec3(30, 30, 30));
		rocketBlas_[index].lock()->ChangeMapTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/UI/metalness2.png"), BLAS::MAP_PARAM::SPECULAR);
	}

	// 車との当たり判定用のチェックボックスをロード
	hitBoxBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Car/collision/", "carHitBox.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], false, true);
	hitBox_ = PolygonInstanceRegister::Ins()->CreateInstance(hitBoxBlas_, PolygonInstanceRegister::DEF, true);
	PolygonInstanceRegister::Ins()->NonDisplay(hitBox_);

	// ラップ数関係
	rapCount_ = 0;
	isPassedMiddlePoint_ = 0;

	// 座標関係
	pos_ = defPos_;
	prevPos_ = pos_;

	// 方向ベクトル関係
	forwardVec_ = Vec3(0, 0, -1);
	bottomVec = Vec3(0, -1, 0);
	upVec_ = Vec3(0, 1, 0);

	// サイズ、回転、移動関係
	size_ = Vec3(50, 30, 50);
	rotY_ = DEF_ROTY;
	speed_ = 0;
	gravity_ = 0;
	jumpBoostSpeed_ = 0;

	// ドリフト関係
	isDrift_ = false;
	isTireMask_ = false;
	isDriftJumpPrev_ = false;

	beforeStartSmokeTimer_ = 0;
	engineWaveTimer_ = 0;
	engineWaveAmount_ = 0;
	boostSpeed_ = 0;
	returnDefPosTimer_ = 0;
	boostTimer_ = 0;
	onGround_ = true;
	isBeforeStartPrev_ = false;
	onGroundPrev_ = false;
	isPrevGameFinish_ = false;
	onGrass_ = false;
	isGameFinish_ = false;
	isGetItem_ = false;
	isUseItem_ = false;
	IsTurningIndicatorRed_ = false;
	isDisplayRocket_ = false;
	turningIndicatorTimer_ = 0;
	canNotMoveTimer_ = 0;
	driftTimer_ = 0;
	defBodyMatRot_ = DirectX::XMMatrixIdentity();
	cameraForwardVec_ = forwardVec_;

	rocketEasingTimer_ = 0.0f;
	rocketAddRotationY_ = 0.0f;
	rocketRotationY_ = 0.0f;

	// キャラクターを回転させる処理をまとめたクラスを生成。
	inclineBody_ = std::make_shared<CharacterInclineBody>();
	inclineBody_->Init();

	// OBBを生成。
	obb_ = std::make_shared<OBB>();
	obb_->Setting(playerModel_.carBodyBlas_, playerModel_.carBodyInstance_);

	if (SceneMgr::Ins()->nowScene_ == BaseScene::SCENE_ID::GAME) {

		leftTailLamp_ = std::make_shared<TailLampMgr>();
		rightTailLamp_ = std::make_shared<TailLampMgr>();

	}

}

void Character::Init()
{

	/*===== 初期化処理 =====*/

	// 初期位置を設定。
	if (charaID_ == CHARA_ID::P1) {
		defPos_ = PLAYER_DEF_POS;
	}
	else if (charaID_ == CHARA_ID::P1_WGHOST) {
		defPos_ = GHOST_DEF_POS[0];
	}
	else if (charaID_ == CHARA_ID::AI1) {
		defPos_ = GHOST_DEF_POS[0];
	}
	else if (charaID_ == CHARA_ID::GHOST) {
		defPos_ = GHOST_DEF_POS[0];
	}

	rapCount_ = 0;
	isPassedMiddlePoint_ = 0;
	pos_ = defPos_;
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
	playerModel_.carBodyInstance_.lock()->ChangeRotate(Vec3(0, 0, 0));
	cameraForwardVec_ = forwardVec_;
	rocketEasingTimer_ = 0.0f;
	rocketAddRotationY_ = 0.0f;
	rocketRotationY_ = 0.0f;
	isDisplayRocket_ = false;

	// キャラクターの回転に関する処理を初期化。
	inclineBody_->Init();

	playerModel_.Delete();

	DriftParticleMgr::Ins()->Init();
	// 臨時のバグ対策です。 最初の一回目のドリフトのときのみオーラが出ないので、ここで一回生成しておく。
	DriftParticleMgr::Ins()->GenerateAura(charaIndex_, playerModel_.carBehindTireInstance_, static_cast<int>(DriftParticle::ID::AURA_BIG), isDriftRight_, 2 <= 0);
	DriftParticleMgr::Ins()->GenerateAura(charaIndex_, playerModel_.carBehindTireInstance_, static_cast<int>(DriftParticle::ID::AURA_SMALL), isDriftRight_, 2 <= 0);

	DriftParticleMgr::Ins()->DestroyAura(charaIndex_);

	if (SceneMgr::Ins()->nowScene_ == BaseScene::SCENE_ID::GAME) {

		leftTailLamp_->Init();
		rightTailLamp_->Init();

	}

}

void Character::Update(std::weak_ptr<BaseStage> StageData, std::vector<std::shared_ptr<Character>> CharaData, bool IsBeforeStart, bool IsGameFinish)
{


	/*===== 更新処理 =====*/


	// ゲーム終了フラグを更新。
	isPrevGameFinish_ = isGameFinish_;
	isGameFinish_ = IsGameFinish;

	// エンジンの動きで動いた分を打ち消す。
	pos_.y_ -= engineWaveAmount_;

	// ブースト時の回転を打ち消す。
	playerModel_.carBodyInstance_.lock()->ChangeRotate(defBodyMatRot_);

	// 入力処理
	Input(IsBeforeStart);

	// ドリフトに関する更新処理
	UpdateDrift();

	// 移動処理
	Move(IsBeforeStart);

	// 当たり判定
	CheckHit(StageData, CharaData);

	// 開始前だったら
	if (IsBeforeStart) {

		// プレイヤーを強制的に初期位置に戻す。
		pos_.x_ = defPos_.x_;
		pos_.z_ = defPos_.z_;

	}

	// エンジンの動き
	EngineSineWave();

	// ゲーム終了時の更新処理
	UpdateGameFinish();

	// 座標を更新。
	playerModel_.carBodyInstance_.lock()->ChangeTrans(pos_);

	// 車体を傾ける処理。
	InclineCarBody();

	// 空中にいるときは初期地点まで戻るタイマーを更新。地上に要るときはタイマーを初期化。
	if (onGround_) {

		returnDefPosTimer_ = 0;

	}
	else {

		++returnDefPosTimer_;

		if (RETURN_DEFPOS_TIMER < returnDefPosTimer_) {

			pos_ = defPos_;
			rotY_ = -0.367411435f;
			playerModel_.carBodyInstance_.lock()->ChangeTrans(defPos_);
			playerModel_.carBodyInstance_.lock()->ChangeRotate(Vec3(0, rotY_, 0));
			forwardVec_ = Vec3(0, 0, -1);
			upVec_ = Vec3(0, 1, 0);
			returnDefPosTimer_ = 0;

			// キャラクターの回転に関するクォータニオンを初期化。
			inclineBody_->InitQuaternion();

		}

	}

	// 当たり判定用データを更新。
	obb_->SetMat(playerModel_.carBodyInstance_);
	hitBox_.lock()->ChangeRotate(playerModel_.carBodyInstance_.lock()->GetRotate());
	hitBox_.lock()->ChangeScale(playerModel_.carBodyInstance_.lock()->GetScale());
	hitBox_.lock()->ChangeTrans(playerModel_.carBodyInstance_.lock()->GetTrans());

	// 座標を保存。
	prevPos_ = pos_;

	// タイヤを更新する。
	for (auto& index : tires_) {

		index->Update();

	}

	// ドリフトパーティクルの更新処理。
	UpdateDriftParticle(IsGameFinish, IsBeforeStart);

	// 移動できないタイマーを更新する。
	if (0 < canNotMoveTimer_) {

		--canNotMoveTimer_;

		// イージング量を求める。
		float easingAmount = static_cast<float>(canNotMoveTimer_) / static_cast<float>(CAN_NOT_MOVE_TIMER_SHELL_HIT);

		easingAmount = easingAmount * easingAmount * easingAmount * easingAmount * easingAmount;

		rotY_ = shellHitRot_ + easingAmount * DirectX::XM_2PI;

	}
	else {

		cameraForwardVec_ = forwardVec_;

	}

	// ブースト量が一定以上だったら集中線を出す。
	isConcentrationLine_ = (MAX_BOOST_SPEED / 2.0f < boostSpeed_) || (JUMP_BOOST_SPEED / 2.0f < jumpBoostSpeed_);

	// ブーストの割合に応じてラジアルブラーをかける。
	if (charaID_ == CHARA_ID::P1) {

		RadialBlur::Ins()->SetBlurPower(FHelper::Saturate(boostSpeed_ / MAX_BOOST_SPEED));

		//_RPTFN(_CRT_WARN, "%f\n", boostSpeed_ / MAX_BOOST_SPEED);

	}

	// 接地フラグを保存。
	onGroundPrev_ = onGround_;

	// ゲーム開始前フラグを保存。
	isBeforeStartPrev_ = IsBeforeStart;


	// ロケットをカメラの左上に移動させる。
	Vec3 nowRocketPos = rocketIns_[0].lock()->GetPos();
	Vec3 rocketPos = GetPos();
	rocketPos -= forwardVec_ * 60.0f;
	rocketPos -= upVec_ * 20.0f;
	// 補間する。
	if (charaID_ == CHARA_ID::P1) {
		nowRocketPos += (rocketPos - nowRocketPos) / 3.0f;
	}
	else {
		nowRocketPos += (rocketPos - nowRocketPos);
	}
	for (auto& index : rocketIns_) {

		index.lock()->ChangeTrans(nowRocketPos);

	}

	// 回転量を本来の値に補間する。
	const float ADD_ROTATION_Y = 0.8f;
	const float MIN_ROTATION_Y = 0.05f;
	rocketAddRotationY_ += (MIN_ROTATION_Y - rocketAddRotationY_) / 10.0f;
	// 回転させる。
	rocketRotationY_ += rocketAddRotationY_;

	// ロケットを斜めにする。
	for (auto& index : rocketIns_) {

		// 初期の回転に戻す。
		index.lock()->ChangeRotate(Vec3(0.4f, 0.0f, 0.0f));

		// カメラ上ベクトル方向のクォータニオンを求める。
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(Camera::Ins()->up_.ConvertXMVECTOR(), rocketRotationY_);

		// 求めたクォータニオンを行列に治す。
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// 回転を加算する。
		index.lock()->AddRotate(quaternionMat);

	}

	// アイテムを取得した瞬間や使った瞬間にイージングタイマーを初期化。
	bool prevFrameDisplayRocket = isDisplayRocket_;
	if (isGetItem_ || isUseItem_) {

		rocketEasingTimer_ = 0.1f;
		rocketAddRotationY_ = ADD_ROTATION_Y;

		if (!isDisplayRocket_) {
			isDisplayRocket_ = true;
		}

	}

	// アイテムのロケットのサイズを更新。
	const float MAX_SCALE = 12.0f;
	rocketEasingTimer_ += 0.06f;
	if (1.0f < rocketEasingTimer_) rocketEasingTimer_ = 1.0f;
	if (GetIsItem()) {

		// イージング量を求める。
		float easingAmount = FEasing::EaseOutQuint(rocketEasingTimer_);
		float scale = MAX_SCALE * easingAmount;
		if (scale <= 0) scale = 0.01f;
		for (auto& index : rocketIns_) {
			index.lock()->ChangeScale(Vec3(scale, scale, scale));
		}

	}
	else {

		// イージング量を求める。
		float easingAmount = FEasing::EaseInQuint(rocketEasingTimer_);
		float scale = MAX_SCALE - MAX_SCALE * easingAmount;
		if (scale <= 0) scale = 0.01f;
		for (auto& index : rocketIns_) {
			index.lock()->ChangeScale(Vec3(scale, scale, scale));
		}
		if (isDisplayRocket_ && scale <= 0) {
			isDisplayRocket_ = false;
		}

	}

	// 描画始まったトリガーだったら描画する。
	if (!prevFrameDisplayRocket && isDisplayRocket_) {
		for (auto& index : rocketIns_) {
			PolygonInstanceRegister::Ins()->Display(index.lock()->GetInstanceIndex());
		}
	}

	// 描画終わったトリガーだったら描画を消す。
	if ((prevFrameDisplayRocket && !isDisplayRocket_) || (!isDisplayRocket_ && rocketIns_[0].lock()->GetScaleVec3().x_ <= 0)) {
		for (auto& index : rocketIns_) {
			PolygonInstanceRegister::Ins()->NonDisplay(index.lock()->GetInstanceIndex());
		}
	}

	// ゲームシーンでのみこの更新処理を行う。
	if (SceneMgr::Ins()->nowScene_ == BaseScene::SCENE_ID::GAME) {

		// 画面内に収まっているか。
		bool inScreen = FHelper::CheckInScreen(GetPos(), 100.0f, 100.0f, Camera::Ins()->matView_, Camera::Ins()->matPerspective_);

		const float TAILLAMP_OFFSET_SPEED = 8.0f;	// テールランプを出すスピード
		if (TAILLAMP_OFFSET_SPEED <= jumpBoostSpeed_ && inScreen) {

			// テールランプを生成。
			static const int VERTEX_SIZE = 4;
			{
				// 右後ろのテールランプ
				DirectX::XMMATRIX matWorld = playerModel_.carRightLightInstance_.lock()->GetWorldMat();
				std::array<Vec3, VERTEX_SIZE> vertex;
				vertex[0] = FHelper::MulMat(playerModel_.carRightLightBlas_.lock()->GetVertexPos()[10], matWorld);	// ライトの頂点を設定。いずれは各頂点の最小と最大から自動で設定できるようにしたい。
				vertex[1] = FHelper::MulMat(playerModel_.carRightLightBlas_.lock()->GetVertexPos()[11], matWorld);
				vertex[2] = FHelper::MulMat(playerModel_.carRightLightBlas_.lock()->GetVertexPos()[1], matWorld);
				vertex[3] = FHelper::MulMat(playerModel_.carRightLightBlas_.lock()->GetVertexPos()[2], matWorld);
				// 生成
				leftTailLamp_->Generate(vertex, TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"), (jumpBoostSpeed_ - TAILLAMP_OFFSET_SPEED) / (JUMP_BOOST_SPEED - TAILLAMP_OFFSET_SPEED));

			}
			{
				// 左後ろのテールランプ
				DirectX::XMMATRIX matWorld = playerModel_.carLeftLightInstance_.lock()->GetWorldMat();
				std::array<Vec3, VERTEX_SIZE> vertex;
				vertex[3] = FHelper::MulMat(playerModel_.carLeftLightBlas_.lock()->GetVertexPos()[3], matWorld);
				vertex[2] = FHelper::MulMat(playerModel_.carLeftLightBlas_.lock()->GetVertexPos()[11], matWorld);
				vertex[1] = FHelper::MulMat(playerModel_.carLeftLightBlas_.lock()->GetVertexPos()[8], matWorld);
				vertex[0] = FHelper::MulMat(playerModel_.carLeftLightBlas_.lock()->GetVertexPos()[2], matWorld);
				// 生成
				rightTailLamp_->Generate(vertex, TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"), (jumpBoostSpeed_ - TAILLAMP_OFFSET_SPEED) / (JUMP_BOOST_SPEED - TAILLAMP_OFFSET_SPEED));

			}

		}

		// テールランプを更新。
		leftTailLamp_->Update();
		rightTailLamp_->Update();

	}

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
			InputRayData.targetPolygonData_ = PolygonInstanceRegister::Ins()->GetMeshCollisionData(BaseStageData.lock()->stageObjectMgr_->GetInstanceIndex(1));

			// 戻り地保存用
			Vec3 ImpactPos;
			Vec3 HitNormal;
			Vec2 HitUV;
			float HitDistance;

			// 左前タイヤ
			InputRayData.rayPos_ = playerModel_.carRightTireInstance_.lock()->GetWorldPos();
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
			InputRayData.rayPos_ = playerModel_.carBehindTireInstance_.lock()->GetWorldPos();
			InputRayData.rayPos_ += FHelper::MulRotationMatNormal(Vec3(1, 0, 0), playerModel_.carBodyInstance_.lock()->GetRotate()) * 20.0f;
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
			InputRayData.targetPolygonData_ = PolygonInstanceRegister::Ins()->GetMeshCollisionData(BaseStageData.lock()->stageObjectMgr_->GetInstanceIndex(1));

			// 戻り地保存用
			Vec3 ImpactPos;
			Vec3 HitNormal;
			Vec2 HitUV;
			float HitDistance;

			// 左前タイヤ
			InputRayData.rayPos_ = playerModel_.carLeftTireInstance_.lock()->GetWorldPos();
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
			InputRayData.rayPos_ = playerModel_.carBehindTireInstance_.lock()->GetWorldPos();
			InputRayData.rayPos_ += FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), playerModel_.carBodyInstance_.lock()->GetRotate()) * 20.0f;
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
	InputRayData.targetPolygonData_ = PolygonInstanceRegister::Ins()->GetMeshCollisionData(BaseStageData.lock()->stageObjectMgr_->GetInstanceIndex(1));

	// 戻り地保存用
	Vec3 ImpactPos;
	Vec3 HitNormal;
	Vec2 HitUV;
	float HitDistance;
	bool isHit = false;

	// 右ドリフトしていたらこの処理は通さない。
	if (!(isDrift_ && !isDriftRight_)) {

		// 左前タイヤ
		InputRayData.rayPos_ = playerModel_.carLeftTireInstance_.lock()->GetWorldPos();
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
		InputRayData.rayPos_ = playerModel_.carRightTireInstance_.lock()->GetWorldPos();
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
	InputRayData.rayPos_ = playerModel_.carRightTireInstance_.lock()->GetWorldPos();
	InputRayData.rayDir_ = bottomVec;

	// 回転した後ろベクトルを求める。
	Vec3 behindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), playerModel_.carBodyInstance_.lock()->GetRotate());
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
	InputRayData.rayPos_ = playerModel_.carLeftTireInstance_.lock()->GetWorldPos();
	InputRayData.rayDir_ = bottomVec;

	// 回転した後ろベクトルを求める。
	behindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), playerModel_.carBodyInstance_.lock()->GetRotate());
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

	return cameraForwardVec_;

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

void Character::Input(bool IsBeforeStart)
{

	/*===== 入力処理 =====*/

	// ゲームが終了している場合は入力を無効化する。 ゲームが終了した最初のFでは入力を取る必要があるので、Prevも比較している。
	if (isGameFinish_ && isPrevGameFinish_) return;

	// 操作オブジェクトからの入力を受け取る。
	BaseOperationObject::OperationInputData operationInputData;
	operationInputData.pos_ = pos_;
	operationInputData.forwradVec_ = forwardVec_;
	if (item_.operator bool()) {
		operationInputData.hasItemID_ = item_->GetItemID();
	}
	operationInputData.isHitJumpBoostGimmick_ = isHitJumpActionGimmick_;
	operationInputData.isPrevFrameDrift_ = isDrift_;
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
	// 甲羅にあたっていたら。
	else if (0 < canNotMoveTimer_) {

		speed_ -= speed_ / 50.0f;

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
		playerModel_.carBodyInstance_.lock()->AddRotate(quaternionMat);
		nowFrameInputLeftStickHori = handleAmount * operation.handleDriveRate_;
		rotY_ += handleAmount * operation.handleDriveRate_;

		// 正面ベクトルを車の回転行列分回転させる。
		forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), playerModel_.carBodyInstance_.lock()->GetRotate());


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

				playerModel_.carRightLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/blackRed.png"));
				playerModel_.carLeftLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));

			}
			else {

				playerModel_.carLeftLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/blackRed.png"));
				playerModel_.carRightLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));

			}

		}
		else {

			playerModel_.carRightLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));
			playerModel_.carLeftLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));

		}

	}
	else {

		// 車のライトの色を元に戻す。
		playerModel_.carRightLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));
		playerModel_.carLeftLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));

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
	bool notJump = !isDriftJump_ && driftJumpSpeed_ <= 0.1f;
	bool isOnGround = onGround_ || IsBeforeStart;	// 設置していたら ゲームが始まっていない場合、キャラは空中に浮いているので、接地判定を取る。
	if (triggerDriftBottom && notJump && isOnGround && canNotMoveTimer_ <= 0) {

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
		item_->Generate(playerModel_.carBodyInstance_);
		isGetItem_ = true;

	}
	else if (charaID_ == CHARA_ID::GHOST) {

		isGetItem_ = false;

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

			shellIndex_ = item_->Use(rotY_, static_cast<int>(ShellItem::PARAM_ID::BEHIND));

		}


	}

	if (operation.isUseItemRelease_ && item_.operator bool()) {

		isUseItem_ = true;

		if (isShotBehind_) {
			item_->Use(rotY_, static_cast<int>(ShellItem::PARAM_ID::BEHIND_THROW));
			timerToSkipShellCollider_ = TIMER_TO_SKIP_SHELL_COLLIDER;
		}
		else {
			item_->Use(rotY_, static_cast<int>(ShellItem::PARAM_ID::FORWARD_THROW));
			timerToSkipShellCollider_ = TIMER_TO_SKIP_SHELL_COLLIDER;
		}
		item_.reset();


	}

	// ジャンプアクションのトリガー判定。
	isJumpActionTrigger_ = operation.isDriftTrigger_;

	// 入力を保存する。
	inclineBody_->SetHandleAmount(operation.handleDriveRate_);

}

void Character::Move(bool IsBeforeStart)
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

	// 移動させる方向。 甲羅にあたっているときはあたった方向に進ませる為。
	Vec3 moveVec = forwardVec_;
	if (0 < canNotMoveTimer_) {
		moveVec = cameraForwardVec_;
	}

	// 座標移動させる。
	pos_ += moveVec * (speed_ + boostSpeed_ + jumpBoostSpeed_);

	// ドリフト時のブースト移動量を0に近づける。
	if (0 < boostSpeed_) {

		boostSpeed_ -= SUB_BOOST_SPEED;

		// デフォルトの移動量をマックスにする。
		speed_ = MAX_SPEED;

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

	// ジャンプ加速量を減速する。
	jumpBoostSpeed_ -= SUB_JUMP_BOOST_SPEED;
	if (jumpBoostSpeed_ < 0) {

		jumpBoostSpeed_ = 0;

	}

	// 座標に重力を加算する。
	pos_ += Vec3(0, -1, 0) * gravity_;

	// 下ベクトルを車の回転行列分回転させる。
	bottomVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), playerModel_.carBodyInstance_.lock()->GetRotate());

}

void Character::UpdateDrift()
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
		if (0.1f < std::fabs(inclineBody_->GetHandleAmount())) {

			isDrift_ = true;
			isDriftRight_ = 0.0f < inclineBody_->GetHandleAmount();

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

#include "BrightnessParam.h"
void Character::CheckHit(std::weak_ptr<BaseStage> StageData, std::vector<std::shared_ptr<Character>> CharaData)
{

	/*===== 当たり判定 =====*/

	// 他の車との当たり判定。
	CheckHitOtherCar(CharaData);

	// 当たり判定関数に入れる値を設定。
	BaseStage::ColliderInput input;
	input.targetInstance_ = playerModel_.carBodyInstance_;
	input.targetOBB_ = obb_;
	input.targetPos_ = pos_;
	input.targetUpVec_ = upVec_;
	input.targetPrevPos_ = prevPos_;
	input.targetRotY_ = rotY_;
	input.targetSize_ = size_;
	input.isInvalidateRotY_ = false;
	input.isPlayer_ = charaID_ == CHARA_ID::P1;

	// 当たり判定関数から返ってくる値。
	BaseStage::ColliderOutput output;

	// 当たり判定を行う。
	output = StageData.lock()->Collider(input);

	// 当たり判定の結果から処理を行う。
	if (output.isHitBoostGimmick_) {

		// ブーストをマックスにする。
		boostSpeed_ = MAX_BOOST_SPEED;
		boostTimer_ = BOOST_GIMMICK_BOOST_TIMER;

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
		playerModel_.carBodyInstance_.lock()->ChangeRotate(output.matRot_);


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
		playerModel_.carBodyInstance_.lock()->ChangeRotate(output.matRot_);

	}
	// ゴーストだったら当たり判定を飛ばす。
	if (output.isHitItemBox_ && charaID_ != CHARA_ID::GHOST && !item_.operator bool()) {

		// アイテムを取得したフラグ
		isGetItem_ = true;

		if (GameSceneMode::Ins()->mode_ == GameSceneMode::MODE::WRITE_GHOST || GameSceneMode::Ins()->mode_ == GameSceneMode::MODE::GHOST) {

			item_ = std::make_shared<BoostItem>();
			item_->Generate(playerModel_.carBodyInstance_);

		}
		else {


			item_ = std::make_shared<BoostItem>();
			item_->Generate(playerModel_.carBodyInstance_);

			//// AI戦だったらランダムでアイテムを生成する。
			//int random = FHelper::GetRand(0, 1);

			//if (random == 0) {
			//	item_ = std::make_shared<BoostItem>();
			//	item_->Generate(playerModel_.carBodyInstance);
			//}
			//else {
			//	item_ = std::make_shared<ShellItem>();
			//	item_->Generate(playerModel_.carBodyInstance);
			//}
			//item_ = std::make_shared<ShellItem>();
			//item_->Generate(playerModel_.carBodyInstance);

		}

	}
	else if (charaID_ != CHARA_ID::GHOST) {

		// アイテムを取得しなかった
		isGetItem_ = false;

	}

	// 段差加速オブジェクトと当たっていて、ジャンプアクションボタンを押していたら。
	isJumpAction_ = false;
	isHitJumpActionGimmick_ = output.isHitStepBoostGimmick_;
	if (output.isHitStepBoostGimmick_ && isJumpActionTrigger_) {

		// 加速させる。
		jumpBoostSpeed_ = JUMP_BOOST_SPEED;

		isJumpAction_ = true;

	}


	// その他の変数を初期化。
	pos_ = output.resultPos_;

	// ゴースト以外だったら。
	if (charaID_ != CHARA_ID::GHOST) {

		int shellIndex = -1;

		if (0 < timerToSkipShellCollider_) {

			--timerToSkipShellCollider_;
			shellIndex = shellIndex_;

		}

		// 甲羅との当たり判定
		bool isHitShell = ShellObjectMgr::Ins()->Collider(obb_, shellIndex);

		if (isHitShell || Input::Ins()->IsKeyTrigger(DIK_P)) {

			canNotMoveTimer_ = CAN_NOT_MOVE_TIMER_SHELL_HIT;
			shellHitRot_ = rotY_;

		}

	}


	// プレイヤーかつ明るさの壁にあたっていたら。
	if (charaID_ == CHARA_ID::P1 && output.isHitBrightnessWall_) {
		BrightnessParam::Ins()->isBright_ = true;
	}
	if (charaID_ == CHARA_ID::P1 && output.isHitDarknessWall_) {
		BrightnessParam::Ins()->isBright_ = false;
	}

}

void Character::InclineCarBody()
{

	/*===== 車体を傾ける処理 =====*/


	// BODYの回転行列を保存。
	defBodyMatRot_ = playerModel_.carBodyInstance_.lock()->GetRotate();

	// 回転に関する処理に必要なデータを詰め込む。
	CharacterInclineBody::InputData inlclineInputData;
	inlclineInputData.boostSpeed_ = boostSpeed_;
	inlclineInputData.isDriftJump_ = isDriftJump_;
	inlclineInputData.isDrift_ = isDrift_;
	inlclineInputData.isPlayer_ = charaID_ == CHARA_ID::P1;
	inlclineInputData.maxBoostSpeed_ = MAX_BOOST_SPEED;
	inlclineInputData.onGround_ = onGround_;
	inlclineInputData.playerModel_ = playerModel_;
	inlclineInputData.pos_ = pos_;
	inlclineInputData.prevPos_ = prevPos_;
	inlclineInputData.rotY_ = rotY_;

	// 車体の回転の処理
	inclineBody_->Update(inlclineInputData);

	// ゲーム終了時演出用の回転
	if (isGameFinish_) {

		// イージングが終わったら。
		if (0.7f <= gameFinishEasingTimer_) {

			// 回転軸を求める。
			Vec3 axisOfRevolution = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), playerModel_.carBodyInstance_.lock()->GetRotate());

			// クォータニオンを求める。
			DirectX::XMVECTOR gameFinishRotQ = DirectX::XMQuaternionRotationAxis(axisOfRevolution.ConvertXMVECTOR(), gameFinishRotStopAmount_);

			// クォータニオンから回転行列を求める。
			DirectX::XMMATRIX gameFinishRotMat = DirectX::XMMatrixRotationQuaternion(gameFinishRotQ);

			// 回転させる。
			playerModel_.carBodyInstance_.lock()->AddRotate(gameFinishRotMat);

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
			Vec3 axisOfRevolution = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), playerModel_.carBodyInstance_.lock()->GetRotate());

			// クォータニオンを求める。
			DirectX::XMVECTOR gameFinishRotQ = DirectX::XMQuaternionRotationAxis(axisOfRevolution.ConvertXMVECTOR(), easingAmount);

			// クォータニオンから回転行列を求める。
			DirectX::XMMATRIX gameFinishRotMat = DirectX::XMMatrixRotationQuaternion(gameFinishRotQ);

			// 回転させる。
			playerModel_.carBodyInstance_.lock()->AddRotate(gameFinishRotMat);

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
	//engineWaveAmount_ = std::sin(engineWaveTimer_) * waveLength;

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
		gameFinishTriggerMatRot_ = playerModel_.carBodyInstance_.lock()->GetRotate();

		// 演出でどちらにドリフトさせるかを取得。
		isGameFinishDriftLeft_ = inclineBody_->GetHandleAmount() < 0;
		if (inclineBody_->GetHandleAmount() == 0) {

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

void Character::UpdateDriftParticle(bool IsGameFinish, bool IsBeforeStart)
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
			Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), playerModel_.carBodyInstance_.lock()->GetRotate());
			DriftParticleMgr::Ins()->GenerateSmoke(playerModel_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, playerModel_.carBodyInstance_.lock()->GetRotate(), IsSmokeBig, DriftParticleMgr::DELAY_ID::DASH);
			driftVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), playerModel_.carBodyInstance_.lock()->GetRotate());
			DriftParticleMgr::Ins()->GenerateSmoke(playerModel_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, playerModel_.carBodyInstance_.lock()->GetRotate(), IsSmokeBig, DriftParticleMgr::DELAY_ID::DASH);
		}

		// 設置していて移動速度が一定以上だったら炎を生成。
		if (15.0f < boostSpeed_) {
			DriftParticleMgr::Ins()->GenerateFire(playerModel_.carBehindTireInstance_.lock()->GetWorldPos(), playerModel_.carBodyInstance_.lock()->GetRotate());
		}

	}

	// 開始前でアクセルを踏んでいたら。
	if (isAccel_ && IsBeforeStart) {

		// 煙を出すタイマーを更新。
		++beforeStartSmokeTimer_;
		if (BEFORE_START_SMOKE_TIMER < beforeStartSmokeTimer_) {

			beforeStartSmokeTimer_ = 0;

			Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), playerModel_.carBodyInstance_.lock()->GetRotate());
			// 左右に散らす。
			Vec3 generatePos = playerModel_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * static_cast<float>(FHelper::GetRand(-2, 2));
			// 後ろ方向に持ってくる。
			generatePos += -forwardVec_ * 20.0f;
			DriftParticleMgr::Ins()->GenerateSmoke(generatePos, playerModel_.carBodyInstance_.lock()->GetRotate(), false, DriftParticleMgr::DELAY_ID::NONE_DELAY, -forwardVec_);

		}

	}

	// 着地した瞬間だったら。
	bool isJumpDriftRelease = (isDriftJumpPrev_ && !isDriftJump_);
	bool onGroundTrigger = (!onGroundPrev_ && onGround_);
	if ((isJumpDriftRelease || onGroundTrigger) && !IsBeforeStart && !isGameFinish_) {

		// 三回ランダムに位置をずらして生成する。
		Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), playerModel_.carBodyInstance_.lock()->GetRotate());
		for (int index = 0; index < 3; ++index) {

			DriftParticleMgr::Ins()->GenerateSmoke(playerModel_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, playerModel_.carBodyInstance_.lock()->GetRotate(), false, DriftParticleMgr::DELAY_ID::NONE_DELAY);

		}
		// 三回ランダムに位置をずらして生成する。
		for (int index = 0; index < 3; ++index) {
			DriftParticleMgr::Ins()->GenerateSmoke(playerModel_.carBehindTireInstance_.lock()->GetWorldPos() - driftVec * 30.0f, playerModel_.carBodyInstance_.lock()->GetRotate(), false, DriftParticleMgr::DELAY_ID::NONE_DELAY);

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
				DriftParticleMgr::Ins()->GenerateAura(charaIndex_, playerModel_.carBehindTireInstance_, static_cast<int>(DriftParticle::ID::AURA_BIG), isDriftRight_, 2 <= nowLevel);
				DriftParticleMgr::Ins()->GenerateAura(charaIndex_, playerModel_.carBehindTireInstance_, static_cast<int>(DriftParticle::ID::AURA_SMALL), isDriftRight_, 2 <= nowLevel);
			}

			// レートを求める。
			float rate = 0;
			rate = FHelper::Saturate(static_cast<float>(driftTimer_) / static_cast<float>(DRIFT_TIMER[0]));

			// パーティクルを生成する。
			DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), rate, false, DriftParticleMgr::DELAY_ID::DELAY1);

			// レベルが上った瞬間だったら一気にパーティクルを生成する。
			if (driftTimer_ == DRIFT_TIMER[0] || driftTimer_ == DRIFT_TIMER[1] || driftTimer_ == DRIFT_TIMER[2]) {

				DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1);
				DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1);
				DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1);
				DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1);
				DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1);

				// オーラを一旦破棄
				DriftParticleMgr::Ins()->DestroyAura(charaIndex_);

			}

		}

		// 煙を出す。
		Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(isDriftRight_ ? -1.0f : 1.0f, 0, 0), playerModel_.carBodyInstance_.lock()->GetRotate());
		DriftParticleMgr::Ins()->GenerateSmoke(playerModel_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, playerModel_.carBodyInstance_.lock()->GetRotate(), nowLevel < 1, DriftParticleMgr::DELAY_ID::DEF);



	}
	else {

		// オーラを破棄 関数内に二重解放対策の条件式がある。
		DriftParticleMgr::Ins()->DestroyAura(charaIndex_);

	}

	// ゲームが終了していたら イージングが終了していなかったら。
	if (IsGameFinish && gameFinishEasingTimer_ < 0.9f) {

		// 煙を出す。
		Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(isGameFinishDriftLeft_ ? -1.0f : 1.0f, 0, 0), playerModel_.carBodyInstance_.lock()->GetRotate());
		DriftParticleMgr::Ins()->GenerateSmoke(playerModel_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, gameFinishTriggerMatRot_, false, DriftParticleMgr::DELAY_ID::NONE_DELAY);
		DriftParticleMgr::Ins()->GenerateSmoke(playerModel_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, gameFinishTriggerMatRot_, false, DriftParticleMgr::DELAY_ID::NONE_DELAY);
		DriftParticleMgr::Ins()->GenerateSmoke(isGameFinishDriftLeft_ ? playerModel_.carLeftTireInstance_.lock()->GetWorldPos() : playerModel_.carRightTireInstance_.lock()->GetWorldPos(), gameFinishTriggerMatRot_, false, DriftParticleMgr::DELAY_ID::NONE_DELAY);


	}

	// ジャンプアクションのパーティクルを生成。
	if (isJumpAction_ || Input::Ins()->IsKeyTrigger(DIK_M)) {

		DriftParticleMgr::Ins()->GenerateJumpEffect(playerModel_.carBodyInstance_);

	}

}

void Character::CheckHitOtherCar(std::vector<std::shared_ptr<Character>> CharaData)
{

	/*===== 他の車との当たり判定 =====*/

	for (auto& index : CharaData) {

		// 今の自分と同じ車だったら処理を飛ばす。
		if (index.get() == this) continue;

		// 一定距離内にいたら当たり判定を行う。
		float distance = Vec3(GetPos() - index->GetPos()).Length();

		const float RANGE_IGNORE_CHECK_HIT = 150.0f;	// 当たり判定を棄却する距離。
		if (RANGE_IGNORE_CHECK_HIT <= distance) continue;

		// メッシュの当たり判定用構造体。
		FHelper::RayToModelCollisionData collisionData;

		// 全方向共通の当たり判定用データを入力。
		collisionData.rayPos_ = GetPos();
		collisionData.targetPolygonData_ = index->hitBox_.lock()->GetMeshCollisionData();

		// 当たり判定用サイズ
		const Vec3 CHECK_HIT_SIZE = Vec3(63.0f, 0.0f, 27.0f);

		// 正面
		{

			// 当たり判定用データを入力。
			collisionData.rayDir_ = forwardVec_;

			CheckHitCar(collisionData, index, CHECK_HIT_SIZE.x_);

		}

		// 背面
		{

			// 当たり判定用データを入力。
			collisionData.rayDir_ *= -1.0f;

			CheckHitCar(collisionData, index, CHECK_HIT_SIZE.x_);

		}

		// 右面
		{

			// 当たり判定用データを入力。
			collisionData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), playerModel_.carBodyInstance_.lock()->GetRotate());

			CheckHitCar(collisionData, index, CHECK_HIT_SIZE.z_);

		}

		// 左面
		{

			// 当たり判定用データを入力。
			collisionData.rayDir_ *= -1.0f;

			CheckHitCar(collisionData, index, CHECK_HIT_SIZE.z_);

		}

	}

}

void Character::CheckHitCar(const FHelper::RayToModelCollisionData& CollisionData, std::weak_ptr<Character> IndexCharacter, float CheckHitSize)
{

	/*===== 車との当たり判定 =====*/

	// 当たり判定結果受け取り用変数
	Vec3 impactPos;
	Vec3 hitNormal;
	Vec2 hitUV;
	float impactLength;

	// 当たり判定を行う。
	bool isHit = FHelper::RayToModelCollision(CollisionData, impactPos, impactLength, hitNormal, hitUV);

	// 衝突していなかったら処理を飛ばす。
	if (!isHit) return;

	// 衝突点までの長さが0より下 or レイの最大の長さより大きかったら処理を飛ばす。
	if (impactLength < 0 || CheckHitSize < impactLength) return;

	// 押し戻す。
	pos_ += (GetPos() - IndexCharacter.lock()->GetPos()).GetNormal() * (CheckHitSize - impactLength);

}

