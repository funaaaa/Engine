#include "ConstBuffers.h"
#include "Camera.h"
#include "WindowsAPI.h"
#include "FHelper.h"	

void CameraConstBufferData::Init()
{

	/*===== カメラ用定数バッファを初期化 =====*/

	Camera::Ins()->GenerateMatView();

	mtxProj_ = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60.0f),				//画角(60度)
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,	//アスペクト比
		0.1f, 1000000.0f							//前端、奥端
	);
	mtxProjInv_ = XMMatrixInverse(nullptr, mtxProj_);
	mtxView_ = DirectX::XMMatrixLookAtLH(Camera::Ins()->eye_.ConvertXMVECTOR(), Camera::Ins()->target_.ConvertXMVECTOR(), Camera::Ins()->up_.ConvertXMVECTOR());
	mtxViewInv_ = XMMatrixInverse(nullptr, mtxView_);

}

void RayLightConstBufferData::Init()
{

	/*===== ライト用定数バッファを初期化 =====*/

	// 並行光源をセッティング
	dirLight_.isActive_ = true;
	dirLight_.lightColor_ = Vec3{ 0.0f,0.0f,1.0f };
	dirLight_.lihgtDir_ = Vec3{ -0.5f,-0.2f,0 };
	dirLight_.lihgtDir_.Normalize();
	dirLight_.seed_ = FHelper::GetRand(0, 1000);

}

void RayConstBufferData::Init()
{

	/*===== 初期化処理 =====*/

	camera_.Init();
	light_.Init();
	alphaData_.Init();
	debug_.Init();

}

void AlphaConstBufferData::Init()
{

	/*===== 初期化処理 =====*/

	for (auto& index : alphaData_) {

		index.alpha_ = 0;
		index.instanceIndex_ = 0;

	}

}

void DebugData::Init()
{

	/*===== デバッグ用情報の初期化処理 =====*/

	isDebugMesnInfo_ = false;
	debugMeshMoveX_ = 0;

}
