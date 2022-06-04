#pragma once
#include "DynamicConstBuffer.h"
#include "Vec.h"
#include "FHelper.h"
#include "DirectXBase.h"
#include "Camera.h"

struct RayPointLightData {

	Vec3 lightPos;
	float lightSize;
	Vec3 lightColor;
	float lightPower;
	int isActive;
	Vec3 pad;

};

struct RayDirLightData {

	Vec3 lihgtDir;
	int isActive;
	Vec3 lightColor;
	float pad;

};

struct RaySpotLightData {

	Vec3 pos;
	float angle;
	Vec3 dir;
	float power;
	Vec3 color;
	int isActive;

};

struct KariConstBufferData {

	XMMATRIX mtxView;			// �r���[�s��B
	XMMATRIX mtxProj;			// �v���W�F�N�V�����s��B
	XMMATRIX mtxViewInv;		// �r���[�t�s��B
	XMMATRIX mtxProjInv;		// �v���W�F�N�V�����t�s��B
	XMVECTOR ambientColor;		// �����B
	RayDirLightData dirLight;
	RayPointLightData pointLight;
	RaySpotLightData spotLight;
	int seed;
	int counter;
	int aoSampleCount;
	int isNoiseScene;
	int isNoiseOnlyScene;
	int isLightHitScene;
	int isNormalScene;
	int isMeshScene;
	int isNoAO;
	Vec3 eye = { 0,0,-10 };
	Vec3 target = { 0,0,0 };
	Vec3 up = { 0,1,0 };


	void Init() {

		Camera::Ins()->GenerateMatView();
		Camera::Ins()->Update();

		eye = Camera::Ins()->eye;
		target = Camera::Ins()->target;
		up = Camera::Ins()->up;

		ambientColor = { 1,1,1,1 };
		mtxProj = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(60.0f),				//��p(60�x)
			(float)window_width / window_height,	//�A�X�y�N�g��
			0.1f, 1000000.0f							//�O�[�A���[
		);
		mtxProjInv = XMMatrixInverse(nullptr, mtxProj);
		mtxView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
		mtxViewInv = XMMatrixInverse(nullptr, mtxView);
		counter = 0;
		isNoiseScene = false;
		isNoiseOnlyScene = false;

		// �_�������Z�b�e�B���O
		pointLight.lightPos = Vec3(0, 300, 0);
		pointLight.lightSize = 30.0f;
		pointLight.lightPower = 300.0f;
		pointLight.isActive = true;

		// ���s�������Z�b�e�B���O
		dirLight.isActive = false;
		dirLight.lightColor = Vec3{ 0,0,0 };
		dirLight.lihgtDir = Vec3{ 0,-1,0 };

		// �X�|�b�g���C�g���Z�b�e�B���O
		spotLight.isActive = false;
		spotLight.dir = Vec3{ 0,-1,0 };
		spotLight.pos = Vec3{ 0,300,0 };
		spotLight.power = 300.0f;
		spotLight.angle = DirectX::XM_PI;

		// ���̑��̃f�o�b�O�����Z�b�e�B���O
		seed = FHelper::GetRand(0, 1000);
		aoSampleCount = 1;
		isLightHitScene = false;
		isNormalScene = false;
		isMeshScene = false;
		isNoAO = false;

	}

};