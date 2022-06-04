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

void BakeAO::ExecutionBake(const int& TexCount, TLAS& Tlas, KariConstBufferData& KariData, DynamicConstBuffer& ConstBuff, RaytracingOutput& DebugTex)
{

	/*===== ベイクを実行 =====*/

	// ベイク先のテクスチャを生成。
	bakeTex.clear();
	bakeTex.resize(TexCount);

	for (auto& index : bakeTex) {

		index = std::make_shared<RaytracingOutput>();
		index->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	}

	// ベイクした結果のUAVをBLASに追加。
	int counter = 0;
	for (auto& index : BLASRegister::Ins()->GetBLAS()) {
		//BakeAO::Ins()->GetBakeTex()[counter]->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS,D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		index->AddTex(BakeAO::Ins()->GetBakeTex()[counter]->GetUAVIndex());
		++counter;
	}

	// ルートシグネチャを生成。
	rootsignature = std::make_shared<RayRootsignature>();

	// ルートシグネチャを設定。
	rootsignature = HitGroupMgr::Ins()->GetLocalRootSig(HitGroupMgr::HITGROUP_ID::BAKE_AO_HIT_GROUP);

	// パイプラインを生成。
	pipline = std::make_shared<RayPiplineBakeAO>();

	// パイプラインを設定。
	vector<RayPiplineShaderData> shaders;
	shaders.push_back({ "Resource/ShaderFiles/RayTracing/BakeAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	pipline->Setting(shaders, HitGroupMgr::HITGROUP_ID::BAKE_AO_HIT_GROUP, 1, 1, 1, sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2), 4);

	// シェーダーテーブルを構築。
	pipline->ConstructionShaderTableBakeAO(bakeTex);

	// 視点などの初期値
	Vec3 defEyePos = KariData.eye;
	Vec3 defTargetPos = KariData.target;

	// 100回サンプリングする。
	for (int index = 0; index < 50; ++index) {

		// 実行する前準備。
		DirectXBase::Ins()->processBeforeDrawing();

		// テクスチャの設定を変更。
		static int firstOne = 0;
		if (firstOne == 0) {
			for (auto& index : bakeTex) {

				index->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

			}
			++firstOne;
		}

		DebugTex.SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		// 乱数の種を更新。
		KariData.seed = FHelper::GetRand(0, 3000);

		// 視点を移動させる。
		KariData.eye = defEyePos;
		KariData.target = defTargetPos;

		// 移動させる量を求める。
		Vec3 randomMovePos = FHelper::GetRandXMFLOAT3(-1000, 1000);
		//KariData.eye += FHelper::GetRandXMFLOAT3(-1000, 1000);
		KariData.target += FHelper::GetRandXMFLOAT3(-4000, 4000);
		KariData.mtxView = XMMatrixLookAtLH(KariData.eye.ConvertXMVECTOR(), KariData.target.ConvertXMVECTOR(), KariData.up.ConvertXMVECTOR());
		KariData.mtxViewInv = XMMatrixInverse(nullptr, KariData.mtxView);
		auto frameIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();
		ConstBuff.Write(frameIndex, &KariData, sizeof(KariConstBufferData));

		// 必要な情報を追加。
		ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
		DirectXBase::Ins()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		DirectXBase::Ins()->cmdList->SetComputeRootSignature(pipline->GetGlobalRootSig()->GetRootSig().Get());
		DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(Tlas.GetDescriptorHeapIndex()));
		DirectXBase::Ins()->cmdList->SetComputeRootConstantBufferView(1, ConstBuff.GetBuffer(frameIndex)->GetGPUVirtualAddress());
		DebugTex.SetComputeRootDescriptorTalbe(2);
		DirectXBase::Ins()->cmdList->SetPipelineState1(pipline->GetStateObject().Get());

		DirectXBase::Ins()->cmdList->DispatchRays(&pipline->GetDispatchRayDesc());

		DebugTex.SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

		D3D12_RESOURCE_BARRIER barriers[] = {
			CD3DX12_RESOURCE_BARRIER::Transition(
			DirectXBase::Ins()->backBuffers[frameIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_COPY_DEST),
		};
		DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(barriers), barriers);

		DirectXBase::Ins()->cmdList->CopyResource(DirectXBase::Ins()->backBuffers[frameIndex].Get(), DebugTex.GetRaytracingOutput().Get());

		// レンダーターゲットのリソースバリアをもとに戻す。
		D3D12_RESOURCE_BARRIER endBarriers[] = {

		CD3DX12_RESOURCE_BARRIER::Transition(
		DirectXBase::Ins()->backBuffers[frameIndex].Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_RENDER_TARGET)

		};
		DirectXBase::Ins()->cmdList->ResourceBarrier(_countof(endBarriers), endBarriers);

		// 実行後の処理。
		DirectXBase::Ins()->processAfterDrawing();

	}

}
