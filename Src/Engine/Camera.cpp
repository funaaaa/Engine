#include "Camera.h"
#include "FHelper.h"
#include "Engine.h"
#include <d3d12.h>

Camera::Camera() {

	/*===== �R���X�g���N�^ =====*/

	eye_ = DirectX::XMFLOAT3(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 500);		// ���_���W		�Q�[�����[���h���ł̃J�������W
	target_ = DirectX::XMFLOAT3(WINDOW_WIDTH / 2 + 1, WINDOW_HEIGHT / 2, 0);	// �����_���W		�Q�[�����[���h���ŃJ���������Ă�����W
	up_ = DirectX::XMFLOAT3(0, 1, 0);										// ������x�N�g��	�Q�[�����[���h���ŃJ�������猩�ď�������w���x�N�g��
	matView_ = DirectX::XMMatrixLookAtLH(eye_.ConvertXMVECTOR(), target_.ConvertXMVECTOR(), up_.ConvertXMVECTOR());
	angleOfView_ = 60.0f;
	convTimer_ = 0;

}

void Camera::GenerateMatView()
{

	/*===== �r���[�s��𐶐� =====*/

	matView_ = DirectX::XMMatrixLookAtLH(eye_.ConvertXMVECTOR(), target_.ConvertXMVECTOR(), up_.ConvertXMVECTOR());
	// �������e�ϊ��s��
	matPerspective_ = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60.0f),		// ��p(60�x)
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,	// �A�X�y�N�g��
		0.1f, 1000000.0f						// �O�[�A���[
	);

	// �ˉe�ϊ��s��
	matProjection_ = DirectX::XMMatrixOrthographicOffCenterLH(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0.01f, 100000.0f);

}

void Camera::Init()
{

	/*===== ���������� ======*/

	eye_ = Vec3(0, 150, 10);
	target_ = Vec3(1, 0, 0);
	up_ = Vec3(0, 1, 0);
	matView_ = DirectX::XMMatrixLookAtLH(eye_.ConvertXMVECTOR(), target_.ConvertXMVECTOR(), up_.ConvertXMVECTOR());
	matPerspective_ = {};
	matProjection_ = {};
	Camera::angleOfView_ = 60;
	angleOfView_ = DEF_ANGLEOFVIEW;
	baseAngleOfView_ = angleOfView_;
	convTimer_ = 0;

}

void Camera::Update(const Vec3& CharaPos, const Vec3& CharaForwardVec, const Vec3& CharaUpVec, float CharaSpeedPer, bool IsBeforeStart, bool IsGameFinish)
{

	/*===== �X�V���� =====*/

	if (Engine::Ins()->isStopGame_) return;

	// �o�ߎ��ԃ^�C�}�[���X�V�B
	++convTimer_;

	// �ŏ���1F�ڂ�������B
	if (convTimer_ == 1) {

		// �x�N�g����ۑ��B
		Vec3 forwardVec = CharaForwardVec;
		Vec3 upVec = CharaUpVec;

		// ���ʃx�N�g���Ə�x�N�g���̖@���x�N�g�������߂�B
		Vec3 sideVec = forwardVec.Cross(upVec);

		// ���ݍ��W���v���C���[�̉��ɐݒ�B
		eye_ = CharaPos + sideVec * BEFORE_START_CAMERA_DISTANCE;
		target_ = CharaPos;
		up_ = CharaUpVec;

	}

	// �Q�[�����I�������ۊǂ��Ȃ��B
	if (IsGameFinish) {

		// ���_�ƃL���������ꂷ���Ă�����B
		float distance = Vec3(eye_ - CharaPos).Length();
		if (EYE_PLAYER_DISTANCE < distance) {

			// ���_����v���C���[�̃x�N�g�������߂�B
			Vec3 eyeCharaVec = Vec3(CharaPos - eye_).GetNormal();

			// �ړ�������B
			eye_ += eyeCharaVec * (distance - EYE_PLAYER_DISTANCE);
			target_ += eyeCharaVec * (distance - EYE_PLAYER_DISTANCE);

		}

	}
	else {

		// ��ƂȂ鎋�_���W�����߂�B
		baseEye_ = CharaPos + CharaForwardVec * -EYE_PLAYER_DISTANCE;
		baseEye_ += up_ * TARGET_UPPER;

		// ��ƂȂ钍���_���W�����߂�B
		baseTarget_ = CharaPos + CharaForwardVec * TARGET_PLAYER_DISTNACE;

		// ��ƂȂ��x�N�g�������߂�B
		baseUp_ = CharaUpVec;

		// ��ԗʁB
		float interpolationAmount = 0.0f;

		// �J�n�O���������Ԃ���߂�B
		if (IsBeforeStart) {

			interpolationAmount = 20.0f;

		}
		// �Q�[������������K��̑��x
		else {

			interpolationAmount = 5.0f;

		}

		// ���_���W���Ԃ���B
		eye_ += (baseEye_ - eye_) / interpolationAmount;

		// �����_���W���Ԃ���B
		target_ += (baseTarget_ - target_) / interpolationAmount;

		// ��x�N�g�����X�V����B
		up_ += (baseUp_ - up_) / interpolationAmount;

		// ���݂̃L�����̈ړ����x�̊��������p�ɉ��Z����ʂ����߂�B
		float addAngleOfView = (MAX_ANGLEOFVIEW - DEF_ANGLEOFVIEW) * CharaSpeedPer;

		// ��ƂȂ��p�̒l��ς���B
		baseAngleOfView_ = DEF_ANGLEOFVIEW + addAngleOfView;

	}

	// ��p�̒l����ƂȂ�l�ɋ߂Â���B
	angleOfView_ += (baseAngleOfView_ - angleOfView_) / 5.0f;

	// �r���[�s��𐶐��B	
	matView_ = DirectX::XMMatrixLookAtLH(eye_.ConvertXMVECTOR(), target_.ConvertXMVECTOR(), up_.ConvertXMVECTOR());
	// �������e�ϊ��s��
	matPerspective_ = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(angleOfView_),	// ��p
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,		// �A�X�y�N�g��
		0.1f, 1000000.0f							// �O�[�A���[
	);
	// �ˉe�ϊ��s��
	matProjection_ = DirectX::XMMatrixOrthographicOffCenterLH(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0.01f, 100000.0f);

}

