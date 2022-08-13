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

Character::Character(CHARA_ID CharaID)
{

	/*===== ���������� =====*/

	// �L������ID��ۑ�
	charaID_ = CharaID;

	// �L����ID�ɉ����đ���I�u�W�F�N�g�𐶐��B
	if (charaID_ == CHARA_ID::P1) {

		operationObject_ = std::make_shared<PlayerOperationObject>(0);

		// �Ԃ̃��f�������[�h
		playerModel_.Load(PlayerModel::COLOR::RED);

	}
	else if (charaID_ == CHARA_ID::AI1) {

		operationObject_ = std::make_shared<FirstAIOperationObject>(static_cast<int>(FirstAIWayPointMgr::WAYPOINT_OFFSET::LEFT_LEARNING));

		// �Ԃ̃��f�������[�h
		playerModel_.Load(PlayerModel::COLOR::BLUE);

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
	isTireMask_ = false;
	IsTurningIndicatorRed_ = false;
	turningIndicatorTimer_ = 0;
	canNotMoveTimer_ = 0;

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
	IsTurningIndicatorRed_ = false;
	isTireMask_ = false;
	PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));

}

void Character::Update(std::weak_ptr<BaseStage> StageData, RayConstBufferData& ConstBufferData, bool& IsPassedMiddlePoint, int& RapCount)
{


	/*===== �X�V���� =====*/

	// ���͏���
	Input(ConstBufferData);

	// �ړ�����
	Move();

	// �����蔻��
	CheckHit(StageData, IsPassedMiddlePoint, RapCount);

	// ���W���X�V�B
	PolygonInstanceRegister::Ins()->ChangeTrans(playerModel_.carBodyInsIndex_, pos_);

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



	// �A�C�e���e�X�g�p
	if (Input::Ins()->IsKeyTrigger(DIK_P) && item_.operator bool()) {

		if (item_->GetItemID() == BaseItem::ItemID::BOOST) {

			boostSpeed_ = MAX_BOOST_SPEED;
			boostTimer_ = 30;
			item_.reset();

		}
		else {

			item_->Use(rotY_, static_cast<int>(ShellItem::PARAM_ID::BEHIND));

		}


	}

	if (Input::Ins()->IsKeyRelease(DIK_P) && item_.operator bool()) {

		if (isShotBehind_) {
			item_->Use(rotY_, static_cast<int>(ShellItem::PARAM_ID::BEHIND_THROW));
		}
		else {
			item_->Use(rotY_, static_cast<int>(ShellItem::PARAM_ID::FORWARD_THROW));
		}
		item_.reset();


	}


	// �u�[�X�g����^�C�}�[�����ȏゾ�����������������B
	if (0 < boostTimer_) {

		--boostTimer_;

		boostSpeed_ = MAX_BOOST_SPEED;

	}

	// �ړ��ł��Ȃ��^�C�}�[���X�V����B
	if (0 < canNotMoveTimer_) {

		--canNotMoveTimer_;

		// �C�[�W���O�ʂ����߂�B
		float easingAmount = static_cast<float>(canNotMoveTimer_) / static_cast<float>(CAN_NOT_MOVE_TIMER_SHELL_HIT);

		easingAmount = easingAmount * easingAmount * easingAmount * easingAmount * easingAmount;

		rotY_ = shellHitRot_ + easingAmount * DirectX::XM_2PI;

	}

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
	InputRayData.targetVertex_ = BLASRegister::Ins()->GetBLAS()[BaseStageData.lock()->stageObjectMgr_->GetBlasIndex(0)]->GetVertexPos();
	InputRayData.targetNormal_ = BLASRegister::Ins()->GetBLAS()[BaseStageData.lock()->stageObjectMgr_->GetBlasIndex(0)]->GetVertexNormal();
	InputRayData.targetIndex_ = BLASRegister::Ins()->GetBLAS()[BaseStageData.lock()->stageObjectMgr_->GetBlasIndex(0)]->GetVertexIndex();
	InputRayData.targetUV_ = BLASRegister::Ins()->GetBLAS()[BaseStageData.lock()->stageObjectMgr_->GetBlasIndex(0)]->GetVertexUV();
	InputRayData.matTrans_ = PolygonInstanceRegister::Ins()->GetTrans(BaseStageData.lock()->stageObjectMgr_->GetBlasIndex(0));
	InputRayData.matScale_ = PolygonInstanceRegister::Ins()->GetScale(BaseStageData.lock()->stageObjectMgr_->GetBlasIndex(0));
	InputRayData.matRot_ = PolygonInstanceRegister::Ins()->GetRotate(BaseStageData.lock()->stageObjectMgr_->GetBlasIndex(0));

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

	return true;


}

float Character::GetNowSpeedPer()
{

	/*====== �ړ����x�̊������v�Z =====*/

	float per = (speed_ + boostSpeed_) / (MAX_SPEED + MAX_BOOST_SPEED);

	return per;

}

