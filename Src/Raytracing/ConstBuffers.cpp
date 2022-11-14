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

	// 点光源をセッティング
	for (auto& index_ : pointLight_) {
		index_.lightColor_ = Vec3(1.0f, 1.0f, 1.0f);
		index_.lightPos_ = Vec3(0, 300, 0);
		index_.lightSize_ = 5.0f;
		index_.lightPower_ = 10.0f;
		index_.isActive_ = false;
		index_.isShadow_ = true;
	}

	// 並行光源をセッティング
	dirLight_.isActive_ = true;
	dirLight_.lightColor_ = Vec3{ 1.0f,1.0f,1.0f };
	dirLight_.lihgtDir_ = Vec3{ -0.5f,-0.2f,0 };
	dirLight_.lihgtDir_.Normalize();
	dirLight_.seed_ = FHelper::GetRand(0, 1000);

}

//void AtmosphericScatteringConstBufferData::Init()
//{
//
//	/*===== 大気散乱用定数バッファを初期化 =====*/
//
//	eSun_ = 20.0f;
//	g_ = -0.999f;
//	innerRadius_ = 10000.0f;
//	outerRadius_ = 10250.0f;
//	km_ = 0.005f;
//	kr_ = 0.0025f;
//	samples_ = 2.0f;
//	aveHeight_ = 0.35f;
//
//}

//void DebugConstBufferData::Init()
//{
//
//	/*===== デバッグ用定数バッファを初期化 =====*/
//
//	seed_ = FHelper::GetRand(0, 1000);
//	isLightHitScene_ = false;
//	isNormalScene_ = false;
//	isMeshScene_ = false;
//	isNoAO_ = false;
//	isNoGI_ = false;
//	isGIOnlyScene_ = false;
//	isNoiseScene_ = false;
//
//}

void RayConstBufferData::Init()
{

	/*===== 初期化処理 =====*/

	camera_.Init();
	light_.Init();
	//as_.Init();
	//debug_.Init();
	alphaData_.Init();

}

void AlphaConstBufferData::Init()
{

	/*===== 初期化処理 =====*/

	for (auto& index : alphaData_) {

		index.alpha_ = 0;
		index.instanceIndex_ = 0;

	}

}
