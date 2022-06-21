#include "Player.h"
#include "BLASRegister.h"
#include "PorygonInstanceRegister.h"
#include "HitGroupMgr.h"
#include "Input.h"
#include "FHelper.h"
#include "BLAS.h"

void Player::Init(const int& StageBlasIndex, const int& StageInstanceIndex)
{

	/*===== ���������� =====*/

	int carBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/", "car.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" });
	carInstanceIndex = PorygonInstanceRegister::Ins()->CreateInstance(carBlasIndex, PorygonInstanceRegister::SHADER_ID_COMPLETE_REFLECTION);
	PorygonInstanceRegister::Ins()->AddScale(carInstanceIndex, Vec3(10, 10, 10));

	stageBlasIndex = StageBlasIndex;
	stageInstanceIndex = StageInstanceIndex;

	pos = Vec3(0, 30, 0);
	forwardVec = Vec3(0, 0, 1);
	bottomVec = Vec3(0, -1, 0);
	size = Vec3(10, 10, 10);
	speed = 0;
	gravity = 0;
	boostSpeed = 0;
	isDrift = false;
	isGround = true;

}

void Player::Update()
{

	/*===== �X�V���� =====*/

	// ���͏���
	Input();

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

Vec3 Player::GetUpVec()
{

	/*===== ��x�N�g�����擾 =====*/

	// ��]�s����擾�B
	DirectX::XMMATRIX matRot = PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex);

	// �f�t�H���g�̏�x�N�g���B
	Vec3 defUpVec = Vec3(0, 1, 0);

	// �f�t�H���g�̏�x�N�g���ɉ�]�s����������l��Ԃ��B
	return FHelper::MulRotationMatNormal(defUpVec, matRot);

}

float Player::GetNowSpeedPer()
{

	/*====== �ړ����x�̊������v�Z =====*/

	float per = (speed + boostSpeed) / (MAX_SPEED + MAX_BOOST_SPEED);

	return per;

}

void Player::Input()
{

	/*===== ���͏��� =====*/

	// RT��������Ă���������B
	const float INPUT_DEADLINE_TRI = 0.5f;
	float inputRightTriValue = Input::Ins()->isPadTri(XINPUT_TRIGGER_RIGHT);
	if (INPUT_DEADLINE_TRI < inputRightTriValue) {

		speed += inputRightTriValue * ADD_SPEED;

	}
	else {

		// �ړ����Ă��Ȃ�������ړ��ʂ�0�ɋ߂Â���B
		speed -= speed / 10.0f;

	}

	// LT��������Ă�����h���t�g��Ԃɂ���B
	const float INPUT_DEADLINE_DRIFT = 0.9f;
	float inputLeftTriValue = Input::Ins()->isPadTri(XINPUT_TRIGGER_LEFT);
	if (INPUT_DEADLINE_DRIFT < inputLeftTriValue) {

		isDrift = true;

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

	// �E�X�e�B�b�N�̉��̌X���ʂŃL��������]������B
	float inputLeftStickHori = Input::Ins()->isPadThumb(XINPUT_THUMB_LEFTSIDE);
	if (0.2f < std::fabs(inputLeftStickHori)) {

		// ��]�� �ʏ��Ԃƃh���t�g��ԂňႤ�B
		float handleAmount = HANDLE_NORMAL;

		// �h���t�g��Ԃ��������]�ʂ𑽂����ɂ���B
		if (isDrift) {

			handleAmount = HANDLE_DRIFT;

			// ���łɃh���t�g��Ԃ̎��̃u�[�X�g����܂ł̃^�C�}�[���X�V����B
			++driftBoostTimer;
			if (DRIFT_BOOST_TIMER < driftBoostTimer) driftBoostTimer = DRIFT_BOOST_TIMER;

		}

		// ��]�����Z����B
		PorygonInstanceRegister::Ins()->AddRotate(carInstanceIndex, Vec3(0, handleAmount * inputLeftStickHori, 0));

		// ���ʃx�N�g�����Ԃ̉�]�s�񕪉�]������B
		forwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex));

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

			// �@�������ɓ��������������߂��B
			pos += hitNormal * (size.y - hitDistance);

			// �n��ɂ��锻��B
			isGround = true;

		}
		else {

			// �󒆂ɂ��锻��B
			isGround = false;

		}

	}

}
