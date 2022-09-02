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

Character::Character(CHARA_ID CharaID)
{

	/*===== ���������� =====*/

	// �L������ID��ۑ�
	charaID_ = CharaID;

	// �L����ID�ɉ����đ���I�u�W�F�N�g�𐶐��B
	if (charaID_ == CHARA_ID::P1) {

		operationObject_ = std::make_shared<PlayerOperationObject>(0, false, this);

		// �Ԃ̃��f�������[�h
		playerModel_.Load(PlayerModel::COLOR::RED, false);

	}
	else if (charaID_ == CHARA_ID::P1_WGHOST) {

		operationObject_ = std::make_shared<PlayerOperationObject>(0, true, this);

		// �Ԃ̃��f�������[�h
		playerModel_.Load(PlayerModel::COLOR::RED, false);

	}
	else if (charaID_ == CHARA_ID::AI1) {

		operationObject_ = std::make_shared<FirstAIOperationObject>(static_cast<int>(FirstAIWayPointMgr::WAYPOINT_OFFSET::LEFT_LEARNING));

		// �Ԃ̃��f�������[�h
		playerModel_.Load(PlayerModel::COLOR::BLUE, false);

	}
	else if (charaID_ == CHARA_ID::GHOST) {

		operationObject_ = std::make_shared<GhostOperationObject>("Resource/Game/CharaGhostData/Dev_0.txt");

		// �Ԃ̃��f�������[�h
		playerModel_.Load(PlayerModel::COLOR::BLUE, true);

	}

	// �^�C�����Z�b�g�B
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
	size_ = Vec3(50, 30, 50);
	rotY_ = -0.367411435;
	speed_ = 0;
	gravity_ = 0;
	beforeStartSmokeTimer_ = 0;
	jumpBoostSpeed_ = 0;
	boostSpeed_ = 0;
	returnDefPosTimer_ = 0;
	boostTimer_ = 0;
	isDrift_ = false;
	onGround_ = true;
	isBeforeStartPrev_ = false;
	onGroundPrev_ = false;
	onGrass_ = false;
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
	beforeStartWaveTimer_ = 0;
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

	// OBB�𐶐��B
	obb_ = std::make_shared<OBB>();
	obb_->Setting(playerModel_.carBodyBlasIndex_, playerModel_.carBodyInsIndex_);

}

void Character::Init()
{

	/*===== ���������� =====*/

	pos_ = PLAYER_DEF_POS;
	prevPos_ = pos_;
	forwardVec_ = Vec3(0, 0, -1);
	bottomVec = Vec3(0, -1, 0);
	upVec_ = Vec3(0, 1, 0);
	size_ = Vec3(50, 30, 50);
	returnDefPosTimer_ = 0;
	rotY_ = -0.367411435;
	speed_ = 0;
	gravity_ = 0;
	boostSpeed_ = 0;
	boostTimer_ = 0;
	jumpBoostSpeed_ = 0;
	turningIndicatorTimer_ = 0;
	canNotMoveTimer_ = 0;
	driftJumpVec_ = Vec3();
	beforeStartWaveTimer_ = 0;
	beforeStartSmokeTimer_ = 0;
	driftJumpSpeed_ = 0;
	isDriftJump_ = false;;
	isDrift_ = false;
	isUseItem_ = false;
	onGround_ = true;
	onGrass_ = false;
	isGetItem_ = false;
	isInputLTPrev_ = false;
	isInputLT_ = false;
	isBeforeStartPrev_ = false;
	IsTurningIndicatorRed_ = false;
	isTireMask_ = false;
	isConcentrationLine_ = false;
	isJumpActionTrigger_ = false;
	PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));

}

