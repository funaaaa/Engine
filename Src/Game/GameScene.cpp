#include "GameScene.h"
#include "HitGroupMgr.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "Character.h"
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
#include "CircuitStage.h"
#include "RayComputeShader.h"
#include "StageObjectMgr.h"
#include "BLAS.h"
#include "ShellObjectMgr.h"
#include "CharacterMgr.h"
#include "GameSceneMode.h"
#include "DriftParticleMgr.h"

GameScene::GameScene()
{

	/*===== ���������� =====*/

	// �b���I�u�W�F�N�g���Z�b�e�B���O�B
	ShellObjectMgr::Ins()->Setting();

	// �萔�o�b�t�@�𐶐��B
	constBufferData_.Init();
	constBuffer_ = std::make_shared<DynamicConstBuffer>();
	constBuffer_->Generate(sizeof(RayConstBufferData), L"CameraConstBuffer");
	constBuffer_->Write(DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex(), &constBufferData_, sizeof(RayConstBufferData));

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	dAOuseShaders_.push_back({ "Resource/ShaderFiles/RayTracing/DenoiseAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	int payloadSize = sizeof(float) * 4 + sizeof(Vec3) * 4 + sizeof(int) * 2 + sizeof(Vec2);
	pipline_ = std::make_shared<RaytracingPipline>();
	pipline_->Setting(dAOuseShaders_, HitGroupMgr::DEF, 1, 1, 5, payloadSize, sizeof(Vec2), 6);

	// �^�C�����p�N���X���Z�b�g�B
	tireMaskTexture_ = std::make_shared<RaytracingOutput>();
	tireMaskTexture_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"TireMaskTexture", Vec2(4096, 4096), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	tireMaskTextureOutput_ = std::make_shared<RaytracingOutput>();
	tireMaskTextureOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"TireMaskTexture", Vec2(4096, 4096), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	tireMaskComputeShader_ = std::make_shared<RayComputeShader>();
	tireMaskComputeShader_->Setting(L"Resource/ShaderFiles/RayTracing/TireMaskComputeShader.hlsl", 0, 1, 1, { tireMaskTextureOutput_->GetUAVIndex() });
	tireMaskConstBuffer_ = std::make_shared<DynamicConstBuffer>();
	tireMaskConstBuffer_->Generate(sizeof(Character::TireMaskUV) * 2, L"TireMaskUV");

	// ���h��R���s���[�g�V�F�[�_�[
	whiteOutComputeShader_ = std::make_shared<RayComputeShader>();
	whiteOutComputeShader_->Setting(L"Resource/ShaderFiles/WhiteMakeUpShader.hlsl", 0, 0, 0, {});

	// �X�e�[�W���Z�b�e�B���O����B
	stages_.emplace_back(std::make_shared<CircuitStage>());

	// ��U�T�[�L�b�g�X�e�[�W��L��������B
	stages_[STAGE_ID::CIRCUIT]->Setting(tireMaskTexture_->GetUAVIndex());

	// �V���p�̃X�t�B�A�𐶐�����B
	skyDomeBlas_ = BLASRegister::Ins()->GenerateObj("Resource/Game/", "skydome.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], { L"Resource/Game/skydome.jpg" });
	skyDomeIns_ = PolygonInstanceRegister::Ins()->CreateInstance(skyDomeBlas_, PolygonInstanceRegister::SHADER_ID::AS);
	PolygonInstanceRegister::Ins()->AddScale(skyDomeIns_, Vec3(1000, 1000, 1000));

	characterMgr_ = std::make_shared<CharacterMgr>();

	// Instance�̃��[���h�s��𐶐��B
	PolygonInstanceRegister::Ins()->CalWorldMat();

	// �ݒ�
	DriftParticleMgr::Ins()->Setting();

	// TLAS�𐶐��B
	tlas_ = std::make_shared<TLAS>();
	tlas_->GenerateTLAS();

	// AO�o�͗p�N���X���Z�b�g�B
	aoOutput_ = std::make_shared<RaytracingOutput>();
	aoOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"AOOutput");
	denoiseAOOutput_ = std::make_shared<RaytracingOutput>();
	denoiseAOOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseAOOutput");

	// �F�o�͗p�N���X���Z�b�g�B
	colorOutput_ = std::make_shared<RaytracingOutput>();
	colorOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"ColorOutput");

	// ���邳���o�͗p�N���X���Z�b�g�B
	lightOutput_ = std::make_shared<RaytracingOutput>();
	lightOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"LightOutput");
	denoiseLightOutput_ = std::make_shared<RaytracingOutput>();
	denoiseLightOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseLightOutput");

	// GI�o�͗p�N���X���Z�b�g�B
	giOutput_ = std::make_shared<RaytracingOutput>();
	giOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"GIOutput");
	denoiseGiOutput_ = std::make_shared<RaytracingOutput>();
	denoiseGiOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseGIOutput");

	// �f�m�C�Y�}�X�N�p�N���X���Z�b�g�B
	denoiseMaskOutput_ = std::make_shared<RaytracingOutput>();
	denoiseMaskOutput_->Setting(DXGI_FORMAT_R32G32B32A32_FLOAT, L"DenoiseMaskOutput");

	// �ŏI�o�͗p�N���X���Z�b�g�B
	denoiseMixTextureOutput_ = std::make_shared<RaytracingOutput>();
	denoiseMixTextureOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseMixTextureOutput");

	// �V�F�[�_�[�e�[�u���𐶐��B
	pipline_->ConstructionShaderTable();

	// ���z�Ɋւ���ϐ�
	sunAngle_ = 0;
	sunSpeed_ = 0.0001f;

	isDisplayFPS_ = false;

	nextScene_ = SCENE_ID::RESULT;
	isTransition_ = false;

	isPassedMiddlePoint_ = false;
	rapCount_ = 0;

	// �X�v���C�g�𐶐��B
	nowRapCountSprite_ = std::make_shared<Sprite>();
	nowRapCountSprite_->GenerateSpecifyTextureID(Vec3(95, 80, 0.1f), Vec2(16.0f, 32.0f), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	maxRapCountSprite_ = std::make_shared<Sprite>();
	maxRapCountSprite_->GenerateSpecifyTextureID(Vec3(180, 80, 0.1f), Vec2(16.0f, 32.0f), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);
	rapSlashSprite_ = std::make_shared<Sprite>();
	rapSlashSprite_->GenerateSpecifyTextureID(Vec3(140, 80, 0.1f), Vec2(16.0f, 32.0f), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, 0);

	// �t�H���g�����[�h
	{
		numFontHandle_[0] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/0.png");
		numFontHandle_[1] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/1.png");
		numFontHandle_[2] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/2.png");
		numFontHandle_[3] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/3.png");
		numFontHandle_[4] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/4.png");
		numFontHandle_[5] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/5.png");
		numFontHandle_[6] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/6.png");
		numFontHandle_[7] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/7.png");
		numFontHandle_[8] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/8.png");
		numFontHandle_[9] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/9.png");
		numFontHandle_[10] = TextureManager::Ins()->LoadTexture(L"Resource/Game/Font/slash.png");
	}

}

