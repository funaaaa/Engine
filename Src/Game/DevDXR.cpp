#include "DevDXR.h"

void DevDXR::Init() {

	// �萔�o�b�t�@�𐶐��B
	constBufferData.Init();
	constBuffer.Generate(sizeof(RayConstBufferData), L"CameraConstBuffer");
	constBuffer.Write(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex(), &constBufferData, sizeof(RayConstBufferData));

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	dAOuseShaders.push_back({ "Resource/ShaderFiles/RayTracing/DenoiseAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	deAOPipline.Setting(dAOuseShaders, HitGroupMgr::DENOISE_AO_HIT_GROUP, 1, 1, 4, sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(UINT) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// SPONZA��ǂݍ��ށB
	//sponzaInstance = MultiMeshLoadOBJ::Ins()->RayMultiMeshLoadOBJ("Resource/", "sponza.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP]);

	// �V���p�̃X�t�B�A�𐶐�����B
	skyDomeBlas = BLASRegister::Ins()->GenerateObj("Resource/", "skydome.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/skydome.jpg" });
	skyDomeIns = PorygonInstanceRegister::Ins()->CreateInstance(skyDomeBlas, PorygonInstanceRegister::SHADER_ID_AS);
	PorygonInstanceRegister::Ins()->AddScale(skyDomeIns, Vec3(100, 100, 100));

	// ���C�g�p�̃X�t�B�A��ǂݍ��ށB
	sphereBlas = BLASRegister::Ins()->GenerateObj("Resource/", "sphere.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/white.png" });
	sphereIns = PorygonInstanceRegister::Ins()->CreateInstance(sphereBlas, PorygonInstanceRegister::SHADER_ID_TEXCOLOR);
	PorygonInstanceRegister::Ins()->AddScale(sphereIns, Vec3(10, 10, 10));
	PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, Vec3(0, 300, 0));

	// �X�e�[�W��ǂݍ��ށB
	stageBlas = BLASRegister::Ins()->GenerateObj("Resource/", "stage3.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/stageUV.png" });
	stageIns = PorygonInstanceRegister::Ins()->CreateInstance(stageBlas, PorygonInstanceRegister::SHADER_ID_TEXCOLOR);
	PorygonInstanceRegister::Ins()->AddScale(stageIns, Vec3(200, 200, 200));


	// �v���C���[���������B
	player.Init(stageBlas, stageIns);

	PorygonInstanceRegister::Ins()->CalWorldMat();

	// TLAS�𐶐��B
	tlas.GenerateTLAS();

	// AO�o�͗p�N���X���Z�b�g�B
	aoOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// �F�o�͗p�N���X���Z�b�g�B
	colorOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// ���邳���o�͗p�N���X���Z�b�g�B
	lightOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// GI�o�͗p�N���X���Z�b�g�B
	giOutput.Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// �ŏI�o�͗p�N���X���Z�b�g�B
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

	Input();

	// �E�B���h�E�̖��O���X�V�B
	if (isDisplayFPS) {

		FPS();

	}
	else {

		SetWindowText(DirectXBase::Ins()->windowsAPI.hwnd, L"LE3A_21_�t�i�N���x_�^�N�~");

	}

	// �v���C���[���X�V�B
	player.Update();

	// �����̎���X�V�B
	constBufferData.debug.seed = FHelper::GetRand(0, 1000);

	// �J�������X�V�B
	Camera::Ins()->Update(player.GetPos(), player.GetForwardVec(), player.GetUpVec(), player.GetNowSpeedPer());

	// �_�����̈ʒu���X�V�B
	PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, constBufferData.light.pointLight.lightPos);
	PorygonInstanceRegister::Ins()->ChangeScale(sphereIns, constBufferData.light.pointLight.lightSize);

	tlas.Update();

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
	constBufferData.camera.mtxView = Camera::Ins()->matView;
	constBufferData.camera.mtxViewInv = DirectX::XMMatrixInverse(nullptr, constBufferData.camera.mtxView);
	constBufferData.camera.mtxProj = Camera::Ins()->matPerspective;
	constBufferData.camera.mtxProjInv = DirectX::XMMatrixInverse(nullptr, constBufferData.camera.mtxProj);

	// �萔�o�b�t�@���Z�b�g�B
	constBuffer.Write(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex(), &constBufferData, sizeof(constBufferData));

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

	// �萔�o�b�t�@���Z�b�g
	DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, constBuffer.GetBuffer(frameIndex)->GetGPUVirtualAddress());

	// �o�͗pUAV��ݒ�B
	lightOutput.SetComputeRootDescriptorTalbe(2);
	aoOutput.SetComputeRootDescriptorTalbe(3);
	colorOutput.SetComputeRootDescriptorTalbe(4);
	giOutput.SetComputeRootDescriptorTalbe(5);

	// �p�C�v���C����ݒ�B
	DirectXBase::Ins()->cmdList->SetPipelineState1(setPipline.GetStateObject().Get());

	// ���C�g���[�V���O�����s�B
	D3D12_DISPATCH_RAYS_DESC rayDesc = setPipline.GetDispatchRayDesc();
	DirectXBase::Ins()->cmdList->DispatchRays(&rayDesc);

	// �f�o�b�O�p�̃p�C�v���C�����f�m�C�Y�p�p�C�v���C����������A�R���s���[�g�V�F�[�_�[���g���ăf�m�C�Y��������B
	if (debugPiplineID == DENOISE_AO_PIPLINE) {

		// [�m�C�Y��`��]�̂Ƃ��̓f�m�C�Y�������Ȃ��B
		if (!constBufferData.debug.isNoiseScene) {

			// �f�o�b�O�@�\��[�@���`��][���b�V���`��][���C�g�ɓ��������_�̂ݕ`��]�̂Ƃ��̓f�m�C�Y�������Ȃ��悤�ɂ���B
			if (!constBufferData.debug.isMeshScene && !constBufferData.debug.isNormalScene && !constBufferData.debug.isLightHitScene) {

				// ���C�g�Ƀf�m�C�Y��������B
				Denoiser::Ins()->Denoise(lightOutput.GetUAVIndex(), 1, 3);

			}

			// [AO���s��Ȃ�]�̂Ƃ��̓f�m�C�Y�������Ȃ��B
			if (!constBufferData.debug.isNoAO) {

				// AO�Ƀf�m�C�Y��������B
				Denoiser::Ins()->Denoise(aoOutput.GetUAVIndex(), 100, 9);

			}

			// [GI���s��Ȃ�]�̂Ƃ��̓f�m�C�Y�������Ȃ��B
			if (!constBufferData.debug.isNoGI) {

				// GI�Ƀf�m�C�Y��������B
				Denoiser::Ins()->Denoise(giOutput.GetUAVIndex(), 100, 5);

			}

		}

		// �f�m�C�Y�����������C�e�B���O���ƐF����������B
		denoiseMixTextureOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		Denoiser::Ins()->MixColorAndLuminance(colorOutput.GetUAVIndex(), aoOutput.GetUAVIndex(), lightOutput.GetUAVIndex(), giOutput.GetUAVIndex(), denoiseMixTextureOutput.GetUAVIndex());
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
	if (constBufferData.debug.isLightHitScene || constBufferData.debug.isMeshScene || constBufferData.debug.isNormalScene) {

		// �f�m�C�Y���ꂽ�ʏ�̕`��
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), lightOutput.GetRaytracingOutput().Get());

	}
	else {

		// �f�m�C�Y���ꂽ�ʏ�̕`��
		colorOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), denoiseMixTextureOutput.GetRaytracingOutput().Get());

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

