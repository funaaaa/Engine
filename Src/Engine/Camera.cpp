#include "Camera.h"
#include "FHelper.h"
#include "DirectXBase.h"
#include <d3d12.h>

Camera::Camera() {

	/*===== コンストラクタ =====*/

	eye_ = DirectX::XMFLOAT3(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 500);		// 視点座標		ゲームワールド内でのカメラ座標
	target_ = DirectX::XMFLOAT3(WINDOW_WIDTH / 2 + 1, WINDOW_HEIGHT / 2, 0);	// 注視点座標		ゲームワールド内でカメラが見ている座標
	up_ = DirectX::XMFLOAT3(0, 1, 0);										// 上方向ベクトル	ゲームワールド内でカメラから見て上方向を指すベクトル
	matView_ = DirectX::XMMatrixLookAtLH(eye_.ConvertXMVECTOR(), target_.ConvertXMVECTOR(), up_.ConvertXMVECTOR());
	angleOfView_ = 60.0f;
	convTimer_ = 0;

}

void Camera::GenerateMatView()
{

	/*===== ビュー行列を生成 =====*/

	matView_ = DirectX::XMMatrixLookAtLH(eye_.ConvertXMVECTOR(), target_.ConvertXMVECTOR(), up_.ConvertXMVECTOR());
	// 透視投影変換行列
	matPerspective_ = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60.0f),		// 画角(60度)
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,	// アスペクト比
		0.1f, 1000000.0f						// 前端、奥端
	);

	// 射影変換行列
	matProjection_ = DirectX::XMMatrixOrthographicOffCenterLH(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0.01f, 100000.0f);

}

void Camera::Init()
{

	/*===== 初期化処理 ======*/

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

void Camera::Update(const Vec3& CharaPos, const Vec3& CharaForwardVec, const Vec3& CharaUpVec, const float& CharaSpeedPer, const bool& IsBeforeStart)
{

	/*===== 更新処理 =====*/

	// 経過時間タイマーを更新。
	++convTimer_;

	// 最初の1F目だったら。
	if (convTimer_ == 1) {

		// ベクトルを保存。
		Vec3 forwardVec = CharaForwardVec;
		Vec3 upVec = CharaUpVec;

		// 正面ベクトルと上ベクトルの法線ベクトルを求める。
		Vec3 sideVec = forwardVec.Cross(upVec);

		// 現在座標をプレイヤーの横に設定。
		eye_ = CharaPos + sideVec * BEFORE_START_CAMERA_DISTANCE;
		target_ = CharaPos;
		up_ = CharaUpVec;

	}

	// 基準となる視点座標を求める。
	baseEye_ = CharaPos + CharaForwardVec * -EYE_PLAYER_DISTANCE;
	baseEye_ += up_ * TARGET_UPPER;

	// 基準となる注視点座標を求める。
	baseTarget_ = CharaPos + CharaForwardVec * TARGET_PLAYER_DISTNACE;

	// 基準となる上ベクトルを求める。
	baseUp_ = CharaUpVec;

	// 補間量。
	float interpolationAmount = 0.0f;

	// 開始前だったら補間を弱める。
	if (IsBeforeStart) {

		interpolationAmount = 20.0f;

	}
	// ゲーム中だったら規定の速度
	else {

		interpolationAmount = 5.0f;

	}

	// 視点座標を補間する。
	eye_ += (baseEye_ - eye_) / interpolationAmount;

	// 注視点座標を補間する。
	target_ += (baseTarget_ - target_) / interpolationAmount;

	// 上ベクトルを更新する。
	up_ += (baseUp_ - up_) / interpolationAmount;

	// 現在のキャラの移動速度の割合から画角に加算する量を求める。
	float addAngleOfView = (MAX_ANGLEOFVIEW - DEF_ANGLEOFVIEW) * CharaSpeedPer;

	// 基準となる画角の値を変える。
	baseAngleOfView_ = DEF_ANGLEOFVIEW + addAngleOfView;

	// 画角の値を基準となる値に近づける。
	angleOfView_ += (baseAngleOfView_ - angleOfView_) / interpolationAmount;

	// ビュー行列を生成。	
	matView_ = DirectX::XMMatrixLookAtLH(eye_.ConvertXMVECTOR(), target_.ConvertXMVECTOR(), up_.ConvertXMVECTOR());
	//透視投影変換行列
	matPerspective_ = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(angleOfView_),				//画角(60度)
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,	//アスペクト比
		0.1f, 1000000.0f							//前端、奥端
	);
	//射影変換行列
	matProjection_ = DirectX::XMMatrixOrthographicOffCenterLH(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0.01f, 100000.0f);

}