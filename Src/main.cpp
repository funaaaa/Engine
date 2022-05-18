#include "SoundManager.h"
#include "PiplineManager.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "Input.h"
#include "FbxLoader.h"

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

#include "HitGroup.h"

#include <utilapiset.h>

#include "FHelper.h"

#define COLORHEX(hex) hex / 255.0f

#define SCREEN_VIRTUAL_WIDTH 300

// fps�X�V
void FPS();

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

struct KariConstBufferData {

	XMMATRIX mtxView;			// �r���[�s��B
	XMMATRIX mtxProj;			// �v���W�F�N�V�����s��B
	XMMATRIX mtxViewInv;		// �r���[�t�s��B
	XMMATRIX mtxProjInv;		// �v���W�F�N�V�����t�s��B
	XMVECTOR ambientColor;		// �����B
	RayDirLightData dirLight;
	RayPointLightData pointLight;
	int seed;
	int counter;
	int aoSampleCount;
	int isNoiseScene;
	int isNoiseOnlyScene;
	int isLightHitScene;
	int isNormalScene;
	int isMeshScene;
	int isNoAO;

};

// �f�o�b�O�p�̃p�C�v���C����؂�ւ����B
enum DEGU_PIPLINE_ID {
	DEF_PIPLINE,
	AO_PIPLINE,
	DENOISE_AO_PIPLINE,
};

