#include "DevDXR.h"

void DevDXR::Init() {


	constBufferData.Init();
	constBuff.Generate(sizeof(KariConstBufferData), L"constBuffer");
	constBuff.Write(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex(), &constBufferData, sizeof(KariConstBufferData));

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	dAOuseShaders.push_back({ "Resource/ShaderFiles/RayTracing/DenoiseAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	deAOPipline.Setting(dAOuseShaders, HitGroupMgr::DENOISE_AO_HIT_GROUP, 1, 1, 4, sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// SPONZA��ǂݍ��ށB
	//sponzaInstance = MultiMeshLoadOBJ::Ins()->RayMultiMeshLoadOBJ("Resource/", "sponza.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP]);

	// ���C�g�p�̃X�t�B�A��ǂݍ��ށB
	//sphereBlas = BLASRegister::Ins()->GenerateObj("Resource/", "sphere.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/white.png" });
	//sphereIns = PorygonInstanceRegister::Ins()->CreateInstance(sphereBlas, 1);
	//PorygonInstanceRegister::Ins()->AddScale(sphereIns, Vec3(10, 10, 10));
	//PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, Vec3(0, 300, 0));

	// �V���p�̃X�t�B�A�𐶐�����B
	skyDomeBlas = BLASRegister::Ins()->GenerateObj("Resource/", "skydome.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/skydome.jpg" });
	skyDomeIns = PorygonInstanceRegister::Ins()->CreateInstance(skyDomeBlas, 1);
	PorygonInstanceRegister::Ins()->AddScale(skyDomeIns, Vec3(10000000, 10000000, 10000000));

	PorygonInstanceRegister::Ins()->CalWorldMat();

	// TLAS�𐶐��B
	tlas.GenerateTLAS(L"TlasDescriptorHeap");

	// ���C�g���o�͗p�N���X���Z�b�g�B
	aoOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// ���C�g���o�͗p�N���X���Z�b�g�B
	colorOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// ���邳���o�͗p�N���X���Z�b�g�B
	lightOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// gi�o�͗p�N���X���Z�b�g�B
	giOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// �f�m�C�Y�̌��ʏo�͗p�N���X���Z�b�g�B
	denoiseMixTextureOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// �V�F�[�_�[�e�[�u���𐶐��B
	deAOPipline.ConstructionShaderTable();

	// �f�o�b�O�p�Ńm�C�Y��ʂ��o���t���O�B
	debugPiplineID = DENOISE_AO_PIPLINE;

	// ���C�g����������
	isMoveLight = false;

	// �f�o�b�O�p�̃p�C�v���C����؂�ւ����B
	enum DEGU_PIPLINE_ID {
		DEF_PIPLINE,
		AO_PIPLINE,
		DENOISE_AO_PIPLINE,
	};

	isDisplayFPS = false;

}

void DevDXR::Update() {

	/*----------���t���[������(�`��O����)----------*/
	DirectXBase::Ins()->processBeforeDrawing();

	/*----- �X�V���� -----*/

	// �V������]������B
	//PorygonInstanceRegister::Ins()->AddRotate(skyDomeIns, { 0.01,0.01,0.01 });

	isMoveLight = false;
	Input(constBufferData, isMoveLight, debugPiplineID);

	// �E�B���h�E�̖��O���X�V�B
	if (isDisplayFPS) {

		FPS();

	}
	else {

		SetWindowText(DirectXBase::Ins()->windowsAPI.hwnd, L"LE3A_21_�t�i�N���x_�^�N�~");

	}

	// �r���[�s��𐶐��B
	Camera::Ins()->GenerateMatView();

	// �����̎���X�V�B
	constBufferData.seed = FHelper::GetRand(0, 1000);

	// �J�������X�V�B
	Camera::Ins()->Update();

	constBufferData.eye = Camera::Ins()->eye;
	constBufferData.target = Camera::Ins()->target;
	constBufferData.up = Camera::Ins()->up;

	// ���C�g���������Ƃ��̂݁A���[���h�s����Čv�Z����TLAS���X�V����B
	//if (isMoveLight) {

		// �_�����̈ʒu���X�V�B
		PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, constBufferData.pointLight.lightPos);
		PorygonInstanceRegister::Ins()->ChangeScale(sphereIns, constBufferData.pointLight.lightSize);

		tlas.Update();

	//}

	/*----- �`�揈�� -----*/

	// ��ʂɕ\������郌���_�[�^�[�Q�b�g�ɖ߂��B
	//DirectXBase::Ins()->SetRenderTarget();

}

