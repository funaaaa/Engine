#include "Player.h"
#include "BLASRegister.h"
#include "PorygonInstanceRegister.h"
#include "HitGroupMgr.h"
#include "Input.h"
#include "FHelper.h"

void Player::Init()
{

	/*===== ���������� =====*/

	int carBlasIndex = BLASRegister::Ins()->GenerateObj("Resource/", "car.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/white.png" });
	carInstanceIndex = PorygonInstanceRegister::Ins()->CreateInstance(carBlasIndex, 2);
	PorygonInstanceRegister::Ins()->AddScale(carInstanceIndex, Vec3(10, 10, 10));

	pos = Vec3();
	forwardVec = Vec3(0, 0, 1);
	speed = 0;

}

void Player::Update()
{

	/*===== �X�V���� =====*/

	// ���͏���
	Input();

	// �ړ�����
	Move();

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

	// �E�X�e�B�b�N�̉��̌X���ʂŃL��������]������B
	float inputLeftStickHori = Input::Ins()->isPadThumb(XINPUT_THUMB_LEFTSIDE);
	if (0.2f < std::fabs(inputLeftStickHori)) {

		// ��]�����Z����B
		PorygonInstanceRegister::Ins()->AddRotate(carInstanceIndex, Vec3(0, HANDLE_NORMAL * inputLeftStickHori, 0));

		// ���ʃx�N�g�����Ԃ̉�]�s�񕪉�]������B
		forwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PorygonInstanceRegister::Ins()->GetRotate(carInstanceIndex));

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
	pos += forwardVec * speed;

}
