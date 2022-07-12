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

	// �Ԃ̃{�f�B�[�����[�h
	carBodyBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carBody.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" }, true);
	carBodyInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carBodyBlasIndex, PolygonInstanceRegister::SHADER_ID::REFLECTION);
	PolygonInstanceRegister::Ins()->AddScale(carBodyInsIndex, Vec3(12, 12, 12));

	// �Ԃ̃^�C�������[�h
	carTireBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carTire.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carTireInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carTireBlasIndex, PolygonInstanceRegister::SHADER_ID::DEF);
	PolygonInstanceRegister::Ins()->SetParentInstance(carTireInsIndex, carBodyInsIndex);

	// �Ԃ̋������[�h
	carMirrorBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carMirror.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carMirrorInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlasIndex, PolygonInstanceRegister::SHADER_ID::COMPLETE_REFLECTION);
	PolygonInstanceRegister::Ins()->SetParentInstance(carMirrorInsIndex, carBodyInsIndex);

	// �Ԃ̋��̃J�o�[�����[�h
	carMirrorCoverBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/car/", "carMirrorCover.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/car/tireColor.png" }, true);
	carMirrorCoverInsIndex = PolygonInstanceRegister::Ins()->CreateInstance(carMirrorBlasIndex, PolygonInstanceRegister::SHADER_ID::COMPLETE_REFLECTION);
	PolygonInstanceRegister::Ins()->SetParentInstance(carMirrorCoverInsIndex, carBodyInsIndex);

	// �Ԃ̃E�B���h�E�����[�h
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

	// OBB�𐶐��B
	obb.Setting(carBodyBlasIndex, carBodyInsIndex);

}

void Player::Init()
{

	/*===== ���������� =====*/

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

	/*===== �X�V���� =====*/

	// ���͏���
	Input(ConstBufferData);

	// �ړ�����
	Move();

	// �����蔻��
	CheckHit(IsPassedMiddlePoint, RapCount);

	// ���W���X�V�B
	PolygonInstanceRegister::Ins()->ChangeTrans(carBodyInsIndex, pos);

	// ���W��ۑ��B
	prevPos = pos;

	// �󒆂ɂ���Ƃ��͏����n�_�܂Ŗ߂�^�C�}�[���X�V�B�n��ɗv��Ƃ��̓^�C�}�[���������B
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

	// OBB���X�V�B
	obb.SetMat(carBodyInsIndex);

}

void Player::Draw()
{

	/*===== �`�揈�� =====*/

}

float Player::GetNowSpeedPer()
{

	/*====== �ړ����x�̊������v�Z =====*/

	float per = (speed + boostSpeed) / (MAX_SPEED + MAX_BOOST_SPEED);

	return per;

}

