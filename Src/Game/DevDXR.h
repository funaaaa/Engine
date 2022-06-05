#pragma once
#include "SoundManager.h"
#include "PiplineManager.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "Input.h"
#include "FbxLoader.h"

#include "RayDenoiser.h"
#include "BakeAO.h"

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

#include "KariConstBuffer.h"

#include "FHelper.h"

class DevDXR {

private:

	/*===== �����o�ϐ� =====*/

	// ���̒萔�o�b�t�@��錾
	KariConstBufferData constBufferData;
	DynamicConstBuffer constBuff;

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	vector<RayPiplineShaderData> dAOuseShaders;
	RaytracingPipline deAOPipline;

	// SPONZA��ǂݍ��ށB
	std::vector<int> sponzaInstance;

	// ���C�g�p�̃X�t�B�A��ǂݍ��ށB
	int sphereBlas;
	int sphereIns;


	// TLAS�𐶐��B
	TLAS tlas;

	// ���C�g���o�͗p�N���X���Z�b�g�B
	RaytracingOutput aoOutput;

	// ���C�g���o�͗p�N���X���Z�b�g�B
	RaytracingOutput colorOutput;

	// ���C�g���o�͗p�N���X���Z�b�g�B
	RaytracingOutput lightOutput;

	// GI�o�͗p�N���X���Z�b�g�B
	RaytracingOutput giOutput;

	// �f�m�C�Y�̌��ʏo�͗p�N���X���Z�b�g�B
	RaytracingOutput denoiseMixTextureOutput;


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


public:

	/*===== �����o�֐� =====*/

	void Init();
	void Update();
	void Draw();


private:



	// fps�X�V
	void FPS();

	// ���͑���
	void Input(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& degugPiplineID);
	void InputImGUI(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& degugPiplineID, bool& isMove);



};