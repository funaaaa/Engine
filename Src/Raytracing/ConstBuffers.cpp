#include "ConstBuffers.h"
#include "Camera.h"
#include "WindowsAPI.h"
#include "FHelper.h"	

void CameraConstBufferData::Init()
{

	/*===== カメラ用定数バッファを初期化 =====*/

	Camera::Ins()->GenerateMatView();
	Camera::Ins()->Update();

	mtxProj = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60.0f),				//画角(60度)
		(float)window_width / window_height,	//アスペクト比
		0.1f, 1000000.0f							//前端、奥端
	);
	mtxProjInv = XMMatrixInverse(nullptr, mtxProj);
	mtxView = DirectX::XMMatrixLookAtLH(Camera::Ins()->eye.ConvertXMVECTOR(), Camera::Ins()->target.ConvertXMVECTOR(), Camera::Ins()->up.ConvertXMVECTOR());
	mtxViewInv = XMMatrixInverse(nullptr, mtxView);

}

void RayLightConstBufferData::Init()
{

	/*===== ライト用定数バッファを初期化 =====*/

	// 点光源をセッティング
	for (auto& index : pointLight) {
		index.lightPos = Vec3(0, 300, 0);
		index.lightSize = 5.0f;
		index.lightPower = 100.0f;
		index.isActive = false;
	}

	// 並行光源をセッティング
	dirLight.isActive = true;
	dirLight.lightColor = Vec3{ 0,0,0 };
	dirLight.lihgtDir = Vec3{ -0.5f,-0.01f,0 };
	dirLight.lihgtDir.Normalize();

}

void AtmosphericScatteringConstBufferData::Init()
{

	/*===== 大気散乱用定数バッファを初期化 =====*/

	eSun = 20.0f;
	g = -0.999f;
	innerRadius = 10000.0f;
	outerRadius = 10250.0f;
	km = 0.005f;
	kr = 0.0025f;
	samples = 2.0f;
	aveHeight = 0.35f;

}

void DebugConstBufferData::Init()
{

	/*===== デバッグ用定数バッファを初期化 =====*/

	seed = FHelper::GetRand(0, 1000);
	aoSampleCount = 1;
	isLightHitScene = false;
	isNormalScene = false;
	isMeshScene = false;
	isNoAO = false;
	isNoGI = false;
	isGIOnlyScene = false;
	isNoiseScene = false;
	counter = 0;

}

void RayConstBufferData::Init()
{

	/*===== 初期化処理 =====*/

	camera.Init();
	light.Init();
	as.Init();
	debug.Init();

}