void Player::Input(RayConstBufferData& ConstBufferData)
{

	/*===== ���͏��� =====*/

	// RT��������Ă���������B
	const float INPUT_DEADLINE_TRI = 0.5f;
	float inputRightTriValue = Input::Ins()->PadTrigger(XINPUT_TRIGGER_RIGHT);
	if ((INPUT_DEADLINE_TRI < inputRightTriValue) && isGround) {

		speed += inputRightTriValue * ADD_SPEED;

	}
	else if (Input::Ins()->IsKey(DIK_W) && isGround) {

		speed += ADD_SPEED;

	}
	else if (isGround) {

		// �ړ����Ă��Ȃ��Ēn��ɂ�����ړ��ʂ�0�ɋ߂Â���B
		speed -= speed / 10.0f;

	}
	else {

		// �ړ����Ă��Ȃ��ċ󒆂ɂ�����ړ��ʂ�0�ɋ߂Â���B
		speed -= speed / 200.0f;

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
		if (isDrift) {

			handleAmount = HANDLE_DRIFT;

			// ���łɃh���t�g��Ԃ̎��̃u�[�X�g����܂ł̃^�C�}�[���X�V����B
			if (isGround) {
				++driftBoostTimer;
				if (DRIFT_BOOST_TIMER < driftBoostTimer) driftBoostTimer = DRIFT_BOOST_TIMER;
			}

		}

		// �N�H�[�^�j�I�������߂�B
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(upVec.ConvertXMVECTOR(), handleAmount * inputLeftStickHori + inputADKey * handleAmount);

		// ���߂��N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// ��]�����Z����B
		PolygonInstanceRegister::Ins()->AddRotate(carBodyInsIndex, quaternionMat);
		rotY += handleAmount * inputLeftStickHori + inputADKey * handleAmount;
		nowFrameInputLeftStickHori = inputLeftStickHori + inputADKey * handleAmount;

		// ���ʃx�N�g�����Ԃ̉�]�s�񕪉�]������B
		forwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(carBodyInsIndex));

	}

	// LT��������Ă�����h���t�g��Ԃɂ���B
	const float INPUT_DEADLINE_DRIFT = 0.9f;
	float inputLeftTriValue = Input::Ins()->PadTrigger(XINPUT_TRIGGER_LEFT);
	bool isInputNowFrameLeftStrick = LEFT_STICK_INPUT_DEADLINE < fabs(nowFrameInputLeftStickHori) || inputADKey != 0;
	bool isInputLShift = Input::Ins()->IsKey(DIK_LSHIFT);
	if ((INPUT_DEADLINE_DRIFT < inputLeftTriValue || isInputLShift) && isInputNowFrameLeftStrick) {

		isDrift = true;

		// �h���t�g�̃x�N�g�������߂�B
		Vec3 driftVec = Vec3();
		if (nowFrameInputLeftStickHori < 0) {

			driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(carBodyInsIndex));

		}
		else {

			driftVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(carBodyInsIndex));

		}

		// �h���t�g���̃p�[�e�B�N���𐶐��B
		//DriftParticleMgr::Ins()->Generate(pos, driftVec, PolygonInstanceRegister::Ins()->GetRotate(carInstanceIndex), ConstBufferData);
		ConstBufferData;

	}
	// ���łɃh���t�g���������珟��ɉ������Ȃ��悤�ɂ���B
	else if ((INPUT_DEADLINE_DRIFT < inputLeftTriValue || isInputLShift) && isDrift) {
	}
	else {

		// �h���X�g�̃u�[�X�g����܂ł̃^�C�}�[���K��l�ȏゾ������u�[�X�g����B
		if (DRIFT_BOOST_TIMER <= driftBoostTimer) {

			boostSpeed = MAX_BOOST_SPEED;

		}

		// �h���t�g�̃u�[�X�g����܂ł̃^�C�}�[������������B
		driftBoostTimer = 0;

		isDrift = false;

	}

	// �f�o�b�O�p B�{�^���������ꂽ�珉���ʒu�ɖ߂��B
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

	/*===== �ړ����� =====*/

	// �ړ����x�����E�𒴂��Ȃ��悤�ɂ���B
	if (MAX_SPEED < speed) {

		speed = MAX_SPEED;

	}

	// ���̏�ɂ�����ړ����x�̌��E�l��������B
	if (isGrass && MAX_SPEED_ON_GRASS < speed) {

		speed = MAX_SPEED_ON_GRASS;

	}

	// ���W�ړ�������B
	pos += forwardVec * (speed + boostSpeed);

	// �h���t�g���̃u�[�X�g�ړ��ʂ�0�ɋ߂Â���B
	if (0 < boostSpeed) {

		boostSpeed -= SUB_BOOST_SPEED;

	}
	else {

		boostSpeed = 0;

	}

	// �n��ɂ�����d�͂𖳌�������B
	if (isGround) {

		gravity = 0;

	}
	// �󒆂ɂ�����d�͂����Z����B
	else {

		gravity += ADD_GRAV;

		// �d�͗ʂ����E�𒴂��Ȃ��悤�ɂ���B
		if (MAX_GRAV < gravity) {

			gravity = MAX_GRAV;

		}

	}

	// ���W�ɏd�͂����Z����B
	pos += Vec3(0, -1, 0) * gravity;

	// ���x�N�g�����Ԃ̉�]�s�񕪉�]������B
	bottomVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(carBodyInsIndex));

}