void Character::Update(std::weak_ptr<BaseStage> StageData, RayConstBufferData& ConstBufferData, bool& IsPassedMiddlePoint, int& RapCount, const bool& IsBeforeStart)
{


	/*===== �X�V���� =====*/

	if (Input::Ins()->IsKey(DIK_I)) {

		rotY_ += 0.05f;

	}
	if (Input::Ins()->IsKey(DIK_O)) {

		rotY_ -= 0.05f;

	}

	// �u�[�X�g���̉�]��ł������B
	PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, defBodyMatRot_);

	// ���͏���
	Input(ConstBufferData, IsBeforeStart);

	// �ړ�����
	Move(IsBeforeStart);

	// �h���t�g�Ɋւ���X�V����
	UpdateDrift(IsBeforeStart);

	// �����蔻��
	CheckHit(StageData, IsPassedMiddlePoint, RapCount);

	// �J�n�O��������
	if (IsBeforeStart && !isDriftJump_) {

		// �v���C���[�̈ʒu�������ʒu�ɖ߂��B
		pos_ = PLAYER_DEF_POS;

		EngineSineWave();

	}

	// ���W���X�V�B
	PolygonInstanceRegister::Ins()->ChangeTrans(playerModel_.carBodyInsIndex_, pos_);

	// �J�n�O��������
	if (IsBeforeStart && !isDriftJump_) {

		// �v���C���[�̈ʒu�������ʒu�ɖ߂��B
		pos_ = PLAYER_DEF_POS;

	}

	// �ԑ̂��X���鏈���B
	InclineCarBody();


	// �󒆂ɂ���Ƃ��͏����n�_�܂Ŗ߂�^�C�}�[���X�V�B�n��ɗv��Ƃ��̓^�C�}�[���������B
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

	// OBB���X�V�B
	obb_->SetMat(playerModel_.carBodyInsIndex_);

	// ���W��ۑ��B
	prevPos_ = pos_;

	// �^�C�����X�V����B
	for (auto& index : tires_) {

		index->Update();

	}


	// �u�[�X�g����^�C�}�[�����ȏゾ�����������������B
	if (0 < boostTimer_) {

		--boostTimer_;

		boostSpeed_ = MAX_BOOST_SPEED;

		// �����̊��������߂�B
		float boostRate = boostSpeed_ / MAX_BOOST_SPEED;

		// ���̑傫�������߂�B
		bool IsSmokeBig = 0.5f < boostRate;

		// �ݒu���Ă����牌�𐶐��B
		if (onGround_) {
			Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
			DriftParticleMgr::Ins()->GenerateSmoke(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * 30.0f, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, IsSmokeBig, DriftParticleMgr::DELAY_ID::DASH);
			driftVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
			DriftParticleMgr::Ins()->GenerateSmoke(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * 30.0f, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, IsSmokeBig, DriftParticleMgr::DELAY_ID::DASH);
		}

		// �ݒu���Ă��Ĉړ����x�����ȏゾ�����牊�𐶐��B
		if (15.0f < boostSpeed_) {
			DriftParticleMgr::Ins()->GenerateFire(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData);
		}

	}

	// �J�n�O�ŃA�N�Z���𓥂�ł�����B
	if (isAccel_ && IsBeforeStart) {

		// �����o���^�C�}�[���X�V�B
		++beforeStartSmokeTimer_;
		if (BEFORE_START_SMOKE_TIMER < beforeStartSmokeTimer_) {

			beforeStartSmokeTimer_ = 0;

			Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
			// ���E�ɎU�炷�B
			Vec3 generatePos = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * FHelper::GetRand(-2, 2);
			// �������Ɏ����Ă���B
			generatePos += -forwardVec_ * 20.0f;
			DriftParticleMgr::Ins()->GenerateSmoke(generatePos, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, false, DriftParticleMgr::DELAY_ID::NONE_DELAY, -forwardVec_);

		}

	}

	// ���n�����u�Ԃ�������B
	bool isJumpDriftRelease = (isDriftJumpPrev_ && !isDriftJump_);
	bool onGroundTrigger = (!onGroundPrev_ && onGround_);
	if ((isJumpDriftRelease || onGroundTrigger) && !IsBeforeStart) {

		// �O�񃉃��_���Ɉʒu�����炵�Đ�������B
		Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
		for (int index = 0; index < 3; ++index) {

			DriftParticleMgr::Ins()->GenerateSmoke(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * 30.0f, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, false, DriftParticleMgr::DELAY_ID::NONE_DELAY);

		}
		// �O�񃉃��_���Ɉʒu�����炵�Đ�������B
		for (int index = 0; index < 3; ++index) {
			DriftParticleMgr::Ins()->GenerateSmoke(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) - driftVec * 30.0f, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, false, DriftParticleMgr::DELAY_ID::NONE_DELAY);

		}

	}

	// �h���t�g���͉����o���B
	if (isDrift_ && onGround_) {


		// ���݂̃��x���B
		int nowLevel = 0;
		for (auto& index : DRIFT_TIMER) {

			if (driftTimer_ < index) continue;

			nowLevel = static_cast<int>(&index - &DRIFT_TIMER[0]);

		}

		// ���݂̃h���t�g���x����1�ȏゾ������p�[�e�B�N���ƃI�[�����o���B
		if (1 <= nowLevel) {

			if (!DriftParticleMgr::Ins()->IsAuraGenerated()) {
				DriftParticleMgr::Ins()->GenerateAura(playerModel_.carBehindTireInsIndex_, static_cast<int>(DriftParticle::ID::AURA_BIG), isDriftRight_, 2 <= nowLevel, ConstBufferData);
				DriftParticleMgr::Ins()->GenerateAura(playerModel_.carBehindTireInsIndex_, static_cast<int>(DriftParticle::ID::AURA_SMALL), isDriftRight_, 2 <= nowLevel, ConstBufferData);
			}

			// ���[�g�����߂�B
			float rate = 0;
			rate = FHelper::Saturate(static_cast<float>(driftTimer_) / static_cast<float>(DRIFT_TIMER[0]));

			// �p�[�e�B�N���𐶐�����B
			DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInsIndex_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), rate, false, DriftParticleMgr::DELAY_ID::DELAY1, ConstBufferData);

			// ���x����������u�Ԃ��������C�Ƀp�[�e�B�N���𐶐�����B
			if (driftTimer_ == DRIFT_TIMER[0] || driftTimer_ == DRIFT_TIMER[1] || driftTimer_ == DRIFT_TIMER[2]) {

				DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInsIndex_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1, ConstBufferData);
				DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInsIndex_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1, ConstBufferData);
				DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInsIndex_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1, ConstBufferData);
				DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInsIndex_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1, ConstBufferData);
				DriftParticleMgr::Ins()->GenerateDriftParticle(playerModel_.carBehindTireInsIndex_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1, ConstBufferData);

				// �I�[������U�j��
				DriftParticleMgr::Ins()->DestroyAura();

			}

		}

		// �����o���B
		Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1 * isDriftRight_ ? -1.0f : 1.0f, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
		DriftParticleMgr::Ins()->GenerateSmoke(PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carBehindTireInsIndex_) + driftVec * 30.0f, PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_), ConstBufferData, nowLevel < 1, DriftParticleMgr::DELAY_ID::DEF);



	}
	else {

		// �I�[����j�� �֐����ɓ�d����΍�̏�����������B
		DriftParticleMgr::Ins()->DestroyAura();

	}

	// �ړ��ł��Ȃ��^�C�}�[���X�V����B
	if (0 < canNotMoveTimer_) {

		--canNotMoveTimer_;

		// �C�[�W���O�ʂ����߂�B
		float easingAmount = static_cast<float>(canNotMoveTimer_) / static_cast<float>(CAN_NOT_MOVE_TIMER_SHELL_HIT);

		easingAmount = easingAmount * easingAmount * easingAmount * easingAmount * easingAmount;

		rotY_ = shellHitRot_ + easingAmount * DirectX::XM_2PI;

	}

	// �u�[�X�g�ʂ����ȏゾ������W�������o���B
	isConcentrationLine_ = MAX_BOOST_SPEED / 2.0f < boostSpeed_;

	// �ڒn�t���O��ۑ��B
	onGroundPrev_ = onGround_;

	// �Q�[���J�n�O�t���O��ۑ��B
	isBeforeStartPrev_ = IsBeforeStart;

}

