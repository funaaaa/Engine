#include "ConstBuffers.h"
#include "Camera.h"
#include "WindowsAPI.h"
#include "FHelper.h"	

void CameraConstBufferData::Init()
{

	/*===== �J�����p�萔�o�b�t�@�������� =====*/

	Camera::Ins()->GenerateMatView();
	Camera::Ins()->Update();

	mtxProj = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60.0f),				//��p(60�x)
		(float)window_width / window_height,	//�A�X�y�N�g��
		0.1f, 1000000.0f							//�O�[�A���[
	);
	mtxProjInv = XMMatrixInverse(nullptr, mtxProj);
	mtxView = DirectX::XMMatrixLookAtLH(Camera::Ins()->eye.ConvertXMVECTOR(), Camera::Ins()->target.ConvertXMVECTOR(), Camera::Ins()->up.ConvertXMVECTOR());
	mtxViewInv = XMMatrixInverse(nullptr, mtxView);

}

void RayLightConstBufferData::Init()
{

	/*===== ���C�g�p�萔�o�b�t�@�������� =====*/

	// �_�������Z�b�e�B���O
	for (auto& index : pointLight) {
		index.lightPos = Vec3(0, 300, 0);
		index.lightSize = 5.0f;
		index.lightPower = 100.0f;
		index.isActive = false;
	}

	// ���s�������Z�b�e�B���O
	dirLight.isActive = true;
	dirLight.lightColor = Vec3{ 0,0,0 };
	dirLight.lihgtDir = Vec3{ -0.5f,-0.01f,0 };
	dirLight.lihgtDir.Normalize();

}

void AtmosphericScatteringConstBufferData::Init()
{

	/*===== ��C�U���p�萔�o�b�t�@�������� =====*/

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

	/*===== �f�o�b�O�p�萔�o�b�t�@�������� =====*/

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

	/*===== ���������� =====*/

	camera.Init();
	light.Init();
	as.Init();
	debug.Init();

}
