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

// �f�o�b�O�p���
struct DebugData {

	Vec3 playerPos_;
	int isDebugMesnInfo_;
	int debugMeshMoveX_;
	float timer_;
	Vec2 pad_;

	// ����������
	void Init();

};

//�{�����[���e�N�X�`���p
struct VolumeTextureData {

	Vec3 pos_;                //�{�����[���e�N�X�`���̃T�C�Y
	Vec3 color_;              //�t�H�O�̐F
	float gridSize_;            //�T���v�����O����O���b�h�̃T�C�Y
	float wrapCount_;           //�T���v�����O���W���͂ݏo�����ۂɉ���܂�Wrap���邩
	float samplingLength_;      //�T���v�����O����
	float sanplingMaxCount_;    //�ő�T���v�����O��
	float density_;             //�Z�x�W��
	float pad;

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
	//std::array<RayPointLightData, POINT_LIGHT_COUNT> pointLight_;// �_����

	// ����������
	void Init();

};

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
	// �A���t�@�p
	AlphaConstBufferData alphaData_;
	// �f�o�b�O�p���
	DebugData debug_;
	// �{�����[���e�N�X�`���p
	VolumeTextureData volumeTextureData_;

	// ����������
	void Init();

};