void Character::Draw()
{

	/*===== �`�揈�� =====*/

}

bool Character::CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, TireMaskUV& TireMaskUVData)
{

	/*===== �^�C���������o =====*/

	if (!isTireMask_) {

		tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

		return false;

	}

	FHelper::RayToModelCollisionData InputRayData;
	InputRayData.targetPolygonData_ = PolygonInstanceRegister::Ins()->GetMeshCollisionData(BaseStageData.lock()->stageObjectMgr_->GetInstanceIndex(0));

	// �߂�n�ۑ��p
	Vec3 ImpactPos;
	Vec3 HitNormal;
	Vec2 HitUV;
	float HitDistance;

	// ���O�^�C��
	InputRayData.rayPos_ = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carLeftTireInsIndex_);
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

	// �^�C�����̈ʒu�����o
	bool isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

	// �����蔻�肪���������ǂ������`�F�b�N�B
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

	// �E�O�^�C��
	InputRayData.rayPos_ = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carRightTireInsIndex_);
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

	// �^�C�����̈ʒu�����o
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

	// �E���^�C��
	InputRayData.rayPos_ = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carRightTireInsIndex_);
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

	// ��]�������x�N�g�������߂�B
	Vec3 behindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
	InputRayData.rayPos_ += behindVec * 70.0f;

	// �^�C�����̈ʒu�����o
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

	// �����^�C��
	InputRayData.rayPos_ = PolygonInstanceRegister::Ins()->GetWorldPos(playerModel_.carLeftTireInsIndex_);
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

	// ��]�������x�N�g�������߂�B
	behindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
	InputRayData.rayPos_ += behindVec * 70.0f;

	// �^�C�����̈ʒu�����o
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

