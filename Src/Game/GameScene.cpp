#include "GameScene.h"
#include "HitGroupMgr.h"
#include "BLASRegister.h"
#include "PorygonInstanceRegister.h"
#include "Player.h"
#include "FHelper.h"
#include "Camera.h"
#include "RayDenoiser.h"
#include "RayRootsignature.h"
#include "DynamicConstBuffer.h"
#include "RaytracingPipline.h"
#include "RaytracingOutput.h"
#include "TLAS.h"
#include "Input.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "Pipline.h"

GameScene::GameScene()
{

	/*===== ���������� =====*/

	// �萔�o�b�t�@�𐶐��B
	constBufferData.Init();
	constBuffer = std::make_shared<DynamicConstBuffer>();
	constBuffer->Generate(sizeof(RayConstBufferData), L"CameraConstBuffer");
	constBuffer->Write(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex(), &constBufferData, sizeof(RayConstBufferData));

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	dAOuseShaders.push_back({ "Resource/ShaderFiles/RayTracing/DenoiseAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	deAOPipline = std::make_shared<RaytracingPipline>();
	deAOPipline->Setting(dAOuseShaders, HitGroupMgr::DENOISE_AO_HIT_GROUP, 1, 1, 4, sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(UINT) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// SPONZA��ǂݍ��ށB
	//sponzaInstance = MultiMeshLoadOBJ::Ins()->RayMultiMeshLoadOBJ("Resource/", "sponza.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP]);

	// �V���p�̃X�t�B�A�𐶐�����B
	skyDomeBlas = BLASRegister::Ins()->GenerateObj("Resource/", "skydome.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/skydome.jpg" });
	skyDomeIns = PorygonInstanceRegister::Ins()->CreateInstance(skyDomeBlas, PorygonInstanceRegister::SHADER_ID_AS);
	PorygonInstanceRegister::Ins()->AddScale(skyDomeIns, Vec3(100, 100, 100));

	// ���C�g�p�̃X�t�B�A��ǂݍ��ށB
	sphereBlas = BLASRegister::Ins()->GenerateObj("Resource/", "sphere.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" });
	for (auto& index : sphereIns) {

		index = PorygonInstanceRegister::Ins()->CreateInstance(sphereBlas, PorygonInstanceRegister::SHADER_ID_LIGHT);
		PorygonInstanceRegister::Ins()->AddScale(index, Vec3(1, 1, 1));
		PorygonInstanceRegister::Ins()->ChangeTrans(index, Vec3(0, 300, 0));

	}

	// �h���t�g���̃p�[�e�B�N���̃N���X���Z�b�e�B���O����B
	//DriftParticleMgr::Ins()->Setting(0);

	// �X�e�[�W��ǂݍ��ށB
	stageBlas = BLASRegister::Ins()->GenerateObj("Resource/", "stage3.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/white.png" });
	stageIns = PorygonInstanceRegister::Ins()->CreateInstance(stageBlas, PorygonInstanceRegister::SHADER_ID_DEF);
	PorygonInstanceRegister::Ins()->AddScale(stageIns, Vec3(200, 200, 200));
	stageGrassBlas = BLASRegister::Ins()->GenerateObj("Resource/", "grass.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/green.png",L"Resource/grassNormal.png" });
	stageGrassIns = PorygonInstanceRegister::Ins()->CreateInstance(stageGrassBlas, PorygonInstanceRegister::SHADER_ID_DEF);
	PorygonInstanceRegister::Ins()->AddScale(stageGrassIns, Vec3(200, 200, 200));

	// �S�[���p�̃I�u�W�F�N�g��ǂݍ��ށB
	goalBlas = BLASRegister::Ins()->GenerateObj("Resource/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" });
	goalIns = PorygonInstanceRegister::Ins()->CreateInstance(goalBlas, PorygonInstanceRegister::SHADER_ID_REFRACTION);
	PorygonInstanceRegister::Ins()->AddScale(goalIns, Vec3(200, 200, 200));
	PorygonInstanceRegister::Ins()->AddTrans(goalIns, GOAL_DEF_POS);

	goalCollisionBlas = BLASRegister::Ins()->GenerateObj("Resource/", "goalCollision.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/inv.png" }, false, false);
	goalCollisionIns = PorygonInstanceRegister::Ins()->CreateInstance(goalCollisionBlas, PorygonInstanceRegister::SHADER_ID_INVISIBILITY);
	PorygonInstanceRegister::Ins()->AddScale(goalCollisionIns, Vec3(200, 200, 200));

	middlePointCollisionBlas = BLASRegister::Ins()->GenerateObj("Resource/", "middlePointCollision.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/inv.png" }, false, false);
	middlePointCollisionIns = PorygonInstanceRegister::Ins()->CreateInstance(middlePointCollisionBlas, PorygonInstanceRegister::SHADER_ID_INVISIBILITY);
	PorygonInstanceRegister::Ins()->AddScale(middlePointCollisionIns, Vec3(200, 200, 200));

	// �v���C���[���������B
	Player::StageData stageData;
	stageData.stageBlasIndex = stageBlas;
	stageData.stageInsIndex = stageIns;
	stageData.stageGrassBlasIndex = stageGrassBlas;
	stageData.stageGrassInsIndex = stageGrassIns;
	stageData.middlePointBlasIndex = middlePointCollisionBlas;
	stageData.middlePointInsIndex = middlePointCollisionIns;
	stageData.goalBlasIndex = goalCollisionBlas;
	stageData.goalInsIndex = goalCollisionIns;
	player = std::make_shared<Player>(stageData);

	PorygonInstanceRegister::Ins()->CalWorldMat();

	// TLAS�𐶐��B
	tlas = std::make_shared<TLAS>();
	tlas->GenerateTLAS();

	// AO�o�͗p�N���X���Z�b�g�B
	aoOutput = std::make_shared<RaytracingOutput>();
	aoOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// �F�o�͗p�N���X���Z�b�g�B
	colorOutput = std::make_shared<RaytracingOutput>();
	colorOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// ���邳���o�͗p�N���X���Z�b�g�B
	lightOutput = std::make_shared<RaytracingOutput>();
	lightOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// GI�o�͗p�N���X���Z�b�g�B
	giOutput = std::make_shared<RaytracingOutput>();
	giOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// �ŏI�o�͗p�N���X���Z�b�g�B
	denoiseMixTextureOutput = std::make_shared<RaytracingOutput>();
	denoiseMixTextureOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// �V�F�[�_�[�e�[�u���𐶐��B
	deAOPipline->ConstructionShaderTable();

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

	nextScene = SCENE_ID::RESULT;
	isTransition = false;

	isPassedMiddlePoint = false;
	rapCount = 0;

	// �X�v���C�g�𐶐��B
	nowRapCountSprite = std::make_shared<Sprite>();
	nowRapCountSprite->GenerateSpecifyTextureID(Vec3(95, 80, 0.1f), DirectX::XMFLOAT2(16, 32), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	maxRapCountSprite = std::make_shared<Sprite>();
	maxRapCountSprite->GenerateSpecifyTextureID(Vec3(180, 80, 0.1f), DirectX::XMFLOAT2(16, 32), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	rapSlashSprite = std::make_shared<Sprite>();
	rapSlashSprite->GenerateSpecifyTextureID(Vec3(140, 80, 0.1f), DirectX::XMFLOAT2(16, 32), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);

	// �t�H���g�����[�h
	{
		numFontHandle[0] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/0.png");
		numFontHandle[1] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/1.png");
		numFontHandle[2] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/2.png");
		numFontHandle[3] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/3.png");
		numFontHandle[4] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/4.png");
		numFontHandle[5] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/5.png");
		numFontHandle[6] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/6.png");
		numFontHandle[7] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/7.png");
		numFontHandle[8] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/8.png");
		numFontHandle[9] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/9.png");
		numFontHandle[10] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/slash.png");
	}

}

void GameScene::Init()
{

	/*===== ���������� =====*/

	nextScene = SCENE_ID::RESULT;
	isTransition = false;
	player->Init();
	Camera::Ins()->Init();

	isPassedMiddlePoint = false;
	rapCount = 0;

}

void GameScene::Update()
{

	/*===== �X�V���� =====*/

	// ���͏���
	Input();

	// �E�B���h�E�̖��O���X�V�B
	if (isDisplayFPS) {

		FPS();

	}
	else {

		SetWindowText(DirectXBase::Ins()->windowsAPI.hwnd, L"LE3A_21_�t�i�N���x_�^�N�~");

	}

	// �v���C���[���X�V�B
	player->Update(constBufferData, isPassedMiddlePoint, rapCount);

	// �����̎���X�V�B
	constBufferData.debug.seed = FHelper::GetRand(0, 1000);

	// �J�������X�V�B
	Camera::Ins()->Update(player->GetPos(), player->GetForwardVec(), player->GetUpVec(), player->GetNowSpeedPer());

	// �_�����̈ʒu���X�V�B
	int counter = 0;
	for (auto& index : sphereIns) {
		PorygonInstanceRegister::Ins()->ChangeTrans(index, constBufferData.light.pointLight[counter].lightPos);

		// ���C�g���L��������Ă��Ȃ�������T�C�Y��0�ɂ��ĕ`�悵�Ȃ��B
		if (constBufferData.light.pointLight[counter].isActive) {
			PorygonInstanceRegister::Ins()->ChangeScale(index, constBufferData.light.pointLight[counter].lightSize);
		}
		else {
			PorygonInstanceRegister::Ins()->ChangeTrans(index, Vec3(-100000, -100000, -100000));
		}
		++counter;
	}

	// ���Ԓn�_�ɒB���Ă�����S�[�����ʒu�ɏo���B
	if (isPassedMiddlePoint) {

		PorygonInstanceRegister::Ins()->ChangeTrans(goalIns, GOAL_DEF_POS);
		PorygonInstanceRegister::Ins()->ChangeScale(goalIns, Vec3(200, 200, 200));
		PorygonInstanceRegister::Ins()->ChangeTrans(goalCollisionIns, GOAL_DEF_POS);
		PorygonInstanceRegister::Ins()->ChangeScale(goalCollisionIns, Vec3(200, 200, 200));

	}
	else {

		PorygonInstanceRegister::Ins()->ChangeTrans(goalIns, Vec3(-10000, -10000, -10000));
		PorygonInstanceRegister::Ins()->ChangeScale(goalIns, Vec3(0, 0, 0));
		PorygonInstanceRegister::Ins()->ChangeTrans(goalCollisionIns, Vec3(-10000, -10000, -10000));
		PorygonInstanceRegister::Ins()->ChangeScale(goalCollisionIns, Vec3(0, 0, 0));

	}

	// 3�T���Ă����烊�U���g�V�[���Ɉړ�����B
	if (3 <= rapCount) {

		isTransition = true;

	}

	// ���b�v����UI���X�V�B
	maxRapCountSprite->ChangeTextureID(numFontHandle[3], 0);
	nowRapCountSprite->ChangeTextureID(numFontHandle[rapCount], 0);
	rapSlashSprite->ChangeTextureID(numFontHandle[10], 0);

	// �S�[���I�u�W�F�N�g����]������B
	PorygonInstanceRegister::Ins()->AddRotate(goalIns, Vec3(0.01f, 0, 0));

	tlas->Update();

}

void GameScene::Draw()
{

	/*===== �`�揈�� =====*/

	// �J�����s����X�V�B
	auto frameIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();
	constBufferData.camera.mtxView = Camera::Ins()->matView;
	constBufferData.camera.mtxViewInv = DirectX::XMMatrixInverse(nullptr, constBufferData.camera.mtxView);
	constBufferData.camera.mtxProj = Camera::Ins()->matPerspective;
	constBufferData.camera.mtxProjInv = DirectX::XMMatrixInverse(nullptr, constBufferData.camera.mtxProj);

	// �萔�o�b�t�@���Z�b�g�B
	constBuffer->Write(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex(), &constBufferData, sizeof(constBufferData));

	// �o���A��ݒ肵�e���\�[�X�̏�Ԃ�J�ڂ�����.
	aoOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	lightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	colorOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	giOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// �O���[�o�����[�g�V�O�l�`���Ŏg���Ɛ錾���Ă��郊�\�[�X����Z�b�g�B
	ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	DirectXBase::Ins()->cmdList->SetComputeRootSignature(deAOPipline->GetGlobalRootSig()->GetRootSig().Get());

	// TLAS��ݒ�B
	DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas->GetDescriptorHeapIndex()));

	// �萔�o�b�t�@���Z�b�g
	DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, constBuffer->GetBuffer(frameIndex)->GetGPUVirtualAddress());

	// �o�͗pUAV��ݒ�B
	aoOutput->SetComputeRootDescriptorTalbe(2);
	lightOutput->SetComputeRootDescriptorTalbe(3);
	colorOutput->SetComputeRootDescriptorTalbe(4);
	giOutput->SetComputeRootDescriptorTalbe(5);

	// �p�C�v���C����ݒ�B
	DirectXBase::Ins()->cmdList->SetPipelineState1(deAOPipline->GetStateObject().Get());

	// ���C�g���[�V���O�����s�B
	D3D12_DISPATCH_RAYS_DESC rayDesc = deAOPipline->GetDispatchRayDesc();
	DirectXBase::Ins()->cmdList->DispatchRays(&rayDesc);

	// �f�o�b�O�p�̃p�C�v���C�����f�m�C�Y�p�p�C�v���C����������A�R���s���[�g�V�F�[�_�[���g���ăf�m�C�Y��������B
	if (debugPiplineID == DENOISE_AO_PIPLINE) {

		// [�m�C�Y��`��]�̂Ƃ��̓f�m�C�Y�������Ȃ��B
		if (!constBufferData.debug.isNoiseScene) {

			// �f�o�b�O�@�\��[�@���`��][���b�V���`��][���C�g�ɓ��������_�̂ݕ`��]�̂Ƃ��̓f�m�C�Y�������Ȃ��悤�ɂ���B
			if (!constBufferData.debug.isMeshScene && !constBufferData.debug.isNormalScene && !constBufferData.debug.isLightHitScene) {

				// ���C�g�Ƀf�m�C�Y��������B
				Denoiser::Ins()->Denoise(lightOutput->GetUAVIndex(), 1, 3);

			}

			// [AO���s��Ȃ�]�̂Ƃ��̓f�m�C�Y�������Ȃ��B
			if (!constBufferData.debug.isNoAO) {

				// AO�Ƀf�m�C�Y��������B
				Denoiser::Ins()->Denoise(aoOutput->GetUAVIndex(), 100, 9);

			}

			// [GI���s��Ȃ�]�̂Ƃ��̓f�m�C�Y�������Ȃ��B
			if (!constBufferData.debug.isNoGI) {

				// GI�Ƀf�m�C�Y��������B
				Denoiser::Ins()->Denoise(giOutput->GetUAVIndex(), 100, 5);

			}

		}

		// �f�m�C�Y�����������C�e�B���O���ƐF����������B
		denoiseMixTextureOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		Denoiser::Ins()->MixColorAndLuminance(colorOutput->GetUAVIndex(), aoOutput->GetUAVIndex(), lightOutput->GetUAVIndex(), giOutput->GetUAVIndex(), denoiseMixTextureOutput->GetUAVIndex());
		denoiseMixTextureOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

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
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), lightOutput->GetRaytracingOutput().Get());

	}
	else {

		// �f�m�C�Y���ꂽ�ʏ�̕`��
		colorOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), denoiseMixTextureOutput->GetRaytracingOutput().Get());

	}

	// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�����Ƃɖ߂��B
	D3D12_RESOURCE_BARRIER endBarriers[] = {

	CD3DX12_RESOURCE_BARRIER::Transition(
	DirectXBase::Ins()->backBuffers[backBufferIndex].Get(),
	D3D12_RESOURCE_STATE_COPY_DEST,
	D3D12_RESOURCE_STATE_RENDER_TARGET)

	};

	// �o���A��ݒ肵�e���\�[�X�̏�Ԃ�J�ڂ�����.
	aoOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	lightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	//colorOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	giOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(endBarriers), endBarriers);

	// UI��`��
	nowRapCountSprite->Draw();
	maxRapCountSprite->Draw();
	rapSlashSprite->Draw();

}

