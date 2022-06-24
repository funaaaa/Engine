#pragma once
#include "SoundManager.h"
#include "Camera.h"
#include "Input.h"
#include "FbxLoader.h"

#include "RayDenoiser.h"

#include "BLASRegister.h"
#include "PorygonInstance.h"
#include "TLAS.h"
#include "RayRootsignature.h"
#include "DynamicConstBuffer.h"
#include "DescriptorHeapMgr.h"
#include "Vec.h"
#include "PorygonInstanceRegister.h"
#include "HitGroupMgr.h"
#include "RaytracingPipline.h"
#include "RaytracingOutput.h"
#include "MultiMeshLoadOBJ.h"
#include "ComputeShader.h"
#include "BLAS.h"

#include "HitGroup.h"

#include <utilapiset.h>

#include "ConstBuffers.h"
#include "FHelper.h"

#include "Player.h"
#include <array>

class DevDXR {

private:

	/*===== �����o�ϐ� =====*/


	// �萔�o�b�t�@
	RayConstBufferData constBufferData;
	DynamicConstBuffer constBuffer;

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	std::vector<RayPiplineShaderData> dAOuseShaders;
	RaytracingPipline deAOPipline;

	// SPONZA��ǂݍ��ށB
	std::vector<int> sponzaInstance;

	// ���C�g�p�̃X�t�B�A��ǂݍ��ށB
	int sphereBlas;
	std::array<int, RayLightConstBufferData::POINT_LIGHT_COUNT> sphereIns;

	// �V���p�̃X�t�B�A
	int skyDomeBlas;
	int skyDomeIns;

	// �X�e�[�W
	int stageBlas;
	int stageIns;


	// TLAS�𐶐��B
	TLAS tlas;

	// �A���r�G���g�I�N���[�W�����o�͗p�N���X���Z�b�g�B
	RaytracingOutput aoOutput;

	// �F�o�͗p�N���X���Z�b�g�B
	RaytracingOutput colorOutput;

	// �f�m�C�Y���郉�C�g�o�͗p�N���X���Z�b�g�B
	RaytracingOutput lightOutput;

	// GI�o�͗p�N���X���Z�b�g�B
	RaytracingOutput giOutput;

	// �f�m�C�Y�̌��ʏo�͗p�N���X���Z�b�g�B
	RaytracingOutput denoiseMixTextureOutput;

	// FPS�\�������邩�ۂ�
	bool isDisplayFPS;


	// �f�o�b�O�p�̃p�C�v���C����؂�ւ����B
	enum DEGU_PIPLINE_ID {
		DEF_PIPLINE,
		AO_PIPLINE,
		DENOISE_AO_PIPLINE,
	};

	// �f�o�b�O�p�Ńm�C�Y��ʂ��o���t���O�B
	DEGU_PIPLINE_ID debugPiplineID;

	// ���C�g����������
	bool isMoveLight;

	// �v���C���[
	Player player;


public:

	/*===== �����o�֐� =====*/

	void Init();
	void Update();
	void Draw();


private:



	// fps�X�V
	void FPS();

	// ���͑���
	void Input();
	void InputImGUI(bool& isMove);



};