Vec3 Character::GetCameraForwardVec()
{
	/*===== �J�����p���ʃx�N�g���擾�֐� =====*/

	//Vec3 movedVec = pos_ - prevPos_;

	return forwardVec_;

	//return FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

	//// �ړ����Ă��Ȃ�������B
	//if (movedVec.Length() == 0) {

	//	return forwardVec_;

	//}

	//// �ړ����Ă�����B
	//return movedVec.GetNormal();

}

float Character::GetNowSpeedPer()
{

	/*====== �ړ����x�̊������v�Z =====*/

	float per = (speed_ + boostSpeed_ + jumpBoostSpeed_) / (MAX_SPEED + MAX_BOOST_SPEED);

	if (1.0f < per) per = 1.0f;

	return per;

}

bool Character::GetIsItem()
{
	return item_.operator bool();
}

void Character::Input(RayConstBufferData& ConstBufferData, const bool& IsBeforeStart)
{

	/*===== ���͏��� =====*/

	ConstBufferData;

	// ����I�u�W�F�N�g����̓��͂��󂯎��B
	BaseOperationObject::OperationInputData operationInputData;
	operationInputData.pos_ = pos_;
	operationInputData.forwradVec_ = forwardVec_;
	BaseOperationObject::Operation operation = operationObject_->Input(operationInputData);

	// �^�C���̃}�X�N�̃t���O������������B
	isTireMask_ = false;

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

	// ���݂̃t���[���̉E�X�e�B�b�N�̌X����B
	float nowFrameInputLeftStickHori = 0;

	// �E�X�e�B�b�N�̉��̌X���ʂŃL��������]������B
	if ((operation.handleDriveRate_ != 0 || isDrift_) && !IsBeforeStart) {

		// ��]�ʂ̍ő�l �ʏ��Ԃƃh���t�g��ԂňႤ�B
		float handleAmount = HANDLE_NORMAL;

		// [�h���t�g���] [�J�n�O����Ȃ�] ���������]�ʂ𑽂����ɂ���B
		if (isDrift_ && !IsBeforeStart) {

			handleAmount = HANDLE_DRIFT;

			// �h���t�g�̌����ɂ���ĉ�]�ʂ�ς���B
			if (isDriftRight_ && operation.handleDriveRate_ < 0.1f) {
				operation.handleDriveRate_ = 0.1f;
			}
			if (!isDriftRight_ && -0.1f < operation.handleDriveRate_) {
				operation.handleDriveRate_ = -0.1f;
			}

			// �^�C������]������B
			for (auto& index : tires_) {

				index->Rot(true, static_cast<float>(operation.handleDriveRate_));

			}

			isTireMask_ = true;


		}
		// �h���t�g��Ԃ���Ȃ�������B
		else {

			// �^�C������]������B
			for (auto& index : tires_) {

				index->Rot(true, static_cast<float>(operation.handleDriveRate_));

			}

		}

		// �N�H�[�^�j�I�������߂�B
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(upVec_.ConvertXMVECTOR(), handleAmount * operation.handleDriveRate_);

		// ���߂��N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// ��]�����Z����B
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, quaternionMat);
		nowFrameInputLeftStickHori = handleAmount * operation.handleDriveRate_;
		rotY_ += handleAmount * operation.handleDriveRate_;

		// ���ʃx�N�g�����Ԃ̉�]�s�񕪉�]������B
		forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));


		// �E�C���J�[�̐F��ς���^�C�}�[���X�V�B
		++turningIndicatorTimer_;
		if (TURNING_INDICATOR_TIMER < turningIndicatorTimer_) {

			turningIndicatorTimer_ = 0;
			IsTurningIndicatorRed_ = IsTurningIndicatorRed_ ? false : true;

		}

		// �E�C���J�[�̐F��ς���B
		if (IsTurningIndicatorRed_) {

			// �Ȃ����Ă���̂��E��������B
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

		// �Ԃ̃��C�g�̐F�����ɖ߂��B
		BLASRegister::Ins()->ChangeTex(playerModel_.carRightLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));
		BLASRegister::Ins()->ChangeTex(playerModel_.carLeftLightBlasIndex_, 0, TextureManager::Ins()->LoadTexture(L"Resource/Game/white.png"));

		// �e�ϐ����������B
		IsTurningIndicatorRed_ = false;
		turningIndicatorTimer_ = 100;	// �`�J�`�J����^�C�}�[���������B

	}

	// �h���t�g�̓��͏���ۑ�����B
	isInputLTPrev_ = isInputLT_;
	isInputLT_ = operation.isDrift_;

	// �h���t�g�W�����v�̃t���O��ۑ����Ă����B
	isDriftJumpPrev_ = isDriftJump_;

	// �h���t�g�{�^���̓��͂��g���K�[��������B
	bool triggerDriftBottom = !isInputLTPrev_ && isInputLT_;
	bool notJump = !isDriftJump_ && driftJumpSpeed_ <= 0.0f;
	if (triggerDriftBottom && notJump && onGround_) {

		isDriftJump_ = true;
		driftJumpVec_ = upVec_;
		driftJumpSpeed_ = DRIFT_JUMP_SPEED;

	}

	// �h���t�g��ԂŃ^�C�}�[���J�E���g����Ă�����B
	bool releaseDriftBottom = isInputLTPrev_ && !isInputLT_;
	if (isDrift_ && 0 < driftTimer_ && releaseDriftBottom) {

		isDrift_ = false;

		// �^�C�}�[�ɉ����ĉ���������B
		int driftLevel = -1;
		for (auto& index : DRIFT_TIMER) {

			if (driftTimer_ < index) break;

			driftLevel = static_cast<int>(&index - &DRIFT_TIMER[0]);

		}

		// �h���t�g���x����0�ȏゾ�������������B
		if (driftLevel != -1) {
			boostTimer_ = DRIFT_BOOST[driftLevel];
		}

	}

	// �o�O�����LT��������Ă��Ȃ�������h���t�g����������B �h���t�g�����̐��K�̏����͂��̏����̏�ɂ���̂Œʏ�͖��Ȃ��B
	// �J�n�O�̏�Ԃł��h���t�g����������B
	if (!isInputLT_ || IsBeforeStart) {

		isDrift_ = false;
		driftTimer_ = 0;

	}

	// ���̃L�������S�[�X�g�������ꍇ�A�A�C�e���擾���߂��o����A�C�e�����擾����B
	if (charaID_ == CHARA_ID::GHOST && operation.isGetItem_) {

		item_ = std::make_shared<BoostItem>();
		item_->Generate(playerModel_.carBodyInsIndex_);

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

	// �W�����v�A�N�V�����̃g���K�[����B
	isJumpActionTrigger_ = operation.isJumpActionTrigger_;

	// ���͂�ۑ�����B
	handleAmount_ = operation.handleDriveRate_;

	// �f�o�b�O�p B�{�^���������ꂽ�珉���ʒu�ɖ߂��B
	if (Input::Ins()->IsPadBottom(XINPUT_GAMEPAD_B) || Input::Ins()->IsKeyTrigger(DIK_R)) {

		pos_ = PLAYER_DEF_POS;
		PolygonInstanceRegister::Ins()->ChangeTrans(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));
		forwardVec_ = Vec3(0, 0, -1);
		rotY_ = 0;
		upVec_ = Vec3(0, 1, 0);

	}

}

