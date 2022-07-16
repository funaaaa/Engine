#include "Camera.h"
#include "FHelper.h"
#include "DirectXBase.h"
#include <d3d12.h>

Camera::Camera() {
	eye_ = DirectX::XMFLOAT3(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 500);		// ���_���W		�Q�[�����[���h���ł̃J�������W
	target_ = DirectX::XMFLOAT3(WINDOW_WIDTH / 2 + 1, WINDOW_HEIGHT / 2, 0);	// �����_���W		�Q�[�����[���h���ŃJ���������Ă�����W
	up_ = DirectX::XMFLOAT3(0, 1, 0);										// ������x�N�g��	�Q�[�����[���h���ŃJ�������猩�ď�������w���x�N�g��
	matView_ = DirectX::XMMatrixLookAtLH(eye_.ConvertXMVECTOR(), target_.ConvertXMVECTOR(), up_.ConvertXMVECTOR());
	angleOfView_ = 60.0f;
}

void Camera::GenerateMatView()
{
	matView_ = DirectX::XMMatrixLookAtLH(eye_.ConvertXMVECTOR(), target_.ConvertXMVECTOR(), up_.ConvertXMVECTOR());
	//�������e�ϊ��s��
	matPerspective_ = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60.0f),		// ��p(60�x)
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,	// �A�X�y�N�g��
		0.1f, 1000000.0f						// �O�[�A���[
	);
	//�ˉe�ϊ��s��
	matProjection_ = DirectX::XMMatrixOrthographicOffCenterLH(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0.01f, 100000.0f);
}

void Camera::Init()
{
	eye_ = Vec3(0, 150, 10);
	target_ = Vec3(1, 0, 0);
	up_ = Vec3(0, 1, 0);
	matView_ = DirectX::XMMatrixLookAtLH(eye_.ConvertXMVECTOR(), target_.ConvertXMVECTOR(), up_.ConvertXMVECTOR());
	matPerspective_ = {};
	matProjection_ = {};
	Camera::angleOfView_ = 60;
	forwardVec_ = Vec3{ 0,0,1 };
	angleOfView_ = DEF_ANGLEOFVIEW;
	baseAngleOfView_ = angleOfView_;

}

void Camera::Update(const Vec3& CharaPos, const Vec3& CharaForwardVec, const Vec3& CharaUpVec, const float& CharaSpeedPer)
{

	// ��ƂȂ鎋�_���W�����߂�B
	baseEye_ = CharaPos + CharaForwardVec * -EYE_PLAYER_DISTANCE;
	baseEye_ += up_ * TARGET_UPPER;

	// ��ƂȂ钍���_���W�����߂�B
	baseTarget_ = CharaPos + CharaForwardVec * TARGET_PLAYER_DISTNACE;

	// ��ƂȂ��x�N�g�������߂�B
	baseUp_ = CharaUpVec;

	// ���_���W���Ԃ���B
	eye_ += (baseEye_ - eye_) / 10.0f;

	// �����_���W���Ԃ���B
	target_ += (baseTarget_ - target_) / 10.0f;

	// ��x�N�g�����X�V����B
	up_ += (baseUp_ - up_) / 10.0f;

	// ���݂̃L�����̈ړ����x�̊��������p�ɉ��Z����ʂ����߂�B
	float addAngleOfView = (MAX_ANGLEOFVIEW - DEF_ANGLEOFVIEW) * CharaSpeedPer;

	// ��ƂȂ��p�̒l��ς���B
	baseAngleOfView_ = DEF_ANGLEOFVIEW + addAngleOfView;

	// ��p�̒l����ƂȂ�l�ɋ߂Â���B
	angleOfView_ += (baseAngleOfView_ - angleOfView_) / 5.0f;

	// �r���[�s��𐶐��B	
	matView_ = DirectX::XMMatrixLookAtLH(eye_.ConvertXMVECTOR(), target_.ConvertXMVECTOR(), up_.ConvertXMVECTOR());
	//�������e�ϊ��s��
	matPerspective_ = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(angleOfView_),				//��p(60�x)
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,	//�A�X�y�N�g��
		0.1f, 1000000.0f							//�O�[�A���[
	);
	//�ˉe�ϊ��s��
	matProjection_ = DirectX::XMMatrixOrthographicOffCenterLH(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0.01f, 100000.0f);

}