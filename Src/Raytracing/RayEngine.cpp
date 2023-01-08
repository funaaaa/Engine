#include "RayEngine.h"
#include "RaytracingOutput.h"
#include "TLAS.h"
#include "HitGroupMgr.h"

void RayEngine::Setting()
{

	/*===== �������� =====*/

	// �F�o�͗p�N���X���Z�b�g�B
	colorOutput_[0] = std::make_shared<RaytracingOutput>();
	colorOutput_[0]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"ColorOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	colorOutput_[0]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	colorOutput_[1] = std::make_shared<RaytracingOutput>();
	colorOutput_[1]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"ColorOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	colorOutput_[1]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// ���邳���o�͗p�N���X���Z�b�g�B
	lightOutput_[0] = std::make_shared<RaytracingOutput>();
	lightOutput_[0]->Setting(DXGI_FORMAT_R11G11B10_FLOAT, L"LightOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	lightOutput_[0]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseLightOutput_[0] = std::make_shared<RaytracingOutput>();
	denoiseLightOutput_[0]->Setting(DXGI_FORMAT_R11G11B10_FLOAT, L"DenoiseLightOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	denoiseLightOutput_[0]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	lightOutput_[1] = std::make_shared<RaytracingOutput>();
	lightOutput_[1]->Setting(DXGI_FORMAT_R11G11B10_FLOAT, L"LightOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	lightOutput_[1]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseLightOutput_[1] = std::make_shared<RaytracingOutput>();
	denoiseLightOutput_[1]->Setting(DXGI_FORMAT_R11G11B10_FLOAT, L"DenoiseLightOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	denoiseLightOutput_[1]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// �f�m�C�Y�}�X�N�p�N���X���Z�b�g�B
	denoiseMaskOutput_[0] = std::make_shared<RaytracingOutput>();
	denoiseMaskOutput_[0]->Setting(DXGI_FORMAT_R11G11B10_FLOAT, L"DenoiseMaskOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	denoiseMaskOutput_[0]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseMaskOutput_[1] = std::make_shared<RaytracingOutput>();
	denoiseMaskOutput_[1]->Setting(DXGI_FORMAT_R11G11B10_FLOAT, L"DenoiseMaskOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	denoiseMaskOutput_[1]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// �f�m�C�Y�ŏI�o�͗p�N���X���Z�b�g�B
	denoiseMixTextureOutput_[0] = std::make_shared<RaytracingOutput>();
	denoiseMixTextureOutput_[0]->Setting(DXGI_FORMAT_R11G11B10_FLOAT, L"DenoiseMixTextureOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	denoiseMixTextureOutput_[0]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseMixTextureOutput_[1] = std::make_shared<RaytracingOutput>();
	denoiseMixTextureOutput_[1]->Setting(DXGI_FORMAT_R11G11B10_FLOAT, L"DenoiseMixTextureOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	denoiseMixTextureOutput_[1]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// �u���[���p�G�~�b�V�u
	emissiveOutput_[0] = std::make_shared<RaytracingOutput>();
	emissiveOutput_[0]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"EmissiveOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	emissiveOutput_[0]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	emissiveOutput_[1] = std::make_shared<RaytracingOutput>();
	emissiveOutput_[1]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"EmissiveOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	emissiveOutput_[1]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// �ŏI�o�͗p�N���X���Z�b�g�B
	finalOutputTexture_[0] = std::make_shared<RaytracingOutput>();
	finalOutputTexture_[0]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"FinalOutputTexture0", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	finalOutputTexture_[0]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	finalOutputTexture_[1] = std::make_shared<RaytracingOutput>();
	finalOutputTexture_[1]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"FinalOutputTexture1", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	finalOutputTexture_[1]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	pipelineShaders_.push_back({ "Resource/ShaderFiles/RayTracing/RaytracingShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	int payloadSize = sizeof(float) * 4 + sizeof(Vec3) * 4 + sizeof(UINT) * 4;
	pipeline_ = std::make_shared<RaytracingPipeline>();
	pipeline_->Setting(pipelineShaders_, HitGroupMgr::DEF, 1, 1, 4, payloadSize, sizeof(Vec2), 6);

	// �V�F�[�_�[�e�[�u���𐶐��B
	pipeline_->ConstructionShaderTable();

	// �萔�o�b�t�@�𐶐��B
	constBufferData_.Init();
	constBuffer_ = std::make_shared<DynamicConstBuffer>();
	constBuffer_->Generate(sizeof(RayConstBufferData), L"CameraConstBuffer");
	constBuffer_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), &constBufferData_, sizeof(RayConstBufferData));

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
#include "Bloom.h"
#include "Camera.h"
#include "RadialBlur.h"
void RayEngine::Draw()
{

	/*===== �`�揈�� =====*/

	// ���C�g���[�V���O�����s�B
	TraceRay();

	// �f�m�C�Y�R�}���h��ςށB
	DenoiseCommand();

	// �R�s�[�R�}���h�ɖ��߂�ςށB
	CopyCommand();

}

void RayEngine::TraceRay()
{

	/*===== ���C�g���[�V���O�����s =====*/

	// �J�����s����X�V�B
	auto backBufferIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();
	constBufferData_.camera_.mtxView_ = Camera::Ins()->matView_;
	constBufferData_.camera_.mtxViewInv_ = DirectX::XMMatrixInverse(nullptr, constBufferData_.camera_.mtxView_);
	constBufferData_.camera_.mtxProj_ = Camera::Ins()->matPerspective_;
	constBufferData_.camera_.mtxProjInv_ = DirectX::XMMatrixInverse(nullptr, constBufferData_.camera_.mtxProj_);

	// �萔�o�b�t�@���Z�b�g�B
	constBuffer_->Write(Engine::Ins()->currentQueueIndex_, &constBufferData_, sizeof(constBufferData_));


	// �O���[�o�����[�g�V�O�l�`���Ŏg���Ɛ錾���Ă��郊�\�[�X����Z�b�g�B
	ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	Engine::Ins()->mainGraphicsCmdList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	Engine::Ins()->mainGraphicsCmdList_->SetComputeRootSignature(pipeline_->GetGlobalRootSig()->GetRootSig().Get());
	// �R���s���[�g�L���[�ɂ��l�ށB
	Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->SetComputeRootSignature(pipeline_->GetGlobalRootSig()->GetRootSig().Get());

	// TLAS��ݒ�B
	Engine::Ins()->mainGraphicsCmdList_->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas_->GetDescriptorHeapIndex()));

	// �萔�o�b�t�@���Z�b�g
	Engine::Ins()->mainGraphicsCmdList_->SetComputeRootConstantBufferView(1, constBuffer_->GetBuffer(backBufferIndex)->GetGPUVirtualAddress());

	// �o�͗pUAV��ݒ�B
	lightOutput_[Engine::Ins()->currentQueueIndex_]->SetComputeRootDescriptorTalbe(2);	// ���C�g�̖��邳�̌��ʏo�͗p
	colorOutput_[Engine::Ins()->currentQueueIndex_]->SetComputeRootDescriptorTalbe(3);	// �e�N�X�`���̐F���o�͗p
	denoiseMaskOutput_[Engine::Ins()->currentQueueIndex_]->SetComputeRootDescriptorTalbe(4);// �f�m�C�Y������ۂ̃}�X�N�o�͗p
	emissiveOutput_[Engine::Ins()->currentQueueIndex_]->SetComputeRootDescriptorTalbe(5);// �f�m�C�Y������ۂ̃}�X�N�o�͗p

	// �p�C�v���C����ݒ�B
	Engine::Ins()->mainGraphicsCmdList_->SetPipelineState1(pipeline_->GetStateObject(Engine::Ins()->currentQueueIndex_).Get());

	// ���C�g���[�V���O�����s�B
	D3D12_DISPATCH_RAYS_DESC rayDesc = pipeline_->GetDispatchRayDesc(Engine::Ins()->currentQueueIndex_);
	Engine::Ins()->mainGraphicsCmdList_->DispatchRays(&rayDesc);

}

void RayEngine::DenoiseCommand()
{

	/*===== �f�m�C�Y�R�}���h��ς� =====*/

	// ���C�g���Ƀf�m�C�Y��������B
	{

		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			lightOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseLightOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseMaskOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get())
		};

		Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->ResourceBarrier(3, barrierToUAV);

		// ���C�g�Ƀf�m�C�Y��������B
		Denoiser::Ins()->Denoise(lightOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseLightOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseMaskOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), 10, 1);

	}

	// �u���[����������B
	{
		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			emissiveOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get())
		};
		Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->ResourceBarrier(1, barrierToUAV);

		Bloom::Ins()->ApplyBloom(emissiveOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex());
	}

	D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
		colorOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
		denoiseLightOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
		denoiseMixTextureOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get())
	};

	Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->ResourceBarrier(3, barrierToUAV);

	// �f�m�C�Y�����������C�e�B���O���ƐF����������B
	Denoiser::Ins()->MixColorAndLuminance(colorOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(),
		denoiseLightOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(),
		emissiveOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseMixTextureOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex());


	// ���W�A���u���[��������B
	{

		D3D12_RESOURCE_BARRIER radialBlurBarrier[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseMixTextureOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			finalOutputTexture_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get())
		};

		Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->ResourceBarrier(2, radialBlurBarrier);

		// ���W�A���u���[��������B
		RadialBlur::Ins()->Blur(denoiseMixTextureOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), finalOutputTexture_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex());

	}

}