void Character::Move(const bool& IsBeforeStart)
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

	// ���W�ړ�������B
	pos_ += forwardVec_ * (speed_ + boostSpeed_ + jumpBoostSpeed_);

	// �h���t�g���̃u�[�X�g�ړ��ʂ�0�ɋ߂Â���B
	if (0 < boostSpeed_) {

		boostSpeed_ -= SUB_BOOST_SPEED;

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

	// ���W�ɏd�͂����Z����B
	pos_ += Vec3(0, -1, 0) * gravity_;

	// ���x�N�g�����Ԃ̉�]�s�񕪉�]������B
	bottomVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

}

void Character::UpdateDrift(const bool& IsBeforeStart)
{

	/*===== �h���t�g�Ɋւ���X�V���� =====*/

	// �h���t�g�W�����v���Ă�����B
	if (isDriftJump_) {

		// ���W���ړ�������B
		pos_ += driftJumpVec_ * driftJumpSpeed_;
		prevPos_ += driftJumpVec_ * driftJumpSpeed_;

		// �h���t�g�ʂ����炷�B
		driftJumpSpeed_ -= SUB_DRIFT_JUMP_SPEED;
		if (driftJumpSpeed_ < 0.0f) {

			driftJumpSpeed_ = 0.0f;

		}

		// �X�e�B�b�N���͂���������B
		if (0.1f < std::fabs(handleAmount_)) {

			isDrift_ = true;
			isDriftRight_ = 0.0f < handleAmount_;

		}



	}

	// �h���t�g����������B
	if (isDrift_) {

		// �h���t�g�̌o�ߎ��Ԃ̃^�C�}�[���X�V�B
		++driftTimer_;

	}
	else {

		driftTimer_ = 0;

	}

	// �W�����v����F�ł��̏������ʂ�Ȃ��悤�ɂ��邽�߂̏���
	bool isNotTriggerLT = !(!isInputLTPrev_ && isInputLT_);
	// �ݒu������h���t�g�W�����v����������B
	if (isDriftJump_ && onGround_ && isNotTriggerLT) {

		isDriftJump_ = false;
		driftJumpSpeed_ = 0.0f;

	}

}

