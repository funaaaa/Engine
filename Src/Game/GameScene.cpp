#include "GameScene.h"
#include "HitGroupMgr.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
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
#include "WindowsAPI.h"
#include "GimmickMgr.h"

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
	pipline->Setting(dAOuseShaders, HitGroupMgr::DENOISE_AO_HIT_GROUP, 1, 1, 5, sizeof(Vec3) * 5 + sizeof(UINT) + sizeof(UINT), sizeof(Vec2), 6);

	// �V���p�̃X�t�B�A�𐶐�����B
	skyDomeBlas = BLASRegister::Ins()->GenerateObj("Resource/Game/", "skydome.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/skydome.jpg" });
	skyDomeIns = PolygonInstanceRegister::Ins()->CreateInstance(skyDomeBlas, PolygonInstanceRegister::SHADER_ID::AS);
	PolygonInstanceRegister::Ins()->AddScale(skyDomeIns, Vec3(1000, 1000, 1000));

	// �X�e�[�W��ǂݍ��ށB
	stageBlas = BLASRegister::Ins()->GenerateObj("Resource/Game/", "stage3.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/white.png" });
	stageIns = PolygonInstanceRegister::Ins()->CreateInstance(stageBlas, PolygonInstanceRegister::DEF_GI);
	PolygonInstanceRegister::Ins()->AddScale(stageIns, Vec3(200, 200, 200));
	stageGrassBlas = BLASRegister::Ins()->GenerateObj("Resource/Game/", "grass.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/green.png",L"Resource/Game/grassNormal.png" });
	stageGrassIns = PolygonInstanceRegister::Ins()->CreateInstance(stageGrassBlas, PolygonInstanceRegister::DEF_GI);
	PolygonInstanceRegister::Ins()->AddScale(stageGrassIns, Vec3(200, 200, 200));



	// �S�[���p�̃I�u�W�F�N�g��ǂݍ��ށB
	goalBlas = BLASRegister::Ins()->GenerateObj("Resource/Game/", "goal.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" });
	goalIns = PolygonInstanceRegister::Ins()->CreateInstance(goalBlas, PolygonInstanceRegister::SHADER_ID::REFRACTION);
	PolygonInstanceRegister::Ins()->AddScale(goalIns, Vec3(200, 200, 200));
	PolygonInstanceRegister::Ins()->AddTrans(goalIns, GOAL_DEF_POS);

	goalCollisionBlas = BLASRegister::Ins()->GenerateObj("Resource/Game/", "goalCollision.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/inv.png" }, false, false);
	goalCollisionIns = PolygonInstanceRegister::Ins()->CreateInstance(goalCollisionBlas, PolygonInstanceRegister::SHADER_ID::INVISIBILITY);
	PolygonInstanceRegister::Ins()->AddScale(goalCollisionIns, Vec3(200, 200, 200));

	middlePointCollisionBlas = BLASRegister::Ins()->GenerateObj("Resource/Game/", "middlePointCollision.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/inv.png" }, false, false);
	middlePointCollisionIns = PolygonInstanceRegister::Ins()->CreateInstance(middlePointCollisionBlas, PolygonInstanceRegister::SHADER_ID::INVISIBILITY);
	PolygonInstanceRegister::Ins()->AddScale(middlePointCollisionIns, Vec3(200, 200, 200));

	// �X�e�[�W�̑����I�u�W�F�N�g�����[�h
	{
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockA.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/gray.png" }));
		stageOrnamentIns.emplace_back(PolygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PolygonInstanceRegister::SHADER_ID::REFLECTION));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockB.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/gray.png" }));
		stageOrnamentIns.emplace_back(PolygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PolygonInstanceRegister::SHADER_ID::REFLECTION));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockC.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/green.png" }));
		stageOrnamentIns.emplace_back(PolygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PolygonInstanceRegister::SHADER_ID::REFRACTION));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockD.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" }));
		stageOrnamentIns.emplace_back(PolygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PolygonInstanceRegister::SHADER_ID::REFRACTION));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockE.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/green.png" }));
		stageOrnamentIns.emplace_back(PolygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PolygonInstanceRegister::SHADER_ID::REFRACTION));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "blockF.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" }));
		stageOrnamentIns.emplace_back(PolygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PolygonInstanceRegister::SHADER_ID::DEF));
		stageOrnamentBlas.emplace_back(BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "goalSideObj.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/gray.png" }));
		stageOrnamentIns.emplace_back(PolygonInstanceRegister::Ins()->CreateInstance(stageOrnamentBlas[static_cast<int>(stageOrnamentBlas.size()) - 1], PolygonInstanceRegister::SHADER_ID::DEF));
	}
	for (auto& index : stageOrnamentIns) {

		PolygonInstanceRegister::Ins()->AddScale(index, Vec3(200, 200, 200));

	}

	// �M�~�b�N�����[�h�B
	GenerateGimmick();

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
	stageData.stageOrnamentBlasIndex = stageOrnamentBlas;
	stageData.stageOrnamentInsIndex = stageOrnamentIns;
	player = std::make_shared<Player>(stageData);

	// �S�[���O�̂ӂ�ӂ킵�Ă���I�u�W�F�N�g��BLAS�����[�h
	beforeTheGoalObjectBlas = BLASRegister::Ins()->GenerateObj("Resource/Game/stageOrnament/", "beforeTheGoalBox.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], { L"Resource/Game/red.png" });
	for (int index = 0; index < 4; ++index) {

		std::pair<int, int> buff;
		std::pair<Vec3, Vec3> defPosBuff;
		buff.first = PolygonInstanceRegister::Ins()->CreateInstance(beforeTheGoalObjectBlas, PolygonInstanceRegister::DEF);
		defPosBuff.first = Vec3(250, 200, 1000 * index + 1500.0f);
		PolygonInstanceRegister::Ins()->AddTrans(buff.first, defPosBuff.first);
		PolygonInstanceRegister::Ins()->AddScale(buff.first, Vec3(40.0f, 40.0f, 40.0f));
		PolygonInstanceRegister::Ins()->AddRotate(buff.first, Vec3(DirectX::XM_2PI / 2.0f * index, DirectX::XM_2PI / 2.0f * index, 0));
		buff.second = PolygonInstanceRegister::Ins()->CreateInstance(beforeTheGoalObjectBlas, PolygonInstanceRegister::DEF);
		defPosBuff.second = Vec3(-250, 200, 1000 * index + 1500.0f);
		PolygonInstanceRegister::Ins()->AddTrans(buff.second, defPosBuff.second);
		PolygonInstanceRegister::Ins()->AddScale(buff.second, Vec3(40.0f, 40.0f, 40.0f));
		PolygonInstanceRegister::Ins()->AddRotate(buff.second, Vec3(DirectX::XM_2PI / 2.0f * index, DirectX::XM_2PI / 2.0f * index, 0));
		beforeTheGoalObjectIns.emplace_back(buff);
		beforeTheGoalObjectDefPos.emplace_back(defPosBuff);
		beforeTheGoalObjectTimer.push_back(1.0f * index);

	}

	// Instance�̃��[���h�s��𐶐��B
	PolygonInstanceRegister::Ins()->CalWorldMat();

	// TLAS�𐶐��B
	tlas = std::make_shared<TLAS>();
	tlas->GenerateTLAS();

	// AO�o�͗p�N���X���Z�b�g�B
	aoOutput = std::make_shared<RaytracingOutput>();
	aoOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"AOOutput");
	denoiseAOOutput = std::make_shared<RaytracingOutput>();
	denoiseAOOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseAOOutput");

	// �F�o�͗p�N���X���Z�b�g�B
	colorOutput = std::make_shared<RaytracingOutput>();
	colorOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"ColorOutput");

	// ���邳���o�͗p�N���X���Z�b�g�B
	lightOutput = std::make_shared<RaytracingOutput>();
	lightOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"LightOutput");
	denoiseLightOutput = std::make_shared<RaytracingOutput>();
	denoiseLightOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseLightOutput");

	// GI�o�͗p�N���X���Z�b�g�B
	giOutput = std::make_shared<RaytracingOutput>();
	giOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"GIOutput");
	denoiseGiOutput = std::make_shared<RaytracingOutput>();
	denoiseGiOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseGIOutput");

	// �f�m�C�Y�}�X�N�p�N���X���Z�b�g�B
	denoiseMaskOutput = std::make_shared<RaytracingOutput>();
	denoiseMaskOutput->Setting(DXGI_FORMAT_R32G32B32A32_FLOAT, L"DenoiseMaskOutput");

	// �ŏI�o�͗p�N���X���Z�b�g�B
	denoiseMixTextureOutput = std::make_shared<RaytracingOutput>();
	denoiseMixTextureOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseMixTextureOutput");

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

		SetWindowText(DirectXBase::Ins()->windowsAPI->hwnd, L"LE3A_21_�t�i�N���x_�^�N�~");

	}

	// ���Ԓn�_�̃I�u�W�F�N�g�����̑傫���ɖ߂��B AO�̉e�łĂ��܂��o�O�̑΍��p�B�V�F�[�_�[���ŕς��鎞�Ԃ��Ȃ������̂ŗՎ��ł��炵�Ă܂��I
	PolygonInstanceRegister::Ins()->ChangeScale(middlePointCollisionIns, Vec3(200, 200, 200));
	PolygonInstanceRegister::Ins()->ChangeTrans(middlePointCollisionIns, Vec3(0, 0, 0));

	// �v���C���[���X�V�B
	player->Update(constBufferData, isPassedMiddlePoint, rapCount);

	// �����̎���X�V�B
	constBufferData.debug.seed = FHelper::GetRand(0, 1000);

	// �J�������X�V�B
	Camera::Ins()->Update(player->GetPos(), player->GetForwardVec(), player->GetUpVec(), player->GetNowSpeedPer());


	// ���Ԓn�_�ɒB���Ă�����S�[�����ʒu�ɏo���B
	if (isPassedMiddlePoint) {

		PolygonInstanceRegister::Ins()->ChangeTrans(goalIns, GOAL_DEF_POS);
		PolygonInstanceRegister::Ins()->ChangeScale(goalIns, Vec3(200, 200, 200));
		PolygonInstanceRegister::Ins()->ChangeTrans(goalCollisionIns, GOAL_DEF_POS);
		PolygonInstanceRegister::Ins()->ChangeScale(goalCollisionIns, Vec3(200, 200, 200));

	}
	else {

		PolygonInstanceRegister::Ins()->ChangeTrans(goalIns, Vec3(0, -100, 0));
		PolygonInstanceRegister::Ins()->ChangeScale(goalIns, Vec3(0, 0, 0));
		PolygonInstanceRegister::Ins()->ChangeTrans(goalCollisionIns, Vec3(0, -100, 0));
		PolygonInstanceRegister::Ins()->ChangeScale(goalCollisionIns, Vec3(0, 0, 0));

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
	PolygonInstanceRegister::Ins()->AddRotate(goalIns, Vec3(0.01f, 0, 0));

	// �S�[���O�̃I�u�W�F�N�g����]������B
	static const int BEFORE_THE_GOAL_OBJECT_COUNT = 4;
	for (int index = 0; index < BEFORE_THE_GOAL_OBJECT_COUNT; ++index) {

		const float SIN_WAVE_MOVE = 50.0f;

		PolygonInstanceRegister::Ins()->AddRotate(beforeTheGoalObjectIns[index].first, Vec3(0.01f, 0.01f, 0));
		PolygonInstanceRegister::Ins()->AddRotate(beforeTheGoalObjectIns[index].second, Vec3(0.01f, 0.01f, 0));
		PolygonInstanceRegister::Ins()->ChangeTrans(beforeTheGoalObjectIns[index].first, beforeTheGoalObjectDefPos[index].first + Vec3(0, sinf(beforeTheGoalObjectTimer[index]) * SIN_WAVE_MOVE, 0));
		PolygonInstanceRegister::Ins()->ChangeTrans(beforeTheGoalObjectIns[index].second, beforeTheGoalObjectDefPos[index].second + Vec3(0, sinf(beforeTheGoalObjectTimer[index]) * SIN_WAVE_MOVE, 0));

		beforeTheGoalObjectTimer[index] += 0.05f;

	}

	// ���Ԓn�_�̃I�u�W�F�N�g������������B AO�̉e�łĂ��܂��o�O�̑΍��p�B�V�F�[�_�[���ŕς��鎞�Ԃ��Ȃ������̂ŗՎ��ŏ��������Ă��܂��I
	PolygonInstanceRegister::Ins()->ChangeScale(middlePointCollisionIns, Vec3(0, 0, 0));
	PolygonInstanceRegister::Ins()->ChangeTrans(middlePointCollisionIns, Vec3(0, -100, 0));


	tlas->Update();

	// ���z�̊p�x���X�V�B
	sunAngle += sunSpeed;
	constBufferData.light.dirLight.lihgtDir = Vec3(-cos(sunAngle), -sin(sunAngle), 0.5f);
	constBufferData.light.dirLight.lihgtDir.Normalize();
	// �V�����̂���]������B
	PolygonInstanceRegister::Ins()->AddRotate(skyDomeIns, Vec3(0.001f, 0, 0));

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

	// �o���A��ݒ肵�e���\�[�X�̏�Ԃ�J�ڂ�����.
	aoOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseAOOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	lightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseLightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	colorOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	giOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseGiOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseMaskOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

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
				lightOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseLightOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseMaskOutput->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList->ResourceBarrier(3, barrierToUAV);

			// ���C�g�Ƀf�m�C�Y��������B
			Denoiser::Ins()->Denoise(lightOutput->GetUAVIndex(), denoiseLightOutput->GetUAVIndex(), denoiseMaskOutput->GetUAVIndex(), 1, 1);

		}

		// AO���Ƀf�m�C�Y��������B
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				aoOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseAOOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseMaskOutput->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList->ResourceBarrier(3, barrierToUAV);

			// AO�Ƀf�m�C�Y��������B
			Denoiser::Ins()->Denoise(aoOutput->GetUAVIndex(), denoiseAOOutput->GetUAVIndex(), denoiseMaskOutput->GetUAVIndex(), 100, 6);
		}


		// GI���Ƀf�m�C�Y��������B
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				giOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseGiOutput->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseMaskOutput->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList->ResourceBarrier(3, barrierToUAV);

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

	// �o���A��ݒ肵�e���\�[�X�̏�Ԃ�J�ڂ�����.
	aoOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseAOOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	lightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseLightOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	colorOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	giOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseGiOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseMaskOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

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
		SetWindowText(DirectXBase::Ins()->windowsAPI->hwnd, fps);
		//OutputDebugString(fps);

		prev_time = now_time;
		frame_count = 0;
	}

}