void GameScene::Init()
{

	/*===== ���������� =====*/

	nextScene_ = SCENE_ID::RESULT;
	isTransition_ = false;


	characterMgr_->Init();

	if (GameSceneMode::Ins()->id_ == GameSceneMode::MODE_ID::AI) {

		// �v���C���[�𐶐��B
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::P1), true);

		// AI�𐶐��B
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::AI1), false);

	}
	else if (GameSceneMode::Ins()->id_ == GameSceneMode::MODE_ID::DEF) {

		// �v���C���[�𐶐��B
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::P1), true);

	}
	else if (GameSceneMode::Ins()->id_ == GameSceneMode::MODE_ID::WRITE_GHOST) {

		// �v���C���[�𐶐��B
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::P1_WGHOST), true);

	}
	else if (GameSceneMode::Ins()->id_ == GameSceneMode::MODE_ID::GHOST) {

		// �v���C���[�𐶐��B
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::P1), true);

		// �S�[�X�g�𐶐��B
		characterMgr_->AddChara(static_cast<int>(Character::CHARA_ID::GHOST), false);

	}


	Camera::Ins()->Init();

	DriftParticleMgr::Ins()->Init();

	isPassedMiddlePoint_ = false;
	rapCount_ = 0;
	sunAngle_ = 0;

}

