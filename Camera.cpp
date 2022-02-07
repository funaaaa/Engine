#include "Camera.h"
#include "Struct.h"
#include "FHelper.h"
#include "DirectXBase.h"
#include <d3d12.h>

XMFLOAT3 Camera::eye = XMFLOAT3(window_width / 2, window_height / 2, -10);
XMFLOAT3 Camera::target = XMFLOAT3(window_width / 2, window_height / 2, 0);
XMFLOAT3 Camera::up = XMFLOAT3(0, 1, 0);
XMFLOAT3 Camera::honraiEye = XMFLOAT3(window_width / 2, window_height / 2, -10);
XMFLOAT3 Camera::honraiTarget = XMFLOAT3(window_width / 2, window_height / 2, 0);
XMFLOAT3 Camera::honraiUp = XMFLOAT3(0, 1, 0);
XMMATRIX Camera::rotationMat = XMMatrixIdentity();
XMMATRIX Camera::upRotationMat = XMMatrixIdentity();
XMMATRIX Camera::matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
XMMATRIX Camera::matPerspective{};
XMMATRIX Camera::matProjection{};
float Camera::angleOfView = 60;

Camera::Camera() {
	eye = XMFLOAT3(window_width / 2, window_height / 2, 500);		//���_���W		�Q�[�����[���h���ł̃J�������W
	target = XMFLOAT3(window_width / 2, window_height / 2, 0);		//�����_���W		�Q�[�����[���h���ŃJ���������Ă�����W
	up = XMFLOAT3(0, 1, 0);											//������x�N�g��	�Q�[�����[���h���ŃJ�������猩�ď�������w���x�N�g��
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	//XMStoreFloat3(&(forwardVec), XMVector3Normalize(XMLoadFloat3(&forwardVec)));
}

void Camera::GenerateMatView()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	//�������e�ϊ��s��
	matPerspective = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),				//��p(60�x)
		(float)window_width / window_height,	//�A�X�y�N�g��
		0.1f, 1000000.0f							//�O�[�A���[
	);
	//�ˉe�ϊ��s��
	matProjection = XMMatrixOrthographicOffCenterLH(0, window_width, window_height, 0, 0.01f, 100000.0f);
}

void Camera::GenerateMatViewSpeed(const float& nowSpeed, const float& maxSpeed)
{
	float rate = nowSpeed / maxSpeed;
	if (fabs(rate) < 0.01f) {
		rate = 0;
	}

	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	//�������e�ϊ��s��
	matPerspective = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(rate * MAX_ANGLEOFVIEW + 60.0f),
		(float)window_width / window_height,	//�A�X�y�N�g��
		0.1f, 100000.0f							//�O�[�A���[
	);
	//�ˉe�ϊ��s��
	matProjection = XMMatrixOrthographicOffCenterLH(0, window_width, window_height, 0, 0.01f, 100000.0f);
}

void Camera::Init()
{
	eye = XMFLOAT3(window_width / 2, window_height / 2, 500);
	target = XMFLOAT3(window_width / 2, window_height / 2, 0);
	up = XMFLOAT3(0, 1, 0);
	rotationMat = XMMatrixIdentity();
	upRotationMat = XMMatrixIdentity();
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	matPerspective = {};
	matProjection = {};
	Camera::angleOfView = 60;

}

void Camera::Update(const XMFLOAT3& playerPos, const XMFLOAT3& playerForwardVec, const XMFLOAT3& playerUpVec, const float& nowSpeed, const float& maxSpeed)
{
	//�{������ׂ����_���W���X�V
	honraiEye = { EYE_PLAYER_DISTANCE * (-playerForwardVec.x) + playerPos.x + (honraiUp.x * 50.0f),
				EYE_PLAYER_DISTANCE * (-playerForwardVec.y) + playerPos.y + (honraiUp.y * 50.0f),
				EYE_PLAYER_DISTANCE * (-playerForwardVec.z) + playerPos.z + (honraiUp.z * 50.0f) };
	//�{������ׂ������_���W���X�V
	honraiTarget = { TARGET_PLAYER_DISTNACE * (playerForwardVec.x) + playerPos.x,
				TARGET_PLAYER_DISTNACE * (playerForwardVec.y) + playerPos.y,
				TARGET_PLAYER_DISTNACE * (playerForwardVec.z) + playerPos.z };
	//�{������ׂ���񍐃x�N�g�����X�V
	honraiUp = playerUpVec;

	//���_���W���X�V
	eye = FHelper::AddDivValueXMFLOAT3(eye, honraiEye, 5.0f);
	//�����_���W���X�V
	target = FHelper::AddDivValueXMFLOAT3(target, honraiTarget, 5.0f);
	//��x�N�g�����X�V
	up = FHelper::AddDivValueXMFLOAT3(up, honraiUp, 5.0f);

	//�J�����s����X�V
	GenerateMatViewSpeed(nowSpeed, maxSpeed);
}

XMFLOAT3 Camera::GetEyeVector()
{
	XMFLOAT3 returnBuff = FHelper::Normalize3D(XMFLOAT3(honraiTarget.x - honraiEye.x, honraiTarget.y - honraiEye.y, honraiTarget.z - honraiEye.z));
		return returnBuff;
}
