#include "Player.h"
#include "BLASRegister.h"
#include "PorygonInstanceRegister.h"
#include "HitGroupMgr.h"
#include "Input.h"
#include "FHelper.h"
#include "BLAS.h"
#include "DriftParticleMgr.h"

void Player::Init(const int& StageBlasIndex, const int& StageInstanceIndex)
{

	/*===== ���������� =====*/

	carBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/", "car.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }, true);
	carInstanceIndex = PorygonInstanceRegister::Ins()->CreateInstance(carBlasIndex, PorygonInstanceRegister::SHADER_ID_REFLECTION);
	PorygonInstanceRegister::Ins()->AddScale(carInstanceIndex, Vec3(10, 10, 10));

	stageBlasIndex = StageBlasIndex;
	stageInstanceIndex = StageInstanceIndex;

	pos = Vec3(0, 30, 0);
	forwardVec = Vec3(0, 0, 1);
	bottomVec = Vec3(0, -1, 0);
	upVec = Vec3(0, 1, 0);
	size = Vec3(10, 10, 10);
	rotY = 0;
	speed = 0;
	gravity = 0;
	boostSpeed = 0;
	isDrift = false;
	isGround = true;

}

void Player::Update(RayConstBufferData& ConstBufferData)
{

	/*===== �X�V���� =====*/

	// ���͏���
	Input(ConstBufferData);

	// �ړ�����
	Move();

	// �����蔻��
	CheckHit();

	// ���W���X�V�B
	PorygonInstanceRegister::Ins()->ChangeTrans(carInstanceIndex, pos);

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
	float inputRightTriValue = Input::Ins()->isPadTri(XINPUT_TRIGGER_RIGHT);
	if (INPUT_DEADLINE_TRI < inputRightTriValue && isGround) {

		speed += inputRightTriValue * ADD_SPEED;

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
	float inputLeftStickHori = Input::Ins()->isPadThumb(XINPUT_THUMB_LEFTSIDE);
	const float LEFT_STICK_INPUT_DEADLINE = 0.2f;
	if (LEFT_STICK_INPUT_DEADLINE < std::fabs(inputLeftStickHori)) {

		// ��]�� �ʏ��Ԃƃh���t�g��ԂňႤ�B
		float handleAmount = HANDLE_NORMAL;

		// �h���t�g��Ԃ��������]�ʂ𑽂����ɂ���B
		if (isDrift) {

			handleAmount = HANDLE_DRIFT;

			// ���łɃh���t�g��Ԃ̎��̃u�[�X�g����܂ł̃^�C�}�[���X�V����B
			++driftBoostTimer;
			if (DRIFT_BOOST_TIMER < driftBoostTimer) driftBoostTimer = DRIFT_BOOST_TIMER;

		}

		// �N�H�[�^�j�I�������߂�B
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(upVec.ConvertXMVECTOR(), handleAmount * inputLeftStickHori);

		// ���߂��N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// ��]�����Z����B
		PorygonInstanceRegister::Ins()->AddRotate(carInstanceIndex, quaternionMat);
		rotY += handleAmount * inputLeftStickHori;
		nowFrameInputLeftStickHori = inputLeftStickHori;

		// ���ʃx�N�g�����Ԃ̉�]�s�񕪉�]������B
		forwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex));

	}

	// LT��������Ă�����h���t�g��Ԃɂ���B
	const float INPUT_DEADLINE_DRIFT = 0.9f;
	float inputLeftTriValue = Input::Ins()->isPadTri(XINPUT_TRIGGER_LEFT);
	bool isInputNowFrameLeftStrick = LEFT_STICK_INPUT_DEADLINE < fabs(nowFrameInputLeftStickHori);
	if (INPUT_DEADLINE_DRIFT < inputLeftTriValue && isInputNowFrameLeftStrick) {

		isDrift = true;

		// �h���t�g�̃x�N�g�������߂�B
		Vec3 driftVec = Vec3();
		if (nowFrameInputLeftStickHori < 0) {

			driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex));

		}
		else {

			driftVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex));

		}

		// �h���t�g���̃p�[�e�B�N���𐶐��B
		//DriftParticleMgr::Ins()->Generate(pos, driftVec, PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex), ConstBufferData);

	}
	// ���łɃh���t�g���������珟��ɉ������Ȃ��悤�ɂ���B
	else if (INPUT_DEADLINE_DRIFT < inputLeftTriValue && isDrift) {
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
	if (Input::Ins()->isPad(XINPUT_GAMEPAD_B)) {

		pos = Vec3(0, 30, 0);

	}

}