void Player::CheckHit(bool& IsPassedMiddlePoint, int& RapCount)
{

	/*===== �����蔻�� =====*/

	{

		/*-- �X�e�[�W�Ƃ̓����蔻�� --*/

		// �����蔻��Ɏg�p����f�[�^
		FHelper::RayToModelCollisionData collistionData;

		// �����蔻��ɕK�v�ȃf�[�^�𖄂߂Ă����B
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageBlasIndex]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData.stageBlasIndex]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageBlasIndex]->GetVertexIndex();
		collistionData.rayPos = pos;
		collistionData.rayDir = bottomVec;
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData.stageInsIndex);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData.stageInsIndex);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData.stageInsIndex);

		// �����蔻��̌��ʕۑ��p�ϐ��B
		bool isHit = false;
		Vec3 impactPos;
		float hitDistance;
		Vec3 hitNormal;

		// �����蔻����s���B
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// ��������������Y���̃T�C�Y����������������B
		isHit &= (hitDistance - size.y) < 0;
		isHit &= 0 < hitDistance;

		// �������Ă����牟���߂��B
		if (isHit) {

			// �҂����艟���߂��Ǝ��̃t���[���ŋ󒆔���ɂȂ��Ă��܂��̂ŁA�኱�I�t�Z�b�g��݂���B
			const float PUSH_BACK_OFFSET = 1.0f;

			// �@�������ɓ��������������߂��B
			pos += hitNormal * (size.y - (hitDistance + PUSH_BACK_OFFSET));

			// �n��ɂ��锻��B
			isGround = true;

			// �΂ߏ��̉�]�����B
			RotObliqueFloor(hitNormal);

		}
		else {

			// �󒆂ɂ��锻��B
			isGround = false;

		}


		/*-- ���Ƃ̓����蔻�� --*/

		// �������Ă��Ȃ������瓖���蔻����s���B
		if (!isHit) {

			// �����蔻��ɕK�v�ȃf�[�^�𖄂߂Ă����B
			collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageGrassBlasIndex]->GetVertexPos();
			collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData.stageGrassBlasIndex]->GetVertexNormal();
			collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageGrassBlasIndex]->GetVertexIndex();
			collistionData.rayPos = pos;
			collistionData.rayDir = bottomVec;
			collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData.stageGrassInsIndex);
			collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData.stageGrassInsIndex);
			collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData.stageGrassInsIndex);

			// �����蔻��̌��ʕۑ��p�ϐ��B
			isHit = false;
			impactPos = Vec3();
			hitDistance = 0;
			hitNormal = Vec3();

			// �����蔻����s���B
			isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

			// ��������������Y���̃T�C�Y����������������B
			isHit &= (hitDistance - size.y) < 0;
			isHit &= 0 < hitDistance;

			// �������Ă����牟���߂��B
			if (isHit) {

				// �҂����艟���߂��Ǝ��̃t���[���ŋ󒆔���ɂȂ��Ă��܂��̂ŁA�኱�I�t�Z�b�g��݂���B
				const float PUSH_BACK_OFFSET = 1.0f;

				// �@�������ɓ��������������߂��B
				pos += hitNormal * (size.y - (hitDistance + PUSH_BACK_OFFSET));

				// �n��ɂ��锻��B
				isGround = true;

				// �΂ߏ��̉�]�����B
				RotObliqueFloor(hitNormal);

				// ���̏�ɂ��锻��B
				isGrass = true;

			}
			else {

				// �󒆂ɂ��锻��B
				isGround = false;

				// ���̏�ɂ��Ȃ�����B
				isGrass = false;

			}

		}
		// �ʏ�̏��œ������Ă���Ƃ������Ƃ͑��̏�ɂ͂��Ȃ��Ƃ������ƂȂ̂ŁAfalse�ɂ���B
		else {

			isGrass = false;

		}


		// ���ʕ����̓����蔻����s�����߁A���C�̔�΂�������ς���B
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageBlasIndex]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData.stageBlasIndex]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageBlasIndex]->GetVertexIndex();
		collistionData.rayPos = prevPos;
		collistionData.rayDir = (pos - prevPos).GetNormal();
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData.stageInsIndex);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData.stageInsIndex);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData.stageInsIndex);

		// �����蔻����s���B
		isHit = false;
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// ��������������Y���̃T�C�Y����������������B
		isHit &= fabs(hitDistance) < (pos - prevPos).Length();

		// �������Ă����牟���߂��B
		if (isHit) {

			// �@�������ɓ��������������߂��B
			pos = impactPos + hitNormal * hitDistance;

		}


	}


	/*-- ���S�n�_�ƃS�[���n�_�Ƃ̓����蔻�� --*/

	// ���Ԓn�_�ɒB���Ă��Ȃ������璆�Ԓn�_�Ƃ̓����蔻����s���B
	if (!IsPassedMiddlePoint) {

		// �����蔻��Ɏg�p����f�[�^
		FHelper::RayToModelCollisionData collistionData;

		// �����蔻��ɕK�v�ȃf�[�^�𖄂߂Ă����B
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData.middlePointBlasIndex]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData.middlePointBlasIndex]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData.middlePointBlasIndex]->GetVertexIndex();
		collistionData.rayPos = prevPos;
		collistionData.rayDir = forwardVec;
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData.middlePointInsIndex);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData.middlePointInsIndex);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData.middlePointInsIndex);

		// �����蔻��̌��ʕۑ��p�ϐ��B
		bool isHit = false;
		Vec3 impactPos;
		float hitDistance;
		Vec3 hitNormal;

		// �����蔻����s���B
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// ��������������Y���̃T�C�Y����������������B
		isHit &= hitDistance < (pos - prevPos).Length();
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
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData.goalBlasIndex]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData.goalBlasIndex]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData.goalBlasIndex]->GetVertexIndex();
		collistionData.rayPos = prevPos;
		collistionData.rayDir = forwardVec;
		collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData.goalInsIndex);
		collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData.goalInsIndex);
		collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData.goalInsIndex);

		// �����蔻��̌��ʕۑ��p�ϐ��B
		bool isHit = false;
		Vec3 impactPos;
		float hitDistance;
		Vec3 hitNormal;

		// �����蔻����s���B
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// ��������������Y���̃T�C�Y����������������B
		isHit &= hitDistance < (pos - prevPos).Length();
		isHit &= 0 < hitDistance;

		// �������Ă�����B
		if (isHit) {

			IsPassedMiddlePoint = false;
			++RapCount;

		}

	}


	/*===== �����I�u�W�F�N�g�Ƃ̓����蔻�� =====*/

	{

		const int BLAS_COUNT = static_cast<int>(stageModelData.stageOrnamentInsIndex.size());
		for (int index = 0; index < BLAS_COUNT; ++index) {

			// �����蔻��Ɏg�p����f�[�^
			FHelper::RayToModelCollisionData collistionData;
			collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageOrnamentBlasIndex[index]]->GetVertexPos();
			collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageModelData.stageOrnamentBlasIndex[index]]->GetVertexNormal();
			collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageModelData.stageOrnamentBlasIndex[index]]->GetVertexIndex();
			collistionData.rayPos = prevPos;
			collistionData.rayDir = (pos - prevPos).GetNormal();
			collistionData.matTrans = PolygonInstanceRegister::Ins()->GetTrans(stageModelData.stageOrnamentInsIndex[index]);
			collistionData.matScale = PolygonInstanceRegister::Ins()->GetScale(stageModelData.stageOrnamentInsIndex[index]);
			collistionData.matRot = PolygonInstanceRegister::Ins()->GetRotate(stageModelData.stageOrnamentInsIndex[index]);

			// �����蔻����s���B
			bool isHit = false;
			Vec3 impactPos;
			float hitDistance;
			Vec3 hitNormal;
			isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

			// ��������������Y���̃T�C�Y����������������B
			isHit &= fabs(hitDistance) < (pos - prevPos).Length();

			// �������Ă����牟���߂��B
			if (isHit) {

				// �@�������ɓ��������������߂��B
				pos = impactPos + hitNormal * hitDistance;
				speed = 0;
				boostSpeed = 0;

			}

		}

	}


	/*===== �M�~�b�N�Ƃ̓����蔻�� =====*/

	{

		std::vector<std::shared_ptr<BaseGimmick>> gimmics = GimmickMgr::Ins()->GetGimmickData();
		for (auto& index : gimmics) {

			// �t���O�������Ă��Ȃ������珈���𑱂���B
			if (!index->GetIsActive()) continue;

			// �����蔻����s���B
			bool isHit = obb.CheckHitOBB(index->GetOBB());

			// �������Ă��Ȃ������珈�����΂��B
			if (!isHit) continue;

			// �u�[�X�g���}�b�N�X�ɂ���B
			boostSpeed = MAX_BOOST_SPEED;

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
		PolygonInstanceRegister::Ins()->ChangeRotate(carBodyInsIndex, quaternionMat);

		// ��x�N�g������Ƃ����N�H�[�^�j�I�������߂�B
		Vec3 normal = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal.ConvertXMVECTOR(), rotY);

		// �N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// �v���C���[����]������B
		PolygonInstanceRegister::Ins()->AddRotate(carBodyInsIndex, upQuaternionMat);


		/*-- �v���C���[�̉�]�s������ƂɊe�x�N�g������] --*/

		// ��]�s����擾�B
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(carBodyInsIndex);

		// ��x�N�g�����X�V�B
		upVec = normal;

		//���ʃx�N�g�����X�V�B
		forwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

	// ���������@�����ゾ�����ꍇ�͍s������������ĉ�]������B
	if (HitNormal == Vec3(0, 1, 0)) {

		// �v���C���[����]������B
		PolygonInstanceRegister::Ins()->ChangeRotate(carBodyInsIndex, DirectX::XMMatrixIdentity());

		// ��x�N�g������Ƃ����N�H�[�^�j�I�������߂�B
		Vec3 normal = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal.ConvertXMVECTOR(), rotY);

		// �N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// �v���C���[����]������B
		PolygonInstanceRegister::Ins()->AddRotate(carBodyInsIndex, upQuaternionMat);


		/*-- �v���C���[�̉�]�s������ƂɊe�x�N�g������] --*/

		// ��]�s����擾�B
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(carBodyInsIndex);

		// ��x�N�g�����X�V�B
		upVec = normal;

		//���ʃx�N�g�����X�V�B
		forwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

}