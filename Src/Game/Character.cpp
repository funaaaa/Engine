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

	/*===== ���������� =====*/

	// �L������ID��ۑ�
	charaID_ = CharaID;

	// �L�����̔ԍ���ۑ��B
	charaIndex_ = CharaIndex;

	// �L����ID�ɉ����đ���I�u�W�F�N�g�𐶐��B
	if (charaID_ == CHARA_ID::P1) {

		operationObject_ = std::make_shared<PlayerOperationObject>(0, false, this);

		// �����ʒu��ݒ�B
		defPos_ = PLAYER_DEF_POS;

		// �Ԃ̃��f�������[�h
		playerModel_.Load(PlayerModel::COLOR::RED, PlayerModel::CHARA::PLAYER);

	}
	else if (charaID_ == CHARA_ID::P1_WGHOST) {

		operationObject_ = std::make_shared<PlayerOperationObject>(0, true, this);

		// �����ʒu��ݒ�B
		defPos_ = GHOST_DEF_POS[0];

		// �Ԃ̃��f�������[�h
		playerModel_.Load(PlayerModel::COLOR::RED, PlayerModel::CHARA::PLAYER);

	}
	else if (charaID_ == CHARA_ID::AI1) {

		operationObject_ = std::make_shared<FirstAIOperationObject>(static_cast<int>(FirstAIWayPointMgr::WAYPOINT_OFFSET::CENTER), Level, CharaPersonality);

		// �����ʒu��ݒ�B
		defPos_ = GHOST_DEF_POS[0];

		// �Ԃ̃��f�������[�h
		playerModel_.Load(PlayerModel::COLOR::BLUE, PlayerModel::CHARA::AI);

	}
	else if (charaID_ == CHARA_ID::GHOST) {

		std::string filePath = "Resource/Game/CharaGhostData/Ghost_";
		std::string dottxt = ".txt";
		std::string number = std::to_string(Level);

		operationObject_ = std::make_shared<GhostOperationObject>(filePath + number + dottxt);

		// �����ʒu��ݒ�B
		defPos_ = GHOST_DEF_POS[0];

		// �Ԃ̃��f�������[�h
		playerModel_.Load(PlayerModel::COLOR::BLUE, PlayerModel::CHARA::GHOST);

	}

	// �^�C�����Z�b�g�B
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carRightTireFrameInstance_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carRightTireInstance_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carLeftTireFrameInstance_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carLeftTireInstance_, false));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carBehindTireFrameInstance_, true));
	tires_.emplace_back(std::make_shared<PlayerTire>(playerModel_.carBehindTireInstance_, true));

	// ���P�b�g�𐶐��B
	rocketBlas_[0] = BLASRegister::Ins()->GenerateObj("Resource/Game/UI/", "RocketHead.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	rocketBlas_[1] = BLASRegister::Ins()->GenerateObj("Resource/Game/UI/", "RocketBody.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	rocketBlas_[2] = BLASRegister::Ins()->GenerateObj("Resource/Game/UI/", "RocketLegs.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	rocketBlas_[3] = BLASRegister::Ins()->GenerateObj("Resource/Game/UI/", "RocketWindow.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF]);
	for (int index = 0; index < 4; ++index) {
		rocketIns_[index] = PolygonInstanceRegister::Ins()->CreateInstance(rocketBlas_[index], PolygonInstanceRegister::DEF);
		rocketIns_[index].lock()->ChangeScale(Vec3(30, 30, 30));
		rocketBlas_[index].lock()->ChangeMapTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/UI/metalness2.png"), BLAS::MAP_PARAM::SPECULAR);
	}

	// �ԂƂ̓����蔻��p�̃`�F�b�N�{�b�N�X�����[�h
	hitBoxBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Car/collision/", "carHitBox.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], false, true);
	hitBox_ = PolygonInstanceRegister::Ins()->CreateInstance(hitBoxBlas_, PolygonInstanceRegister::DEF, true);
	PolygonInstanceRegister::Ins()->NonDisplay(hitBox_);

	// ���b�v���֌W
	rapCount_ = 0;
	isPassedMiddlePoint_ = 0;

	// ���W�֌W
	pos_ = defPos_;
	prevPos_ = pos_;

	// �����x�N�g���֌W
	forwardVec_ = Vec3(0, 0, -1);
	bottomVec = Vec3(0, -1, 0);
	upVec_ = Vec3(0, 1, 0);

	// �T�C�Y�A��]�A�ړ��֌W
	size_ = Vec3(50, 30, 50);
	rotY_ = DEF_ROTY;
	speed_ = 0;
	gravity_ = 0;
	jumpBoostSpeed_ = 0;

	// �h���t�g�֌W
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

	// �L�����N�^�[����]�����鏈�����܂Ƃ߂��N���X�𐶐��B
	inclineBody_ = std::make_shared<CharacterInclineBody>();
	inclineBody_->Init();

	// �Q�[���I�����ɍs���������܂Ƃ߂��N���X�𐶐��B
	gameFinish_ = std::make_shared<CharacterGameFinish>();

	// OBB�𐶐��B
	obb_ = std::make_shared<OBB>();
	obb_->Setting(playerModel_.carBodyBlas_, playerModel_.carBodyInstance_);

	if (SceneMgr::Ins()->nowScene_ == BaseScene::SCENE_ID::GAME) {

		leftTailLamp_ = std::make_shared<TailLampMgr>();
		rightTailLamp_ = std::make_shared<TailLampMgr>();

	}

}