void GameScene::Update()
{

	/*===== �X�V���� =====*/

	// ���͏���
	Input();

	// �E�B���h�E�̖��O���X�V�B
	if (isDisplayFPS_) {

		FPS();

	}
	else {

		SetWindowText(DirectXBase::Ins()->windowsAPI_->hwnd_, L"LE3A_21_�t�i�N���x_�^�N�~");

	}

	// �L�������X�V�B
	characterMgr_->Update(stages_[STAGE_ID::CIRCUIT], constBufferData_, rapCount_, isPassedMiddlePoint_);

	// �����̎���X�V�B
	constBufferData_.debug_.seed_ = FHelper::GetRand(0, 1000);

	// �J�������X�V�B
	Camera::Ins()->Update(characterMgr_->GetPlayerIns().lock()->GetPos(), characterMgr_->GetPlayerIns().lock()->GetForwardVec(), characterMgr_->GetPlayerIns().lock()->GetUpVec(), characterMgr_->GetPlayerIns().lock()->GetNowSpeedPer());

	// 3�T���Ă����烊�U���g�V�[���Ɉړ�����B
	if (3 <= rapCount_) {

		isTransition_ = true;

	}

	// ���b�v����UI���X�V�B
	maxRapCountSprite_->ChangeTextureID(numFontHandle_[3], 0);
	nowRapCountSprite_->ChangeTextureID(numFontHandle_[rapCount_], 0);
	rapSlashSprite_->ChangeTextureID(numFontHandle_[10], 0);

	// �X�e�[�W���X�V�B
	stages_[STAGE_ID::CIRCUIT]->Update();

	// �S�[���̕\����\����؂�ւ��B
	if (isPassedMiddlePoint_) {
		stages_[STAGE_ID::CIRCUIT]->DisplayGoal();
	}
	else {
		stages_[STAGE_ID::CIRCUIT]->NonDisplayGoal();
	}


	// BLAS�̏���ύX�B������͕ύX�����ӏ��̂ݏ�����������悤�ɂ������B
	pipline_->MapHitGroupInfo();

	tlas_->Update();

	// ���z�̊p�x���X�V�B
	sunAngle_ += sunSpeed_;
	constBufferData_.light_.dirLight_.lihgtDir_ = Vec3(-cos(sunAngle_), -sin(sunAngle_), 0.5f);
	constBufferData_.light_.dirLight_.lihgtDir_.Normalize();
	// �V�����̂���]������B
	PolygonInstanceRegister::Ins()->AddRotate(skyDomeIns_, Vec3(0.001f, 0, 0));

	// �b�����X�V�B
	ShellObjectMgr::Ins()->Update(stages_[STAGE_ID::CIRCUIT]);

	// �����X�V����B
	DriftParticleMgr::Ins()->Update(constBufferData_);

}