void Player::Move()
{

	/*===== �ړ����� =====*/

	// �ړ����x�����E�𒴂��Ȃ��悤�ɂ���B
	if (MAX_SPEED < speed) {

		speed = MAX_SPEED;

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
	bottomVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex));

}

void Player::CheckHit()
{

	/*===== �����蔻�� =====*/

	{

		/*-- �X�e�[�W�Ƃ̓����蔻�� --*/

		// �����蔻��Ɏg�p����f�[�^
		FHelper::RayToModelCollisionData collistionData;

		// �����蔻��ɕK�v�ȃf�[�^�𖄂߂Ă����B
		collistionData.targetVertex = BLASRegister::Ins()->GetBLAS()[stageBlasIndex]->GetVertexPos();
		collistionData.targetNormal = BLASRegister::Ins()->GetBLAS()[stageBlasIndex]->GetVertexNormal();
		collistionData.targetIndex = BLASRegister::Ins()->GetBLAS()[stageBlasIndex]->GetVertexIndex();
		collistionData.rayPos = pos;
		collistionData.rayDir = bottomVec;
		collistionData.matTrans = PorygonInstanceRegister::Ins()->GetTrans(stageInstanceIndex);
		collistionData.matScale = PorygonInstanceRegister::Ins()->GetScale(stageInstanceIndex);
		collistionData.matRot = PorygonInstanceRegister::Ins()->GetRotate(stageInstanceIndex);

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

		// ���ʕ����̓����蔻����s�����߁A���C�̔�΂�������ς���B
		collistionData.rayDir = forwardVec;

		// �����蔻����s���B
		isHit = false;
		isHit = FHelper::RayToModelCollision(collistionData, impactPos, hitDistance, hitNormal);

		// ��������������Y���̃T�C�Y����������������B
		isHit &= (hitDistance - size.y) < 0;
		isHit &= 0 < hitDistance;

		// �������Ă����牟���߂��B
		if (isHit) {

			// �@�������ɓ��������������߂��B
			pos += hitNormal * (size.x - hitDistance);

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
		PorygonInstanceRegister::Ins()->ChangeRotate(carInstanceIndex, quaternionMat);

		// ��x�N�g������Ƃ����N�H�[�^�j�I�������߂�B
		Vec3 normal = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal.ConvertXMVECTOR(), rotY);

		// �N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// �v���C���[����]������B
		PorygonInstanceRegister::Ins()->AddRotate(carInstanceIndex, upQuaternionMat);


		/*-- �v���C���[�̉�]�s������ƂɊe�x�N�g������] --*/

		// ��]�s����擾�B
		DirectX::XMMATRIX rotationMatBuff = PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex);

		// ��x�N�g�����X�V�B
		upVec = normal;

		//���ʃx�N�g�����X�V�B
		forwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), rotationMatBuff);

	}

	// ���������@�����ゾ�����ꍇ�͍s������������ĉ�]������B
	if (HitNormal == Vec3(0, 1, 0)) {

		// �v���C���[����]������B
		PorygonInstanceRegister::Ins()->ChangeRotate(carInstanceIndex, DirectX::XMMatrixIdentity());

		// ��x�N�g������Ƃ����N�H�[�^�j�I�������߂�B
		Vec3 normal = HitNormal;
		DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal.ConvertXMVECTOR(), rotY);

		// �N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

		// �v���C���[����]������B
		PorygonInstanceRegister::Ins()->AddRotate(carInstanceIndex, upQuaternionMat);


		/*-- �v���C���[�̉�]�s������ƂɊe�x�N�g������] --*/

		// ��]�s����擾�B
		DirectX::XMMATRIX rotationMatBuff = PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex);

		// ��x�N�g�����X�V�B
		upVec = normal;

		//���ʃx�N�g�����X�V�B
		forwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), rotationMatBuff);

	}

}