void Character::Init()
{

	/*===== ���������� =====*/

	// �����ʒu��ݒ�B
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

	// �L�����N�^�[�̉�]�Ɋւ��鏈�����������B
	inclineBody_->Init();

	// �L�����N�^�[�̃Q�[���I�����ɍs���������܂Ƃ߂��N���X���������B
	gameFinish_->Init();

	// �h���t�g�Ɋւ���ϐ����������B
	drift_->Init();

	playerModel_.Delete();

	DriftParticleMgr::Ins()->Init();
	// �Վ��̃o�O�΍�ł��B �ŏ��̈��ڂ̃h���t�g�̂Ƃ��̂݃I�[�����o�Ȃ��̂ŁA�����ň�񐶐����Ă����B
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


	/*===== �X�V���� =====*/


	// �Q�[���I���t���O���X�V�B
	gameFinish_->UpdateGameFinishFlag(IsGameFinish);

	// �G���W���̓����œ���������ł������B
	pos_.y_ -= engineWaveAmount_;

	// �u�[�X�g���̉�]��ł������B
	playerModel_.carBodyInstance_.lock()->ChangeRotate(defBodyMatRot_);

	// ���͏���
	Input(IsBeforeStart);

	// �h���t�g�Ɋւ���X�V����
	UpdateDrift();

	// �ړ�����
	Move(IsBeforeStart);

	// �����蔻��
	CheckHit(StageData, CharaData);

	// �J�n�O��������
	if (IsBeforeStart) {

		// �v���C���[�������I�ɏ����ʒu�ɖ߂��B
		pos_.x_ = defPos_.x_;
		pos_.z_ = defPos_.z_;

	}

	// �G���W���̓���
	EngineSineWave();

	// �Q�[���I�����̍X�V����
	UpdateGameFinish();

	// ���W���X�V�B
	playerModel_.carBodyInstance_.lock()->ChangeTrans(pos_);

	// �ԑ̂��X���鏈���B
	InclineCarBody();

	// �󒆂ɂ���Ƃ��͏����n�_�܂Ŗ߂�^�C�}�[���X�V�B�n��ɗv��Ƃ��̓^�C�}�[���������B
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

			// �L�����N�^�[�̉�]�Ɋւ���N�H�[�^�j�I�����������B
			inclineBody_->InitQuaternion();

		}

	}

	// �����蔻��p�f�[�^���X�V�B
	obb_->SetMat(playerModel_.carBodyInstance_);
	hitBox_.lock()->ChangeRotate(playerModel_.carBodyInstance_.lock()->GetRotate());
	hitBox_.lock()->ChangeScale(playerModel_.carBodyInstance_.lock()->GetScale());
	hitBox_.lock()->ChangeTrans(playerModel_.carBodyInstance_.lock()->GetTrans());

	// ���W��ۑ��B
	prevPos_ = pos_;

	// �^�C�����X�V����B
	for (auto& index : tires_) {

		index->Update();

	}

	// �h���t�g�p�[�e�B�N���̍X�V�����B
	UpdateDriftParticle(IsGameFinish, IsBeforeStart);

	// �ړ��ł��Ȃ��^�C�}�[���X�V����B
	if (0 < canNotMoveTimer_) {

		--canNotMoveTimer_;

		// �C�[�W���O�ʂ����߂�B
		float easingAmount = static_cast<float>(canNotMoveTimer_) / static_cast<float>(CAN_NOT_MOVE_TIMER_SHELL_HIT);

		easingAmount = easingAmount * easingAmount * easingAmount * easingAmount * easingAmount;

		rotY_ = shellHitRot_ + easingAmount * DirectX::XM_2PI;

	}
	else {

		cameraForwardVec_ = forwardVec_;

	}

	// �u�[�X�g�ʂ����ȏゾ������W�������o���B
	if (charaID_ == CHARA_ID::P1) {
		isConcentrationLine_ = (MAX_BOOST_SPEED / 2.0f < boostSpeed_) || (JUMP_BOOST_SPEED / 2.0f < jumpBoostSpeed_);
	}

	// �u�[�X�g�̊����ɉ����ă��W�A���u���[��������B
	if (charaID_ == CHARA_ID::P1) {

		RadialBlur::Ins()->SetBlurPower(FHelper::Saturate(boostSpeed_ / MAX_BOOST_SPEED));

		//_RPTFN(_CRT_WARN, "%f\n", boostSpeed_ / MAX_BOOST_SPEED);

	}

	// �ڒn�t���O��ۑ��B
	onGroundPrev_ = onGround_;

	// �Q�[���J�n�O�t���O��ۑ��B
	isBeforeStartPrev_ = IsBeforeStart;


	// ���P�b�g���J�����̍���Ɉړ�������B
	Vec3 nowRocketPos = rocketIns_[0].lock()->GetPos();
	Vec3 rocketPos = GetPos();
	rocketPos -= forwardVec_ * 60.0f;
	rocketPos -= upVec_ * 20.0f;
	// ��Ԃ���B
	if (charaID_ == CHARA_ID::P1) {
		nowRocketPos += (rocketPos - nowRocketPos) / 3.0f;
	}
	else {
		nowRocketPos += (rocketPos - nowRocketPos);
	}
	for (auto& index : rocketIns_) {

		index.lock()->ChangeTrans(nowRocketPos);

	}

	// ��]�ʂ�{���̒l�ɕ�Ԃ���B
	const float ADD_ROTATION_Y = 0.8f;
	const float MIN_ROTATION_Y = 0.05f;
	rocketAddRotationY_ += (MIN_ROTATION_Y - rocketAddRotationY_) / 10.0f;
	// ��]������B
	rocketRotationY_ += rocketAddRotationY_;

	// ���P�b�g���΂߂ɂ���B
	for (auto& index : rocketIns_) {

		// �����̉�]�ɖ߂��B
		index.lock()->ChangeRotate(Vec3(0.4f, 0.0f, 0.0f));

		// �J������x�N�g�������̃N�H�[�^�j�I�������߂�B
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(Camera::Ins()->up_.ConvertXMVECTOR(), rocketRotationY_);

		// ���߂��N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// ��]�����Z����B
		index.lock()->AddRotate(quaternionMat);

	}

	// �A�C�e�����擾�����u�Ԃ�g�����u�ԂɃC�[�W���O�^�C�}�[���������B
	bool prevFrameDisplayRocket = isDisplayRocket_;
	if (isGetItem_ || isUseItem_) {

		rocketEasingTimer_ = 0.1f;
		rocketAddRotationY_ = ADD_ROTATION_Y;

		if (!isDisplayRocket_) {
			isDisplayRocket_ = true;
		}

	}

	// �A�C�e���̃��P�b�g�̃T�C�Y���X�V�B
	const float MAX_SCALE = 12.0f;
	rocketEasingTimer_ += 0.06f;
	if (1.0f < rocketEasingTimer_) rocketEasingTimer_ = 1.0f;
	if (GetIsItem()) {

		// �C�[�W���O�ʂ����߂�B
		float easingAmount = FEasing::EaseOutQuint(rocketEasingTimer_);
		float scale = MAX_SCALE * easingAmount;
		if (scale <= 0) scale = 0.01f;
		for (auto& index : rocketIns_) {
			index.lock()->ChangeScale(Vec3(scale, scale, scale));
		}

	}
	else {

		// �C�[�W���O�ʂ����߂�B
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

	// �`��n�܂����g���K�[��������`�悷��B
	if (!prevFrameDisplayRocket && isDisplayRocket_) {
		for (auto& index : rocketIns_) {
			PolygonInstanceRegister::Ins()->Display(index.lock()->GetInstanceIndex());
		}
	}

	// �`��I������g���K�[��������`��������B
	if ((prevFrameDisplayRocket && !isDisplayRocket_) || (!isDisplayRocket_ && rocketIns_[0].lock()->GetScaleVec3().x_ <= 0)) {
		for (auto& index : rocketIns_) {
			PolygonInstanceRegister::Ins()->NonDisplay(index.lock()->GetInstanceIndex());
		}
	}

	// �Q�[���V�[���ł݂̂��̍X�V�������s���B
	if (SceneMgr::Ins()->nowScene_ == BaseScene::SCENE_ID::GAME) {

		// ��ʓ��Ɏ��܂��Ă��邩�B
		bool inScreen = FHelper::CheckInScreen(GetPos(), 100.0f, 100.0f, Camera::Ins()->matView_, Camera::Ins()->matPerspective_);

		const float TAILLAMP_OFFSET_SPEED = 8.0f;	// �e�[�������v���o���X�s�[�h
		if (TAILLAMP_OFFSET_SPEED <= jumpBoostSpeed_ && inScreen) {

			// �e�[�������v�𐶐��B
			static const int VERTEX_SIZE = 4;
			{
				// �E���̃e�[�������v
				DirectX::XMMATRIX matWorld = playerModel_.carRightLightInstance_.lock()->GetWorldMat();
				std::array<Vec3, VERTEX_SIZE> vertex;
				vertex[0] = FHelper::MulMat(playerModel_.carRightLightBlas_.lock()->GetVertexPos()[10], matWorld);	// ���C�g�̒��_��ݒ�B������͊e���_�̍ŏ��ƍő傩�玩���Őݒ�ł���悤�ɂ������B
				vertex[1] = FHelper::MulMat(playerModel_.carRightLightBlas_.lock()->GetVertexPos()[11], matWorld);
				vertex[2] = FHelper::MulMat(playerModel_.carRightLightBlas_.lock()->GetVertexPos()[1], matWorld);
				vertex[3] = FHelper::MulMat(playerModel_.carRightLightBlas_.lock()->GetVertexPos()[2], matWorld);
				// ����
				leftTailLamp_->Generate(vertex, TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"), (jumpBoostSpeed_ - TAILLAMP_OFFSET_SPEED) / (JUMP_BOOST_SPEED - TAILLAMP_OFFSET_SPEED));

			}
			{
				// �����̃e�[�������v
				DirectX::XMMATRIX matWorld = playerModel_.carLeftLightInstance_.lock()->GetWorldMat();
				std::array<Vec3, VERTEX_SIZE> vertex;
				vertex[3] = FHelper::MulMat(playerModel_.carLeftLightBlas_.lock()->GetVertexPos()[3], matWorld);
				vertex[2] = FHelper::MulMat(playerModel_.carLeftLightBlas_.lock()->GetVertexPos()[11], matWorld);
				vertex[1] = FHelper::MulMat(playerModel_.carLeftLightBlas_.lock()->GetVertexPos()[8], matWorld);
				vertex[0] = FHelper::MulMat(playerModel_.carLeftLightBlas_.lock()->GetVertexPos()[2], matWorld);
				// ����
				rightTailLamp_->Generate(vertex, TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"), (jumpBoostSpeed_ - TAILLAMP_OFFSET_SPEED) / (JUMP_BOOST_SPEED - TAILLAMP_OFFSET_SPEED));

			}

		}

		// �e�[�������v���X�V�B
		leftTailLamp_->Update();
		rightTailLamp_->Update();

	}

}

void Character::Draw()
{

	/*===== �`�揈�� =====*/

}

bool Character::CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, TireMaskUV& TireMaskUVData)
{

	/*===== �^�C���������o =====*/

	// �Q�[�����I�������g���K�[���肾������B
	if (gameFinish_->GetIsGameFinishTrigger()) {

		tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

	}

	// �󒆂ɂ�����A�h���t�g�W�����v����������B
	if (!onGround_ || drift_->GetIsDriftJump()) {

		tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

	}

	const float RAY_LENGTH = 80.0f;

	// �Q�[�����I�����Ă�����B
	if (gameFinish_->GetIsGameFinish()) {

		// �����Ƀh���t�g���Ă�����B
		if (!gameFinish_->GetIsGameFinishDriftLeft()) {

			FHelper::RayToModelCollisionData InputRayData;
			InputRayData.targetPolygonData_ = PolygonInstanceRegister::Ins()->GetMeshCollisionData(BaseStageData.lock()->stageObjectMgr_->GetInstanceIndex(1));

			// �߂�n�ۑ��p
			Vec3 ImpactPos;
			Vec3 HitNormal;
			Vec2 HitUV;
			float HitDistance;

			// ���O�^�C��
			InputRayData.rayPos_ = playerModel_.carRightTireInstance_.lock()->GetWorldPos();
			InputRayData.rayDir_ = bottomVec;

			// �^�C�����̈ʒu�����o
			bool isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

			// �����蔻�肪���������ǂ������`�F�b�N�B
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

			// �E�O�^�C��
			InputRayData.rayPos_ = playerModel_.carBehindTireInstance_.lock()->GetWorldPos();
			InputRayData.rayPos_ += FHelper::MulRotationMatNormal(Vec3(1, 0, 0), playerModel_.carBodyInstance_.lock()->GetRotate()) * 20.0f;
			InputRayData.rayDir_ = bottomVec;

			// �^�C�����̈ʒu�����o
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

			// �߂�n�ۑ��p
			Vec3 ImpactPos;
			Vec3 HitNormal;
			Vec2 HitUV;
			float HitDistance;

			// ���O�^�C��
			InputRayData.rayPos_ = playerModel_.carLeftTireInstance_.lock()->GetWorldPos();
			InputRayData.rayDir_ = bottomVec;

			// �^�C�����̈ʒu�����o
			bool isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

			// �����蔻�肪���������ǂ������`�F�b�N�B
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

			// �E�O�^�C��
			InputRayData.rayPos_ = playerModel_.carBehindTireInstance_.lock()->GetWorldPos();
			InputRayData.rayPos_ += FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), playerModel_.carBodyInstance_.lock()->GetRotate()) * 20.0f;
			InputRayData.rayDir_ = bottomVec;

			// �^�C�����̈ʒu�����o
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

	// �߂�n�ۑ��p
	Vec3 ImpactPos;
	Vec3 HitNormal;
	Vec2 HitUV;
	float HitDistance;
	bool isHit = false;

	// �E�h���t�g���Ă����炱�̏����͒ʂ��Ȃ��B
	if (!(drift_->GetIsDrift() && !drift_->GetIsDriftRight())) {

		// ���O�^�C��
		InputRayData.rayPos_ = playerModel_.carLeftTireInstance_.lock()->GetWorldPos();
		InputRayData.rayDir_ = bottomVec;

		// �^�C�����̈ʒu�����o
		isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

		// �����蔻�肪���������ǂ������`�F�b�N�B
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


	// ���h���t�g���Ă����炱�̏����͒ʂ��Ȃ��B
	if (!(drift_->GetIsDrift() && drift_->GetIsDriftRight())) {

		// �E�O�^�C��
		InputRayData.rayPos_ = playerModel_.carRightTireInstance_.lock()->GetWorldPos();
		InputRayData.rayDir_ = bottomVec;

		// �^�C�����̈ʒu�����o
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

	// �E���^�C��
	InputRayData.rayPos_ = playerModel_.carRightTireInstance_.lock()->GetWorldPos();
	InputRayData.rayDir_ = bottomVec;

	// ��]�������x�N�g�������߂�B
	Vec3 behindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), playerModel_.carBodyInstance_.lock()->GetRotate());
	InputRayData.rayPos_ += behindVec * 70.0f;

	// �^�C�����̈ʒu�����o
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

	// �����^�C��
	InputRayData.rayPos_ = playerModel_.carLeftTireInstance_.lock()->GetWorldPos();
	InputRayData.rayDir_ = bottomVec;

	// ��]�������x�N�g�������߂�B
	behindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), playerModel_.carBodyInstance_.lock()->GetRotate());
	InputRayData.rayPos_ += behindVec * 70.0f;

	// �^�C�����̈ʒu�����o
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
	/*===== �J�����p���ʃx�N�g���擾�֐� =====*/

	return cameraForwardVec_;

}