// ���͑���
void Input(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& degugPiplineID);
void InputImGUI(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& degugPiplineID, bool& isMove);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	/*----------DirectX����������----------*/
	DirectXBase directXBase;							// DirectX��Օ���
	directXBase.Init();									// DirectX��Ղ̏�����
	SoundManager::Ins()->SettingSoundManager();	// �T�E���h�}�l�[�W���[���Z�b�g����

	/*----------�p�C�v���C������----------*/
	PiplineManager::Ins()->Init();

	/*----------�ϐ��錾----------*/
	srand(time(NULL));

	// �f�B�X�N���v�^�q�[�v���������B
	DescriptorHeapMgr::Ins()->GenerateDescriptorHeap();

	// FBXLoader���������B
	FbxLoader::Ins()->Init();

	// �q�b�g�O���[�v��ݒ�B
	HitGroupMgr::Ins()->Setting();

	// AO�p�̃p�C�v���C����ݒ�B
	vector<RayPiplineShaderData> useShaders;
	useShaders.push_back({ "Resource/ShaderFiles/RayTracing/AOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	RaytracingPipline aoPipline;
	aoPipline.Setting(useShaders, HitGroupMgr::AO_HIT_GROUP, 1, 1, 2, sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	vector<RayPiplineShaderData> dAOuseShaders;
	dAOuseShaders.push_back({ "Resource/ShaderFiles/RayTracing/DenoiseAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	RaytracingPipline deAOPipline;
	deAOPipline.Setting(dAOuseShaders, HitGroupMgr::DENOISE_AO_HIT_GROUP, 1, 1, 2, sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// �f�t�H���g�̃V�F�[�_�[��ݒ�B
	vector<RayPiplineShaderData> defShaders;
	defShaders.push_back({ "Resource/ShaderFiles/RayTracing/TriangleShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	RaytracingPipline defPipline;
	defPipline.Setting(defShaders, HitGroupMgr::DEF_HIT_GROUP, 1, 1, 2, sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// SPONZA��ǂݍ��ށB
	std::vector<int> sponzaInstance = MultiMeshLoadOBJ::Ins()->RayMultiMeshLoadOBJ("Resource/", "sponza.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF_HIT_GROUP]);

	// ���C�g�p�̃X�t�B�A��ǂݍ��ށB
	int sphereBlas = BLASRegister::Ins()->GenerateObj("Resource/", "sphere.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::AO_HIT_GROUP], { L"Resource/white.png" });
	int sphereIns = PorygonInstanceRegister::Ins()->CreateInstance(sphereBlas, 3);
	PorygonInstanceRegister::Ins()->AddScale(sphereIns, Vec3(10, 10, 10));
	PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, Vec3(0, 300, 0));

	PorygonInstanceRegister::Ins()->CalWorldMat();

	// TLAS�𐶐��B
	TLAS tlas;
	tlas.GenerateTLAS(L"TlasDescriptorHeap");

	// ���C�g���o�͗p�N���X���Z�b�g�B
	RaytracingOutput raytracingOutput;
	raytracingOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	RaytracingOutput raytracingOutputData;
	raytracingOutputData.Setting(DXGI_FORMAT_R32G32B32A32_FLOAT);

	// �V�F�[�_�[�e�[�u���𐶐��B
	aoPipline.ConstructionShaderTable();
	deAOPipline.ConstructionShaderTable();
	defPipline.ConstructionShaderTable();

	// �f�m�C�Y�p�̃K�E�V�A���u���[�R���s���[�g�V�F�[�_�[���Z�b�g�B
	//ComputeShader blurX;
	//blurX.Init(L"Resource/hlsl/Raytracing/GaussianBlur.hlsl",)

	// ���̒萔�o�b�t�@��錾
	KariConstBufferData constBufferData;
	constBufferData.ambientColor = { 1,1,1,1 };
	constBufferData.mtxProj = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),				//��p(60�x)
		(float)window_width / window_height,	//�A�X�y�N�g��
		0.1f, 1000000.0f							//�O�[�A���[
	);
	constBufferData.mtxProjInv = XMMatrixInverse(nullptr, constBufferData.mtxProj);
	Vec3 eye = { 0,0,-10 };
	Vec3 target = { 0,0,0 };
	Vec3 up = { 0,1,0 };
	constBufferData.mtxView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	constBufferData.mtxViewInv = XMMatrixInverse(nullptr, constBufferData.mtxView);
	constBufferData.counter = 0;
	constBufferData.isNoiseScene = false;
	constBufferData.isNoiseOnlyScene = false;
	constBufferData.pointLight.lightPos = Vec3(0, 300, 0);
	constBufferData.pointLight.lightSize = 30.0f;
	constBufferData.pointLight.lightPower = 300.0f;
	constBufferData.dirLight.isActive = false;
	constBufferData.dirLight.lightColor = Vec3{ 0,0,0 };
	constBufferData.dirLight.lihgtDir = Vec3{ 0,-1,0 };
	constBufferData.aoSampleCount = 1;
	constBufferData.isLightHitScene = false;
	constBufferData.isNormalScene = false;
	constBufferData.isMeshScene = false;
	constBufferData.isNoAO = false;

	DynamicConstBuffer constBuff;
	constBuff.Generate(sizeof(KariConstBufferData), L"constBuffer");

	// �f�o�b�O�p�Ńm�C�Y��ʂ��o���t���O�B
	DEGU_PIPLINE_ID debugPiplineID = AO_PIPLINE;

	// �J�������������B
	Camera::Ins()->Init();

	/*----------�Q�[�����[�v----------*/
	while (true) {
		/*----------���t���[������(�`��O����)----------*/
		directXBase.processBeforeDrawing();

		/*----- �X�V���� -----*/

		// �r���[�s��𐶐��B
		Camera::Ins()->GenerateMatView();

		FPS();

		// �����̎���X�V�B
		constBufferData.seed = FHelper::GetRand(0, 1000);

		// ���C�g����������
		bool isMoveLight = false;

		Input(constBufferData, isMoveLight, debugPiplineID);

		// �J�������X�V�B
		Camera::Ins()->Update();

		eye = Camera::Ins()->eye;
		target = Camera::Ins()->target;
		up = Camera::Ins()->up;

		// ���C�g���������Ƃ��̂݁A���[���h�s����Čv�Z����TLAS���X�V����B
		if (isMoveLight) {

			PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, constBufferData.pointLight.lightPos);
			PorygonInstanceRegister::Ins()->ChangeScale(sphereIns, constBufferData.pointLight.lightSize);

			tlas.Update();

		}

		/*----- �`�揈�� -----*/

		// ��ʂɕ\������郌���_�[�^�[�Q�b�g�ɖ߂��B
		//DirectXBase::Ins()->SetRenderTarget();

		RaytracingPipline setPipline = {};

		// �f�o�b�O�p�̃p�C�v���C��ID�ɉ������p�C�v���C�����Z�b�g����B
		if (debugPiplineID == DEF_PIPLINE) {

			constBufferData.counter = 0;
			setPipline = defPipline;

		}
		else if (debugPiplineID == AO_PIPLINE) {

			setPipline = aoPipline;

		}
		else if (debugPiplineID == DENOISE_AO_PIPLINE) {

			setPipline = deAOPipline;

		}

		auto frameIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();
		constBufferData.mtxView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
		constBufferData.mtxViewInv = XMMatrixInverse(nullptr, constBufferData.mtxView);
		// �萔�o�b�t�@�̒��g���X�V����B
		constBuff.Write(frameIndex, &constBufferData, sizeof(KariConstBufferData));
		auto sceneConstantBuffer = constBuff.GetBuffer(frameIndex);

		// �O���[�o�����[�g�V�O�l�`���Ŏg���Ɛ錾���Ă��郊�\�[�X����Z�b�g�B
		ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
		DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		DirectXBase::Ins()->cmdList->SetComputeRootSignature(setPipline.GetGlobalRootSig()->GetRootSig().Get());
		DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas.GetDescriptorHeapIndex()));
		raytracingOutput.SetComputeRootDescriptorTalbe(2);
		raytracingOutputData.SetComputeRootDescriptorTalbe(3);
		DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, sceneConstantBuffer->GetGPUVirtualAddress());


		// ���C�g���[�V���O���ʃo�b�t�@��UAV��Ԃ�
		raytracingOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		raytracingOutputData.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		DirectXBase::Ins()->cmdList->SetPipelineState1(setPipline.GetStateObject().Get());

		DirectXBase::Ins()->cmdList->DispatchRays(&setPipline.GetDispatchRayDesc());

		// �f�o�b�O�p�̃p�C�v���C�����f�m�C�Y�p�p�C�v���C����������A�R���s���[�g�V�F�[�_�[���g���ăf�m�C�Y��������B
		if (debugPiplineID == DENOISE_AO_PIPLINE) {



		}

		// �o�b�N�o�b�t�@�̃C���f�b�N�X���擾����B
		UINT backBufferIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();

		// �o���A��ݒ肵�e���\�[�X�̏�Ԃ�J�ڂ�����.
		raytracingOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		raytracingOutputData.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		D3D12_RESOURCE_BARRIER barriers[] = {
			CD3DX12_RESOURCE_BARRIER::Transition(
			DirectXBase::Ins()->backBuffers[backBufferIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_COPY_DEST),
		};
		DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(barriers), barriers);
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), raytracingOutput.GetRaytracingOutput().Get());

		// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�����Ƃɖ߂��B
		D3D12_RESOURCE_BARRIER endBarriers[] = {

		CD3DX12_RESOURCE_BARRIER::Transition(
		DirectXBase::Ins()->backBuffers[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_RENDER_TARGET)

		};
		DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(endBarriers), endBarriers);

		directXBase.processAfterDrawing();

	}

	return 0;
}


// �^�C�g���o�[��FPS�̍X�V
void FPS()
{
	static DWORD prev_time = timeGetTime();	// �O��̎���
	static int frame_count = 0;		// �t���[���J�E���g
	DWORD now_time = timeGetTime();		// ����̃t���[���̎���

	frame_count++;	// �t���[�������J�E���g����

	// �o�ߎ��Ԃ��P�b�𒴂�����J�E���g�Ǝ��Ԃ����Z�b�g
	if (now_time - prev_time >= 1000)
	{
		wchar_t fps[1000];
		_itow_s(frame_count, fps, 10);
		wchar_t moji[] = L"FPS";
		wcscat_s(fps, moji);
		SetWindowText(DirectXBase::Ins()->windowsAPI.hwnd, fps);
		//OutputDebugString(fps);

		prev_time = now_time;
		frame_count = 0;
	}
}

void Input(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& debugPiplineID) {

	bool isMove = false;

	float speed = 5.0f;
	float rot = 0.03f;
	if (Input::isKey(DIK_W)) {
		Camera::Ins()->Move(speed);
		isMove = true;
	}
	if (Input::isKey(DIK_S)) {
		Camera::Ins()->Move(-speed);
		isMove = true;
	}
	if (Input::isKey(DIK_A)) {
		Camera::Ins()->MoveRight(speed);
		isMove = true;
	}
	if (Input::isKey(DIK_D)) {
		Camera::Ins()->MoveRight(-speed);
		isMove = true;
	}
	if (Input::isKey(DIK_UP)) {
		Camera::Ins()->forwardVec.y += rot;
		isMove = true;
	}
	if (Input::isKey(DIK_DOWN)) {
		Camera::Ins()->forwardVec.y -= rot;
		isMove = true;
	}
	if (Input::isKey(DIK_LEFT)) {
		Camera::Ins()->AddRotationXZ(rot);
		isMove = true;
	}
	if (Input::isKey(DIK_RIGHT)) {
		Camera::Ins()->AddRotationXZ(-rot);
		isMove = true;
	}
	if (Input::isKey(DIK_LSHIFT)) {
		Camera::Ins()->eye.y -= 10.0f;
		isMove = true;
	}
	if (Input::isKey(DIK_SPACE)) {
		Camera::Ins()->eye.y += 10.0f;
		isMove = true;
	}

	InputImGUI(constBufferData, isMoveLight, debugPiplineID, isMove);

}

void InputImGUI(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& debugPiplineID, bool& isMove)
{

	// DirLight�ɂ���
	if (ImGui::TreeNode("DirLight")) {

		// ���C�g��\�����邩�ǂ����̃t���O���X�V�B
		bool isActive = static_cast<bool>(constBufferData.dirLight.isActive);
		ImGui::Checkbox("IsActive", &isActive);
		if (isActive != constBufferData.dirLight.isActive) isMove = true;
		constBufferData.dirLight.isActive = static_cast<int>(isActive);

		// �l��ۑ�����B
		float dirX = constBufferData.dirLight.lihgtDir.x;
		float dirZ = constBufferData.dirLight.lihgtDir.z;
		ImGui::SliderFloat("DirLightX", &constBufferData.dirLight.lihgtDir.x, -1.0f, 1.0f);
		ImGui::SliderFloat("DirLightZ", &constBufferData.dirLight.lihgtDir.z, -1.0f, 1.0f);

		// �ς���Ă�����
		if (dirX != constBufferData.dirLight.lihgtDir.x || dirZ != constBufferData.dirLight.lihgtDir.z) {

			isMove = true;
			isMoveLight = true;

		}

		// ���K������B
		constBufferData.dirLight.lihgtDir.Normalize();

		// ���C�g�̐F��ݒ�B
		array<float, 3> lightColor = { constBufferData.dirLight.lightColor.x,constBufferData.dirLight.lightColor.y,constBufferData.dirLight.lightColor.z };
		ImGui::ColorPicker3("LightColor", lightColor.data());
		// �F���ς���Ă�����B
		if (lightColor[0] != constBufferData.dirLight.lightColor.x || lightColor[1] != constBufferData.dirLight.lightColor.y || lightColor[2] != constBufferData.dirLight.lightColor.z) {
			isMove = true;
		}
		constBufferData.dirLight.lightColor.x = lightColor[0];
		constBufferData.dirLight.lightColor.y = lightColor[1];
		constBufferData.dirLight.lightColor.z = lightColor[2];

		ImGui::TreePop();

	}

	// PointLight�ɂ���
	if (ImGui::TreeNode("PointLight")) {

		// ���C�g��\�����邩�ǂ����̃t���O���X�V�B
		bool isActive = static_cast<bool>(constBufferData.pointLight.isActive);
		ImGui::Checkbox("IsActive", &isActive);
		if (isActive != constBufferData.pointLight.isActive) isMove = true;
		constBufferData.pointLight.isActive = static_cast<int>(isActive);

		// �l��ۑ�����B
		float dirX = constBufferData.pointLight.lightPos.x;
		float dirY = constBufferData.pointLight.lightPos.y;
		float dirZ = constBufferData.pointLight.lightPos.z;
		float lightSize = constBufferData.pointLight.lightSize;
		float aoSampleCount = constBufferData.aoSampleCount;
		float pointLightPower = constBufferData.pointLight.lightPower;
		float MOVE_LENGTH = 1500.0f;
		ImGui::SliderFloat("PointLightX", &constBufferData.pointLight.lightPos.x, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("PointLightY", &constBufferData.pointLight.lightPos.y, 0.0f, 1000.0f);
		ImGui::SliderFloat("PointLightZ", &constBufferData.pointLight.lightPos.z, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("PointLightRadius", &constBufferData.pointLight.lightSize, 1.0f, 50.0f);
		ImGui::SliderFloat("PointLightPower", &constBufferData.pointLight.lightPower, 300.0f, 1000.0f);
		ImGui::SliderFloat("AOSampleCount", &aoSampleCount, 1.0f, 30.0f);
		constBufferData.aoSampleCount = aoSampleCount;

		// �ς���Ă�����
		if (dirX != constBufferData.pointLight.lightPos.x || dirY != constBufferData.pointLight.lightPos.y || dirZ != constBufferData.pointLight.lightPos.z || lightSize != constBufferData.pointLight.lightSize || pointLightPower != constBufferData.pointLight.lightPower) {

			isMove = true;
			isMoveLight = true;

		}

		// ���C�g�̐F��ݒ�B
		array<float, 3> lightColor = { constBufferData.pointLight.lightColor.x,constBufferData.pointLight.lightColor.y,constBufferData.pointLight.lightColor.z };
		ImGui::ColorPicker3("LightColor", lightColor.data());
		// �F���ς���Ă�����B
		if (lightColor[0] != constBufferData.pointLight.lightColor.x || lightColor[1] != constBufferData.pointLight.lightColor.y || lightColor[2] != constBufferData.pointLight.lightColor.z) {
			isMove = true;
		}
		constBufferData.pointLight.lightColor.x = lightColor[0];
		constBufferData.pointLight.lightColor.y = lightColor[1];
		constBufferData.pointLight.lightColor.z = lightColor[2];

		ImGui::TreePop();

	}

	if (isMove) {
		constBufferData.counter = 0;
	}
	else {
		++constBufferData.counter;
	}

	// �K�w�\���ɂ���B
	if (ImGui::TreeNode("Debug")) {

		// ���b�V����\������B
		bool isMesh = constBufferData.isMeshScene;
		bool prevIsMesh = isMesh;
		ImGui::Checkbox("Mesh Scene", &isMesh);
		constBufferData.isMeshScene = isMesh;
		// �l�������������Ă�����A�T���v�����O������������B
		if (isMesh != prevIsMesh) {
			constBufferData.counter = 0;
		}

		// �@����\������B
		bool isNormal = constBufferData.isNormalScene;
		bool prevIsNormal = isNormal;
		ImGui::Checkbox("Normal Scene", &isNormal);
		constBufferData.isNormalScene = isNormal;
		// �l�������������Ă�����A�T���v�����O������������B
		if (isNormal != prevIsNormal) {
			constBufferData.counter = 0;
		}

		// ���C�g�����������ʂ����\������t���O���X�V�B
		bool isLightHit = constBufferData.isLightHitScene;
		bool prevIsLightHit = isLightHit;
		ImGui::Checkbox("LightHit Scene", &isLightHit);
		constBufferData.isLightHitScene = isLightHit;
		// �l�������������Ă�����A�T���v�����O������������B
		if (isLightHit != prevIsLightHit) {
			constBufferData.counter = 0;
		}

		// �p�C�v���C����I���B
		int debugPiplineBuff = debugPiplineID;
		ImGui::RadioButton("DEF PIPLINE", &debugPiplineBuff, 0);
		ImGui::SameLine();
		ImGui::RadioButton("AO PIPLINE", &debugPiplineBuff, 1);
		ImGui::SameLine();
		ImGui::RadioButton("DENOISE AO PIPLINE", &debugPiplineBuff, 2);
		debugPiplineID = (DEGU_PIPLINE_ID)debugPiplineBuff;

		// AO�̃p�C�v���C����I������Ă����Ƃ��̂݁A�m�C�Y���o�����̃t���O��\������B
		if (debugPiplineID == AO_PIPLINE) {

			// �f�o�b�O�p�Ńm�C�Y��ʂ��o�����߂̃t���O���Z�b�g�B
			bool isNoise = constBufferData.isNoiseScene;
			ImGui::Checkbox("Noise Scene", &isNoise);
			constBufferData.isNoiseScene = isNoise;

			// �f�o�b�O�p�Ńm�C�Y��ʂ݂̂��o�����߂̃t���O���Z�b�g�B
			bool isNoiseOnly = constBufferData.isNoiseOnlyScene;
			ImGui::Checkbox("NoiseOnly Scene", &isNoiseOnly);
			// �t���O������������Ă�����f�m�C�Y�J�E���^�[������������B
			if (isNoiseOnly != constBufferData.isNoiseOnlyScene) {
				constBufferData.counter = 0;
			}
			constBufferData.isNoiseOnlyScene = isNoiseOnly;

			// �A���r�G���g�I�N�����[�W�������s�����̃t���O���Z�b�g�B
			bool isNoAO = constBufferData.isNoAO;
			ImGui::Checkbox("NoAO Scene", &isNoAO);
			// �t���O������������Ă�����f�m�C�Y�J�E���^�[������������B
			if (isNoAO != constBufferData.isNoAO) {
				constBufferData.counter = 0;
			}
			constBufferData.isNoAO = isNoAO;

		}

		ImGui::TreePop();

	}

}


/*

���@��肽�����ƁE���Ȃ���΂����Ȃ�����
//�E�\�t�g�V���h�E�̃f�o�b�O�����킩��₷������B
//�@�������̈ʒu�̕\����\�����킩��₷������B
//�@�������ɂ���ďƂ炳�ꂽ�ʒu�݂̂�\���ł���悤�ɂ���B
//�E���̂��߂ɂ̓p�C�v���C����������ƕ�������悤�ɂ���K�v������H
//�EAO�̃o�O���C���B���邳���ꏊ�ɂ���ĈႤ�H
//�E�オ�I�������A���K�������o�[�g�ɂ��Ċw�ԁB
//�@���ǂ����Đ��K�������o�[�g���g���̂��܂ł�����Ɨ�������B
//�E���j�A���[�N�t���[����������B
//�@���K���}�l�␳�H
//�E���C�e�B���O�̃o�O���C������B
//�E�y�ʉ��̏���������B
�E���A���^�C���f�m�C�Y����������B
//�@���Ȃ��T���v�����𑝂₵�ĕ��ς��Ă��F���ς��Ȃ��̂��B���������ς���Ă���̂��H �� �����̎킪�ς���ĂȂ���������B
�@���ȑO�����P�x�ɏd�݂��������u���[�ł͎����ł��Ȃ������B
�@���m�C�Y��01�Ȃ̂ŁA�t�ɋP�x���傫������c
�@���t�Ɍ����΁A01�̋P�x�̍�������Ƃ���Ȃ�ăm�C�Y�������肦�Ȃ��H����Ă݂悤�I
�E�ȏ�̂��Ƃ����T�܂łɎ�������B


�E�f�o�b�O�p�̐؂�ւ�ID�ɐV�������ǉ�����B
�EIF���ŐV������̂Ƃ��̓R���s���[�g�V�F�[�_�[�ŉ��H���鏈�������ށB
�E���������Ȃ񂩂���K�E�V�A���u���[�̃R�[�h�������Ă���(�����R�[�h���炢����Ȃ炻���)�R���s���[�g�V�F�[�_�[�ɏ����B
�E���H�I�����I


�E���C�e�B���O�̏������A���C�g�ɂ������Ă���ʂ����s���悤�ɂ���B


*/