#pragma once
#include "DynamicConstBuffer.h"
#include "Vec.h"
#include "FHelper.h"
#include "DirectXBase.h"
#include "Camera.h"

// ��C�U���p
struct AtmosphericScattering {

	float kr;				// ���C���[�U���萔
	float km;				// �~�[�U���萔
	float samples;			// ��C�U���T���v����
	float outerRadius;		// ��C���̍Œ��_�̍���
	float innerRadius;		// �n��̍���
	float eSun;				// ���z�̋���
	float g;				// �U���萔�����߂�ۂɎg�p����l
	float aveHeight;		// ���ϑ�C���x�����߂邽�߂̍���

};

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
	AtmosphericScattering AS;	// ��C�U���p
	int seed;
	int counter;
	int aoSampleCount;
	int isNoiseScene;
	int isLightHitScene;
	int isNormalScene;
	int isMeshScene;
	int isNoAO;
	int isNoGI;
	int isGIOnlyScene;
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

		// �_�������Z�b�e�B���O
		pointLight.lightPos = Vec3(0, 300, 0);
		pointLight.lightSize = 30.0f;
		pointLight.lightPower = 300.0f;
		pointLight.isActive = true;

		// ���s�������Z�b�e�B���O
		dirLight.isActive = false;
		dirLight.lightColor = Vec3{ 0,0,0 };
		dirLight.lihgtDir = Vec3{ -0.5f,-1,0 };
		dirLight.lihgtDir.Normalize();

		// �X�|�b�g���C�g���Z�b�e�B���O
		spotLight.isActive = false;
		spotLight.dir = Vec3{ 0,-1,0 };
		spotLight.pos = Vec3{ 0,300,0 };
		spotLight.power = 300.0f;
		spotLight.angle = DirectX::XM_PI;

		// ��C�U���p�萔���Z�b�e�B���O
		AS.eSun = 20.0f;
		AS.g = -0.999f;
		AS.innerRadius = 10000.0f;
		AS.outerRadius = 10250.0f;
		AS.km = 0.01f;
		AS.kr = 0.0025f;
		AS.samples = 2.0f;
		AS.aveHeight = 0.25f;

		// ���̑��̃f�o�b�O�����Z�b�e�B���O
		seed = FHelper::GetRand(0, 1000);
		aoSampleCount = 1;
		isLightHitScene = false;
		isNormalScene = false;
		isMeshScene = false;
		isNoAO = false;
		isNoGI = false;
		isGIOnlyScene = false;

	}

};