float Character::GetNowSpeedPer()
{

	/*====== �ړ����x�̊������v�Z =====*/

	float per = FHelper::Saturate((speed_ + boostSpeed_ + jumpBoostSpeed_ * 2.0f) / (MAX_SPEED + MAX_BOOST_SPEED));

	return per;

}

bool Character::GetIsItem()
{
	return item_.operator bool();
}

void Character::DeleteInstance()
{

	/*===== ���f���̃C���X�^���X���폜 =====*/

	playerModel_.Delete();

}

void Character::Input(bool IsBeforeStart)
{

	/*===== ���͏��� =====*/

	// �Q�[�����I�����Ă���ꍇ�͓��͂𖳌�������B �Q�[�����I�������ŏ���F�ł͓��͂����K�v������̂ŁAPrev����r���Ă���B
	if (gameFinish_->CheckGameFinish()) return;

	// ����I�u�W�F�N�g����̓��͂��󂯎��B
	BaseOperationObject::OperationInputData operationInputData;
	operationInputData.pos_ = pos_;
	operationInputData.forwradVec_ = forwardVec_;
	if (item_.operator bool()) {
		operationInputData.hasItemID_ = item_->GetItemID();
	}
	operationInputData.isHitJumpBoostGimmick_ = isHitJumpActionGimmick_;
	operationInputData.isPrevFrameDrift_ = drift_->GetIsDrift();
	BaseOperationObject::Operation operation = operationObject_->Input(operationInputData);

	// �����Ȃ��^�C�}�[�������Ă����瓮�����Ȃ��B
	if (0 < canNotMoveTimer_) {

		operation.accelerationRate_ = 0;
		operation.handleDriveRate_ = 0;

	}

	// �J�n�O�̃A�N�Z���̃t���O���X�V�B
	isAccel_ = 0 < operation.accelerationRate_;

	// RT��������Ă���������B
	if (0 < operation.accelerationRate_ && onGround_ && !IsBeforeStart) {

		speed_ += operation.accelerationRate_ * ADD_SPEED;

	}
	// �b���ɂ������Ă�����B
	else if (0 < canNotMoveTimer_) {

		speed_ -= speed_ / 50.0f;

	}
	else if (onGround_) {

		// �ړ����Ă��Ȃ��Ēn��ɂ�����ړ��ʂ�0�ɋ߂Â���B
		speed_ -= speed_ / 10.0f;

	}
	else {

		// �ړ����Ă��Ȃ��ċ󒆂ɂ�����ړ��ʂ�0�ɋ߂Â���B
		speed_ -= speed_ / 100.0f;

	}

	// �A�C�e�������ɓ�����t���O���X�V�B
	isShotBehind_ = operation.isShotBehind_;

	// �h���t�g�̓��͂Ɋւ��鏈��
	{
		// ���o�͗p�f�[�^���l�ߍ��ށB
		CharacterDrift::InputData inData;
		inData.canNotMoveTimer_ = canNotMoveTimer_;
		inData.handleNormal_ = HANDLE_NORMAL;
		inData.isBeforeStart_ = IsBeforeStart;
		inData.onGround_ = onGround_;
		inData.upVec_ = upVec_;

		// �h���t�g�̓��͂Ɋւ���X�V����
		drift_->Input(inData, operation, tires_, playerModel_, rotY_, boostSpeed_, forwardVec_);

	}


	// ���̃L�������S�[�X�g�������ꍇ�A�A�C�e���擾���߂��o����A�C�e�����擾����B
	if (charaID_ == CHARA_ID::GHOST && operation.isGetItem_) {

		item_ = std::make_shared<BoostItem>();
		item_->Generate(playerModel_.carBodyInstance_);
		isGetItem_ = true;

	}
	else if (charaID_ == CHARA_ID::GHOST) {

		isGetItem_ = false;

	}

	// �A�C�e���𑀍�
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

	// �W�����v�A�N�V�����̃g���K�[����B
	isJumpActionTrigger_ = operation.isDriftTrigger_;

	// ���͂�ۑ�����B
	inclineBody_->SetHandleAmount(operation.handleDriveRate_);

}