void DevDXR::Input() {

	bool isMove = false;

	InputImGUI(isMove);

}

void DevDXR::InputImGUI(bool& IsMove)
{

	// DirLight�ɂ���
	if (ImGui::TreeNode("DirLight")) {

		// ���C�g��\�����邩�ǂ����̃t���O���X�V�B
		bool isActive = static_cast<bool>(constBufferData.light.dirLight.isActive);
		ImGui::Checkbox("IsActive", &isActive);
		if (isActive != static_cast<bool>(constBufferData.light.dirLight.isActive)) IsMove = true;
		constBufferData.light.dirLight.isActive = static_cast<int>(isActive);

		// �l��ۑ�����B
		float dirX = constBufferData.light.dirLight.lihgtDir.x;
		float dirY = constBufferData.light.dirLight.lihgtDir.y;
		float dirZ = constBufferData.light.dirLight.lihgtDir.z;
		ImGui::SliderFloat("DirLightX", &constBufferData.light.dirLight.lihgtDir.x, -1.0f, 1.0f);
		ImGui::SliderFloat("DirLightY", &constBufferData.light.dirLight.lihgtDir.y, -1.0f, 1.0f);
		ImGui::SliderFloat("DirLightZ", &constBufferData.light.dirLight.lihgtDir.z, -1.0f, 1.0f);

		// �ς���Ă�����
		if (dirX != constBufferData.light.dirLight.lihgtDir.x || dirY != constBufferData.light.dirLight.lihgtDir.y || dirZ != constBufferData.light.dirLight.lihgtDir.z) {

			IsMove = true;

		}

		// ���K������B
		constBufferData.light.dirLight.lihgtDir.Normalize();

		ImGui::TreePop();

	}

	// PointLight�ɂ���
	if (ImGui::TreeNode("PointLight")) {

		// ���C�g��\�����邩�ǂ����̃t���O���X�V�B
		bool isActive = static_cast<bool>(constBufferData.light.pointLight.isActive);
		ImGui::Checkbox("IsActive", &isActive);
		if (isActive != static_cast<bool>(constBufferData.light.pointLight.isActive)) IsMove = true;
		constBufferData.light.pointLight.isActive = static_cast<int>(isActive);

		// �l��ۑ�����B
		float dirX = constBufferData.light.pointLight.lightPos.x;
		float dirY = constBufferData.light.pointLight.lightPos.y;
		float dirZ = constBufferData.light.pointLight.lightPos.z;
		float lightSize = constBufferData.light.pointLight.lightSize;
		float aoSampleCount = static_cast<float>(constBufferData.debug.aoSampleCount);
		float pointLightPower = constBufferData.light.pointLight.lightPower;
		float MOVE_LENGTH = 1500.0f;
		ImGui::SliderFloat("PointLightX", &constBufferData.light.pointLight.lightPos.x, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("PointLightY", &constBufferData.light.pointLight.lightPos.y, 0.0f, 1000.0f);
		ImGui::SliderFloat("PointLightZ", &constBufferData.light.pointLight.lightPos.z, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("PointLightRadius", &constBufferData.light.pointLight.lightSize, 1.0f, 50.0f);
		ImGui::SliderFloat("PointLightPower", &constBufferData.light.pointLight.lightPower, 300.0f, 1000.0f);
		ImGui::SliderFloat("AOSampleCount", &aoSampleCount, 1.0f, 30.0f);
		constBufferData.debug.aoSampleCount = static_cast<int>(aoSampleCount);

		// �ς���Ă�����
		if (dirX != constBufferData.light.pointLight.lightPos.x || dirY != constBufferData.light.pointLight.lightPos.y || dirZ != constBufferData.light.pointLight.lightPos.z || lightSize != constBufferData.light.pointLight.lightSize || pointLightPower != constBufferData.light.pointLight.lightPower) {

			IsMove = true;

		}

		// ���C�g�̐F��ݒ�B
		std::array<float, 3> lightColor = { constBufferData.light.pointLight.lightColor.x,constBufferData.light.pointLight.lightColor.y,constBufferData.light.pointLight.lightColor.z };
		ImGui::ColorPicker3("LightColor", lightColor.data());
		// �F���ς���Ă�����B
		if (lightColor[0] != constBufferData.light.pointLight.lightColor.x || lightColor[1] != constBufferData.light.pointLight.lightColor.y || lightColor[2] != constBufferData.light.pointLight.lightColor.z) {
			IsMove = true;
		}
		constBufferData.light.pointLight.lightColor.x = lightColor[0];
		constBufferData.light.pointLight.lightColor.y = lightColor[1];
		constBufferData.light.pointLight.lightColor.z = lightColor[2];

		ImGui::TreePop();

	}


	if (IsMove) {
		constBufferData.debug.counter = 0;
	}
	else {
		++constBufferData.debug.counter;
	}

	// �K�w�\���ɂ���B
	if (ImGui::TreeNode("Debug")) {

		// ���b�V����\������B
		bool isMesh = constBufferData.debug.isMeshScene;
		bool prevIsMesh = isMesh;
		ImGui::Checkbox("Mesh Scene", &isMesh);
		constBufferData.debug.isMeshScene = isMesh;
		// �l�������������Ă�����A�T���v�����O������������B
		if (isMesh != prevIsMesh) {
			constBufferData.debug.counter = 0;
		}

		// �@����\������B
		bool isNormal = constBufferData.debug.isNormalScene;
		bool prevIsNormal = isNormal;
		ImGui::Checkbox("Normal Scene", &isNormal);
		constBufferData.debug.isNormalScene = isNormal;
		// �l�������������Ă�����A�T���v�����O������������B
		if (isNormal != prevIsNormal) {
			constBufferData.debug.counter = 0;
		}

		// ���C�g�����������ʂ����\������t���O���X�V�B
		bool isLightHit = constBufferData.debug.isLightHitScene;
		bool prevIsLightHit = isLightHit;
		ImGui::Checkbox("LightHit Scene", &isLightHit);
		constBufferData.debug.isLightHitScene = isLightHit;
		// �l�������������Ă�����A�T���v�����O������������B
		if (isLightHit != prevIsLightHit) {
			constBufferData.debug.counter = 0;
		}

		// �f�o�b�O�p�Ńm�C�Y��ʂ��o�����߂̃t���O���Z�b�g�B
		bool isNoise = constBufferData.debug.isNoiseScene;
		ImGui::Checkbox("Noise Scene", &isNoise);
		constBufferData.debug.isNoiseScene = isNoise;

		// AO���s�����̃t���O���Z�b�g�B
		bool isNoAO = constBufferData.debug.isNoAO;
		ImGui::Checkbox("NoAO Scene", &isNoAO);
		constBufferData.debug.isNoAO = isNoAO;

		// GI���s�����̃t���O���Z�b�g�B
		bool isNoGI = constBufferData.debug.isNoGI;
		ImGui::Checkbox("NoGI Scene", &isNoGI);
		constBufferData.debug.isNoGI = isNoGI;

		// GI�݂̂�`�悷�邩�̃t���O���Z�b�g�B
		bool isGIOnlyScene = constBufferData.debug.isGIOnlyScene;
		ImGui::Checkbox("GIOnly Scene", &isGIOnlyScene);
		constBufferData.debug.isGIOnlyScene = isGIOnlyScene;

		// FPS��\�����邩�̃t���O���Z�b�g�B
		ImGui::Checkbox("Display FPS", &isDisplayFPS);


		ImGui::TreePop();

	}

	// �K�w�\���ɂ���B
	if (ImGui::TreeNode("AS")) {

		// ���z�����̋�����ݒ肷��B
		ImGui::SliderFloat("Sun Power", &constBufferData.as.eSun, -10, 100);

		// ���C���[�U���萔�̒l��ݒ肷��B
		ImGui::SliderFloat("Rayleigh Scattering Power", &constBufferData.as.kr, -1, 1);

		// �~�[�U���萔�̒l��ݒ肷��B
		ImGui::SliderFloat("Mie Scattering Power", &constBufferData.as.km, -1, 1);

		// �T���v�����O����ݒ肷��B
		ImGui::SliderFloat("Sample Count", &constBufferData.as.samples, 0, 10);

		// ��C���̈�ԏ�̍���
		ImGui::SliderFloat("Outer Radius", &constBufferData.as.outerRadius, 0, 20000);

		// �n��̍���
		ImGui::SliderFloat("Inner Radius", &constBufferData.as.innerRadius, 0, 20000);

		// ��C�U�������߂�ۂɎg�p����萔
		ImGui::SliderFloat("Scattering G", &constBufferData.as.g, -1.0f, 1.0f);

		// ���ϑ�C���x�����߂邽�߂̍���
		ImGui::SliderFloat("Aveheight", &constBufferData.as.aveHeight, 0.0f, 1.0f);

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


/*

��C�U���̌v�Z���͊p�x(�x�N�g��)�����킩��ΐF���擾�ł���H
���V���̃T�C�Y�Ǝ��_���Œ肾����B
�����Ƃ�����F�擾�p�Ń��C���΂��K�v���Ȃ��Ȃ�B
���f�B���N�V���i�����C�g�p�̃��C���y�����̂��g�����Ƃ��ł���B

*/