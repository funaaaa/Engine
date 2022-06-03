#include "BakeAO.h"
#include "RaytracingOutput.h"
#include "RayPiplineBakeAO.h"
#include "RayRootsignature.h"
#include "DescriptorHeapMgr.h"
#include "HitGroupMgr.h"
#include "DirectXBase.h"
#include <DirectXMath.h>

void BakeAO::ExecutionBake(const int& TexCount, TLAS& Tlas, Microsoft::WRL::ComPtr<ID3D12Resource>& ConstBuff, RaytracingOutput& DebugTex)
{

	/*===== �x�C�N�����s =====*/

	// �x�C�N��̃e�N�X�`���𐶐��B
	bakeTex.clear();
	bakeTex.resize(TexCount);

	for (auto& index : bakeTex) {

		index = std::make_shared<RaytracingOutput>();
		index->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	}

	// ���[�g�V�O�l�`���𐶐��B
	rootsignature = std::make_shared<RayRootsignature>();

	// ���[�g�V�O�l�`����ݒ�B
	rootsignature = HitGroupMgr::Ins()->GetLocalRootSig(HitGroupMgr::HITGROUP_ID::BAKE_AO_HIT_GROUP);

	// �p�C�v���C���𐶐��B
	pipline = std::make_shared<RayPiplineBakeAO>();

	// �p�C�v���C����ݒ�B
	vector<RayPiplineShaderData> shaders;
	shaders.push_back({ "Resource/ShaderFiles/RayTracing/BakeAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	pipline->Setting(shaders, HitGroupMgr::HITGROUP_ID::BAKE_AO_HIT_GROUP, 1, 1, 1, sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// �V�F�[�_�[�e�[�u�����\�z�B
	pipline->ConstructionShaderTableBakeAO(bakeTex);

	// ���s����O�����B
	DirectXBase::Ins()->processBeforeDrawing();

	// �e�N�X�`���̐ݒ��ύX�B
	for (auto& index : bakeTex) {

		index->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	}

	DebugTex.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// �K�v�ȏ���ǉ��B
	ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	DirectXBase::Ins()->cmdList->SetComputeRootSignature(pipline->GetGlobalRootSig()->GetRootSig().Get());
	DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(Tlas.GetDescriptorHeapIndex()));
	DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, ConstBuff->GetGPUVirtualAddress());
	DebugTex.SetComputeRootDescriptorTalbe(2);
	DirectXBase::Ins()->cmdList->SetPipelineState1(pipline->GetStateObject().Get());

	DirectXBase::Ins()->cmdList->DispatchRays(&pipline->GetDispatchRayDesc());

	DebugTex.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	// ���s��̏����B
	DirectXBase::Ins()->processAfterDrawing();

}
