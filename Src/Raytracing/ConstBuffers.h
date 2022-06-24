#pragma once
#include "Vec.h"
#include <DirectXMath.h>
#include <array>

// �g�p����萔�o�b�t�@�B

// �J�����p�萔�o�b�t�@
struct CameraConstBufferData {

	DirectX::XMMATRIX mtxView;			// �r���[�s��B
	DirectX::XMMATRIX mtxProj;			// �v���W�F�N�V�����s��B
	DirectX::XMMATRIX mtxViewInv;		// �r���[�t�s��B
	DirectX::XMMATRIX mtxProjInv;		// �v���W�F�N�V�����t�s��B

	// ����������
	void Init();

};

struct RayLightConstBufferData {

	// �f�B���N�V���i�����C�g�p�萔�o�b�t�@
	struct RayDirLightConstBufferData {

		Vec3 lihgtDir;		// ���C�g�̕���
		int isActive;		// �L�������邩�ǂ����̃t���O
		Vec3 lightColor;	// ���C�g�̐F
		float pad;			// �p�f�B���O

	};

	// �_�����p�萔�o�b�t�@
	struct RayPointLightData {

		Vec3 lightPos;		// ���C�g�̍��W
		float lightSize;	// ���C�g�̃T�C�Y
		Vec3 lightColor;	// ���C�g�̐F
		float lightPower;	// ���C�g�̋���
		int isActive;		// �L�������邩�ǂ����̃t���O
		Vec3 pad;			// �p�f�B���O

	};

	static const int POINT_LIGHT_COUNT = 10;

	RayDirLightConstBufferData dirLight;						// ���s����
	std::array<RayPointLightData, POINT_LIGHT_COUNT> pointLight;// �_����

	// ����������
	void Init();

};

// ��C�U���p�萔�o�b�t�@
struct AtmosphericScatteringConstBufferData {

	float kr;				// ���C���[�U���萔
	float km;				// �~�[�U���萔
	float samples;			// ��C�U���T���v����
	float outerRadius;		// ��C���̍Œ��_�̍���
	float innerRadius;		// �n��̍���
	float eSun;				// ���z�̋���
	float g;				// �U���萔�����߂�ۂɎg�p����l
	float aveHeight;		// ���ϑ�C���x�����߂邽�߂̍���

	// ����������
	void Init();

};

// �f�o�b�O�p�萔�o�b�t�@
struct DebugConstBufferData {

	int seed;			// �����̎�
	int counter;		// �ݐσf�m�C�Y�p�̃T���v����
	int aoSampleCount;	// AO�ŉ���T���v�����邩�̐�
	int isNoiseScene;	// �m�C�Y�݂̂̉�ʂ�`�悷�邩
	int isLightHitScene;// ���C�g�ɓ��������ꏊ�݂̂�`�悷�邩
	int isNormalScene;	// �@�����݂̂�`�悷�邩
	int isMeshScene;	// ���b�V�����݂̂�`�悷�邩
	int isNoAO;			// AO���s��Ȃ���
	int isNoGI;			// GI���s��Ȃ���
	int isGIOnlyScene;	// GI�݂̂��s����
	float pad1;			// �p�f�B���O
	float pad2;			// �p�f�B���O

	// ����������
	void Init();

};

// ���ׂĂ̒萔�o�b�t�@��Z�߂����́B
struct RayConstBufferData {

	// �J����
	CameraConstBufferData camera;
	// ���C�g
	RayLightConstBufferData light;
	// AS�p
	AtmosphericScatteringConstBufferData as;
	// �f�o�b�O�p
	DebugConstBufferData debug;

	// ����������
	void Init();

};