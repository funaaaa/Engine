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

	// �V���p�̃X�t�B�A�𐶐�����B
	skyDomeBlas = BLASRegister::Ins()->GenerateObj("Resource/", "skydome.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/skydome.jpg" });
	skyDomeIns = PorygonInstanceRegister::Ins()->CreateInstance(skyDomeBlas, 1);
	PorygonInstanceRegister::Ins()->AddScale(skyDomeIns, Vec3(10000000, 10000000, 10000000));

	// ���C�g�p�̃X�t�B�A��ǂݍ��ށB
	sphereBlas = BLASRegister::Ins()->GenerateObj("Resource/", "sphere.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/white.png" });
	sphereIns = PorygonInstanceRegister::Ins()->CreateInstance(sphereBlas, 0);
	PorygonInstanceRegister::Ins()->AddScale(sphereIns, Vec3(10, 10, 10));
	PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, Vec3(0, 300, 0));

	PorygonInstanceRegister::Ins()->CalWorldMat();

	// TLAS�𐶐��B
	tlas.GenerateTLAS();

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
	constBufferData.mtxView = DirectX::XMMatrixLookAtLH(constBufferData.eye.ConvertXMVECTOR(), constBufferData.target.ConvertXMVECTOR(), constBufferData.up.ConvertXMVECTOR());
	constBufferData.mtxViewInv = DirectX::XMMatrixInverse(nullptr, constBufferData.mtxView);

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
	D3D12_DISPATCH_RAYS_DESC rayDesc = setPipline.GetDispatchRayDesc();
	DirectXBase::Ins()->cmdList->DispatchRays(&rayDesc);

	// �f�o�b�O�p�̃p�C�v���C�����f�m�C�Y�p�p�C�v���C����������A�R���s���[�g�V�F�[�_�[���g���ăf�m�C�Y��������B
	if (debugPiplineID == DENOISE_AO_PIPLINE) {

		// [�m�C�Y��`��]�̂Ƃ��̓f�m�C�Y�������Ȃ��B
		if (!constBufferData.isNoiseScene) {

			// �f�o�b�O�@�\��[�@���`��][���b�V���`��][���C�g�ɓ��������_�̂ݕ`��]�̂Ƃ��̓f�m�C�Y�������Ȃ��悤�ɂ���B
			if (!constBufferData.isMeshScene && !constBufferData.isNormalScene && !constBufferData.isLightHitScene) {

				// ���C�g�Ƀf�m�C�Y��������B
				Denoiser::Ins()->Denoise(lightOutput.GetUAVIndex(), 1, 3);

			}

			// [AO���s��Ȃ�]�̂Ƃ��̓f�m�C�Y�������Ȃ��B
			if (!constBufferData.isNoAO) {

				// AO�Ƀf�m�C�Y��������B
				Denoiser::Ins()->Denoise(aoOutput.GetUAVIndex(), 100, 9);

			}

			// [GI���s��Ȃ�]�̂Ƃ��̓f�m�C�Y�������Ȃ��B
			if (!constBufferData.isNoGI) {

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
	if (constBufferData.isLightHitScene || constBufferData.isMeshScene || constBufferData.isNormalScene) {

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

void DevDXR::Input(KariConstBufferData& ConstBufferData, bool& IsMoveLight, DEGU_PIPLINE_ID& DebugPiplineID) {

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

	InputImGUI(ConstBufferData, IsMoveLight, DebugPiplineID, isMove);

}

void DevDXR::InputImGUI(KariConstBufferData& ConstBufferData, bool& IsMoveLight, DEGU_PIPLINE_ID& DebugPiplineID, bool& IsMove)
{

	// DirLight�ɂ���
	if (ImGui::TreeNode("DirLight")) {

		// ���C�g��\�����邩�ǂ����̃t���O���X�V�B
		bool isActive = static_cast<bool>(ConstBufferData.dirLight.isActive);
		ImGui::Checkbox("IsActive", &isActive);
		if (isActive != static_cast<bool>(ConstBufferData.dirLight.isActive)) IsMove = true;
		ConstBufferData.dirLight.isActive = static_cast<int>(isActive);

		// �l��ۑ�����B
		float dirX = ConstBufferData.dirLight.lihgtDir.x;
		float dirY = ConstBufferData.dirLight.lihgtDir.y;
		float dirZ = ConstBufferData.dirLight.lihgtDir.z;
		ImGui::SliderFloat("DirLightX", &ConstBufferData.dirLight.lihgtDir.x, -1.0f, 1.0f);
		ImGui::SliderFloat("DirLightY", &ConstBufferData.dirLight.lihgtDir.y, -1.0f, 1.0f);
		ImGui::SliderFloat("DirLightZ", &ConstBufferData.dirLight.lihgtDir.z, -1.0f, 1.0f);

		// �ς���Ă�����
		if (dirX != ConstBufferData.dirLight.lihgtDir.x || dirY != ConstBufferData.dirLight.lihgtDir.y || dirZ != ConstBufferData.dirLight.lihgtDir.z) {

			IsMove = true;
			IsMoveLight = true;

		}

		DebugPiplineID;

		// ���K������B
		ConstBufferData.dirLight.lihgtDir.Normalize();

		// ���C�g�̐F��ݒ�B
		std::array<float, 3> lightColor = { ConstBufferData.dirLight.lightColor.x,ConstBufferData.dirLight.lightColor.y,ConstBufferData.dirLight.lightColor.z };
		ImGui::ColorPicker3("LightColor", lightColor.data());
		// �F���ς���Ă�����B
		if (lightColor[0] != ConstBufferData.dirLight.lightColor.x || lightColor[1] != ConstBufferData.dirLight.lightColor.y || lightColor[2] != ConstBufferData.dirLight.lightColor.z) {
			IsMove = true;
		}
		ConstBufferData.dirLight.lightColor.x = lightColor[0];
		ConstBufferData.dirLight.lightColor.y = lightColor[1];
		ConstBufferData.dirLight.lightColor.z = lightColor[2];

		ImGui::TreePop();

	}

	// PointLight�ɂ���
	if (ImGui::TreeNode("PointLight")) {

		// ���C�g��\�����邩�ǂ����̃t���O���X�V�B
		bool isActive = static_cast<bool>(ConstBufferData.pointLight.isActive);
		ImGui::Checkbox("IsActive", &isActive);
		if (isActive != static_cast<bool>(ConstBufferData.pointLight.isActive)) IsMove = true;
		ConstBufferData.pointLight.isActive = static_cast<int>(isActive);

		// �l��ۑ�����B
		float dirX = ConstBufferData.pointLight.lightPos.x;
		float dirY = ConstBufferData.pointLight.lightPos.y;
		float dirZ = ConstBufferData.pointLight.lightPos.z;
		float lightSize = ConstBufferData.pointLight.lightSize;
		float aoSampleCount = static_cast<float>(ConstBufferData.aoSampleCount);
		float pointLightPower = ConstBufferData.pointLight.lightPower;
		float MOVE_LENGTH = 1500.0f;
		ImGui::SliderFloat("PointLightX", &ConstBufferData.pointLight.lightPos.x, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("PointLightY", &ConstBufferData.pointLight.lightPos.y, 0.0f, 1000.0f);
		ImGui::SliderFloat("PointLightZ", &ConstBufferData.pointLight.lightPos.z, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("PointLightRadius", &ConstBufferData.pointLight.lightSize, 1.0f, 50.0f);
		ImGui::SliderFloat("PointLightPower", &ConstBufferData.pointLight.lightPower, 300.0f, 1000.0f);
		ImGui::SliderFloat("AOSampleCount", &aoSampleCount, 1.0f, 30.0f);
		ConstBufferData.aoSampleCount = static_cast<int>(aoSampleCount);

		// �ς���Ă�����
		if (dirX != ConstBufferData.pointLight.lightPos.x || dirY != ConstBufferData.pointLight.lightPos.y || dirZ != ConstBufferData.pointLight.lightPos.z || lightSize != ConstBufferData.pointLight.lightSize || pointLightPower != ConstBufferData.pointLight.lightPower) {

			IsMove = true;
			IsMoveLight = true;

		}

		// ���C�g�̐F��ݒ�B
		std::array<float, 3> lightColor = { ConstBufferData.pointLight.lightColor.x,ConstBufferData.pointLight.lightColor.y,ConstBufferData.pointLight.lightColor.z };
		ImGui::ColorPicker3("LightColor", lightColor.data());
		// �F���ς���Ă�����B
		if (lightColor[0] != ConstBufferData.pointLight.lightColor.x || lightColor[1] != ConstBufferData.pointLight.lightColor.y || lightColor[2] != ConstBufferData.pointLight.lightColor.z) {
			IsMove = true;
		}
		ConstBufferData.pointLight.lightColor.x = lightColor[0];
		ConstBufferData.pointLight.lightColor.y = lightColor[1];
		ConstBufferData.pointLight.lightColor.z = lightColor[2];

		ImGui::TreePop();

	}


	if (IsMove) {
		ConstBufferData.counter = 0;
	}
	else {
		++ConstBufferData.counter;
	}

	// �K�w�\���ɂ���B
	if (ImGui::TreeNode("Debug")) {

		// ���b�V����\������B
		bool isMesh = ConstBufferData.isMeshScene;
		bool prevIsMesh = isMesh;
		ImGui::Checkbox("Mesh Scene", &isMesh);
		ConstBufferData.isMeshScene = isMesh;
		// �l�������������Ă�����A�T���v�����O������������B
		if (isMesh != prevIsMesh) {
			ConstBufferData.counter = 0;
		}

		// �@����\������B
		bool isNormal = ConstBufferData.isNormalScene;
		bool prevIsNormal = isNormal;
		ImGui::Checkbox("Normal Scene", &isNormal);
		ConstBufferData.isNormalScene = isNormal;
		// �l�������������Ă�����A�T���v�����O������������B
		if (isNormal != prevIsNormal) {
			ConstBufferData.counter = 0;
		}

		// ���C�g�����������ʂ����\������t���O���X�V�B
		bool isLightHit = ConstBufferData.isLightHitScene;
		bool prevIsLightHit = isLightHit;
		ImGui::Checkbox("LightHit Scene", &isLightHit);
		ConstBufferData.isLightHitScene = isLightHit;
		// �l�������������Ă�����A�T���v�����O������������B
		if (isLightHit != prevIsLightHit) {
			ConstBufferData.counter = 0;
		}

		// �f�o�b�O�p�Ńm�C�Y��ʂ��o�����߂̃t���O���Z�b�g�B
		bool isNoise = ConstBufferData.isNoiseScene;
		ImGui::Checkbox("Noise Scene", &isNoise);
		ConstBufferData.isNoiseScene = isNoise;

		// AO���s�����̃t���O���Z�b�g�B
		bool isNoAO = ConstBufferData.isNoAO;
		ImGui::Checkbox("NoAO Scene", &isNoAO);
		ConstBufferData.isNoAO = isNoAO;

		// GI���s�����̃t���O���Z�b�g�B
		bool isNoGI = ConstBufferData.isNoGI;
		ImGui::Checkbox("NoGI Scene", &isNoGI);
		ConstBufferData.isNoGI = isNoGI;

		// GI�݂̂�`�悷�邩�̃t���O���Z�b�g�B
		bool isGIOnlyScene = ConstBufferData.isGIOnlyScene;
		ImGui::Checkbox("GIOnly Scene", &isGIOnlyScene);
		ConstBufferData.isGIOnlyScene = isGIOnlyScene;

		// FPS��\�����邩�̃t���O���Z�b�g�B
		ImGui::Checkbox("Display FPS", &isDisplayFPS);


		ImGui::TreePop();

	}

	// �K�w�\���ɂ���B
	if (ImGui::TreeNode("AS")) {

		// ���z�����̋�����ݒ肷��B
		ImGui::SliderFloat("Sun Power", &ConstBufferData.AS.eSun, -10, 100);

		// ���C���[�U���萔�̒l��ݒ肷��B
		ImGui::SliderFloat("Rayleigh Scattering Power", &ConstBufferData.AS.kr, -1, 1);

		// �~�[�U���萔�̒l��ݒ肷��B
		ImGui::SliderFloat("Mie Scattering Power", &ConstBufferData.AS.km, -1, 1);

		// �T���v�����O����ݒ肷��B
		ImGui::SliderFloat("Sample Count", &ConstBufferData.AS.samples, 0, 10);

		// ��C���̈�ԏ�̍���
		ImGui::SliderFloat("Outer Radius", &ConstBufferData.AS.outerRadius, 0, 20000);

		// �n��̍���
		ImGui::SliderFloat("Inner Radius", &ConstBufferData.AS.innerRadius, 0, 20000);

		// ��C�U�������߂�ۂɎg�p����萔
		ImGui::SliderFloat("Scattering G", &ConstBufferData.AS.g, -1.0f, 1.0f);

		// ���ϑ�C���x�����߂邽�߂̍���
		ImGui::SliderFloat("Aveheight", &ConstBufferData.AS.aveHeight, 0.0f, 1.0f);

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