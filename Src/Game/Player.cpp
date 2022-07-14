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

	/*===== ���������� =====*/

	// �Ԃ̃��f�������[�h
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

	// OBB�𐶐��B
	obb_.Setting(playerModel_.carBodyBlasIndex_, playerModel_.carBodyInsIndex_);

}

void Player::Init()
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
	isDrift_ = false;
	isGround_ = true;
	isGrass_ = false;
	PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carBodyInsIndex_, Vec3(0, 0, 0));

}

void Player::Update(RayConstBufferData& ConstBufferData, bool& IsPassedMiddlePoint, int& RapCount)
{

	/*===== �X�V���� =====*/

	// ���͏���
	Input(ConstBufferData);

	// �ړ�����
	Move();

	// �����蔻��
	CheckHit(IsPassedMiddlePoint, RapCount);

	// ���W���X�V�B
	PolygonInstanceRegister::Ins()->ChangeTrans(playerModel_.carBodyInsIndex_, pos_);

	// ���W��ۑ��B
	prevPos_ = pos_;

	// �󒆂ɂ���Ƃ��͏����n�_�܂Ŗ߂�^�C�}�[���X�V�B�n��ɗv��Ƃ��̓^�C�}�[���������B
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

	// OBB���X�V�B
	obb_.SetMat(playerModel_.carBodyInsIndex_);

}

void Player::Draw()
{

	/*===== �`�揈�� =====*/

}

float Player::GetNowSpeedPer()
{

	/*====== �ړ����x�̊������v�Z =====*/

	float per = (speed_ + boostSpeed_) / (MAX_SPEED + MAX_BOOST_SPEED);

	return per;

}

