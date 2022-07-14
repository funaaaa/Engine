#include "Camera.h"
#include "FHelper.h"
#include "DirectXBase.h"
#include <d3d12.h>

Camera::Camera() {
	eye = DirectX::XMFLOAT3(window_width / 2, window_height / 2, 500);		// 視点座標		ゲームワールド内でのカメラ座標
	target = DirectX::XMFLOAT3(window_width / 2 + 1, window_height / 2, 0);	// 注視点座標		ゲームワールド内でカメラが見ている座標
	up = DirectX::XMFLOAT3(0, 1, 0);										// 上方向ベクトル	ゲームワールド内でカメラから見て上方向を指すベクトル
	matView = DirectX::XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	angleOfView = 60.0f;
}

void Camera::GenerateMatView()
{
	matView = DirectX::XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	//透視投影変換行列
	matPerspective = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60.0f),		// 画角(60度)
		(float)window_width / window_height,	// アスペクト比
		0.1f, 1000000.0f						// 前端、奥端
	);
	//射影変換行列
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

	// 基準となる視点座標を求める。
	baseEye = CharaPos + CharaForwardVec * -EYE_PLAYER_DISTANCE;
	baseEye += up * TARGET_UPPER;

	// 基準となる注視点座標を求める。
	baseTarget = CharaPos + CharaForwardVec * TARGET_PLAYER_DISTNACE;

	// 基準となる上ベクトルを求める。
	baseUp = CharaUpVec;

	// 視点座標を補間する。
	eye += (baseEye - eye) / 10.0f;

	// 注視点座標を補間する。
	target += (baseTarget - target) / 10.0f;

	// 上ベクトルを更新する。
	up += (baseUp - up) / 10.0f;

	// 現在のキャラの移動速度の割合から画角に加算する量を求める。
	float addAngleOfView = (MAX_ANGLEOFVIEW - DEF_ANGLEOFVIEW) * CharaSpeedPer;

	// 基準となる画角の値を変える。
	baseAngleOfView = DEF_ANGLEOFVIEW + addAngleOfView;

	// 画角の値を基準となる値に近づける。
	angleOfView += (baseAngleOfView - angleOfView) / 5.0f;

	// ビュー行列を生成。	
	matView = DirectX::XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	//透視投影変換行列
	matPerspective = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(angleOfView),				//画角(60度)
		(float)window_width / window_height,	//アスペクト比
		0.1f, 1000000.0f							//前端、奥端
	);
	//射影変換行列
	matProjection = DirectX::XMMatrixOrthographicOffCenterLH(0, window_width, window_height, 0, 0.01f, 100000.0f);

}