void GameScene::Input()
{

	/*===== ���͏��� =====*/

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

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

	// 1��
	GimmickMgr::Ins()->ChangeTrans(0, Vec3(100, -15, 1400));
	GimmickMgr::Ins()->ChangeRotate(0, Vec3(0, 0, 0));
	GimmickMgr::Ins()->ChangeScale(0, Vec3(100, 200, 200));

	// 2��
	GimmickMgr::Ins()->ChangeTrans(1, Vec3(-80, -15, 3000));
	GimmickMgr::Ins()->ChangeRotate(1, Vec3(0, 0, 0));
	GimmickMgr::Ins()->ChangeScale(1, Vec3(100, 200, 200));

	// 3��
	GimmickMgr::Ins()->ChangeTrans(2, Vec3(100, -15, 4000));
	GimmickMgr::Ins()->ChangeRotate(2, Vec3(0, 0, 0));
	GimmickMgr::Ins()->ChangeScale(2, Vec3(100, 200, 200));

	// 4��
	GimmickMgr::Ins()->ChangeTrans(3, Vec3(-5842, -29, -167));
	GimmickMgr::Ins()->ChangeRotate(3, Vec3(0, 1.48f, 0));
	GimmickMgr::Ins()->ChangeScale(3, Vec3(200, 200, 200));

	// 5��
	GimmickMgr::Ins()->ChangeTrans(4, Vec3(-2352, -18, 6336));
	GimmickMgr::Ins()->ChangeRotate(4, Vec3(0, 1.58f, 0));
	GimmickMgr::Ins()->ChangeScale(4, Vec3(190, 200, 200));


}

void GameScene::GenerateGimmick()
{

	// 1��
	GimmickMgr::Ins()->AddGimmick(BaseGimmick::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], PolygonInstanceRegister::SHADER_ID::DEF);

	// 2��
	GimmickMgr::Ins()->AddGimmick(BaseGimmick::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], PolygonInstanceRegister::SHADER_ID::DEF);
	
	// 3��
	GimmickMgr::Ins()->AddGimmick(BaseGimmick::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], PolygonInstanceRegister::SHADER_ID::DEF);
	
	// 4��
	GimmickMgr::Ins()->AddGimmick(BaseGimmick::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], PolygonInstanceRegister::SHADER_ID::DEF);
	
	// 5��
	GimmickMgr::Ins()->AddGimmick(BaseGimmick::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DENOISE_AO_HIT_GROUP], PolygonInstanceRegister::SHADER_ID::DEF);
	
}
