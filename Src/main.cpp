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

#include "HitGroup.h"

#include <utilapiset.h>

#define COLORHEX(hex) hex / 255.0f

#define SCREEN_VIRTUAL_WIDTH 300

// fps�X�V
void FPS();

struct KariConstBufferData {

	XMMATRIX mtxView;			// �r���[�s��B
	XMMATRIX mtxProj;			// �v���W�F�N�V�����s��B
	XMMATRIX mtxViewInv;		// �r���[�t�s��B
	XMMATRIX mtxProjInv;		// �v���W�F�N�V�����t�s��B
	XMVECTOR lightDirection;	// ���s�����̌����B
	XMVECTOR lightColor;		// ���s�����F�B
	XMVECTOR ambientColor;		// �����B

};


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

	// �g�p����V�F�[�_�[��񋓁B
	vector<RayPiplineShaderData> useShaders;
	useShaders.push_back({ "Resource/ShaderFiles/RayTracing/TriangleShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS"} });

	// ���C�g���p�C�v���C����ݒ�B
	RaytracingPipline pipline;
	pipline.Setting(useShaders);

	// SPONZA��ǂݍ��ށB
	std::vector<int> sponzaInstance = MultiMeshLoadOBJ::Ins()->RayMultiLeshLoadOBJ("Resource/", "sponza.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF_HIT_GROUP]);

	// ����BLAS�𐶐��B
	//int boneBlas = BLASRegister::Ins()->GenerateFbx("Resource/", "boneTest.fbx", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF_HIT_GROUP], L"Resource/backGround.png");

	// ����BLAS�𐶐��B
	//int monkeyBlas = BLASRegister::Ins()->GenerateObj("Resource/", "monkey.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF_HIT_GROUP], L"Resource/backGround.png");

	// ����BLAS�𐶐��B
	//int groundBlas = BLASRegister::Ins()->GenerateObj("Resource/", "ground.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF_HIT_GROUP], L"Resource/Fine_Basin.jpg");

	// �C���X�^���X�𐶐�
	//int boneA = PorygonInstanceRegister::Ins()->CreateInstance(boneBlas, 1);
	//int boneB = PorygonInstanceRegister::Ins()->CreateInstance(boneBlas, 1);
	//int boneC = PorygonInstanceRegister::Ins()->CreateInstance(boneBlas, 1);
	//int boneD = PorygonInstanceRegister::Ins()->CreateInstance(boneBlas, 1);
	//int monkey = PorygonInstanceRegister::Ins()->CreateInstance(monkeyBlas, 2);
	//int ground = PorygonInstanceRegister::Ins()->CreateInstance(groundBlas, 2);

	// �ړ�������B
	float addTrans = 200.0f;
	//PorygonInstanceRegister::Ins()->AddTrans(boneA, -addTrans, 0.0f, 0);
	//PorygonInstanceRegister::Ins()->AddTrans(boneB, addTrans, 0.0f, 0);
	//PorygonInstanceRegister::Ins()->AddTrans(boneC, 0.0f, 0.0f, addTrans);
	//PorygonInstanceRegister::Ins()->AddTrans(boneD, 0.0f, 0.0f, -addTrans);
	//PorygonInstanceRegister::Ins()->AddTrans(monkey, 0.0f, 0.0f, 0);
	//PorygonInstanceRegister::Ins()->AddTrans(ground, 0.0f, -1.0f, 0);

	// ������x��]������B
	//PorygonInstanceRegister::Ins()->AddRotate(boneA, 0.0f, -0.5f, 0);
	//PorygonInstanceRegister::Ins()->AddRotate(boneB, 0.0f, -0.5f, 0);
	//PorygonInstanceRegister::Ins()->AddRotate(boneC, 0.0f, -0.5f, 0);
	//PorygonInstanceRegister::Ins()->AddRotate(boneD, 0.0f, -0.5f, 0);

	// �g�傳����B
	//PorygonInstanceRegister::Ins()->AddScale(boneA, Vec3(70));
	//PorygonInstanceRegister::Ins()->AddScale(boneB, Vec3(70));
	//PorygonInstanceRegister::Ins()->AddScale(boneC, Vec3(70));
	//PorygonInstanceRegister::Ins()->AddScale(boneD, Vec3(70));

	// TLAS�𐶐��B
	TLAS tlas;
	tlas.GenerateTLAS(L"TlasDescriptorHeap");

	// ���C�g���o�͗p�N���X���Z�b�g�B
	RaytracingOutput raytracingOutput;
	raytracingOutput.Setting();

	// �V�F�[�_�[�e�[�u���𐶐��B
	pipline.ConstructionShaderTable();


	// ���̒萔�o�b�t�@��錾
	KariConstBufferData constBufferData;
	constBufferData.ambientColor = { 1,1,1,1 };
	constBufferData.lightColor = { 1,1,1,1 };
	constBufferData.lightDirection = { 0,1,-0.27f,0 };
	constBufferData.lightDirection = XMVector4Normalize(constBufferData.lightDirection);
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

	DynamicConstBuffer constBuff;
	constBuff.Generate(sizeof(KariConstBufferData), L"constBuffer");

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

		//Camera::target = triangle.GetPos();

		// �X�L�j���O�A�j���[�V����������B
		//BLASRegister::Ins()->ComputeSkin(boneBlas);

		float speed = 5.0f;
		float rot = 0.03f;
		if (Input::isKey(DIK_W)) Camera::Ins()->Move(speed);
		if (Input::isKey(DIK_S)) Camera::Ins()->Move(-speed);
		if (Input::isKey(DIK_A)) Camera::Ins()->MoveRight(speed);
		if (Input::isKey(DIK_D)) Camera::Ins()->MoveRight(-speed);
		if (Input::isKey(DIK_UP)) Camera::Ins()->forwardVec.y += rot;
		if (Input::isKey(DIK_DOWN)) Camera::Ins()->forwardVec.y -= rot;
		if (Input::isKey(DIK_LEFT)) Camera::Ins()->AddRotationXZ(rot);
		if (Input::isKey(DIK_RIGHT)) Camera::Ins()->AddRotationXZ(-rot);
		if (Input::isKey(DIK_LSHIFT)) Camera::Ins()->eye.y -= 10.0f;
		if (Input::isKey(DIK_SPACE)) Camera::Ins()->eye.y += 10.0f;

		//if (Input::isKey(DIK_I)) porygonInstance[0].AddTrans(0.0f, 0.0f, -0.1f);
		//if (Input::isKey(DIK_K)) porygonInstance[0].AddTrans(0.0f, 0.0f, 0.1f);
		//if (Input::isKey(DIK_J)) porygonInstance[0].AddTrans(0.1f, 0.0f, 0.0f);
		//if (Input::isKey(DIK_L)) porygonInstance[0].AddTrans(-0.1f, 0.0f, 0.0f);

		if (Input::isKey(DIK_1)) {

			//BLASRegister::Ins()->InitAnimation(boneBlas);

		}
		if (Input::isKey(DIK_2)) {

			//BLASRegister::Ins()->PlayAnimation(boneBlas);

		}
		if (Input::isKey(DIK_3)) {

			//BLASRegister::Ins()->StopAnimation(boneBlas);

		}

		//BLASRegister::Ins()->Update(boneBlas);

		// TLAS���X�V�B
		//tlas.Update();

		// �J�������X�V�B
		Camera::Ins()->Update();

		eye = Camera::Ins()->eye;
		target = Camera::Ins()->target;
		up = Camera::Ins()->up;


		/*----- �`�揈�� -----*/

		// ��ʂɕ\������郌���_�[�^�[�Q�b�g�ɖ߂��B
		//DirectXBase::Ins()->SetRenderTarget();

		auto frameIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();
		constBufferData.mtxView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
		constBufferData.mtxViewInv = XMMatrixInverse(nullptr, constBufferData.mtxView);
		// �萔�o�b�t�@�̒��g���X�V����B
		constBuff.Write(frameIndex, &constBufferData, sizeof(KariConstBufferData));
		auto sceneConstantBuffer = constBuff.GetBuffer(frameIndex);

		// �O���[�o�����[�g�V�O�l�`���Ŏg���Ɛ錾���Ă��郊�\�[�X����Z�b�g�B
		ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
		DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		DirectXBase::Ins()->cmdList->SetComputeRootSignature(pipline.GetGlobalRootSig()->GetRootSig().Get());
		DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas.GetDescriptorHeapIndex()));
		raytracingOutput.SetComputeRootDescriptorTalbe(2);
		DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, sceneConstantBuffer->GetGPUVirtualAddress());


		// ���C�g���[�V���O���ʃo�b�t�@��UAV��Ԃ�
		auto barrierToUAV = CD3DX12_RESOURCE_BARRIER::Transition(
			raytracingOutput.GetRaytracingOutput().Get(),
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);
		DirectXBase::Ins()->cmdList->ResourceBarrier(1, &barrierToUAV);

		DirectXBase::Ins()->cmdList->SetPipelineState1(pipline.GetStateObject().Get());

		DirectXBase::Ins()->cmdList->DispatchRays(&pipline.GetDispatchRayDesc());

		// �o�b�N�o�b�t�@�̃C���f�b�N�X���擾����B
		UINT backBufferIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();

		// �o���A��ݒ肵�e���\�[�X�̏�Ԃ�J�ڂ�����.
		D3D12_RESOURCE_BARRIER barriers[] = {
		CD3DX12_RESOURCE_BARRIER::Transition(
		raytracingOutput.GetRaytracingOutput().Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE),
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