void Camera::FreeCamera()
{

	/*===== �t���[�J�����̑��� =====*/

	// �J�����ړ��p�萔
	float CAMERA_MOVE_SPEED = 10.0f;
	float CAMERA_ANGLE_SPEED = 0.02f;

	CAMERA_MOVE_SPEED += (CAMERA_MOVE_SPEED * 2.0f) * Input::Ins()->IsKey(DIK_E);

	// �㉺�ɓ������B
	eye_.y_ += (Input::Ins()->IsKey(DIK_SPACE) * CAMERA_MOVE_SPEED) - (Input::Ins()->IsKey(DIK_LSHIFT) * CAMERA_MOVE_SPEED);

	// XZ���ʂ𓮂����B
	angleOfXZPlane_ += (Input::Ins()->IsKey(DIK_LEFT) * CAMERA_ANGLE_SPEED) - (Input::Ins()->IsKey(DIK_RIGHT) * CAMERA_ANGLE_SPEED);

	// XY���ʂ𓮂����B
	angleOfXYPlane_ += (Input::Ins()->IsKey(DIK_UP) * CAMERA_ANGLE_SPEED) - (Input::Ins()->IsKey(DIK_DOWN) * CAMERA_ANGLE_SPEED);

	// �ړ�������B
	Vec3 dir = Vec3(cosf(angleOfXZPlane_ + DirectX::XM_PIDIV2), 0.0f, sinf(angleOfXZPlane_ + DirectX::XM_PIDIV2));
	eye_ += dir * static_cast<float>(Input::Ins()->IsKey(DIK_A)) * CAMERA_MOVE_SPEED;
	eye_ -= dir * static_cast<float>(Input::Ins()->IsKey(DIK_D)) * CAMERA_MOVE_SPEED;
	dir = Vec3(cosf(angleOfXZPlane_), 0.0f, sinf(angleOfXZPlane_));
	eye_ -= dir * static_cast<float>(Input::Ins()->IsKey(DIK_S)) * CAMERA_MOVE_SPEED;
	eye_ += dir * static_cast<float>(Input::Ins()->IsKey(DIK_W)) * CAMERA_MOVE_SPEED;

	// TARGET�̈ʒu���X�V����B
	target_ = eye_ + (dir + Vec3(0, angleOfXYPlane_, 0)) * TARGET_PLAYER_DISTNACE;

	// �r���[�s��𐶐��B	
	matView_ = DirectX::XMMatrixLookAtLH(eye_.ConvertXMVECTOR(), target_.ConvertXMVECTOR(), up_.ConvertXMVECTOR());
	// �������e�ϊ��s��
	matPerspective_ = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(DEF_ANGLEOFVIEW),	// ��p(60�x)
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,			// �A�X�y�N�g��
		0.1f, 1000000.0f								// �O�[�A���[
	);
	// �ˉe�ϊ��s��
	matProjection_ = DirectX::XMMatrixOrthographicOffCenterLH(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0.01f, 100000.0f);

}

void Camera::InitFreeCamera()
{

	/*===== �t���[�J�����p�̏��������� =====*/

	// �J�����̊p�x�����߂�B
	Vec3 cameraVec = Vec3(target_ - eye_).GetNormal();

	// �s������߂�B
	angleOfXZPlane_ = atan2f(cameraVec.z_, cameraVec.x_);
	angleOfXYPlane_ = cameraVec.y_;

}
