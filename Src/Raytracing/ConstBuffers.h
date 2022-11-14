#pragma once
#include "Vec.h"
#include <DirectXMath.h>
#include <array>

// �g�p����萔�o�b�t�@�B

// �J�����p�萔�o�b�t�@
struct CameraConstBufferData {

	DirectX::XMMATRIX mtxView_;			// �r���[�s��B
	DirectX::XMMATRIX mtxProj_;			// �v���W�F�N�V�����s��B
	DirectX::XMMATRIX mtxViewInv_;		// �r���[�t�s��B
	DirectX::XMMATRIX mtxProjInv_;		// �v���W�F�N�V�����t�s��B

	// ����������
	void Init();

};

struct RayLightConstBufferData {

	// �f�B���N�V���i�����C�g�p�萔�o�b�t�@
	struct RayDirLightConstBufferData {

		Vec3 lihgtDir_;		// ���C�g�̕���
		int isActive_;		// �L�������邩�ǂ����̃t���O
		Vec3 lightColor_;	// ���C�g�̐F
		int seed_;			// �����̎�

	};

	// �_�����p�萔�o�b�t�@
	struct RayPointLightData {

		Vec3 lightPos_;			// ���C�g�̍��W
		float lightSize_;		// ���C�g�̃T�C�Y
		Vec3 lightColor_;		// ���C�g�̐F
		float lightPower_;		// ���C�g�̋���
		int isActive_;			// �L�������邩�ǂ����̃t���O
		int isShadow_;			// �e���o���Ȃ��t���O
		DirectX::XMFLOAT2 pad_;	// �p�f�B���O

	};

	static const int POINT_LIGHT_COUNT = 30;

	RayDirLightConstBufferData dirLight_;						// ���s����
	std::array<RayPointLightData, POINT_LIGHT_COUNT> pointLight_;// �_����

	// ����������
	void Init();

};

// ��C�U���p�萔�o�b�t�@
//struct AtmosphericScatteringConstBufferData {
//
//	float kr_;				// ���C���[�U���萔
//	float km_;				// �~�[�U���萔
//	float samples_;			// ��C�U���T���v����
//	float outerRadius_;		// ��C���̍Œ��_�̍���
//	float innerRadius_;		// �n��̍���
//	float eSun_;				// ���z�̋���
//	float g_;				// �U���萔�����߂�ۂɎg�p����l
//	float aveHeight_;		// ���ϑ�C���x�����߂邽�߂̍���
//
//	// ����������
//	void Init();
//
//};

// �f�o�b�O�p�萔�o�b�t�@
//struct DebugConstBufferData {
//
//	int seed_;			// �����̎�
//	int isNoiseScene_;	// �m�C�Y�݂̂̉�ʂ�`�悷�邩
//	int isLightHitScene_;// ���C�g�ɓ��������ꏊ�݂̂�`�悷�邩
//	int isNormalScene_;	// �@�����݂̂�`�悷�邩
//	int isMeshScene_;	// ���b�V�����݂̂�`�悷�邩
//	int isNoAO_;			// AO���s��Ȃ���
//	int isNoGI_;			// GI���s��Ȃ���
//	int isGIOnlyScene_;	// GI�݂̂��s����
//
//	// ����������
//	void Init();
//
//};

// �A���t�@�p�f�[�^
struct AlphaData {

	int instanceIndex_;
	float alpha_;

};
struct AlphaConstBufferData {

	std::array<AlphaData, 120> alphaData_;

	// ����������
	void Init();

};

// ���ׂĂ̒萔�o�b�t�@��Z�߂����́B
struct RayConstBufferData {

	// �J����
	CameraConstBufferData camera_;
	// ���C�g
	RayLightConstBufferData light_;
	// AS�p
	//AtmosphericScatteringConstBufferData as_;
	// �f�o�b�O�p
	//DebugConstBufferData debug_;
	// �A���t�@�p
	AlphaConstBufferData alphaData_;

	// ����������
	void Init();

};