void DevDXR::Draw() {


	RaytracingPipline setPipline = {};

	// TLAS�X�V�B
	//tlas.Update();

	// �f�o�b�O�p�̃p�C�v���C��ID�ɉ������p�C�v���C�����Z�b�g����B
	setPipline = deAOPipline;

	// �J�����s����X�V�B
	auto frameIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();
	constBufferData.mtxView = XMMatrixLookAtLH(constBufferData.eye.ConvertXMVECTOR(), constBufferData.target.ConvertXMVECTOR(), constBufferData.up.ConvertXMVECTOR());
	constBufferData.mtxViewInv = XMMatrixInverse(nullptr, constBufferData.mtxView);

	// �萔�o�b�t�@�̒��g���X�V����B
	constBuff.Write(frameIndex, &constBufferData, sizeof(KariConstBufferData));
	auto sceneConstantBuffer = constBuff.GetBuffer(frameIndex);

	// �o���A��ݒ肵�e���\�[�X�̏�Ԃ�J�ڂ�����.
	aoOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	lightOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	colorOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	giOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// �O���[�o�����[�g�V�O�l�`���Ŏg���Ɛ錾���Ă��郊�\�[�X����Z�b�g�B
	ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	DirectXBase::Ins()->cmdList->SetComputeRootSignature(setPipline.GetGlobalRootSig()->GetRootSig().Get());

	// TLAS��ݒ�B
	DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas.GetDescriptorHeapIndex()));

	// �o�͗pUAV��ݒ�B
	lightOutput.SetComputeRootDescriptorTalbe(2);
	aoOutput.SetComputeRootDescriptorTalbe(3);
	colorOutput.SetComputeRootDescriptorTalbe(4);
	giOutput.SetComputeRootDescriptorTalbe(5);
	DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, sceneConstantBuffer->GetGPUVirtualAddress());

	// �p�C�v���C����ݒ�B
	DirectXBase::Ins()->cmdList->SetPipelineState1(setPipline.GetStateObject().Get());

	// ���C�g���[�V���O�����s�B
	DirectXBase::Ins()->cmdList->DispatchRays(&setPipline.GetDispatchRayDesc());

	// �f�o�b�O�p�̃p�C�v���C�����f�m�C�Y�p�p�C�v���C����������A�R���s���[�g�V�F�[�_�[���g���ăf�m�C�Y��������B
	if (debugPiplineID == DENOISE_AO_PIPLINE) {

		//// [�m�C�Y��`��]�̂Ƃ��̓f�m�C�Y�������Ȃ��B
		//if (!constBufferData.isNoiseScene) {

		//	// �f�o�b�O�@�\��[�@���`��][���b�V���`��][���C�g�ɓ��������_�̂ݕ`��]�̂Ƃ��̓f�m�C�Y�������Ȃ��悤�ɂ���B
		//	if (!constBufferData.isMeshScene && !constBufferData.isNormalScene && !constBufferData.isLightHitScene) {

		//		// ���C�g�Ƀf�m�C�Y��������B
		//		Denoiser::Ins()->Denoise(lightOutput.GetUAVIndex(), 1, 3);

		//	}

		//	// [AO���s��Ȃ�]�̂Ƃ��̓f�m�C�Y�������Ȃ��B
		//	if (!constBufferData.isNoAO) {

		//		// AO�Ƀf�m�C�Y��������B
		//		Denoiser::Ins()->Denoise(aoOutput.GetUAVIndex(), 100, 9);

		//	}

		//	// [GI���s��Ȃ�]�̂Ƃ��̓f�m�C�Y�������Ȃ��B
		//	if (!constBufferData.isNoGI) {

		//		// GI�Ƀf�m�C�Y��������B
		//		Denoiser::Ins()->Denoise(giOutput.GetUAVIndex(), 100, 5);

		//	}

		//}

		//// �f�m�C�Y�����������C�e�B���O���ƐF����������B
		denoiseMixTextureOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		//Denoiser::Ins()->MixColorAndLuminance(colorOutput.GetUAVIndex(), aoOutput.GetUAVIndex(), lightOutput.GetUAVIndex(), giOutput.GetUAVIndex(), denoiseMixTextureOutput.GetUAVIndex());
		denoiseMixTextureOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	}


	// �o�b�N�o�b�t�@�̃C���f�b�N�X���擾����B
	UINT backBufferIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barriers[] = {
		CD3DX12_RESOURCE_BARRIER::Transition(
		DirectXBase::Ins()->backBuffers[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_COPY_DEST),
	};
	DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(barriers), barriers);

	// �f�o�b�O���ɂ���ĕ`�悷��f�[�^��ς���B
	if (constBufferData.isLightHitScene || constBufferData.isMeshScene || constBufferData.isNormalScene) {

		// �f�m�C�Y���ꂽ�ʏ�̕`��
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), lightOutput.GetRaytracingOutput().Get());

	}
	else {

		// �f�m�C�Y���ꂽ�ʏ�̕`��
		colorOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), colorOutput.GetRaytracingOutput().Get());

	}

	// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�����Ƃɖ߂��B
	D3D12_RESOURCE_BARRIER endBarriers[] = {

	CD3DX12_RESOURCE_BARRIER::Transition(
	DirectXBase::Ins()->backBuffers[backBufferIndex].Get(),
	D3D12_RESOURCE_STATE_COPY_DEST,
	D3D12_RESOURCE_STATE_RENDER_TARGET)

	};

	// �o���A��ݒ肵�e���\�[�X�̏�Ԃ�J�ڂ�����.
	aoOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	lightOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	//colorOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	giOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(endBarriers), endBarriers);

	DirectXBase::Ins()->processAfterDrawing();

}