void Character::CheckHit(std::weak_ptr<BaseStage> StageData, bool& IsPassedMiddlePoint, int& RapCount)
{

	/*===== �����蔻�� =====*/

	// �����蔻��֐��ɓ����l��ݒ�B
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

	// �����蔻��֐�����Ԃ��Ă���l�B
	BaseStage::ColliderOutput output;

	// �����蔻����s���B
	output = StageData.lock()->Collider(input);

	// �����蔻��̌��ʂ��珈�����s���B
	if (output.isHitBoostGimmick_) {

		// �u�[�X�g���}�b�N�X�ɂ���B
		boostSpeed_ = MAX_BOOST_SPEED;
		boostTimer_ = 20;

	}
	if (output.isHitGoal_) {

		// �S�[��
		if (IsPassedMiddlePoint) {
			IsPassedMiddlePoint = false;
			++RapCount;

		}

		operationObject_->OnGoal();

	}
	if (output.isHitMiddlePoint_) {

		// ���Ԓn�_�Ƃ̓����蔻��
		IsPassedMiddlePoint = true;

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

	}
	// �S�[�X�g�������瓖���蔻����΂��B
	if (output.isHitItemBox_ && charaID_ != CHARA_ID::GHOST && !item_.operator bool()) {

		// �A�C�e�����擾�����t���O
		isGetItem_ = true;

		if (GameSceneMode::Ins()->id_ == GameSceneMode::MODE_ID::WRITE_GHOST || GameSceneMode::Ins()->id_ == GameSceneMode::MODE_ID::GHOST) {

			item_ = std::make_shared<BoostItem>();
			item_->Generate(playerModel_.carBodyInsIndex_);

		}
		else {

			// �����_���ŃA�C�e���𐶐�����B
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

		// �A�C�e�����擾���Ȃ�����
		isGetItem_ = false;

	}

	// �i�������I�u�W�F�N�g�Ɠ������Ă��āA�W�����v�A�N�V�����{�^���������Ă�����B
	if (output.isHitStepBoostGimmick_ && isJumpActionTrigger_) {

		// ����������B
		jumpBoostSpeed_ = JUMP_BOOST_SPEED;

	}

	// ���̑��̕ϐ����������B
	pos_ = output.resultPos_;

	// �S�[�X�g�ȊO��������B
	if (charaID_ != CHARA_ID::GHOST) {

		// �b���Ƃ̓����蔻��
		bool isHitShell = ShellObjectMgr::Ins()->Collider(obb_);

		if (isHitShell) {

			canNotMoveTimer_ = CAN_NOT_MOVE_TIMER_SHELL_HIT;
			shellHitRot_ = rotY_;

		}

	}





	/*	if (charaID_ == CHARA_ID::P1 && (Input::Ins()->IsKeyTrigger(DIK_1) || Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_LEFT_SHOULDER))) {

			jumpBoostSpeed_ = JUMP_BOOST_SPEED;

		}
		else */if (charaID_ == CHARA_ID::P1 && (Input::Ins()->IsKeyTrigger(DIK_2) || Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_LEFT_SHOULDER))) {

			boostSpeed_ = MAX_BOOST_SPEED;
			boostTimer_ = 10;

		}
		else {

			jumpBoostSpeed_ -= SUB_JUMP_BOOST_SPEED;
			if (jumpBoostSpeed_ < 0) {

				jumpBoostSpeed_ = 0;

			}

		}

}