void Player::Input(RayConstBufferData& ConstBufferData)
{

	/*===== ���͏��� =====*/

	// RT��������Ă���������B
	const float INPUT_DEADLINE_TRI = 0.5f;
	float inputRightTriValue = Input::Ins()->PadTrigger(XINPUT_TRIGGER_RIGHT);
	if ((INPUT_DEADLINE_TRI < inputRightTriValue) && isGround_) {

		speed_ += inputRightTriValue * ADD_SPEED;

	}
	else if (Input::Ins()->IsKey(DIK_W) && isGround_) {

		speed_ += ADD_SPEED;

	}
	else if (isGround_) {

		// �ړ����Ă��Ȃ��Ēn��ɂ�����ړ��ʂ�0�ɋ߂Â���B
		speed_ -= speed_ / 10.0f;

	}
	else {

		// �ړ����Ă��Ȃ��ċ󒆂ɂ�����ړ��ʂ�0�ɋ߂Â���B
		speed_ -= speed_ / 200.0f;

	}

	// ���݂̃t���[���̉E�X�e�B�b�N�̌X����B
	float nowFrameInputLeftStickHori = 0;

	// �E�X�e�B�b�N�̉��̌X���ʂŃL��������]������B
	float inputLeftStickHori = Input::Ins()->PadStick(XINPUT_THUMB_LEFTSIDE);
	const float LEFT_STICK_INPUT_DEADLINE = 0.2f;
	int inputADKey = Input::Ins()->IsKey(DIK_D) - Input::Ins()->IsKey(DIK_A);
	if (LEFT_STICK_INPUT_DEADLINE < std::fabs(inputLeftStickHori) || inputADKey != 0) {

		// ��]�� �ʏ��Ԃƃh���t�g��ԂňႤ�B
		float handleAmount = HANDLE_NORMAL;

		// �h���t�g��Ԃ��������]�ʂ𑽂����ɂ���B
		if (isDrift_) {

			handleAmount = HANDLE_DRIFT;

			// ���łɃh���t�g��Ԃ̎��̃u�[�X�g����܂ł̃^�C�}�[���X�V����B
			if (isGround_) {
				++driftBoostTimer_;
				if (DRIFT_BOOST_TIMER < driftBoostTimer_) driftBoostTimer_ = DRIFT_BOOST_TIMER;
			}

			// �^�C������]������B
			Vec3 rot = Vec3(0.0f, 0.5f, 0.0f);
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carRightTireFrameInsIndex_, rot * static_cast<float>(inputADKey));
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carRightTireInsIndex_, rot * static_cast<float>(inputADKey));
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carLeftTireFrameInsIndex_, rot * static_cast<float>(inputADKey));
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carLeftTireInsIndex_, rot * static_cast<float>(inputADKey));

		}
		// �h���t�g��Ԃ���Ȃ�������B
		else {

			// �^�C������]������B
			Vec3 rot = Vec3(0.0f, 0.3f, 0.0f);
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carRightTireFrameInsIndex_, rot * static_cast<float>(inputADKey));
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carRightTireInsIndex_, rot * static_cast<float>(inputADKey));
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carLeftTireFrameInsIndex_, rot * static_cast<float>(inputADKey));
			PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carLeftTireInsIndex_, rot * static_cast<float>(inputADKey));

		}

		// �N�H�[�^�j�I�������߂�B
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(upVec_.ConvertXMVECTOR(), handleAmount * inputLeftStickHori + static_cast<float>(inputADKey) * handleAmount);

		// ���߂��N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// ��]�����Z����B
		PolygonInstanceRegister::Ins()->AddRotate(playerModel_.carBodyInsIndex_, quaternionMat);
		rotY_ += handleAmount * inputLeftStickHori + static_cast<float>(inputADKey) * handleAmount;
		nowFrameInputLeftStickHori = inputLeftStickHori + static_cast<float>(inputADKey) * handleAmount;

		// ���ʃx�N�g�����Ԃ̉�]�s�񕪉�]������B
		forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(playerModel_.carBodyInsIndex_));

	}
	else {

		// �^�C���̉�]���f�t�H���g�ɖ߂��B
		Vec3 rot = Vec3(0.0f, 0.0f, 0.0f);
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carRightTireFrameInsIndex_, rot);
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carRightTireInsIndex_, rot);
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carLeftTireFrameInsIndex_, rot);
		PolygonInstanceRegister::Ins()->ChangeRotate(playerModel_.carLeftTireInsIndex_, rot);

	}

	// LT��������Ă�����h���t�g��Ԃɂ���B
	const float INPUT_DEADLINE_DRIFT = 0.9f;
	float inputLeftTriValue = Input::Ins()->PadTrigger(XINPUT_TRIGGER_LEFT);
	bool isInputNowFrameLeftStrick = LEFT_STICK_INPUT_DEADLINE < fabs(nowFrameInputLeftStickHori) || inputADKey != 0;
	bool isInputLShift = Input::Ins()->IsKey(DIK_LSHIFT);
	if ((INPUT_DEADLINE_DRIFT < inputLeftTriValue || isInputLShift) && isInputNowFrameLeftStrick) {

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
	else if ((INPUT_DEADLINE_DRIFT < inputLeftTriValue || isInputLShift) && isDrift_) {
	}
	else {

		// �h���X�g�̃u�[�X�g����܂ł̃^�C�}�[���K��l�ȏゾ������u�[�X�g����B
		if (DRIFT_BOOST_TIMER <= driftBoostTimer_) {

			boostSpeed_ = MAX_BOOST_SPEED;

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

void Player::Move()
{

	/*===== �ړ����� =====*/

	// �ړ����x�����E�𒴂��Ȃ��悤�ɂ���B
	if (MAX_SPEED < speed_) {

		speed_ = MAX_SPEED;

	}

	// ���̏�ɂ�����ړ����x�̌��E�l��������B
	if (isGrass_ && MAX_SPEED_ON_GRASS < speed_) {

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
	if (isGround_) {

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

void Player::CheckHit(bool& IsPassedMiddlePoint, int& RapCount)
{

	/*===== �����蔻�� =====*/

	{

		/*-- �X�e�[�W�Ƃ̓����蔻�� --*/

		// �����蔻��Ɏg�p����f�[�^
		FHelper::RayToModelCollisionData collistionData;

		// �����蔻��ɕK�v�ȃf�[�^�𖄂߂Ă����B
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageBlasIndex_]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageBlasIndex_]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageBlasIndex_]->GetVertexIndex();
		collistionData.rayPos = pos_;
		collistionData.rayDir = bottomVec;
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData_.stageInsIndex_);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData_.stageInsIndex_);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData_.stageInsIndex_);

		// �����蔻��̌��ʕۑ��p�ϐ��B
		bool isHit = false;
		Vec3 impactPos;
		float hitDistance;
		Vec3 hitNormal;

		// �����蔻����s���B
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// ��������������Y���̃T�C�Y����������������B
		isHit &= (hitDistance - size_.y_) < 0;
		isHit &= 0 < hitDistance;

		// �������Ă����牟���߂��B
		if (isHit) {

			// �҂����艟���߂��Ǝ��̃t���[���ŋ󒆔���ɂȂ��Ă��܂��̂ŁA�኱�I�t�Z�b�g��݂���B
			const float PUSH_BACK_OFFSET = 1.0f;

			// �@�������ɓ��������������߂��B
			pos_ += hitNormal * (size_.y_ - (hitDistance + PUSH_BACK_OFFSET));

			// �n��ɂ��锻��B
			isGround_ = true;

			// �΂ߏ��̉�]�����B
			RotObliqueFloor(hitNormal);

		}
		else {

			// �󒆂ɂ��锻��B
			isGround_ = false;

		}


		/*-- ���Ƃ̓����蔻�� --*/

		// �������Ă��Ȃ������瓖���蔻����s���B
		if (!isHit) {

			// �����蔻��ɕK�v�ȃf�[�^�𖄂߂Ă����B
			collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageGrassBlasIndex_]->GetVertexPos();
			collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageGrassBlasIndex_]->GetVertexNormal();
			collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageGrassBlasIndex_]->GetVertexIndex();
			collistionData.rayPos = pos_;
			collistionData.rayDir = bottomVec;
			collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData_.stageGrassInsIndex_);
			collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData_.stageGrassInsIndex_);
			collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData_.stageGrassInsIndex_);

			// �����蔻��̌��ʕۑ��p�ϐ��B
			isHit = false;
			impactPos = Vec3();
			hitDistance = 0;
			hitNormal = Vec3();

			// �����蔻����s���B
			isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

			// ��������������Y���̃T�C�Y����������������B
			isHit &= (hitDistance - size_.y_) < 0;
			isHit &= 0 < hitDistance;

			// �������Ă����牟���߂��B
			if (isHit) {

				// �҂����艟���߂��Ǝ��̃t���[���ŋ󒆔���ɂȂ��Ă��܂��̂ŁA�኱�I�t�Z�b�g��݂���B
				const float PUSH_BACK_OFFSET = 1.0f;

				// �@�������ɓ��������������߂��B
				pos_ += hitNormal * (size_.y_ - (hitDistance + PUSH_BACK_OFFSET));

				// �n��ɂ��锻��B
				isGround_ = true;

				// �΂ߏ��̉�]�����B
				RotObliqueFloor(hitNormal);

				// ���̏�ɂ��锻��B
				isGrass_ = true;

			}
			else {

				// �󒆂ɂ��锻��B
				isGround_ = false;

				// ���̏�ɂ��Ȃ�����B
				isGrass_ = false;

			}

		}
		// �ʏ�̏��œ������Ă���Ƃ������Ƃ͑��̏�ɂ͂��Ȃ��Ƃ������ƂȂ̂ŁAfalse�ɂ���B
		else {

			isGrass_ = false;

		}


		// ���ʕ����̓����蔻����s�����߁A���C�̔�΂�������ς���B
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageBlasIndex_]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageBlasIndex_]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageBlasIndex_]->GetVertexIndex();
		collistionData.rayPos = prevPos_;
		collistionData.rayDir = (pos_ - prevPos_).GetNormal();
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData_.stageInsIndex_);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData_.stageInsIndex_);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData_.stageInsIndex_);

		// �����蔻����s���B
		isHit = false;
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// ��������������Y���̃T�C�Y����������������B
		isHit &= fabs(hitDistance) < (pos_ - prevPos_).Length();

		// �������Ă����牟���߂��B
		if (isHit) {

			// �@�������ɓ��������������߂��B
			pos_ = impactPos + hitNormal * hitDistance;

		}


	}


	/*-- ���S�n�_�ƃS�[���n�_�Ƃ̓����蔻�� --*/

	// ���Ԓn�_�ɒB���Ă��Ȃ������璆�Ԓn�_�Ƃ̓����蔻����s���B
	if (!IsPassedMiddlePoint) {

		// �����蔻��Ɏg�p����f�[�^
		FHelper::RayToModelCollisionData collistionData;

		// �����蔻��ɕK�v�ȃf�[�^�𖄂߂Ă����B
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData_.middlePointBlasIndex_]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData_.middlePointBlasIndex_]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData_.middlePointBlasIndex_]->GetVertexIndex();
		collistionData.rayPos = prevPos_;
		collistionData.rayDir = forwardVec_;
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData_.middlePointInsIndex_);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData_.middlePointInsIndex_);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData_.middlePointInsIndex_);

		// �����蔻��̌��ʕۑ��p�ϐ��B
		bool isHit = false;
		Vec3 impactPos;
		float hitDistance;
		Vec3 hitNormal;

		// �����蔻����s���B
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// ��������������Y���̃T�C�Y����������������B
		isHit &= hitDistance < (pos_ - prevPos_).Length();
		isHit &= 0 < hitDistance;

		// �������Ă�����B
		if (isHit) {

			IsPassedMiddlePoint = true;

		}

	}
	else {

		// �����蔻��Ɏg�p����f�[�^
		FHelper::RayToModelCollisionData collistionData;

		// �����蔻��ɕK�v�ȃf�[�^�𖄂߂Ă����B
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData_.goalBlasIndex_]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData_.goalBlasIndex_]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData_.goalBlasIndex_]->GetVertexIndex();
		collistionData.rayPos = prevPos_;
		collistionData.rayDir = forwardVec_;
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData_.goalInsIndex_);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData_.goalInsIndex_);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData_.goalInsIndex_);

		// �����蔻��̌��ʕۑ��p�ϐ��B
		bool isHit = false;
		Vec3 impactPos;
		float hitDistance;
		Vec3 hitNormal;

		// �����蔻����s���B
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// ��������������Y���̃T�C�Y����������������B
		isHit &= hitDistance < (pos_ - prevPos_).Length();
		isHit &= 0 < hitDistance;

		// �������Ă�����B
		if (isHit) {

			IsPassedMiddlePoint = false;
			++RapCount;

		}

	}


	/*===== �����I�u�W�F�N�g�Ƃ̓����蔻�� =====*/

	{

		const int BLAS_COUNT = static_cast<int>(stageModelData_.stageOrnamentInsIndex_.size());
		for (int index_ = 0; index_ < BLAS_COUNT; ++index_) {

			// �����蔻��Ɏg�p����f�[�^
			FHelper::RayToModelCollisionData collistionData;
			collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageOrnamentBlasIndex_[index_]]->GetVertexPos();
			collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageOrnamentBlasIndex_[index_]]->GetVertexNormal();
			collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData_.stageOrnamentBlasIndex_[index_]]->GetVertexIndex();
			collistionData.rayPos = prevPos_;
			collistionData.rayDir = (pos_ - prevPos_).GetNormal();
			collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData_.stageOrnamentInsIndex_[index_]);
			collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData_.stageOrnamentInsIndex_[index_]);
			collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData_.stageOrnamentInsIndex_[index_]);

			// �����蔻����s���B
			bool isHit = false;
			Vec3 impactPos;
			float hitDistance;
			Vec3 hitNormal;
			isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

			// ��������������Y���̃T�C�Y����������������B
			isHit &= fabs(hitDistance) < (pos_ - prevPos_).Length();

			// �������Ă����牟���߂��B
			if (isHit) {

				// �@�������ɓ��������������߂��B
				pos_ = impactPos + hitNormal * hitDistance;
				speed_ = 0;
				boostSpeed_ = 0;

			}

		}

	}


	/*===== �M�~�b�N�Ƃ̓����蔻�� =====*/

	{

		std::vector<std::shared_ptr<BaseGimmick>> gimmics = GimmickMgr::Ins()->GetGimmickData();
		for (auto& index_ : gimmics) {

			// �t���O�������Ă��Ȃ������珈���𑱂���B
			if (!index_->GetIsActive()) continue;

			// �����蔻����s���B
			bool isHit = obb_.CheckHitOBB(index_->GetOBB());

			// �������Ă��Ȃ������珈�����΂��B
			if (!isHit) continue;

			// �u�[�X�g���}�b�N�X�ɂ���B
			boostSpeed_ = MAX_BOOST_SPEED;

		}


	}


}

void Player::RotObliqueFloor(const Vec3& HitNormal)
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