void Character::Move(bool IsBeforeStart)
{

	/*===== �ړ����� =====*/

	// �ړ����x�����E�𒴂��Ȃ��悤�ɂ���B
	if (MAX_SPEED < speed_) {

		speed_ = MAX_SPEED;

	}

	// ���̏�ɂ�����ړ����x�̌��E�l��������B
	if (onGrass_ && MAX_SPEED_ON_GRASS < speed_) {

		speed_ = MAX_SPEED_ON_GRASS;

	}

	// �ړ�����������B �b���ɂ������Ă���Ƃ��͂������������ɐi�܂���ׁB
	Vec3 moveVec = forwardVec_;
	if (0 < canNotMoveTimer_) {
		moveVec = cameraForwardVec_;
	}

	// ���W�ړ�������B
	pos_ += moveVec * (speed_ + boostSpeed_ + jumpBoostSpeed_);

	// �h���t�g���̃u�[�X�g�ړ��ʂ�0�ɋ߂Â���B
	if (0 < boostSpeed_) {

		boostSpeed_ -= SUB_BOOST_SPEED;

		// �f�t�H���g�̈ړ��ʂ��}�b�N�X�ɂ���B
		speed_ = MAX_SPEED;

	}
	else {

		boostSpeed_ = 0;

	}

	// �n��ɂ�����d�͂𖳌�������B
	if (onGround_) {

		gravity_ = 0;

	}
	// �󒆂ɂ�����d�͂����Z����B
	else {

		gravity_ += ADD_GRAV;

		// �d�͗ʂ����E�𒴂��Ȃ��悤�ɂ���B
		if (MAX_GRAV < gravity_) {

			gravity_ = MAX_GRAV;

		}

	}

	// �Q�[�����J�n�����g���K�[�̂Ƃ��ɃA�N�Z�����ӂ܂�Ă���������B
	bool isGameStartRelease = isBeforeStartPrev_ && !IsBeforeStart;
	if (isGameStartRelease && isAccel_) {

		boostTimer_ = 20;
		boostSpeed_ = MAX_BOOST_SPEED;

	}

	// �W�����v�����ʂ���������B
	jumpBoostSpeed_ -= SUB_JUMP_BOOST_SPEED;
	if (jumpBoostSpeed_ < 0) {

		jumpBoostSpeed_ = 0;

	}

	// ���W�ɏd�͂����Z����B
	pos_ += Vec3(0, -1, 0) * gravity_;

	// ���x�N�g�����Ԃ̉�]�s�񕪉�]������B
	bottomVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), playerModel_.carBodyInstance_.lock()->GetRotate());

}

