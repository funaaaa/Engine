#include "PBRTestScene.h"
#include "HitGroupMgr.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "Character.h"
#include "FHelper.h"
#include "Camera.h"
#include "RayDenoiser.h"
#include "RayRootsignature.h"
#include "DynamicConstBuffer.h"
#include "RaytracingPipeline.h"
#include "RaytracingOutput.h"
#include "TLAS.h"
#include "Input.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "Pipeline.h"
#include "WindowsAPI.h"
#include "CircuitStage.h"
#include "MugenStage.h"
#include "RayComputeShader.h"
#include "StageObjectMgr.h"
#include "BLAS.h"
#include "ShellObjectMgr.h"
#include "CharacterMgr.h"
#include "GameSceneMode.h"
#include "DriftParticleMgr.h"
#include "ConcentrationLineMgr.h"
#include "PolygonInstance.h"

#include "GLTF.h"

PBRTestScene::PBRTestScene()
{
	/*===== ���������� =====*/

	// �萔�o�b�t�@�𐶐��B
	constBufferData_.Init();
	constBuffer_ = std::make_shared<DynamicConstBuffer>();
	constBuffer_->Generate(sizeof(RayConstBufferData), L"CameraConstBuffer");
	constBuffer_->Write(Engine::Ins()->swapchain_->GetCurrentBackBufferIndex(), &constBufferData_, sizeof(RayConstBufferData));

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	dAOuseShaders_.push_back({ "Resource/ShaderFiles/RayTracing/DenoiseAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	int payloadSize = sizeof(float) * 4 + sizeof(Vec3) * 4 + sizeof(int) * 2 + sizeof(Vec2);
	pipeline_ = std::make_shared<RaytracingPipeline>();
	pipeline_->Setting(dAOuseShaders_, HitGroupMgr::DEF, 1, 1, 5, payloadSize, sizeof(Vec2), 6);

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

	// Instance�̃��[���h�s��𐶐��B
	PolygonInstanceRegister::Ins()->CalWorldMat();

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
	pipeline_->ConstructionShaderTable();

	// ���z�Ɋւ���ϐ�
	sunAngle_ = 0.1f;
	sunSpeed_ = 0.01f;

	isDisplayFPS_ = false;

	nextScene_ = SCENE_ID::RESULT;
	isTransition_ = false;


	// PBR�p�̉~�����[�h
	pbrSphereBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Gimmick/gltfTest.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	pbrSphereIns_ = PolygonInstanceRegister::Ins()->CreateInstance(pbrSphereBlas_, PolygonInstanceRegister::SHADER_ID::DEF);
	pbrSphereIns_.lock()->AddScale(Vec3(10, 10, 10));
	pbrSphereIns_.lock()->AddTrans(Vec3(0, 20, 0));
	pbrSphereIns_.lock()->AddRotate(Vec3(0, DirectX::XM_PI, 0));

	//lightSphereBlas_ = BLASRegister::Ins()->GenerateGLTF(L"Resource/Game/Gimmick/gltfTest.glb", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::DEF], true, true);
	//lightSphereIns_ = PolygonInstanceRegister::Ins()->CreateInstance(lightSphereBlas_, PolygonInstanceRegister::SHADER_ID::LIGHT);
	//lightSphereIns_.lock()->AddScale(Vec3(2, 2, 2));

	constBufferData_.light_.pointLight_[0].lightPos_ = Vec3();

}

void PBRTestScene::Init()
{
}

void PBRTestScene::Update()
{
	/*===== �X�V���� =====*/

	isTransition_ = false;

	// ���C�g����L�����B
	constBufferData_.light_.pointLight_[0].isActive_ = true;
	float lightSpeed = 1.0f;
	if (Input::Ins()->IsKey(DIK_W)) {
		constBufferData_.light_.pointLight_[0].lightPos_.z_ += lightSpeed;
	}
	if (Input::Ins()->IsKey(DIK_S)) {
		constBufferData_.light_.pointLight_[0].lightPos_.z_ -= lightSpeed;
	}
	if (Input::Ins()->IsKey(DIK_D)) {
		constBufferData_.light_.pointLight_[0].lightPos_.x_ += lightSpeed;
	}
	if (Input::Ins()->IsKey(DIK_A)) {
		constBufferData_.light_.pointLight_[0].lightPos_.x_ -= lightSpeed;
	}
	if (Input::Ins()->IsKey(DIK_SPACE)) {
		constBufferData_.light_.pointLight_[0].lightPos_.y_ += lightSpeed;
	}
	if (Input::Ins()->IsKey(DIK_LSHIFT)) {
		constBufferData_.light_.pointLight_[0].lightPos_.y_ -= lightSpeed;
	}
	if (Input::Ins()->IsKey(DIK_UP)) {
		constBufferData_.light_.pointLight_[0].lightPower_ += 0.1f;
	}
	if (Input::Ins()->IsKey(DIK_DOWN)) {
		constBufferData_.light_.pointLight_[0].lightPower_ -= 0.1f;
	}

	//lightSphereIns_.lock()->ChangeTrans(constBufferData_.light_.pointLight_[0].lightPos_);


	// �}�e���A���̒l������������B
	ImGui::Text("Menu");
	ImGui::SliderFloat("Metalness", &pbrSphereBlas_.lock()->GetMaterial().metalness_, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular", &pbrSphereBlas_.lock()->GetMaterial().specular_, 0.0f, 1.0f);
	ImGui::SliderFloat("Roughness", &pbrSphereBlas_.lock()->GetMaterial().roughness_, 0.0f, 1.0f);

	ImGui::SliderFloat("DirLight_X", &constBufferData_.light_.dirLight_.lihgtDir_.x_, -1.0f, 1.0f);
	ImGui::SliderFloat("DirLight_Y", &constBufferData_.light_.dirLight_.lihgtDir_.y_, -1.0f, 1.0f);
	ImGui::SliderFloat("DirLight_Z", &constBufferData_.light_.dirLight_.lihgtDir_.z_, -1.0f, 1.0f);
	constBufferData_.light_.dirLight_.lihgtDir_.Normalize();

	pbrSphereBlas_.lock()->IsChangeMaterial();


	// �����̎���X�V�B
	constBufferData_.debug_.seed_ = FHelper::GetRand(0, 1000);

	// �J�������X�V�B
	Camera::Ins()->Update(Vec3(0, -15, 120), Vec3(0, 0, 1), Vec3(0, 1, 0), 0.0f, false, false);


	// BLAS�̏���ύX�B������͕ύX�����ӏ��̂ݏ�����������悤�ɂ������B
	pipeline_->MapHitGroupInfo();

	tlas_->Update();

	// ���z�̊p�x���X�V�B
	//sunAngle_ += sunSpeed_;
	//constBufferData_.light_.dirLight_.lihgtDir_ = Vec3(-cos(sunAngle_), -sin(sunAngle_), 0.5f);
	//constBufferData_.light_.dirLight_.lihgtDir_.Normalize();
	//sunAngle_ = 30.0f;

	//constBufferData_.light_.dirLight_.lihgtDir_ = Vec3(0.0f, -0.8f, -0.4f);
	//constBufferData_.light_.dirLight_.lihgtDir_.Normalize();

	// �����X�V����B
	//DriftParticleMgr::Ins()->Update(constBufferData_);

}

void PBRTestScene::Draw()
{

	/*===== �`�揈�� =====*/

	// �J�����s����X�V�B
	auto frameIndex = Engine::Ins()->swapchain_->GetCurrentBackBufferIndex();
	constBufferData_.camera_.mtxView_ = Camera::Ins()->matView_;
	constBufferData_.camera_.mtxViewInv_ = DirectX::XMMatrixInverse(nullptr, constBufferData_.camera_.mtxView_);
	constBufferData_.camera_.mtxProj_ = Camera::Ins()->matPerspective_;
	constBufferData_.camera_.mtxProjInv_ = DirectX::XMMatrixInverse(nullptr, constBufferData_.camera_.mtxProj_);

	// �萔�o�b�t�@���Z�b�g�B
	constBuffer_->Write(Engine::Ins()->swapchain_->GetCurrentBackBufferIndex(), &constBufferData_, sizeof(constBufferData_));


	// �O���[�o�����[�g�V�O�l�`���Ŏg���Ɛ錾���Ă��郊�\�[�X����Z�b�g�B
	ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	Engine::Ins()->cmdList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	Engine::Ins()->cmdList_->SetComputeRootSignature(pipeline_->GetGlobalRootSig()->GetRootSig().Get());

	// TLAS��ݒ�B
	Engine::Ins()->cmdList_->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas_->GetDescriptorHeapIndex()));

	// �萔�o�b�t�@���Z�b�g
	Engine::Ins()->cmdList_->SetComputeRootConstantBufferView(1, constBuffer_->GetBuffer(frameIndex)->GetGPUVirtualAddress());

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
	Engine::Ins()->cmdList_->SetPipelineState1(pipeline_->GetStateObject().Get());

	// ���C�g���[�V���O�����s�B
	D3D12_DISPATCH_RAYS_DESC rayDesc = pipeline_->GetDispatchRayDesc();
	Engine::Ins()->cmdList_->DispatchRays(&rayDesc);


	// ���𔒓h��
	static int a = 0;
	if (Input::Ins()->IsKeyTrigger(DIK_R) || a == 0) {

		whiteOutComputeShader_->Dispatch(4096 / 32, 4096 / 32, 1, tireMaskTexture_->GetUAVIndex());
		++a;

	}



	// �^�C�������������ށB
	std::vector<Character::TireMaskUV> tireMaskUV;

	if (false) {

		// UAV���������ށB
		tireMaskConstBuffer_->Write(Engine::Ins()->swapchain_->GetCurrentBackBufferIndex(), tireMaskUV.data(), sizeof(Character::TireMaskUV) * 2);
		tireMaskComputeShader_->Dispatch(1, 1, 1, tireMaskTexture_->GetUAVIndex(), { tireMaskConstBuffer_->GetBuffer(Engine::Ins()->swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() });
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
						tireMaskTexture_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
						tireMaskTextureOutput_->GetRaytracingOutput().Get())
			};

			Engine::Ins()->cmdList_->ResourceBarrier(2, barrierToUAV);
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

			Engine::Ins()->cmdList_->ResourceBarrier(3, barrierToUAV);

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

			Engine::Ins()->cmdList_->ResourceBarrier(3, barrierToUAV);

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

			Engine::Ins()->cmdList_->ResourceBarrier(3, barrierToUAV);

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

		Engine::Ins()->cmdList_->ResourceBarrier(5, barrierToUAV);

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

		Engine::Ins()->cmdList_->ResourceBarrier(5, barrierToUAV);

		// �f�m�C�Y�����������C�e�B���O���ƐF����������B
		Denoiser::Ins()->MixColorAndLuminance(colorOutput_->GetUAVIndex(), aoOutput_->GetUAVIndex(), lightOutput_->GetUAVIndex(), giOutput_->GetUAVIndex(), denoiseMixTextureOutput_->GetUAVIndex());
		denoiseMixTextureOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	}


	// �o�b�N�o�b�t�@�̃C���f�b�N�X���擾����B
	UINT backBufferIndex = Engine::Ins()->swapchain_->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barriers[] = {
		CD3DX12_RESOURCE_BARRIER::Transition(
		Engine::Ins()->backBuffers_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_COPY_DEST),
	};
	Engine::Ins()->cmdList_->ResourceBarrier(_countof(barriers), barriers);

	// �f�o�b�O���ɂ���ĕ`�悷��f�[�^��ς���B
	if (constBufferData_.debug_.isLightHitScene_ || constBufferData_.debug_.isMeshScene_ || constBufferData_.debug_.isNormalScene_) {

		// �f�m�C�Y���ꂽ�ʏ�̕`��
		lightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		Engine::Ins()->cmdList_->CopyResource(Engine::Ins()->backBuffers_[backBufferIndex].Get(), lightOutput_->GetRaytracingOutput().Get());
		lightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	}
	else {

		Engine::Ins()->cmdList_->CopyResource(Engine::Ins()->backBuffers_[backBufferIndex].Get(), denoiseMixTextureOutput_->GetRaytracingOutput().Get());

	}

	// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�����Ƃɖ߂��B
	D3D12_RESOURCE_BARRIER endBarriers[] = {

	CD3DX12_RESOURCE_BARRIER::Transition(
	Engine::Ins()->backBuffers_[backBufferIndex].Get(),
	D3D12_RESOURCE_STATE_COPY_DEST,
	D3D12_RESOURCE_STATE_RENDER_TARGET)

	};

	Engine::Ins()->cmdList_->ResourceBarrier(_countof(endBarriers), endBarriers);

	// �o���A��ݒ肵�e���\�[�X�̏�Ԃ�J�ڂ�����.
	aoOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseAOOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	lightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseLightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	colorOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	giOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseGiOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseMaskOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

}
