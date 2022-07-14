#include "Camera.h"
#include "FHelper.h"
#include "DirectXBase.h"
#include <d3d12.h>

Camera::Camera() {
	eye = DirectX::XMFLOAT3(window_width / 2, window_height / 2, 500);		// ���_���W		�Q�[�����[���h���ł̃J�������W
	target = DirectX::XMFLOAT3(window_width / 2 + 1, window_height / 2, 0);	// �����_���W		�Q�[�����[���h���ŃJ���������Ă�����W
	up = DirectX::XMFLOAT3(0, 1, 0);										// ������x�N�g��	�Q�[�����[���h���ŃJ�������猩�ď�������w���x�N�g��
	matView = DirectX::XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	angleOfView = 60.0f;
}

void Camera::GenerateMatView()
{
	matView = DirectX::XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	//�������e�ϊ��s��
	matPerspective = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60.0f),		// ��p(60�x)
		(float)window_width / window_height,	// �A�X�y�N�g��
		0.1f, 1000000.0f						// �O�[�A���[
	);
	//�ˉe�ϊ��s��
	matProjection = DirectX::XMMatrixOrthographicOffCenterLH(0, window_width, window_height, 0, 0.01f, 100000.0f);
}

void Camera::Init()
{
	eye = Vec3(0, 150, 10);
	target = Vec3(1, 0, 0);
	up = Vec3(0, 1, 0);
	matView = DirectX::XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	matPerspective = {};
	matProjection = {};
	Camera::angleOfView = 60;
	forwardVec_ = Vec3{ 0,0,1 };
	angleOfView = DEF_ANGLEOFVIEW;
	baseAngleOfView = angleOfView;

}

void Camera::Update(const Vec3& CharaPos, const Vec3& CharaForwardVec, const Vec3& CharaUpVec, const float& CharaSpeedPer)
{

	// ��ƂȂ鎋�_���W�����߂�B
	baseEye = CharaPos + CharaForwardVec * -EYE_PLAYER_DISTANCE;
	baseEye += up * TARGET_UPPER;

	// ��ƂȂ钍���_���W�����߂�B
	baseTarget = CharaPos + CharaForwardVec * TARGET_PLAYER_DISTNACE;

	// ��ƂȂ��x�N�g�������߂�B
	baseUp = CharaUpVec;

	// ���_���W���Ԃ���B
	eye += (baseEye - eye) / 10.0f;

	// �����_���W���Ԃ���B
	target += (baseTarget - target) / 10.0f;

	// ��x�N�g�����X�V����B
	up += (baseUp - up) / 10.0f;

	// ���݂̃L�����̈ړ����x�̊��������p�ɉ��Z����ʂ����߂�B
	float addAngleOfView = (MAX_ANGLEOFVIEW - DEF_ANGLEOFVIEW) * CharaSpeedPer;

	// ��ƂȂ��p�̒l��ς���B
	baseAngleOfView = DEF_ANGLEOFVIEW + addAngleOfView;

	// ��p�̒l����ƂȂ�l�ɋ߂Â���B
	angleOfView += (baseAngleOfView - angleOfView) / 5.0f;

	// �r���[�s��𐶐��B	
	matView = DirectX::XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	//�������e�ϊ��s��
	matPerspective = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(angleOfView),				//��p(60�x)
		(float)window_width / window_height,	//�A�X�y�N�g��
		0.1f, 1000000.0f							//�O�[�A���[
	);
	//�ˉe�ϊ��s��
	matProjection = DirectX::XMMatrixOrthographicOffCenterLH(0, window_width, window_height, 0, 0.01f, 100000.0f);

}