void GameScene::Draw()
{

	/*===== �`�揈�� =====*/

	// �J�����s����X�V�B
	auto frameIndex = DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex();
	constBufferData_.camera_.mtxView_ = Camera::Ins()->matView_;
	constBufferData_.camera_.mtxViewInv_ = DirectX::XMMatrixInverse(nullptr, constBufferData_.camera_.mtxView_);
	constBufferData_.camera_.mtxProj_ = Camera::Ins()->matPerspective_;
	constBufferData_.camera_.mtxProjInv_ = DirectX::XMMatrixInverse(nullptr, constBufferData_.camera_.mtxProj_);

	// �萔�o�b�t�@���Z�b�g�B
	constBuffer_->Write(DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex(), &constBufferData_, sizeof(constBufferData_));


	// �O���[�o�����[�g�V�O�l�`���Ŏg���Ɛ錾���Ă��郊�\�[�X����Z�b�g�B
	ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	DirectXBase::Ins()->cmdList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	DirectXBase::Ins()->cmdList_->SetComputeRootSignature(pipline_->GetGlobalRootSig()->GetRootSig().Get());

	// TLAS��ݒ�B
	DirectXBase::Ins()->cmdList_->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas_->GetDescriptorHeapIndex()));

	// �萔�o�b�t�@���Z�b�g
	DirectXBase::Ins()->cmdList_->SetComputeRootConstantBufferView(1, constBuffer_->GetBuffer(frameIndex)->GetGPUVirtualAddress());

	// �o���A��ݒ肵�e���\�[�X�̏�Ԃ�J�ڂ�����.
	aoOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseAOOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	lightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseLightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	colorOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	giOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseGiOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseMaskOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// �o�͗pUAV��ݒ�B
	aoOutput_->SetComputeRootDescriptorTalbe(2);		// AO�̌��ʏo�͗p
	lightOutput_->SetComputeRootDescriptorTalbe(3);	// ���C�g�̖��邳�̌��ʏo�͗p
	colorOutput_->SetComputeRootDescriptorTalbe(4);	// �e�N�X�`���̐F���o�͗p
	giOutput_->SetComputeRootDescriptorTalbe(5);		// gi�̌��ʏo�͗p
	denoiseMaskOutput_->SetComputeRootDescriptorTalbe(6);// �f�m�C�Y������ۂ̃}�X�N�o�͗p

	// �p�C�v���C����ݒ�B
	DirectXBase::Ins()->cmdList_->SetPipelineState1(pipline_->GetStateObject().Get());

	// ���C�g���[�V���O�����s�B
	D3D12_DISPATCH_RAYS_DESC rayDesc = pipline_->GetDispatchRayDesc();
	DirectXBase::Ins()->cmdList_->DispatchRays(&rayDesc);


	// ���𔒓h��
	static int a = 0;
	if (Input::Ins()->IsKeyTrigger(DIK_R) || a == 0) {

		whiteOutComputeShader_->Dispatch(4096 / 32, 4096 / 32, 1, tireMaskTexture_->GetUAVIndex());
		++a;

	}



	// �^�C�������������ށB
	std::vector<Character::TireMaskUV> tireMaskUV;
	bool isWriteTireMask = characterMgr_->CheckTireMask(stages_[STAGE_ID::CIRCUIT], tireMaskUV);

	if (isWriteTireMask) {

		// UAV���������ށB
		tireMaskConstBuffer_->Write(DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex(), tireMaskUV.data(), sizeof(Character::TireMaskUV) * 2);
		tireMaskComputeShader_->Dispatch(1, 1, 1, tireMaskTexture_->GetUAVIndex(), { tireMaskConstBuffer_->GetBuffer(DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() });
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
						tireMaskTexture_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
						tireMaskTextureOutput_->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList_->ResourceBarrier(2, barrierToUAV);
		}

	}


	// [�m�C�Y��`��]�̂Ƃ��̓f�m�C�Y�������Ȃ��B
	if (!constBufferData_.debug_.isNoiseScene_) {

		// �f�o�b�O�@�\��[�@���`��][���b�V���`��][���C�g�ɓ��������_�̂ݕ`��]�̂Ƃ��̓f�m�C�Y�������Ȃ��悤�ɂ���B
		if (!constBufferData_.debug_.isMeshScene_ && !constBufferData_.debug_.isNormalScene_ && !constBufferData_.debug_.isLightHitScene_) {

			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				lightOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseLightOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseMaskOutput_->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList_->ResourceBarrier(3, barrierToUAV);

			// ���C�g�Ƀf�m�C�Y��������B
			Denoiser::Ins()->Denoise(lightOutput_->GetUAVIndex(), denoiseLightOutput_->GetUAVIndex(), denoiseMaskOutput_->GetUAVIndex(), 1, 1);

		}

		// AO���Ƀf�m�C�Y��������B
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				aoOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseAOOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseMaskOutput_->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList_->ResourceBarrier(3, barrierToUAV);

			// AO�Ƀf�m�C�Y��������B
			Denoiser::Ins()->Denoise(aoOutput_->GetUAVIndex(), denoiseAOOutput_->GetUAVIndex(), denoiseMaskOutput_->GetUAVIndex(), 100, 6);
		}


		// GI���Ƀf�m�C�Y��������B
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				giOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseGiOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseMaskOutput_->GetRaytracingOutput().Get())
			};

			DirectXBase::Ins()->cmdList_->ResourceBarrier(3, barrierToUAV);

			// GI�Ƀf�m�C�Y��������B
			Denoiser::Ins()->Denoise(giOutput_->GetUAVIndex(), denoiseGiOutput_->GetUAVIndex(), denoiseMaskOutput_->GetUAVIndex(), 100, 1);

		}

		denoiseMixTextureOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			colorOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseAOOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseLightOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseGiOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseMixTextureOutput_->GetRaytracingOutput().Get())
		};

		DirectXBase::Ins()->cmdList_->ResourceBarrier(5, barrierToUAV);

		// �f�m�C�Y�����������C�e�B���O���ƐF����������B
		Denoiser::Ins()->MixColorAndLuminance(colorOutput_->GetUAVIndex(), denoiseAOOutput_->GetUAVIndex(), denoiseLightOutput_->GetUAVIndex(), denoiseGiOutput_->GetUAVIndex(), denoiseMixTextureOutput_->GetUAVIndex());
		denoiseMixTextureOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	}
	// �f�m�C�Y���Ȃ��f�o�b�O��Ԃ̏ꍇ�́A���C�g���֐�����o�͂��ꂽ���̒l����������B
	else {


		denoiseMixTextureOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			colorOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			aoOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			lightOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			giOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseMixTextureOutput_->GetRaytracingOutput().Get())
		};

		DirectXBase::Ins()->cmdList_->ResourceBarrier(5, barrierToUAV);

		// �f�m�C�Y�����������C�e�B���O���ƐF����������B
		Denoiser::Ins()->MixColorAndLuminance(colorOutput_->GetUAVIndex(), aoOutput_->GetUAVIndex(), lightOutput_->GetUAVIndex(), giOutput_->GetUAVIndex(), denoiseMixTextureOutput_->GetUAVIndex());
		denoiseMixTextureOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	}


	// �o�b�N�o�b�t�@�̃C���f�b�N�X���擾����B
	UINT backBufferIndex = DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barriers[] = {
		CD3DX12_RESOURCE_BARRIER::Transition(
		DirectXBase::Ins()->backBuffers_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_COPY_DEST),
	};
	DirectXBase::Ins()->cmdList_->ResourceBarrier(_countof(barriers), barriers);

	// �f�o�b�O���ɂ���ĕ`�悷��f�[�^��ς���B
	if (constBufferData_.debug_.isLightHitScene_ || constBufferData_.debug_.isMeshScene_ || constBufferData_.debug_.isNormalScene_) {

		// �f�m�C�Y���ꂽ�ʏ�̕`��
		lightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		DirectXBase::Ins()->cmdList_->CopyResource(DirectXBase::Ins()->backBuffers_[backBufferIndex].Get(), lightOutput_->GetRaytracingOutput().Get());
		lightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	}
	else {

		DirectXBase::Ins()->cmdList_->CopyResource(DirectXBase::Ins()->backBuffers_[backBufferIndex].Get(), denoiseMixTextureOutput_->GetRaytracingOutput().Get());

	}

	// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�����Ƃɖ߂��B
	D3D12_RESOURCE_BARRIER endBarriers[] = {

	CD3DX12_RESOURCE_BARRIER::Transition(
	DirectXBase::Ins()->backBuffers_[backBufferIndex].Get(),
	D3D12_RESOURCE_STATE_COPY_DEST,
	D3D12_RESOURCE_STATE_RENDER_TARGET)

	};

	DirectXBase::Ins()->cmdList_->ResourceBarrier(_countof(endBarriers), endBarriers);

	// �o���A��ݒ肵�e���\�[�X�̏�Ԃ�J�ڂ�����.
	aoOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseAOOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	lightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseLightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	colorOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	giOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseGiOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseMaskOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	// UI��`��
	static int firstTime = 0;
	if (firstTime != 0) {
		nowRapCountSprite_->Draw();
		maxRapCountSprite_->Draw();
		rapSlashSprite_->Draw();
	}
	if (firstTime == 0) ++firstTime;

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
		SetWindowText(DirectXBase::Ins()->windowsAPI_->hwnd_, fps);
		//OutputDebugString(fps);

		prev_time = now_time;
		frame_count = 0;
	}

}