void Character::UpdateDrift()
{

	/*===== �h���t�g�Ɋւ���X�V���� =====*/

	drift_->Update(inclineBody_, pos_, prevPos_, onGround_);

}

#include "BrightnessParam.h"
void Character::CheckHit(std::weak_ptr<BaseStage> StageData, std::vector<std::shared_ptr<Character>> CharaData)
{

	/*===== �����蔻�� =====*/

	// ���̎ԂƂ̓����蔻��B
	CheckHitOtherCar(CharaData);

	// �����蔻��֐��ɓ����l��ݒ�B
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

	// �����蔻��֐�����Ԃ��Ă���l�B
	BaseStage::ColliderOutput output;

	// �����蔻����s���B
	output = StageData.lock()->Collider(input);

	// �����蔻��̌��ʂ��珈�����s���B
	if (output.isHitBoostGimmick_) {

		// �u�[�X�g���}�b�N�X�ɂ���B
		boostSpeed_ = MAX_BOOST_SPEED;
		boostTimer_ = BOOST_GIMMICK_BOOST_TIMER;

	}
	if (output.isHitGoal_) {

		// �S�[��
		if (isPassedMiddlePoint_) {
			isPassedMiddlePoint_ = false;
			++rapCount_;

			// �O���ȏ�ɂȂ�Ȃ��悤�ɂ���B
			if (3 <= rapCount_) rapCount_ = 3;

		}

		operationObject_->OnGoal();

	}
	if (output.isHitMiddlePoint_) {

		// ���Ԓn�_�Ƃ̓����蔻��
		isPassedMiddlePoint_ = true;

	}
	if (output.isHitOrnament_) {

		// �����I�u�W�F�N�g�Ƃ̓�����������
		speed_ = 0;
		boostSpeed_ = 0;

	}
	// �ݒu������������B
	onGround_ = false;
	onGrass_ = false;
	if (output.isHitStage_) {

		// �X�e�[�W�Ƃ̓����蔻��
		onGround_ = true;

		forwardVec_ = output.forwardVec_;
		upVec_ = output.upVec_;

		// ��]���X�V�B
		playerModel_.carBodyInstance_.lock()->ChangeRotate(output.matRot_);


	}
	if (output.isHitStageGrass_) {

		// �X�e�[�W�Ɠ������Ă��Ȃ�������
		if (!onGround_) {

			// ���Ƃ�����������
			onGround_ = true;
			onGrass_ = true;

			forwardVec_ = output.forwardVec_;
			upVec_ = output.upVec_;

		}

		// ��]���X�V�B
		playerModel_.carBodyInstance_.lock()->ChangeRotate(output.matRot_);

	}
	// �S�[�X�g�������瓖���蔻����΂��B
	if (output.isHitItemBox_ && charaID_ != CHARA_ID::GHOST && !item_.operator bool()) {

		// �A�C�e�����擾�����t���O
		isGetItem_ = true;

		if (GameSceneMode::Ins()->mode_ == GameSceneMode::MODE::WRITE_GHOST || GameSceneMode::Ins()->mode_ == GameSceneMode::MODE::GHOST) {

			item_ = std::make_shared<BoostItem>();
			item_->Generate(playerModel_.carBodyInstance_);

		}
		else {


			item_ = std::make_shared<BoostItem>();
			item_->Generate(playerModel_.carBodyInstance_);

			//// AI�킾�����烉���_���ŃA�C�e���𐶐�����B
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

		// �A�C�e�����擾���Ȃ�����
		isGetItem_ = false;

	}

	// �i�������I�u�W�F�N�g�Ɠ������Ă��āA�W�����v�A�N�V�����{�^���������Ă�����B
	isJumpAction_ = false;
	isHitJumpActionGimmick_ = output.isHitStepBoostGimmick_;
	if (output.isHitStepBoostGimmick_ && isJumpActionTrigger_) {

		// ����������B
		jumpBoostSpeed_ = JUMP_BOOST_SPEED;

		isJumpAction_ = true;

	}


	// ���̑��̕ϐ����������B
	pos_ = output.resultPos_;

	// �S�[�X�g�ȊO��������B
	if (charaID_ != CHARA_ID::GHOST) {

		int shellIndex = -1;

		if (0 < timerToSkipShellCollider_) {

			--timerToSkipShellCollider_;
			shellIndex = shellIndex_;

		}

		// �b���Ƃ̓����蔻��
		bool isHitShell = ShellObjectMgr::Ins()->Collider(obb_, shellIndex);

		if (isHitShell || Input::Ins()->IsKeyTrigger(DIK_P)) {

			canNotMoveTimer_ = CAN_NOT_MOVE_TIMER_SHELL_HIT;
			shellHitRot_ = rotY_;

		}

	}


	// �v���C���[�����邳�̕ǂɂ������Ă�����B
	if (charaID_ == CHARA_ID::P1 && output.isHitBrightnessWall_) {
		BrightnessParam::Ins()->isBright_ = true;
	}
	if (charaID_ == CHARA_ID::P1 && output.isHitDarknessWall_) {
		BrightnessParam::Ins()->isBright_ = false;
	}

}

void Character::InclineCarBody()
{

	/*===== �ԑ̂��X���鏈�� =====*/


	// BODY�̉�]�s���ۑ��B
	defBodyMatRot_ = playerModel_.carBodyInstance_.lock()->GetRotate();

	// ��]�Ɋւ��鏈���ɕK�v�ȃf�[�^���l�ߍ��ށB
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

	// �ԑ̂̉�]�̏���
	inclineBody_->Update(inlclineInputData);

	// �Q�[���I�������o�p�̉�]
	gameFinish_->AKIRAERotation(playerModel_);

}

void Character::EngineSineWave()
{

	/*===== �G���W���̏㉺ =====*/

	// �T�C���g�p�̃^�C�}�[���X�V����B
	engineWaveTimer_ += 1.5f;

	// �ω���
	float waveLength = 0;

	// �J�n�O�ŃA�N�Z�����ӂ܂�Ă����猃�����������B
	if (isAccel_ && isBeforeStartPrev_) {

		waveLength = BEFORE_START_WAVE_LENGTH_ACCELL;

	}
	// [�n��ɂ��Ď~�܂��Ă�����] or [�J�n�O��������] �G���W���𓮂����B
	else if ((onGround_ && speed_ <= 0.1f && boostSpeed_ <= 0.1f) || isBeforeStartPrev_) {

		waveLength = BEFORE_START_WAVE_LENGTH_DEF;

	}
	else if (onGround_) {

		waveLength = BEFORE_START_WAVE_LENGTH_RUN;

	}

	// �T�C���g���v�Z����B
	//engineWaveAmount_ = std::sin(engineWaveTimer_) * waveLength;

	// �������B
	pos_.y_ += engineWaveAmount_;

}

void Character::UpdateGameFinish()
{

	/*===== �Q�[���I�����̍X�V���� =====*/

	// ���͂ɕK�v�ȃf�[�^���l�ߍ��ށB
	CharacterGameFinish::UpdateInputData finishInputData;
	finishInputData.inclineBody_ = inclineBody_;
	finishInputData.maxSpeed_ = MAX_SPEED;
	finishInputData.playerModel_ = playerModel_;

	// �Q�[���I�����̊�{�I�ȍX�V����
	gameFinish_->Update(finishInputData, boostTimer_, speed_, rotY_, forwardVec_);

}

void Character::UpdateDriftParticle(bool IsGameFinish, bool IsBeforeStart)
{

	/*===== �h���t�g�p�[�e�B�N���̍X�V���� =====*/

	// �h���t�g�p�[�e�B�N���̍X�V�ɕK�v�ȓ��͏����l�ߍ��ށB
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

	// �h���t�g�p�[�e�B�N���̍X�V�����ŋA���Ă���l���l�ߍ��ށB
	CharacterDrift::DriftParticleOutData outData(boostTimer_, boostSpeed_, playerModel_, gameFinish_);

	// �X�V���s���B
	drift_->UpdateDriftParticle(inData, outData);

	// �o�̓f�[�^���󂯎��B
	boostTimer_ = outData.boostTimer_;
	boostSpeed_ = outData.boostSpeed_;

}

void Character::CheckHitOtherCar(std::vector<std::shared_ptr<Character>> CharaData)
{

	/*===== ���̎ԂƂ̓����蔻�� =====*/

	for (auto& index : CharaData) {

		// ���̎����Ɠ����Ԃ������珈�����΂��B
		if (index.get() == this) continue;

		// ��苗�����ɂ����瓖���蔻����s���B
		float distance = Vec3(GetPos() - index->GetPos()).Length();

		const float RANGE_IGNORE_CHECK_HIT = 150.0f;	// �����蔻������p���鋗���B
		if (RANGE_IGNORE_CHECK_HIT <= distance) continue;

		// ���b�V���̓����蔻��p�\���́B
		FHelper::RayToModelCollisionData collisionData;

		// �S�������ʂ̓����蔻��p�f�[�^����́B
		collisionData.rayPos_ = GetPos();
		collisionData.targetPolygonData_ = index->hitBox_.lock()->GetMeshCollisionData();

		// �����蔻��p�T�C�Y
		const Vec3 CHECK_HIT_SIZE = Vec3(63.0f, 0.0f, 27.0f);

		// ����
		{

			// �����蔻��p�f�[�^����́B
			collisionData.rayDir_ = forwardVec_;

			CheckHitCar(collisionData, index, CHECK_HIT_SIZE.x_);

		}

		// �w��
		{

			// �����蔻��p�f�[�^����́B
			collisionData.rayDir_ *= -1.0f;

			CheckHitCar(collisionData, index, CHECK_HIT_SIZE.x_);

		}

		// �E��
		{

			// �����蔻��p�f�[�^����́B
			collisionData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), playerModel_.carBodyInstance_.lock()->GetRotate());

			CheckHitCar(collisionData, index, CHECK_HIT_SIZE.z_);

		}

		// ����
		{

			// �����蔻��p�f�[�^����́B
			collisionData.rayDir_ *= -1.0f;

			CheckHitCar(collisionData, index, CHECK_HIT_SIZE.z_);

		}

	}

}

void Character::CheckHitCar(const FHelper::RayToModelCollisionData& CollisionData, std::weak_ptr<Character> IndexCharacter, float CheckHitSize)
{

	/*===== �ԂƂ̓����蔻�� =====*/

	// �����蔻�茋�ʎ󂯎��p�ϐ�
	Vec3 impactPos;
	Vec3 hitNormal;
	Vec2 hitUV;
	float impactLength;

	// �����蔻����s���B
	bool isHit = FHelper::RayToModelCollision(CollisionData, impactPos, impactLength, hitNormal, hitUV);

	// �Փ˂��Ă��Ȃ������珈�����΂��B
	if (!isHit) return;

	// �Փ˓_�܂ł̒�����0��艺 or ���C�̍ő�̒������傫�������珈�����΂��B
	if (impactLength < 0 || CheckHitSize < impactLength) return;

	// �����߂��B
	pos_ += (GetPos() - IndexCharacter.lock()->GetPos()).GetNormal() * (CheckHitSize - impactLength);

}