void DevDXR::Input(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& debugPiplineID) {

	bool isMove = false;

	float speed = 10.0f;
	float rot = 0.03f;
	if (Input::Ins()->isKey(DIK_W)) {
		Camera::Ins()->Move(speed);
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_S)) {
		Camera::Ins()->Move(-speed);
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_A)) {
		Camera::Ins()->MoveRight(speed);
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_D)) {
		Camera::Ins()->MoveRight(-speed);
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_UP)) {
		Camera::Ins()->forwardVec.y += rot;
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_DOWN)) {
		Camera::Ins()->forwardVec.y -= rot;
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_LEFT)) {
		Camera::Ins()->AddRotationXZ(rot);
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_RIGHT)) {
		Camera::Ins()->AddRotationXZ(-rot);
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_LSHIFT)) {
		Camera::Ins()->eye.y -= 10.0f;
		isMove = true;
	}
	if (Input::Ins()->isKey(DIK_SPACE)) {
		Camera::Ins()->eye.y += 10.0f;
		isMove = true;
	}

	InputImGUI(constBufferData, isMoveLight, debugPiplineID, isMove);

}

void DevDXR::InputImGUI(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& debugPiplineID, bool& isMove)
{

	// DirLight�ɂ���
	if (ImGui::TreeNode("DirLight")) {

		// ���C�g��\�����邩�ǂ����̃t���O���X�V�B
		bool isActive = static_cast<bool>(constBufferData.dirLight.isActive);
		ImGui::Checkbox("IsActive", &isActive);
		if (isActive != static_cast<bool>(constBufferData.dirLight.isActive)) isMove = true;
		constBufferData.dirLight.isActive = static_cast<int>(isActive);

		// �l��ۑ�����B
		float dirX = constBufferData.dirLight.lihgtDir.x;
		float dirY = constBufferData.dirLight.lihgtDir.y;
		float dirZ = constBufferData.dirLight.lihgtDir.z;
		ImGui::SliderFloat("DirLightX", &constBufferData.dirLight.lihgtDir.x, -1.0f, 1.0f);
		ImGui::SliderFloat("DirLightY", &constBufferData.dirLight.lihgtDir.y, -1.0f, 1.0f);
		ImGui::SliderFloat("DirLightZ", &constBufferData.dirLight.lihgtDir.z, -1.0f, 1.0f);

		// �ς���Ă�����
		if (dirX != constBufferData.dirLight.lihgtDir.x || dirY != constBufferData.dirLight.lihgtDir.y || dirZ != constBufferData.dirLight.lihgtDir.z) {

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
		if (isActive != static_cast<bool>(constBufferData.pointLight.isActive)) isMove = true;
		constBufferData.pointLight.isActive = static_cast<int>(isActive);

		// �l��ۑ�����B
		float dirX = constBufferData.pointLight.lightPos.x;
		float dirY = constBufferData.pointLight.lightPos.y;
		float dirZ = constBufferData.pointLight.lightPos.z;
		float lightSize = constBufferData.pointLight.lightSize;
		float aoSampleCount = static_cast<float>(constBufferData.aoSampleCount);
		float pointLightPower = constBufferData.pointLight.lightPower;
		float MOVE_LENGTH = 1500.0f;
		ImGui::SliderFloat("PointLightX", &constBufferData.pointLight.lightPos.x, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("PointLightY", &constBufferData.pointLight.lightPos.y, 0.0f, 1000.0f);
		ImGui::SliderFloat("PointLightZ", &constBufferData.pointLight.lightPos.z, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("PointLightRadius", &constBufferData.pointLight.lightSize, 1.0f, 50.0f);
		ImGui::SliderFloat("PointLightPower", &constBufferData.pointLight.lightPower, 300.0f, 1000.0f);
		ImGui::SliderFloat("AOSampleCount", &aoSampleCount, 1.0f, 30.0f);
		constBufferData.aoSampleCount = static_cast<int>(aoSampleCount);

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

		// �f�o�b�O�p�Ńm�C�Y��ʂ��o�����߂̃t���O���Z�b�g�B
		bool isNoise = constBufferData.isNoiseScene;
		ImGui::Checkbox("Noise Scene", &isNoise);
		constBufferData.isNoiseScene = isNoise;

		// AO���s�����̃t���O���Z�b�g�B
		bool isNoAO = constBufferData.isNoAO;
		ImGui::Checkbox("NoAO Scene", &isNoAO);
		constBufferData.isNoAO = isNoAO;

		// GI���s�����̃t���O���Z�b�g�B
		bool isNoGI = constBufferData.isNoGI;
		ImGui::Checkbox("NoGI Scene", &isNoGI);
		constBufferData.isNoGI = isNoGI;

		// GI�݂̂�`�悷�邩�̃t���O���Z�b�g�B
		bool isGIOnlyScene = constBufferData.isGIOnlyScene;
		ImGui::Checkbox("GIOnly Scene", &isGIOnlyScene);
		constBufferData.isGIOnlyScene = isGIOnlyScene;

		// FPS��\�����邩�̃t���O���Z�b�g�B
		ImGui::Checkbox("Display FPS", &isDisplayFPS);


		ImGui::TreePop();

	}

	// �K�w�\���ɂ���B
	if (ImGui::TreeNode("AS")) {

		// ���z�����̋�����ݒ肷��B
		ImGui::SliderFloat("Sun Power", &constBufferData.AS.eSun, -10, 100);

		// ���C���[�U���萔�̒l��ݒ肷��B
		ImGui::SliderFloat("Rayleigh Scattering Power", &constBufferData.AS.kr, -1, 1);

		// �~�[�U���萔�̒l��ݒ肷��B
		ImGui::SliderFloat("Mie Scattering Power", &constBufferData.AS.km, -1, 1);

		// �T���v�����O����ݒ肷��B
		ImGui::SliderFloat("Sample Count", &constBufferData.AS.samples, 0, 10);

		// ��C���̈�ԏ�̍���
		ImGui::SliderFloat("Outer Radius", &constBufferData.AS.outerRadius, 0, 20000);

		// �n��̍���
		ImGui::SliderFloat("Inner Radius", &constBufferData.AS.innerRadius, 0, 20000);

		// ��C�U�������߂�ۂɎg�p����萔
		ImGui::SliderFloat("Scattering G", &constBufferData.AS.g, -1.0f, 1.0f);

		// ���ϑ�C���x�����߂邽�߂̍���
		ImGui::SliderFloat("Aveheight", &constBufferData.AS.aveHeight, 0.0f, 1.0f);

		ImGui::TreePop();

	}

}

#pragma comment (lib, "winmm.lib")

// �^�C�g���o�[��FPS�̍X�V
void DevDXR::FPS()
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