void GameScene::Input()
{

	/*===== ���͏��� =====*/

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		isTransition_ = true;

	}

	InputImGUI();

}
#include "BaseItem.h"
void GameScene::InputImGUI()
{

	/*===== IMGUI�X�V =====*/

	// ���z�̈ړ����x���X�V�B
	ImGui::SliderFloat("Sun Speed", &sunSpeed_, 0.0f, 0.1f, "%.5f");

	// ���b�V����\������B
	bool isMesh = constBufferData_.debug_.isMeshScene_;
	ImGui::Checkbox("Mesh Scene", &isMesh);
	constBufferData_.debug_.isMeshScene_ = isMesh;

	// �@����\������B
	bool isNormal = constBufferData_.debug_.isNormalScene_;
	ImGui::Checkbox("Normal Scene", &isNormal);
	constBufferData_.debug_.isNormalScene_ = isNormal;

	// ���C�g�����������ʂ����\������t���O���X�V�B
	bool isLightHit = constBufferData_.debug_.isLightHitScene_;
	ImGui::Checkbox("LightHit Scene", &isLightHit);
	constBufferData_.debug_.isLightHitScene_ = isLightHit;

	// �f�o�b�O�p�Ńm�C�Y��ʂ��o�����߂̃t���O���Z�b�g�B
	bool isNoise = constBufferData_.debug_.isNoiseScene_;
	ImGui::Checkbox("Noise Scene", &isNoise);
	constBufferData_.debug_.isNoiseScene_ = isNoise;

	// AO���s�����̃t���O���Z�b�g�B
	bool isNoAO_ = constBufferData_.debug_.isNoAO_;
	ImGui::Checkbox("NoAO Scene", &isNoAO_);
	constBufferData_.debug_.isNoAO_ = isNoAO_;

	// GI���s�����̃t���O���Z�b�g�B
	bool isNoGI_ = constBufferData_.debug_.isNoGI_;
	ImGui::Checkbox("NoGI Scene", &isNoGI_);
	constBufferData_.debug_.isNoGI_ = isNoGI_;

	// GI�݂̂�`�悷�邩�̃t���O���Z�b�g�B
	bool isGIOnlyScene_ = constBufferData_.debug_.isGIOnlyScene_;
	ImGui::Checkbox("GIOnly Scene", &isGIOnlyScene_);
	constBufferData_.debug_.isGIOnlyScene_ = isGIOnlyScene_;

	// FPS��\�����邩�̃t���O���Z�b�g�B
	ImGui::Checkbox("Display FPS", &isDisplayFPS_);

	// �A�C�e���f�o�b�O�p�B
	bool haveItem = characterMgr_->GetPlayerIns().lock()->item_.operator bool();

	if (haveItem) {

		bool haveBoostItem = characterMgr_->GetPlayerIns().lock()->item_->GetItemID() == BaseItem::ItemID::BOOST;

		ImGui::Checkbox("BoostItem", &haveBoostItem);

		bool haveShellItem = characterMgr_->GetPlayerIns().lock()->item_->GetItemID() == BaseItem::ItemID::SHELL;

		ImGui::Checkbox("ShellItem", &haveShellItem);

	}





	//int index = 71;

	//Vec3 pos = PolygonInstanceRegister::Ins()->GetPos(index);

	//float posArray[3] = { pos.x_, pos.y_, pos.z_ };

	//ImGui::DragFloat3("Pos", posArray, 0.5f);

	//pos.x_ = posArray[0];
	//pos.y_ = posArray[1];
	//pos.z_ = posArray[2];

	//PolygonInstanceRegister::Ins()->ChangeTrans(index, pos);


	//Vec3 rotate = PolygonInstanceRegister::Ins()->GetRotateVec3(index);

	//float rotateArray[3] = { rotate.x_, rotate.y_, rotate.z_ };

	//ImGui::DragFloat3("Rotate", rotateArray, 0.001f);

	//rotate.x_ = rotateArray[0];
	//rotate.y_ = rotateArray[1];
	//rotate.z_ = rotateArray[2];

	//PolygonInstanceRegister::Ins()->ChangeRotate(index, rotate);


	//// 1��
	//GimmickMgr::Ins()->ChangeTrans(0, Vec3(100, -15, 1400));
	//GimmickMgr::Ins()->ChangeRotate(0, Vec3(0, 0, 0));
	//GimmickMgr::Ins()->ChangeScale(0, Vec3(100, 200, 200));

	//// 2��
	//GimmickMgr::Ins()->ChangeTrans(1, Vec3(-80, -15, 3000));
	//GimmickMgr::Ins()->ChangeRotate(1, Vec3(0, 0, 0));
	//GimmickMgr::Ins()->ChangeScale(1, Vec3(100, 200, 200));

	//// 3��
	//GimmickMgr::Ins()->ChangeTrans(2, Vec3(100, -15, 4000));
	//GimmickMgr::Ins()->ChangeRotate(2, Vec3(0, 0, 0));
	//GimmickMgr::Ins()->ChangeScale(2, Vec3(100, 200, 200));

	//// 4��
	//GimmickMgr::Ins()->ChangeTrans(3, Vec3(-5842, -29, -167));
	//GimmickMgr::Ins()->ChangeRotate(3, Vec3(0, 1.48f, 0));
	//GimmickMgr::Ins()->ChangeScale(3, Vec3(200, 200, 200));

	//// 5��
	//GimmickMgr::Ins()->ChangeTrans(4, Vec3(-2352, -18, 6336));
	//GimmickMgr::Ins()->ChangeRotate(4, Vec3(0, 1.58f, 0));
	//GimmickMgr::Ins()->ChangeScale(4, Vec3(190, 200, 200));


}

void GameScene::GenerateGimmick()
{

	//// 1��
	//GimmickMgr::Ins()->AddGimmick(BaseStageObject::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::SHADER_ID::DEF);

	//// 2��
	//GimmickMgr::Ins()->AddGimmick(BaseStageObject::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::SHADER_ID::DEF);

	//// 3��
	//GimmickMgr::Ins()->AddGimmick(BaseStageObject::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::SHADER_ID::DEF);

	//// 4��
	//GimmickMgr::Ins()->AddGimmick(BaseStageObject::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::SHADER_ID::DEF);

	//// 5��
	//GimmickMgr::Ins()->AddGimmick(BaseStageObject::ID::BOOST, "Resource/Game/", "goal.obj", { L"Resource/Game/yellow.png" }, HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], PolygonInstanceRegister::SHADER_ID::DEF);

}