void Character::RotObliqueFloor(const Vec3& HitNormal)
{

	/*===== �΂ߏ��̉�]���� =====*/

	/*-- �O�ς����]�����擾 --*/

	// �f�t�H���g�̏�x�N�g���B
	Vec3 defUpVec = Vec3(0, 1, 0);

	// ��]�������߂�B
	Vec3 axisOfRevolution = defUpVec.Cross(HitNormal);

	// ��]���𐳋K������B
	if (axisOfRevolution.Length() != 0) {
		axisOfRevolution.Normalize();
	}

	/*-- ���ς����]�ʂ��擾 --*/

	// ��]�ʂ����߂�B
	float amountOfRotation = defUpVec.Dot(HitNormal);

	// �t�]�������߂�֐����g�p���ċ��߂�cos�Ƃ����W�A���ɕϊ��B
	amountOfRotation = acosf(amountOfRotation);


	/*-- �N�H�[�^�j�I�����g���ĉ�] --*/

	// ��]����{0,0,0}�������珈�����΂��B
	if (axisOfRevolution.Length() != 0 && amountOfRotation != 0) {

		// �N�H�[�^�j�I�������߂�B
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(axisOfRevolution.ConvertXMVECTOR(), amountOfRotation);

		// ���߂��N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// �v���C���[����]������B
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, quaternionMat);

		// ��x�N�g������Ƃ����N�H�[�^�j�I�������߂�B
		Vec3 normal_ = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), rotY_);

		// �N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// �v���C���[����]������B
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, upQuaternionMat);


		/*-- �v���C���[�̉�]�s������ƂɊe�x�N�g������] --*/

		// ��]�s����擾�B
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_);

		// ��x�N�g�����X�V�B
		upVec_ = normal_;


		//���ʃx�N�g�����X�V�B
		forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

	// ���������@�����ゾ�����ꍇ�͍s������������ĉ�]������B
	if (HitNormal == Vec3(0, 1, 0)) {

		// �v���C���[����]������B
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, DirectX::XMMatrixIdentity());

		// ��x�N�g������Ƃ����N�H�[�^�j�I�������߂�B
		Vec3 normal_ = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), rotY_);

		// �N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// �v���C���[����]������B
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, upQuaternionMat);


		/*-- �v���C���[�̉�]�s������ƂɊe�x�N�g������] --*/

		// ��]�s����擾�B
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_);

		// ��x�N�g�����X�V�B
		upVec_ = normal_;

		//���ʃx�N�g�����X�V�B
		forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

}

