#include "BakeAO.h"
#include "RaytracingOutput.h"
#include "RayPiplineBakeAO.h"
#include "RayRootsignature.h"
#include "DescriptorHeapMgr.h"
#include "HitGroupMgr.h"
#include "BLASRegister.h"
#include "DirectXBase.h"
#include "BLAS.h"
#include <DirectXMath.h>

#include "Input.h"

void BakeAO::ExecutionBake(const int& TexCount, TLAS& Tlas, KariConstBufferData& KariData, DynamicConstBuffer& ConstBuff, RaytracingOutput& DebugTex)
{

	/*===== �x�C�N�����s =====*/

	// �x�C�N��̃e�N�X�`���𐶐��B
	bakeTex.clear();
	bakeTex.resize(TexCount);

	for (auto& index : bakeTex) {

		index = std::make_shared<RaytracingOutput>();
		index->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	}

	// �x�C�N�������ʂ�UAV��BLAS�ɒǉ��B
	int counter = 0;
	for (auto& index : BLASRegister::Ins()->GetBLAS()) {
		//BakeAO::Ins()->GetBakeTex()[counter]->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS,D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		index->AddTex(BakeAO::Ins()->GetBakeTex()[counter]->GetUAVIndex());
		++counter;
	}

	//// ���[�g�V�O�l�`���𐶐��B
	//rootsignature = std::make_shared<RayRootsignature>();

	//// ���[�g�V�O�l�`����ݒ�B
	//rootsignature = HitGroupMgr::Ins()->GetLocalRootSig(HitGroupMgr::HITGROUP_ID::BAKE_AO_HIT_GROUP);

	//// �p�C�v���C���𐶐��B
	//pipline = std::make_shared<RayPiplineBakeAO>();

	//// �p�C�v���C����ݒ�B
	//vector<RayPiplineShaderData> shaders;
	//shaders.push_back({ "Resource/ShaderFiles/RayTracing/BakeAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	//pipline->Setting(shaders, HitGroupMgr::HITGROUP_ID::BAKE_AO_HIT_GROUP, 1, 1, 1, sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2), 10);

	//// �V�F�[�_�[�e�[�u�����\�z�B
	//pipline->ConstructionShaderTableBakeAO(bakeTex, window_width * 1.0f, window_height * 1.0f);

	//// ���_�Ȃǂ̏����l
	//Vec3 defEyePos = KariData.eye;
	//Vec3 defTargetPos = KariData.eye + Vec3(0, 0, 100);
	////KariData.target = KariData.eye + Vec3(0, 0, 100);

	//// 100��T���v�����O����B
	//bool isEndBake = true;
	//while(isEndBake) {

	//	// ���s����O�����B
	//	DirectXBase::Ins()->processBeforeDrawing();

	//	// �e�N�X�`���̐ݒ��ύX�B
	//	static int firstOne = 0;
	//	if (firstOne == 0) {
	//		for (auto& index : bakeTex) {

	//			index->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	//		}
	//		++firstOne;
	//	}

	//	DebugTex.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	//	// �����̎���X�V�B
	//	KariData.seed = FHelper::GetRand(0, 3000);

	//	//// ���_���ړ�������B
	//	//KariData.eye = defEyePos;
	//	//KariData.target = defTargetPos;
	//	//KariData.eye.x += 1.0f;
	//	//KariData.target.x += 1.0f;

	//	////// �ړ�������ʂ����߂�B
	//	////Vec3 randomMovePos = FHelper::GetRandXMFLOAT3(-1500, 1500);
	//	//KariData.eye += FHelper::GetRandXMFLOAT3(-500, 500);
	//	//KariData.eye.y = FHelper::GetRand(0, 100);
	//	//KariData.target += FHelper::GetRandXMFLOAT3(-3000, 3000);
	//	//KariData.target.y = FHelper::GetRand(0, 2000);

	//	bool isMove = false;

	//	// �J�������X�V�B
	//	Camera::Ins()->Update();

	//	float speed = 5.0f;
	//	float rot = 0.03f;
	//	if (Input::isKey(DIK_W)) {
	//		Camera::Ins()->Move(speed);
	//		isMove = true;
	//	}
	//	if (Input::isKey(DIK_S)) {
	//		Camera::Ins()->Move(-speed);
	//		isMove = true;
	//	}
	//	if (Input::isKey(DIK_A)) {
	//		Camera::Ins()->MoveRight(speed);
	//		isMove = true;
	//	}
	//	if (Input::isKey(DIK_D)) {
	//		Camera::Ins()->MoveRight(-speed);
	//		isMove = true;
	//	}
	//	if (Input::isKey(DIK_UP)) {
	//		Camera::Ins()->forwardVec.y += rot;
	//		isMove = true;
	//	}
	//	if (Input::isKey(DIK_DOWN)) {
	//		Camera::Ins()->forwardVec.y -= rot;
	//		isMove = true;
	//	}
	//	if (Input::isKey(DIK_LEFT)) {
	//		Camera::Ins()->AddRotationXZ(rot);
	//		isMove = true;
	//	}
	//	if (Input::isKey(DIK_RIGHT)) {
	//		Camera::Ins()->AddRotationXZ(-rot);
	//		isMove = true;
	//	}
	//	if (Input::isKey(DIK_LSHIFT)) {
	//		Camera::Ins()->eye.y -= 10.0f;
	//		isMove = true;
	//	}
	//	if (Input::isKey(DIK_SPACE)) {
	//		Camera::Ins()->eye.y += 10.0f;
	//		isMove = true;
	//	}
	//	if (Input::isKey(DIK_1)) {
	//		isEndBake = false;
	//	}

	//	KariData.eye = Camera::Ins()->eye;
	//	KariData.target = Camera::Ins()->target;
	//	KariData.up = Camera::Ins()->up;


	//	KariData.mtxView = XMMatrixLookAtLH(KariData.eye.ConvertXMVECTOR(), KariData.target.ConvertXMVECTOR(), KariData.up.ConvertXMVECTOR());
	//	KariData.mtxViewInv = XMMatrixInverse(nullptr, KariData.mtxView);
	//	auto frameIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();
	//	ConstBuff.Write(frameIndex, &KariData, sizeof(KariConstBufferData));

	//	// �K�v�ȏ���ǉ��B
	//	ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	//	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	//	DirectXBase::Ins()->cmdList->SetComputeRootSignature(pipline->GetGlobalRootSig()->GetRootSig().Get());
	//	DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(Tlas.GetDescriptorHeapIndex()));
	//	DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, ConstBuff.GetBuffer(frameIndex)->GetGPUVirtualAddress());
	//	DebugTex.SetComputeRootDescriptorTalbe(2);
	//	DirectXBase::Ins()->cmdList->SetPipelineState1(pipline->GetStateObject().Get());

	//	DirectXBase::Ins()->cmdList->DispatchRays(&pipline->GetDispatchRayDesc());

	//	DebugTex.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	//	D3D12_RESOURCE_BARRIER barriers[] = {
	//		CD3DX12_RESOURCE_BARRIER::Transition(
	//		DirectXBase::Ins()->backBuffers[frameIndex].Get(),
	//		D3D12_RESOURCE_STATE_RENDER_TARGET,
	//		D3D12_RESOURCE_STATE_COPY_DEST),
	//	};
	//	DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(barriers), barriers);

	//	DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[frameIndex].Get(), DebugTex.GetRaytracingOutput().Get());

	//	// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�����Ƃɖ߂��B
	//	D3D12_RESOURCE_BARRIER endBarriers[] = {

	//	CD3DX12_RESOURCE_BARRIER::Transition(
	//	DirectXBase::Ins()->backBuffers[frameIndex].Get(),
	//	D3D12_RESOURCE_STATE_COPY_DEST,
	//	D3D12_RESOURCE_STATE_RENDER_TARGET)

	//	};
	//	DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(endBarriers), endBarriers);

	//	// ���s��̏����B
	//	DirectXBase::Ins()->processAfterDrawing();

	//}

}
