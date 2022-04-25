#include "Camera.h"
#include "Struct.h"
#include "FHelper.h"
#include "DirectXBase.h"
#include <d3d12.h>

Camera::Camera() {
	eye = XMFLOAT3(window_width / 2, window_height / 2, 500);		//視点座標		ゲームワールド内でのカメラ座標
	target = XMFLOAT3(window_width / 2, window_height / 2, 0);		//注視点座標		ゲームワールド内でカメラが見ている座標
	up = XMFLOAT3(0, 1, 0);											//上方向ベクトル	ゲームワールド内でカメラから見て上方向を指すベクトル
	matView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	//XMStoreFloat3(&(forwardVec), XMVector3Normalize(XMLoadFloat3(&forwardVec)));
	angleOfView = 60.0f;
}

void Camera::GenerateMatView()
{
	matView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	//透視投影変換行列
	matPerspective = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),				//画角(60度)
		(float)window_width / window_height,	//アスペクト比
		0.1f, 1000000.0f							//前端、奥端
	);
	//射影変換行列
	matProjection = XMMatrixOrthographicOffCenterLH(0, window_width, window_height, 0, 0.01f, 100000.0f);
}

void Camera::GenerateMatViewSpeed(const float& nowSpeed, const float& maxSpeed)
{
	float rate = nowSpeed / maxSpeed;
	if (fabs(rate) < 0.01f) {
		rate = 0;
	}

	matView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	//透視投影変換行列
	matPerspective = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(rate * MAX_ANGLEOFVIEW + 60.0f),
		(float)window_width / window_height,	//アスペクト比
		0.1f, 100000.0f							//前端、奥端
	);
	//射影変換行列
	matProjection = XMMatrixOrthographicOffCenterLH(0, window_width, window_height, 0, 0.01f, 100000.0f);
}

void Camera::Init()
{
	eye = Vec3(0, 0, 10);
	target = Vec3(0, 0, 0);
	up = Vec3(0, 1, 0);
	rotationMat = XMMatrixIdentity();
	upRotationMat = XMMatrixIdentity();
	matView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	matPerspective = {};
	matProjection = {};
	Camera::angleOfView = 60;

}

void Camera::UpdateRacingGame(const Vec3& playerPos, const Vec3& playerForwardVec, const Vec3& playerUpVec, const float& nowSpeed, const float& maxSpeed)
{
	//本来あるべき視点座標を更新
	//honraiEye = { EYE_PLAYER_DISTANCE * (-playerForwardVec.x) + playerPos.x + (honraiUp.x * 50.0f),
	//			EYE_PLAYER_DISTANCE * (-playerForwardVec.y) + playerPos.y + (honraiUp.y * 50.0f),
	//			EYE_PLAYER_DISTANCE * (-playerForwardVec.z) + playerPos.z + (honraiUp.z * 50.0f) };
	////本来あるべき注視点座標を更新
	//honraiTarget = { TARGET_PLAYER_DISTNACE * (playerForwardVec.x) + playerPos.x,
	//			TARGET_PLAYER_DISTNACE * (playerForwardVec.y) + playerPos.y,
	//			TARGET_PLAYER_DISTNACE * (playerForwardVec.z) + playerPos.z };
	////本来あるべき上報告ベクトルを更新
	//honraiUp = playerUpVec;

	////視点座標を更新
	//eye = FHelper::AddDivValueXMFLOAT3(eye, honraiEye, 5.0f);
	////注視点座標を更新
	//target = FHelper::AddDivValueXMFLOAT3(target, honraiTarget, 5.0f);
	////上ベクトルを更新
	//up = FHelper::AddDivValueXMFLOAT3(up, honraiUp, 5.0f);

	//カメラ行列を更新
	GenerateMatViewSpeed(nowSpeed, maxSpeed);
}

void Camera::Update()
{

	// 正面ベクトルを求める。
	//forwardVec = FHelper::MulRotationMatNormal({ 0,0,-1 }, rotationMat);
	forwardVec = eye.GetNormal();
	forwardVec = forwardVec * -1.0f;

	float angleXZ = atan2f(forwardVec.z, forwardVec.x);

	// 回転行列を求める。
	rotationMat = XMMatrixIdentity();
	rotationMat *= XMMatrixRotationY(angleXZ);

	// 視点座標から視点点座標を求める。
	const float EYE_TARGET = 100.0f;
	//target = FHelper::AddXMFLOAT3(eye, FHelper::MulXMFLOAT3(forwardVec, XMFLOAT3(EYE_TARGET, EYE_TARGET, EYE_TARGET)));
	target = 0;

	// 上ベクトルを求める。
	up = FHelper::MulRotationMatNormal({ 0,1,0 }, rotationMat);

}

void Camera::AddRotation(const float& RotX, const float& RotY, const float& RotZ)
{

	// カメラの回転行列を回転させる。

	rotationMat *= XMMatrixRotationZ(RotX);
	rotationMat *= XMMatrixRotationX(RotX);
	rotationMat *= XMMatrixRotationY(RotY);

}

void Camera::Move(const float& Speed)
{

	eye += forwardVec * Speed;

}

void Camera::MoveRight(const float& Speed)
{
	XMMATRIX mat = FHelper::CalRotationMat(XMFLOAT3(0, -3.14f / 2.0f, 0));
	Vec3 moveDir = FHelper::MulRotationMatNormal(forwardVec.ConvertXMFLOAT3(), mat);

	eye += moveDir * Speed;

}

Vec3 Camera::GetEyeVector()
{
	XMFLOAT3 returnBuff = FHelper::Normalize3D(XMFLOAT3(honraiTarget.x - honraiEye.x, honraiTarget.y - honraiEye.y, honraiTarget.z - honraiEye.z));
	return returnBuff;
}
