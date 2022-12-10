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
#include "CharacterGameFinish.h"
#include "CharacterDrift.h"

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
	drift_ = std::make_shared<CharacterDrift>();

	engineWaveTimer_ = 0;
	engineWaveAmount_ = 0;
	returnDefPosTimer_ = 0;
	onGround_ = true;
	isBeforeStartPrev_ = false;
	onGroundPrev_ = false;
	onGrass_ = false;
	isGetItem_ = false;
	isUseItem_ = false;
	isDisplayRocket_ = false;
	boostSpeed_ = 0.0f;
	boostTimer_ = 0;
	canNotMoveTimer_ = 0;
	defBodyMatRot_ = DirectX::XMMatrixIdentity();
	cameraForwardVec_ = forwardVec_;

	rocketEasingTimer_ = 0.0f;
	rocketAddRotationY_ = 0.0f;
	rocketRotationY_ = 0.0f;

	// キャラクターを回転させる処理をまとめたクラスを生成。
	inclineBody_ = std::make_shared<CharacterInclineBody>();
	inclineBody_->Init();

	// ゲーム終了時に行う処理をまとめたクラスを生成。
	gameFinish_ = std::make_shared<CharacterGameFinish>();

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
	jumpBoostSpeed_ = 0;
	boostSpeed_ = 0.0f;
	boostTimer_ = 0;
	canNotMoveTimer_ = 0;
	engineWaveTimer_ = 0;
	engineWaveAmount_ = 0;
	isUseItem_ = false;
	onGround_ = true;
	onGrass_ = false;
	isGetItem_ = false;
	isBeforeStartPrev_ = false;
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

	// キャラクターのゲーム終了時に行う処理をまとめたクラスを初期化。
	gameFinish_->Init();

	// ドリフトに関する変数を初期化。
	drift_->Init();

	playerModel_.Delete();

	DriftParticleMgr::Ins()->Init();
	// 臨時のバグ対策です。 最初の一回目のドリフトのときのみオーラが出ないので、ここで一回生成しておく。
	DriftParticleMgr::Ins()->GenerateAura(charaIndex_, playerModel_.carBehindTireInstance_, static_cast<int>(DriftParticle::ID::AURA_BIG), false, 2 <= 0);
	DriftParticleMgr::Ins()->GenerateAura(charaIndex_, playerModel_.carBehindTireInstance_, static_cast<int>(DriftParticle::ID::AURA_SMALL), false, 2 <= 0);

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
	gameFinish_->UpdateGameFinishFlag(IsGameFinish);

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
	if (charaID_ == CHARA_ID::P1) {
		isConcentrationLine_ = (MAX_BOOST_SPEED / 2.0f < boostSpeed_) || (JUMP_BOOST_SPEED / 2.0f < jumpBoostSpeed_);
	}

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
	if (gameFinish_->GetIsGameFinishTrigger()) {

		tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

	}

	// 空中にいたら、ドリフトジャンプ中だったら。
	if (!onGround_ || drift_->GetIsDriftJump()) {

		tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

	}

	const float RAY_LENGTH = 80.0f;

	// ゲームが終了していたら。
	if (gameFinish_->GetIsGameFinish()) {

		// 左側にドリフトしていたら。
		if (!gameFinish_->GetIsGameFinishDriftLeft()) {

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

	if (!drift_->GetIsTireMask()) {

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
	if (!(drift_->GetIsDrift() && !drift_->GetIsDriftRight())) {

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
	if (!(drift_->GetIsDrift() && drift_->GetIsDriftRight())) {

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
	if (gameFinish_->CheckGameFinish()) return;

	// 操作オブジェクトからの入力を受け取る。
	BaseOperationObject::OperationInputData operationInputData;
	operationInputData.pos_ = pos_;
	operationInputData.forwradVec_ = forwardVec_;
	if (item_.operator bool()) {
		operationInputData.hasItemID_ = item_->GetItemID();
	}
	operationInputData.isHitJumpBoostGimmick_ = isHitJumpActionGimmick_;
	operationInputData.isPrevFrameDrift_ = drift_->GetIsDrift();
	BaseOperationObject::Operation operation = operationObject_->Input(operationInputData);

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

	// ドリフトの入力に関する処理
	{
		// 入出力用データを詰め込む。
		CharacterDrift::InputData inData;
		inData.canNotMoveTimer_ = canNotMoveTimer_;
		inData.handleNormal_ = HANDLE_NORMAL;
		inData.isBeforeStart_ = IsBeforeStart;
		inData.onGround_ = onGround_;
		inData.upVec_ = upVec_;

		// ドリフトの入力に関する更新処理
		drift_->Input(inData, operation, tires_, playerModel_, rotY_, boostSpeed_, forwardVec_);

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

	drift_->Update(inclineBody_, pos_, prevPos_, onGround_);

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
	inlclineInputData.isDriftJump_ = drift_->GetIsDriftJump();
	inlclineInputData.isDrift_ = drift_->GetIsDrift();
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
	gameFinish_->AKIRAERotation(playerModel_);

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

	// 入力に必要なデータを詰め込む。
	CharacterGameFinish::UpdateInputData finishInputData;
	finishInputData.inclineBody_ = inclineBody_;
	finishInputData.maxSpeed_ = MAX_SPEED;
	finishInputData.playerModel_ = playerModel_;

	// ゲーム終了時の基本的な更新処理
	gameFinish_->Update(finishInputData, boostTimer_, speed_, rotY_, forwardVec_);

}

void Character::UpdateDriftParticle(bool IsGameFinish, bool IsBeforeStart)
{

	/*===== ドリフトパーティクルの更新処理 =====*/

	// ドリフトパーティクルの更新に必要な入力情報を詰め込む。
	CharacterDrift::DriftParticleInData inData;
	inData.forwardVec_ = forwardVec_;
	inData.charaIndex_ = charaIndex_;
	inData.isAccel_ = isAccel_;
	inData.maxBoostSpeed_ = MAX_BOOST_SPEED;
	inData.isBeforeStart_ = IsBeforeStart;
	inData.isGameFinish_ = IsGameFinish;
	inData.IsJumpAction_ = isJumpAction_;
	inData.onGroundPrev_ = onGroundPrev_;
	inData.onGround_ = onGround_;

	// ドリフトパーティクルの更新処理で帰ってくる値を詰め込む。
	CharacterDrift::DriftParticleOutData outData(boostTimer_, boostSpeed_, playerModel_, gameFinish_);

	// 更新を行う。
	drift_->UpdateDriftParticle(inData, outData);

	// 出力データを受け取る。
	boostTimer_ = outData.boostTimer_;
	boostSpeed_ = outData.boostSpeed_;

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