void RayEngine::CopyCommand()
{

	/*===== �R�s�[�R�}���h��ς� =====*/


	auto backBufferIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barriers[] = {
		CD3DX12_RESOURCE_BARRIER::Transition(
		Engine::Ins()->swapchain_.backBuffers_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_COPY_DEST),
	};
	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(_countof(barriers), barriers);

	finalOutputTexture_[Engine::Ins()->pastQueueIndex_]->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, Engine::Ins()->copyResourceCmdList_);

	Engine::Ins()->copyResourceCmdList_->CopyResource(Engine::Ins()->swapchain_.backBuffers_[backBufferIndex].Get(), finalOutputTexture_[Engine::Ins()->pastQueueIndex_]->GetRaytracingOutput().Get());

	finalOutputTexture_[Engine::Ins()->pastQueueIndex_]->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, Engine::Ins()->copyResourceCmdList_);

	// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�����Ƃɖ߂��B
	D3D12_RESOURCE_BARRIER endBarriers[] = {

	CD3DX12_RESOURCE_BARRIER::Transition(
	Engine::Ins()->swapchain_.backBuffers_[backBufferIndex].Get(),
	D3D12_RESOURCE_STATE_COPY_DEST,
	D3D12_RESOURCE_STATE_PRESENT)

	};

	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(_countof(endBarriers), endBarriers);

}