void Character::Input(RayConstBufferData& ConstBufferData)
{

	/*===== ���͏��� =====*/

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

	// RT��������Ă���������B
	if (0 < operation.accelerationRate_ && onGround_) {

		speed_ += operation.accelerationRate_ * ADD_SPEED;

	}
	else if (onGround_) {

		// �ړ����Ă��Ȃ��Ēn��ɂ�����ړ��ʂ�0�ɋ߂Â���B
		speed_ -= speed_ / 10.0f;

	}
	else {

		// �ړ����Ă��Ȃ��ċ󒆂ɂ�����ړ��ʂ�0�ɋ߂Â���B
		speed_ -= speed_ / 200.0f;

	}

	// �A�C�e�������ɓ�����t���O���X�V�B
	isShotBehind_ = operation.isShotBehind_;

	// ���݂̃t���[���̉E�X�e�B�b�N�̌X����B
	float nowFrameInputLeftStickHori = 0;

	// �E�X�e�B�b�N�̉��̌X���ʂŃL��������]������B
	if (operation.handleDriveRate_ != 0) {

		// ��]�� �ʏ��Ԃƃh���t�g��ԂňႤ�B
		float handleAmount = HANDLE_NORMAL;

		// �h���t�g��Ԃ��������]�ʂ𑽂����ɂ���B
		if (isDrift_) {

			handleAmount = HANDLE_DRIFT;

			// ���łɃh���t�g��Ԃ̎��̃u�[�X�g����܂ł̃^�C�}�[���X�V����B
			if (onGround_) {
				++driftBoostTimer_;
				if (DRIFT_BOOST_TIMER < driftBoostTimer_) driftBoostTimer_ = DRIFT_BOOST_TIMER;
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
		rotY_ += handleAmount * operation.handleDriveRate_;
		nowFrameInputLeftStickHori = handleAmount * operation.handleDriveRate_;

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

	// LT��������Ă�����h���t�g��Ԃɂ���B
	if (operation.isDrift_ && operation.handleDriveRate_ != 0) {

		isDrift_ = true;

		// �h���t�g�̃x�N�g�������߂�B
		Vec3 driftVec = Vec3();
		if (nowFrameInputLeftStickHori < 0) {

			driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

		}
		else {

			driftVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

		}

		// �h���t�g���̃p�[�e�B�N���𐶐��B
		//DriftParticleMgr::Ins()->Generate(pos, driftVec, PolygonInstanceRegister::Ins()->GetRotate(carInstanceIndex), ConstBufferData);
		ConstBufferData;

	}
	// ���łɃh���t�g���������珟��ɉ������Ȃ��悤�ɂ���B
	else if (operation.isDrift_ && isDrift_) {
	}
	else {

		// �h���X�g�̃u�[�X�g����܂ł̃^�C�}�[���K��l�ȏゾ������u�[�X�g����B
		if (DRIFT_BOOST_TIMER <= driftBoostTimer_) {

			boostSpeed_ = MAX_BOOST_SPEED;
			boostTimer_ = 10;

		}

		// �h���t�g�̃u�[�X�g����܂ł̃^�C�}�[������������B
		driftBoostTimer_ = 0;

		isDrift_ = false;

	}

	// �f�o�b�O�p B�{�^���������ꂽ�珉���ʒu�ɖ߂��B
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
	pos_ += forwardVec_ * (speed_ + boostSpeed_);

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

	// ���W�ɏd�͂����Z����B
	pos_ += Vec3(0, -1, 0) * gravity_;

	// ���x�N�g�����Ԃ̉�]�s�񕪉�]������B
	bottomVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

}

void Character::CheckHit(std::weak_ptr<BaseStage> StageData, bool& IsPassedMiddlePoint, int& RapCount)
{

	/*===== �����蔻�� =====*/

	// �����蔻��֐��ɓ����l��ݒ�B
	BaseStage::ColliderInput input;
	input.targetInsIndex_ = playerModel_.carBodyInsIndex_;
	input.targetOBB_ = obb_;
	input.targetPos_ = pos_;
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
		boostTimer_ = 10;

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
	if (output.isHitItemBox_) {

		// �����_���ŃA�C�e���𐶐�����B
		int random = FHelper::GetRand(0, 1000);

		//if (random % 2 == 0) {

		//	item_ = std::make_shared<BoostItem>();
		//	item_->Generate(playerModel_.carBodyInsIndex_);

		//}
		//else {

		item_ = std::make_shared<ShellItem>();
		item_->Generate(playerModel_.carBodyInsIndex_);

		//}

	}

	// ���̑��̕ϐ����������B
	pos_ = output.resultPos_;



	// �b���Ƃ̓����蔻��
	bool isHitShell = ShellObjectMgr::Ins()->Collider(obb_);

	if (isHitShell) {

		canNotMoveTimer_ = CAN_NOT_MOVE_TIMER_SHELL_HIT;
		shellHitRot_ = rotY_;

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
