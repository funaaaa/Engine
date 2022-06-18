#include "Camera.h"
#include "FHelper.h"
#include "DirectXBase.h"
#include <d3d12.h>

Camera::Camera() {
	eye = DirectX::XMFLOAT3(window_width / 2, window_height / 2, 500);		//視点座標		ゲームワールド内でのカメラ座標
	target = DirectX::XMFLOAT3(window_width / 2 + 1, window_height / 2, 0);		//注視点座標		ゲームワールド内でカメラが見ている座標
	up = DirectX::XMFLOAT3(0, 1, 0);											//上方向ベクトル	ゲームワールド内でカメラから見て上方向を指すベクトル
	matView = DirectX::XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	//XMStoreFloat3(&(forwardVec), XMVector3Normalize(XMLoadFloat3(&forwardVec)));
	angleOfView = 60.0f;
}

void Camera::GenerateMatView()
{
	matView = DirectX::XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	//透視投影変換行列
	matPerspective = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60.0f),				//画角(60度)
		(float)window_width / window_height,	//アスペクト比
		0.1f, 1000000.0f							//前端、奥端
	);
	//射影変換行列
	matProjection = DirectX::XMMatrixOrthographicOffCenterLH(0, window_width, window_height, 0, 0.01f, 100000.0f);
}

void Camera::GenerateMatViewSpeed(const float& nowSpeed, const float& maxSpeed)
{
	float rate = nowSpeed / maxSpeed;
	if (fabs(rate) < 0.01f) {
		rate = 0;
	}

	matView = DirectX::XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	//透視投影変換行列
	matPerspective = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(rate * MAX_ANGLEOFVIEW + 60.0f),
		(float)window_width / window_height,	//アスペクト比
		0.1f, 100000.0f							//前端、奥端
	);
	//射影変換行列
	matProjection = DirectX::XMMatrixOrthographicOffCenterLH(0, window_width, window_height, 0, 0.01f, 100000.0f);
}

void Camera::Init()
{
	eye = Vec3(0, 150, 10);
	target = Vec3(1, 0, 0);
	up = Vec3(0, 1, 0);
	rotationMat = DirectX::XMMatrixIdentity();
	upRotationMat = DirectX::XMMatrixIdentity();
	matView = DirectX::XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	matPerspective = {};
	matProjection = {};
	Camera::angleOfView = 60;
	angleXZ = 0;
	forwardVec = Vec3{ 0,0,1 };

}

void Camera::Update()
{

	// 正面ベクトルを求める。
	//forwardVec = FHelper::MulRotationMatNormal({ 0,0,-1 }, rotationMat);
	//forwardVec.Normalize();
	forwardVec.x = cosf(angleXZ);
	forwardVec.z = sinf(angleXZ);

	//forwardVec.Normalize();

	// 視点が限界を超えないようにする。
	if(1.0f < forwardVec.y) forwardVec.y = 1.0f;
	if(forwardVec.y < -1.0f) forwardVec.y = -1.0f;

	// 視点座標から視点点座標を求める。
	const float EYE_TARGET = 100.0f;
	target = eye + forwardVec * EYE_TARGET;

	// 上ベクトルを求める。
	//up = FHelper::MulRotationMatNormal({ 0,1,0 }, rotationMat);

}

void Camera::AddRotation(const float& RotX, const float& RotY, const float& RotZ)
{

	// カメラの回転行列を回転させる。
	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();
	buff *= DirectX::XMMatrixRotationZ(RotZ);
	buff *= DirectX::XMMatrixRotationX(RotX);
	buff *= DirectX::XMMatrixRotationY(RotY);

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
	DirectX::XMMATRIX mat = FHelper::CalRotationMat(DirectX::XMFLOAT3(0, -3.14f / 2.0f, 0));
	Vec3 forwardVecBuff = forwardVec;
	forwardVecBuff.y = 0;
	Vec3 moveDir = FHelper::MulRotationMatNormal(forwardVecBuff.ConvertXMFLOAT3(), mat);

	eye += moveDir * Speed;

}

Vec3 Camera::GetEyeVector()
{
	Vec3 returnBuff = (honraiTarget - honraiEye).GetNormal();
	return returnBuff;
}
