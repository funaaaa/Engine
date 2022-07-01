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
	pipline = std::make_shared<RaytracingPipline>();
	pipline->Setting(dAOuseShaders, HitGroupMgr::DENOISE_AO_HIT_GROUP, 1, 1, 5, sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(UINT) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// SPONZA��ǂݍ��ށB
	//sponzaInstance = MultiMeshLoadOBJ::Ins()->RayMultiMeshLoadOBJ("Resource/", "sponza.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP]);

	// �V���p�̃X�t�B�A�𐶐�����B
	skyDomeBlas = BLASRegister::Ins()->GenerateObj("Resource/", "skydome.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/skydome.jpg" });
	skyDomeIns = PorygonInstanceRegister::Ins()->CreateInstance(skyDomeBlas, PorygonInstanceRegister::SHADER_ID_AS);
	PorygonInstanceRegister::Ins()->AddScale(skyDomeIns, Vec3(1000, 1000, 1000));

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
	stageIns = PorygonInstanceRegister::Ins()->CreateInstance(stageBlas, PorygonInstanceRegister::SHADER_ID_DEF_GI);
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

	// �X�e�[�W�̑����I�u�W�F�N�g�����[�h
	{
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockA.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }));
		stageOrnamentIns.emplace_back(PorygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PorygonInstanceRegister::SHADER_ID_DEF));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockB.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }));
		stageOrnamentIns.emplace_back(PorygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PorygonInstanceRegister::SHADER_ID_DEF));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockC.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }));
		stageOrnamentIns.emplace_back(PorygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PorygonInstanceRegister::SHADER_ID_DEF));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockD.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }));
		stageOrnamentIns.emplace_back(PorygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PorygonInstanceRegister::SHADER_ID_DEF));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockE.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }));
		stageOrnamentIns.emplace_back(PorygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PorygonInstanceRegister::SHADER_ID_DEF));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockF.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }));
		stageOrnamentIns.emplace_back(PorygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PorygonInstanceRegister::SHADER_ID_DEF));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "goalSideObj.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/red.png" }));
		stageOrnamentIns.emplace_back(PorygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PorygonInstanceRegister::SHADER_ID_DEF));
	}
	for (auto& index : stageOrnamentIns) {

		PorygonInstanceRegister::Ins()->AddScale(index, Vec3(200, 200, 200));

	}

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
	denoiseAOOutput = std::make_shared<RaytracingOutput>();
	denoiseAOOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// �F�o�͗p�N���X���Z�b�g�B
	colorOutput = std::make_shared<RaytracingOutput>();
	colorOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// ���邳���o�͗p�N���X���Z�b�g�B
	lightOutput = std::make_shared<RaytracingOutput>();
	lightOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);
	denoiseLightOutput = std::make_shared<RaytracingOutput>();
	denoiseLightOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// GI�o�͗p�N���X���Z�b�g�B
	giOutput = std::make_shared<RaytracingOutput>();
	giOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);
	denoiseGiOutput = std::make_shared<RaytracingOutput>();
	denoiseGiOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// �f�m�C�Y�}�X�N�p�N���X���Z�b�g�B
	denoiseMaskOutput = std::make_shared<RaytracingOutput>();
	denoiseMaskOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// �ŏI�o�͗p�N���X���Z�b�g�B
	denoiseMixTextureOutput = std::make_shared<RaytracingOutput>();
	denoiseMixTextureOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// �V�F�[�_�[�e�[�u���𐶐��B
	pipline->ConstructionShaderTable();

	// ���z�Ɋւ���ϐ�
	sunAngle = 0;
	sunSpeed = 0.0001f;

	isDisplayFPS = false;

	nextScene = SCENE_ID::RESULT;
	isTransition = false;

	isPassedMiddlePoint = false;
	rapCount = 0;

	// �X�v���C�g�𐶐��B
	nowRapCountSprite = std::make_shared<Sprite>();
	nowRapCountSprite->GenerateSpecifyTextureID(Vec3(95, 80, 0.1f), Vec2(16.0f, 32.0f), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	maxRapCountSprite = std::make_shared<Sprite>();
	maxRapCountSprite->GenerateSpecifyTextureID(Vec3(180, 80, 0.1f), Vec2(16.0f, 32.0f), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	rapSlashSprite = std::make_shared<Sprite>();
	rapSlashSprite->GenerateSpecifyTextureID(Vec3(140, 80, 0.1f), Vec2(16.0f, 32.0f), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);

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

	// �o���A��ݒ肵�e���\�[�X�̏�Ԃ�J�ڂ�����.
	aoOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseAOOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	lightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseLightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	colorOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	giOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseGiOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseMaskOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

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
	sunAngle = 0;

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

	// ���z�̊p�x���X�V�B
	sunAngle += sunSpeed;
	constBufferData.light.dirLight.lihgtDir = Vec3(-cos(sunAngle), -sin(sunAngle), 0.5f);
	constBufferData.light.dirLight.lihgtDir.Normalize();
	// �V�����̂���]������B
	PorygonInstanceRegister::Ins()->AddRotate(skyDomeIns, Vec3(0.001f, 0, 0));

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


	// �O���[�o�����[�g�V�O�l�`���Ŏg���Ɛ錾���Ă��郊�\�[�X����Z�b�g�B
	ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	DirectXBase::Ins()->cmdList->SetComputeRootSignature(pipline->GetGlobalRootSig()->GetRootSig().Get());

	// TLAS��ݒ�B
	DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas->GetDescriptorHeapIndex()));

	// �萔�o�b�t�@���Z�b�g
	DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, constBuffer->GetBuffer(frameIndex)->GetGPUVirtualAddress());

	// �o�͗pUAV��ݒ�B
	aoOutput->SetComputeRootDescriptorTalbe(2);		// AO�̌��ʏo�͗p
	lightOutput->SetComputeRootDescriptorTalbe(3);	// ���C�g�̖��邳�̌��ʏo�͗p
	colorOutput->SetComputeRootDescriptorTalbe(4);	// �e�N�X�`���̐F���o�͗p
	giOutput->SetComputeRootDescriptorTalbe(5);		// gi�̌��ʏo�͗p
	denoiseMaskOutput->SetComputeRootDescriptorTalbe(6);// �f�m�C�Y������ۂ̃}�X�N�o�͗p

	// �p�C�v���C����ݒ�B
	DirectXBase::Ins()->cmdList->SetPipelineState1(pipline->GetStateObject().Get());

	// ���C�g���[�V���O�����s�B
	D3D12_DISPATCH_RAYS_DESC rayDesc = pipline->GetDispatchRayDesc();
	DirectXBase::Ins()->cmdList->DispatchRays(&rayDesc);

	// [�m�C�Y��`��]�̂Ƃ��̓f�m�C�Y�������Ȃ��B
	if (!constBufferData.debug.isNoiseScene) {

		// �f�o�b�O�@�\��[�@���`��][���b�V���`��][���C�g�ɓ��������_�̂ݕ`��]�̂Ƃ��̓f�m�C�Y�������Ȃ��悤�ɂ���B
		if (!constBufferData.debug.isMeshScene && !constBufferData.debug.isNormalScene && !constBufferData.debug.isLightHitScene) {

			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				lightOutput->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList->ResourceBarrier(1, barrierToUAV);

			// ���C�g�Ƀf�m�C�Y��������B
			Denoiser::Ins()->Denoise(lightOutput->GetUAVIndex(), denoiseLightOutput->GetUAVIndex(), denoiseMaskOutput->GetUAVIndex(), 1, 1);

		}

		// AO���Ƀf�m�C�Y��������B
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				aoOutput->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList->ResourceBarrier(1, barrierToUAV);

			// AO�Ƀf�m�C�Y��������B
			Denoiser::Ins()->Denoise(aoOutput->GetUAVIndex(), denoiseAOOutput->GetUAVIndex(), denoiseMaskOutput->GetUAVIndex(), 100, 6);
		}


		// GI���Ƀf�m�C�Y��������B
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				giOutput->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList->ResourceBarrier(1, barrierToUAV);

			// GI�Ƀf�m�C�Y��������B
			Denoiser::Ins()->Denoise(giOutput->GetUAVIndex(), denoiseGiOutput->GetUAVIndex(), denoiseMaskOutput->GetUAVIndex(), 100, 1);

		}

		denoiseMixTextureOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			colorOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseAOOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseLightOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseGiOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseMixTextureOutput->GetRaytracingOutput().Get())
		};

		DirectXBase::Ins()->cmdList->ResourceBarrier(5, barrierToUAV);

		// �f�m�C�Y�����������C�e�B���O���ƐF����������B
		Denoiser::Ins()->MixColorAndLuminance(colorOutput->GetUAVIndex(), denoiseAOOutput->GetUAVIndex(), denoiseLightOutput->GetUAVIndex(), denoiseGiOutput->GetUAVIndex(), denoiseMixTextureOutput->GetUAVIndex());
		denoiseMixTextureOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	}
	// �f�m�C�Y���Ȃ��f�o�b�O��Ԃ̏ꍇ�́A���C�g���֐�����o�͂��ꂽ���̒l����������B
	else {


		denoiseMixTextureOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			colorOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			aoOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			lightOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			giOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseMixTextureOutput->GetRaytracingOutput().Get())
		};

		DirectXBase::Ins()->cmdList->ResourceBarrier(5, barrierToUAV);

		// �f�m�C�Y�����������C�e�B���O���ƐF����������B
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
		lightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), lightOutput->GetRaytracingOutput().Get());
		lightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	}
	else {

		// �f�m�C�Y���ꂽ�ʏ�̕`��
		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[backBufferIndex].Get(), denoiseMixTextureOutput->GetRaytracingOutput().Get());


	}

	// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�����Ƃɖ߂��B
	D3D12_RESOURCE_BARRIER endBarriers[] = {

	CD3DX12_RESOURCE_BARRIER::Transition(
	DirectXBase::Ins()->backBuffers[backBufferIndex].Get(),
	D3D12_RESOURCE_STATE_COPY_DEST,
	D3D12_RESOURCE_STATE_RENDER_TARGET)

	};

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

	if (Input::Ins()->isKeyTrigger(DIK_RETURN)) {

		isTransition = true;

	}

	InputImGUI();

}

void GameScene::InputImGUI()
{

	/*===== IMGUI�X�V =====*/

	// ���z�̈ړ����x���X�V�B
	ImGui::SliderFloat("Sun Speed", &sunSpeed, 0.0f, 0.1f, "%.5f");

	// ���b�V����\������B
	bool isMesh = constBufferData.debug.isMeshScene;
	ImGui::Checkbox("Mesh Scene", &isMesh);
	constBufferData.debug.isMeshScene = isMesh;

	// �@����\������B
	bool isNormal = constBufferData.debug.isNormalScene;
	ImGui::Checkbox("Normal Scene", &isNormal);
	constBufferData.debug.isNormalScene = isNormal;

	// ���C�g�����������ʂ����\������t���O���X�V�B
	bool isLightHit = constBufferData.debug.isLightHitScene;
	ImGui::Checkbox("LightHit Scene", &isLightHit);
	constBufferData.debug.isLightHitScene = isLightHit;

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


}