void Character::InclineCarBody()
{

	/*===== �ԑ̂��X���鏈�� =====*/

	// BODY�̉�]�s���ۑ��B
	defBodyMatRot_ = PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_);

	// �󒆂ɋ�����B
	if (!onGround_ && !isDriftJump_) {

		// �ړ����������x�N�g��
		Vec3 movedVec = (pos_ - prevPos_).GetNormal();

		if (!std::isnan(movedVec.x_) && charaID_ == CHARA_ID::P1) {

			// �f�t�H���g�̏�x�N�g���B
			Vec3 defUpVec = Vec3(0, -1, 0);

			// ��]�������߂�B
			Vec3 axisOfRevolution = defUpVec.Cross(movedVec);

			// ��]���𐳋K������B
			if (axisOfRevolution.Length() != 0) {
				axisOfRevolution.Normalize();
			}

			/*-- ���ς����]�ʂ��擾 --*/

			// ��]�ʂ����߂�B
			float amountOfRotation = defUpVec.Dot(movedVec);

			// �t�]�������߂�֐����g�p���ċ��߂�cos�Ƃ����W�A���ɕϊ��B
			amountOfRotation = acosf(amountOfRotation) - DirectX::XM_PIDIV2;


			/*-- �N�H�[�^�j�I�����g���ĉ�] --*/

			// ��]����{0,0,0}�������珈�����΂��B
			if (axisOfRevolution.Length() != 0 && amountOfRotation != 0) {

				// �N�H�[�^�j�I�������߂�B
				DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(axisOfRevolution.ConvertXMVECTOR(), amountOfRotation);

				// ���߂��N�H�[�^�j�I�����s��Ɏ����B
				DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

				// �v���C���[����]������B
				PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, quaternionMat);

				// ��x�N�g������Ƃ����N�H�[�^�j�I�������߂�B
				Vec3 normal_ = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
				DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), rotY_);

				// �N�H�[�^�j�I�����s��Ɏ����B
				DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

				// �v���C���[����]������B
				PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, upQuaternionMat);

			}
		}

	}

	// �u�[�X�g���̉�]�B
	if (0 < boostSpeed_) {

		float boostRate = boostSpeed_ / MAX_BOOST_SPEED;
		const float MAX_ROT = 0.7f;

		baseBoostRot_ = MAX_ROT * boostRate;

		// �o�����X�����^�C�}�[���X�V�B
		++forwardTireLollingTimer_;
		if (FORWARD_TIMER_LOLLING_TIMER < forwardTireLollingTimer_) {

			forwardTireLollingTimer_ = 0;

			// �o�����X������Ă��镗�ɂ��邽�߂Ƀ����_���ŉ�]�ʂ�ύX����B
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

		// �N�H�[�^�j�I�������߂�B
		boostRotQ_ = DirectX::XMQuaternionRotationAxis(horiVec.ConvertXMVECTOR(), nowBoostRot_);

		// �u�[�X�g�̎ԑ̂̃N�H�[�^�j�I�����ԁB
		nowBoostRotQ_ = DirectX::XMQuaternionSlerp(nowBoostRotQ_, boostRotQ_, 0.2f);
		// ��]�s������߂�B
		DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(nowBoostRotQ_);
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, mat);

	}


	// �h���t�g���̉�]�B
	if (isDrift_ && 0 != handleAmount_) {

		// �O�t���[���Ɖ�]�������Ⴉ�����珉��������B
		if (isRotRightSide_ && handleAmount_ < 0) {
			handleRotQ_ = DirectX::XMVECTOR();
			baseDriftRot_ = 0;
			driftRotTimer_ = 0;
		}

		++driftRotTimer_;
		if (MAX_DRIFT_ROT_TIMER < driftRotTimer_) driftRotTimer_ = MAX_DRIFT_ROT_TIMER;

		float timerRate = static_cast<float>(driftRotTimer_) / static_cast<float>(MAX_DRIFT_ROT_TIMER);

		// ��]�ʂɃC�[�W���O��������B �}�W�b�N�i���o�[�͌X���̔������p�B
		if (0.35f < fabs(handleAmount_)) {
			baseDriftRot_ = 0.6f * (0 < handleAmount_ ? 1.0f : -1.0f);
		}
		else {
			baseDriftRot_ = std::fabs(FEasing::EaseOutCubic(timerRate) * MAX_DRIFT_ROT) * handleAmount_;
		}

		// ��]������ۑ��B
		isRotRightSide_ = 0 < handleAmount_;

		// �o�����X������Ă��镗�ɂ��邽�߂Ƀ����_���ŉ�]�ʂ�ύX����B
		const float MAX_RANDOM = 0.1f;
		tireLollingAmount_ = FHelper::GetRand(-MAX_RANDOM * timerRate, MAX_RANDOM * timerRate);
		baseDriftRot_ += tireLollingAmount_;

	}
	else {

		handleRotQ_ = DirectX::XMVECTOR();

		baseDriftRot_ = 0;
		driftRotTimer_ = 0;

	}

	// ��]������B
	{
		nowDriftRot_ += (baseDriftRot_ - nowDriftRot_) / 10.0f;

		Vec3 horiVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
		Vec3 forwardVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));
		Vec3 upVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

		// ���N�H�[�^�j�I�������߂�B
		handleRotQ_ = DirectX::XMQuaternionRotationAxis(horiVec.ConvertXMVECTOR(), nowDriftRot_);
		// ���ʃN�H�[�^�j�I�������߂�B ���������Ɖ�]��������Ƃ�������Ă��邽�߁B
		DirectX::XMVECTOR forwardQ = DirectX::XMQuaternionRotationAxis(forwardVec.ConvertXMVECTOR(), 0.01f);
		// ��N�H�[�^�j�I�������߂�B
		DirectX::XMVECTOR upQ = DirectX::XMQuaternionRotationAxis(upVec.ConvertXMVECTOR(), -0.2f * handleAmount_);

		// �N�H�[�^�j�I����������B
		handleRotQ_ = DirectX::XMQuaternionMultiply(handleRotQ_, forwardQ);
		handleRotQ_ = DirectX::XMQuaternionMultiply(handleRotQ_, upQ);
	}

	// �u�[�X�g�̎ԑ̂̃N�H�[�^�j�I�����ԁB
	nowHandleRotQ_ = DirectX::XMQuaternionSlerp(nowHandleRotQ_, handleRotQ_, 0.2f);
	// ��]�s������߂�B
	DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(nowHandleRotQ_);
	PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, mat);

}

void Character::EngineSineWave()
{

	/*===== �G���W���̏㉺ =====*/

	// �T�C���g�p�̃^�C�}�[���X�V����B
	beforeStartWaveTimer_ += 1.5f;

	// �T�C���g���v�Z����B
	float sineWave = std::sin(beforeStartWaveTimer_) * (isAccel_ ? BEFORE_START_WAVE_LENGTH_ACCELL : BEFORE_START_WAVE_LENGTH_DEF);

	// �������B
	pos_.y_ += sineWave;

}