#pragma comment (lib, "winmm.lib")

void GameScene::FPS()
{

	/*===== �^�C�g���o�[��FPS�\�� =====*/

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

void GameScene::Input()
{

	/*===== ���͏��� =====*/

	bool isMove = false;

	if (Input::Ins()->isKeyTrigger(DIK_RETURN)) {

		isTransition = true;

	}

	InputImGUI(isMove);

}

void GameScene::InputImGUI(bool& IsMove)
{

	/*===== IMGUI�X�V =====*/

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

	//// PointLight�ɂ���
	//if (ImGui::TreeNode("PointLight")) {

	//	// ���C�g��\�����邩�ǂ����̃t���O���X�V�B
	//	bool isActive = static_cast<bool>(constBufferData.light.pointLight.isActive);
	//	ImGui::Checkbox("IsActive", &isActive);
	//	if (isActive != static_cast<bool>(constBufferData.light.pointLight.isActive)) IsMove = true;
	//	constBufferData.light.pointLight.isActive = static_cast<int>(isActive);

	//	// �l��ۑ�����B
	//	float dirX = constBufferData.light.pointLight.lightPos.x;
	//	float dirY = constBufferData.light.pointLight.lightPos.y;
	//	float dirZ = constBufferData.light.pointLight.lightPos.z;
	//	float lightSize = constBufferData.light.pointLight.lightSize;
	//	float aoSampleCount = static_cast<float>(constBufferData.debug.aoSampleCount);
	//	float pointLightPower = constBufferData.light.pointLight.lightPower;
	//	float MOVE_LENGTH = 1500.0f;
	//	ImGui::SliderFloat("PointLightX", &constBufferData.light.pointLight.lightPos.x, -MOVE_LENGTH, MOVE_LENGTH);
	//	ImGui::SliderFloat("PointLightY", &constBufferData.light.pointLight.lightPos.y, 0.0f, 1000.0f);
	//	ImGui::SliderFloat("PointLightZ", &constBufferData.light.pointLight.lightPos.z, -MOVE_LENGTH, MOVE_LENGTH);
	//	ImGui::SliderFloat("PointLightRadius", &constBufferData.light.pointLight.lightSize, 1.0f, 50.0f);
	//	ImGui::SliderFloat("PointLightPower", &constBufferData.light.pointLight.lightPower, 100.0f, 1000.0f);
	//	ImGui::SliderFloat("AOSampleCount", &aoSampleCount, 1.0f, 30.0f);
	//	constBufferData.debug.aoSampleCount = static_cast<int>(aoSampleCount);

	//	// �ς���Ă�����
	//	if (dirX != constBufferData.light.pointLight.lightPos.x || dirY != constBufferData.light.pointLight.lightPos.y || dirZ != constBufferData.light.pointLight.lightPos.z || lightSize != constBufferData.light.pointLight.lightSize || pointLightPower != constBufferData.light.pointLight.lightPower) {

	//		IsMove = true;

	//	}

	//	// ���C�g�̐F��ݒ�B
	//	std::array<float, 3> lightColor = { constBufferData.light.pointLight.lightColor.x,constBufferData.light.pointLight.lightColor.y,constBufferData.light.pointLight.lightColor.z };
	//	ImGui::ColorPicker3("LightColor", lightColor.data());
	//	// �F���ς���Ă�����B
	//	if (lightColor[0] != constBufferData.light.pointLight.lightColor.x || lightColor[1] != constBufferData.light.pointLight.lightColor.y || lightColor[2] != constBufferData.light.pointLight.lightColor.z) {
	//		IsMove = true;
	//	}
	//	constBufferData.light.pointLight.lightColor.x = lightColor[0];
	//	constBufferData.light.pointLight.lightColor.y = lightColor[1];
	//	constBufferData.light.pointLight.lightColor.z = lightColor[2];

	//	ImGui::TreePop();

	//}


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
