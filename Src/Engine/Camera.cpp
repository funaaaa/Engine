#include "Camera.h"
#include "FHelper.h"
#include "DirectXBase.h"
#include <d3d12.h>

Camera::Camera() {
	eye = XMFLOAT3(window_width / 2, window_height / 2, 500);		//���_���W		�Q�[�����[���h���ł̃J�������W
	target = XMFLOAT3(window_width / 2 + 1, window_height / 2, 0);		//�����_���W		�Q�[�����[���h���ŃJ���������Ă�����W
	up = XMFLOAT3(0, 1, 0);											//������x�N�g��	�Q�[�����[���h���ŃJ�������猩�ď�������w���x�N�g��
	matView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	//XMStoreFloat3(&(forwardVec), XMVector3Normalize(XMLoadFloat3(&forwardVec)));
	angleOfView = 60.0f;
}

void Camera::GenerateMatView()
{
	matView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
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

	matView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
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
	eye = Vec3(0, 150, 10);
	target = Vec3(1, 0, 0);
	up = Vec3(0, 1, 0);
	rotationMat = XMMatrixIdentity();
	upRotationMat = XMMatrixIdentity();
	matView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	matPerspective = {};
	matProjection = {};
	Camera::angleOfView = 60;
	angleXZ = 0;
	forwardVec = Vec3{ 0,0,1 };

}

void Camera::UpdateRacingGame(const Vec3& playerPos, const Vec3& playerForwardVec, const Vec3& playerUpVec, const float& nowSpeed, const float& maxSpeed)
{
	//�{������ׂ����_���W���X�V
	//honraiEye = { EYE_PLAYER_DISTANCE * (-playerForwardVec.x) + playerPos.x + (honraiUp.x * 50.0f),
	//			EYE_PLAYER_DISTANCE * (-playerForwardVec.y) + playerPos.y + (honraiUp.y * 50.0f),
	//			EYE_PLAYER_DISTANCE * (-playerForwardVec.z) + playerPos.z + (honraiUp.z * 50.0f) };
	////�{������ׂ������_���W���X�V
	//honraiTarget = { TARGET_PLAYER_DISTNACE * (playerForwardVec.x) + playerPos.x,
	//			TARGET_PLAYER_DISTNACE * (playerForwardVec.y) + playerPos.y,
	//			TARGET_PLAYER_DISTNACE * (playerForwardVec.z) + playerPos.z };
	////�{������ׂ���񍐃x�N�g�����X�V
	//honraiUp = playerUpVec;

	////���_���W���X�V
	//eye = FHelper::AddDivValueXMFLOAT3(eye, honraiEye, 5.0f);
	////�����_���W���X�V
	//target = FHelper::AddDivValueXMFLOAT3(target, honraiTarget, 5.0f);
	////��x�N�g�����X�V
	//up = FHelper::AddDivValueXMFLOAT3(up, honraiUp, 5.0f);

	//�J�����s����X�V
	GenerateMatViewSpeed(nowSpeed, maxSpeed);
}

void Camera::Update()
{

	// ���ʃx�N�g�������߂�B
	//forwardVec = FHelper::MulRotationMatNormal({ 0,0,-1 }, rotationMat);
	//forwardVec.Normalize();
	forwardVec.x = cosf(angleXZ);
	forwardVec.z = sinf(angleXZ);

	//forwardVec.Normalize();

	// ���_�����E�𒴂��Ȃ��悤�ɂ���B
	if(1.0f < forwardVec.y) forwardVec.y = 1.0f;
	if(forwardVec.y < -1.0f) forwardVec.y = -1.0f;

	// ���_���W���王�_�_���W�����߂�B
	const float EYE_TARGET = 100.0f;
	target = eye + forwardVec * EYE_TARGET;

	// ��x�N�g�������߂�B
	//up = FHelper::MulRotationMatNormal({ 0,1,0 }, rotationMat);

}

void Camera::AddRotation(const float& RotX, const float& RotY, const float& RotZ)
{

	// �J�����̉�]�s�����]������B
	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();
	buff *= XMMatrixRotationZ(RotZ);
	buff *= XMMatrixRotationX(RotX);
	buff *= XMMatrixRotationY(RotY);

	rotationMat = buff * rotationMat;

}

void Camera::AddRotationXZ(const float& Rot)
{

	angleXZ += Rot;

}

void Camera::Move(const float& Speed)
{
	Vec3 forwardVecBuff = forwardVec;
	forwardVecBuff.y = 0;
	eye += forwardVecBuff * Speed;

}

void Camera::MoveRight(const float& Speed)
{
	XMMATRIX mat = FHelper::CalRotationMat(XMFLOAT3(0, -3.14f / 2.0f, 0));
	Vec3 forwardVecBuff = forwardVec;
	forwardVecBuff.y = 0;
	Vec3 moveDir = FHelper::MulRotationMatNormal(forwardVecBuff.ConvertXMFLOAT3(), mat);

	eye += moveDir * Speed;

}

Vec3 Camera::GetEyeVector()
{
	XMFLOAT3 returnBuff = FHelper::Normalize3D(XMFLOAT3(honraiTarget.x - honraiEye.x, honraiTarget.y - honraiEye.y, honraiTarget.z - honraiEye.z));
	return returnBuff;
}
