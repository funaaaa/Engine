#include "DevDXR.h"

void DevDXR::Init() {


	constBufferData.Init();
	constBuff.Generate(sizeof(KariConstBufferData), L"constBuffer");
	constBuff.Write(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex(), &constBufferData, sizeof(KariConstBufferData));

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	dAOuseShaders.push_back({ "Resource/ShaderFiles/RayTracing/DenoiseAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	deAOPipline.Setting(dAOuseShaders, HitGroupMgr::DENOISE_AO_HIT_GROUP, 1, 1, 4, sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// SPONZA��ǂݍ��ށB
	sponzaInstance = MultiMeshLoadOBJ::Ins()->RayMultiMeshLoadOBJ("Resource/", "sponza.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP]);

	// ���C�g�p�̃X�t�B�A��ǂݍ��ށB
	BLASRegister::Ins()->GenerateObj("Resource/", "sphere.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/white.png" });
	PorygonInstanceRegister::Ins()->CreateInstance(sphereBlas, 3);
	PorygonInstanceRegister::Ins()->AddScale(sphereIns, Vec3(10, 10, 10));
	PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, Vec3(0, 300, 0));

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

}

void DevDXR::Update() {

	// IMGUI�n
	//ImGuiWindow::Ins()->processBeforeDrawing();

	// �E�B���h�E�̖��O���Đݒ�B
	//SetWindowText(ImGuiWindow::Ins()->windowsAPI.hwnd, L"ImGuiWindow");

	isMoveLight = false;
	Input(constBufferData, isMoveLight, debugPiplineID);

	//ImGuiWindow::Ins()->processAfterDrawing();


	/*----------���t���[������(�`��O����)----------*/
	DirectXBase::Ins()->processBeforeDrawing();

	/*----- �X�V���� -----*/

	// �r���[�s��𐶐��B
	Camera::Ins()->GenerateMatView();

	FPS();

	// �����̎���X�V�B
	constBufferData.seed = FHelper::GetRand(0, 1000);

	// �J�������X�V�B
	Camera::Ins()->Update();

	constBufferData.eye = Camera::Ins()->eye;
	constBufferData.target = Camera::Ins()->target;
	constBufferData.up = Camera::Ins()->up;

	// ���C�g���������Ƃ��̂݁A���[���h�s����Čv�Z����TLAS���X�V����B
	if (isMoveLight) {

		// �_�����̈ʒu���X�V�B
		PorygonInstanceRegister::Ins()->ChangeTrans(sphereIns, constBufferData.pointLight.lightPos);
		PorygonInstanceRegister::Ins()->ChangeScale(sphereIns, constBufferData.pointLight.lightSize);

		tlas.Update();

	}

	/*----- �`�揈�� -----*/

	// ��ʂɕ\������郌���_�[�^�[�Q�b�g�ɖ߂��B
	//DirectXBase::Ins()->SetRenderTarget();

}

void DevDXR::Draw() {


	RaytracingPipline setPipline = {};

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

		// AO�Ƀf�m�C�Y��������B
		Denoiser::Ins()->Denoise(aoOutput.GetUAVIndex(), 100, 9);

		// ���C�g�Ƀf�m�C�Y��������B
		Denoiser::Ins()->Denoise(lightOutput.GetUAVIndex(), 100, 3);

		// GI�Ƀf�m�C�Y��������B
		Denoiser::Ins()->Denoise(giOutput.GetUAVIndex(), 100, 5);

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

	// �ʏ�p�C�v���C����������
	DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), denoiseMixTextureOutput.GetRaytracingOutput().Get());

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
	colorOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	giOutput.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(endBarriers), endBarriers);

	DirectXBase::Ins()->processAfterDrawing();

}


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

void DevDXR::Input(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& debugPiplineID) {

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

	//InputImGUI(constBufferData, isMoveLight, debugPiplineID, isMove);

}

void DevDXR::InputImGUI(KariConstBufferData& constBufferData, bool& isMoveLight, DEGU_PIPLINE_ID& debugPiplineID, bool& isMove)
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

	// SpotLight�ɂ���
	if (ImGui::TreeNode("SpotLight")) {

		// ���C�g��\�����邩�ǂ����̃t���O���X�V�B
		bool isActive = static_cast<bool>(constBufferData.spotLight.isActive);
		ImGui::Checkbox("IsActive", &isActive);
		if (isActive != constBufferData.spotLight.isActive) isMove = true;
		constBufferData.spotLight.isActive = static_cast<int>(isActive);

		// �l��ۑ�����B
		float posX = constBufferData.spotLight.pos.x;
		float posY = constBufferData.spotLight.pos.y;
		float posZ = constBufferData.spotLight.pos.z;
		float dirX = constBufferData.spotLight.dir.x;
		float dirY = constBufferData.spotLight.dir.y;
		float dirZ = constBufferData.spotLight.dir.z;
		float angle = constBufferData.spotLight.angle;
		float power = constBufferData.spotLight.power;
		float MOVE_LENGTH = 1500.0f;
		ImGui::SliderFloat("SpotLightX", &constBufferData.spotLight.pos.x, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("SpotLightY", &constBufferData.spotLight.pos.y, 0.0f, 1000.0f);
		ImGui::SliderFloat("SpotLightZ", &constBufferData.spotLight.pos.z, -MOVE_LENGTH, MOVE_LENGTH);
		ImGui::SliderFloat("SpotLightDirX", &constBufferData.spotLight.dir.x, -1.0f, 1.0f);
		ImGui::SliderFloat("SpotLightDirY", &constBufferData.spotLight.dir.y, -1.0f, 1.0f);
		ImGui::SliderFloat("SpotLightDirZ", &constBufferData.spotLight.dir.z, -1.0f, 1.0f);
		ImGui::SliderFloat("SpotLightAngle", &constBufferData.spotLight.angle, 0.001f, DirectX::XM_PI * 2.0f);
		ImGui::SliderFloat("SpotLightPower", &constBufferData.spotLight.power, 300.0f, 1000.0f);

		// �ς���Ă�����
		if (posX != constBufferData.spotLight.pos.x || posY != constBufferData.spotLight.pos.y || posZ != constBufferData.spotLight.pos.z ||
			power != constBufferData.spotLight.power || angle != constBufferData.spotLight.angle ||
			dirX != constBufferData.spotLight.dir.x || dirY != constBufferData.spotLight.dir.y || dirZ != constBufferData.spotLight.dir.z) {

			isMove = true;
			isMoveLight = true;
			constBufferData.spotLight.dir.Normalize();

		}

		// ���C�g�̐F��ݒ�B
		array<float, 3> lightColor = { constBufferData.spotLight.color.x,constBufferData.spotLight.color.y,constBufferData.spotLight.color.z };
		ImGui::ColorPicker3("LightColor", lightColor.data());
		// �F���ς���Ă�����B
		if (lightColor[0] != constBufferData.spotLight.color.x || lightColor[1] != constBufferData.spotLight.color.y || lightColor[2] != constBufferData.spotLight.color.z) {
			isMove = true;
		}
		constBufferData.spotLight.color.x = lightColor[0];
		constBufferData.spotLight.color.y = lightColor[1];
		constBufferData.spotLight.color.z = lightColor[2];

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