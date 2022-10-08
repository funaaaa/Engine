#include "RayEngine.h"
#include "RaytracingOutput.h"
#include "TLAS.h"
#include "HitGroupMgr.h"

void RayEngine::Setting()
{

	/*===== �������� =====*/

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

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	pipelineShaders_.push_back({ "Resource/ShaderFiles/RayTracing/DenoiseAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	int payloadSize = sizeof(float) * 4 + sizeof(Vec3) * 4 + sizeof(int) * 2 + sizeof(Vec2);
	pipeline_ = std::make_shared<RaytracingPipeline>();
	pipeline_->Setting(pipelineShaders_, HitGroupMgr::DEF, 1, 1, 5, payloadSize, sizeof(Vec2), 6);

	// �V�F�[�_�[�e�[�u���𐶐��B
	pipeline_->ConstructionShaderTable();

	// �萔�o�b�t�@�𐶐��B
	constBufferData_.Init();
	constBuffer_ = std::make_shared<DynamicConstBuffer>();
	constBuffer_->Generate(sizeof(RayConstBufferData), L"CameraConstBuffer");
	constBuffer_->Write(Engine::Ins()->swapchain_->GetCurrentBackBufferIndex(), &constBufferData_, sizeof(RayConstBufferData));

}

void RayEngine::SettingTLAS()
{

	/*===== TLAS��ݒ� =====*/

	tlas_ = std::make_shared<TLAS>();
	tlas_->GenerateTLAS();

}

void RayEngine::Update()
{

	/*===== �X�V���� =====*/

	// BLAS�̏���ύX�B������͕ύX�����ӏ��̂ݏ�����������悤�ɂ������B
	pipeline_->MapHitGroupInfo();

	// TLAS���X�V�B
	tlas_->Update();

}

#include "RayDenoiser.h"
#include "Camera.h"
void RayEngine::Draw()
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
	Engine::Ins()->cmdList_->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(RayEngine::Ins()->tlas_->GetDescriptorHeapIndex()));

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
			Denoiser::Ins()->Denoise(lightOutput_->GetUAVIndex(), denoiseLightOutput_->GetUAVIndex(), denoiseMaskOutput_->GetUAVIndex(), 10, 3);

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
			Denoiser::Ins()->Denoise(aoOutput_->GetUAVIndex(), denoiseAOOutput_->GetUAVIndex(), denoiseMaskOutput_->GetUAVIndex(), 1000, 8);
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
