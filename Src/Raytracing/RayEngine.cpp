#include "RayEngine.h"
#include "RaytracingOutput.h"
#include "TLAS.h"
#include "HitGroupMgr.h"

void RayEngine::Setting()
{

	/*===== 生成処理 =====*/

	// 色出力用クラスをセット。
	colorOutput_[0] = std::make_shared<RaytracingOutput>();
	colorOutput_[0]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"ColorOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	colorOutput_[0]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	colorOutput_[1] = std::make_shared<RaytracingOutput>();
	colorOutput_[1]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"ColorOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	colorOutput_[1]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// 明るさ情報出力用クラスをセット。
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

	// デノイズマスク用クラスをセット。
	denoiseMaskOutput_[0] = std::make_shared<RaytracingOutput>();
	denoiseMaskOutput_[0]->Setting(DXGI_FORMAT_R11G11B10_FLOAT, L"DenoiseMaskOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	denoiseMaskOutput_[0]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseMaskOutput_[1] = std::make_shared<RaytracingOutput>();
	denoiseMaskOutput_[1]->Setting(DXGI_FORMAT_R11G11B10_FLOAT, L"DenoiseMaskOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	denoiseMaskOutput_[1]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// デノイズ最終出力用クラスをセット。
	denoiseMixTextureOutput_[0] = std::make_shared<RaytracingOutput>();
	denoiseMixTextureOutput_[0]->Setting(DXGI_FORMAT_R11G11B10_FLOAT, L"DenoiseMixTextureOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	denoiseMixTextureOutput_[0]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseMixTextureOutput_[1] = std::make_shared<RaytracingOutput>();
	denoiseMixTextureOutput_[1]->Setting(DXGI_FORMAT_R11G11B10_FLOAT, L"DenoiseMixTextureOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	denoiseMixTextureOutput_[1]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// ブルーム用エミッシブ
	emissiveOutput_[0] = std::make_shared<RaytracingOutput>();
	emissiveOutput_[0]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"EmissiveOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	emissiveOutput_[0]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	emissiveOutput_[1] = std::make_shared<RaytracingOutput>();
	emissiveOutput_[1]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"EmissiveOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	emissiveOutput_[1]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// 最終出力用クラスをセット。
	finalOutputTexture_[0] = std::make_shared<RaytracingOutput>();
	finalOutputTexture_[0]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"FinalOutputTexture0", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	finalOutputTexture_[0]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	finalOutputTexture_[1] = std::make_shared<RaytracingOutput>();
	finalOutputTexture_[1]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"FinalOutputTexture1", Vec2(1280, 720), D3D12_RESOURCE_STATE_COMMON);
	finalOutputTexture_[1]->SetResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// デノイズAO用のパイプラインを設定。
	pipelineShaders_.push_back({ "Resource/ShaderFiles/RayTracing/RaytracingShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	int payloadSize = sizeof(float) * 4 + sizeof(Vec3) * 4 + sizeof(UINT) * 4;
	pipeline_ = std::make_shared<RaytracingPipeline>();
	pipeline_->Setting(pipelineShaders_, HitGroupMgr::DEF, 1, 1, 4, payloadSize, sizeof(Vec2), 6);

	// シェーダーテーブルを生成。
	pipeline_->ConstructionShaderTable();

	// 定数バッファを生成。
	constBufferData_.Init();
	constBuffer_ = std::make_shared<DynamicConstBuffer>();
	constBuffer_->Generate(sizeof(RayConstBufferData), L"CameraConstBuffer");
	constBuffer_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), &constBufferData_, sizeof(RayConstBufferData));

}

void RayEngine::SettingTLAS()
{

	/*===== TLASを設定 =====*/

	tlas_ = std::make_shared<TLAS>();
	tlas_->GenerateTLAS();

}

void RayEngine::Update()
{

	/*===== 更新処理 =====*/

	// BLASの情報を変更。いずれは変更した箇所のみ書き換えられるようにしたい。
	pipeline_->MapHitGroupInfo();

	// TLASを更新。
	tlas_->Update();

}

#include "RayDenoiser.h"
#include "Bloom.h"
#include "Camera.h"
#include "RadialBlur.h"
void RayEngine::Draw()
{

	/*===== 描画処理 =====*/

	// レイトレーシングを実行。
	TraceRay();

	// デノイズコマンドを積む。
	DenoiseCommand();

	// コピーコマンドに命令を積む。
	CopyCommand();

}

void RayEngine::TraceRay()
{

	/*===== レイトレーシングを実行 =====*/

	// カメラ行列を更新。
	auto backBufferIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();
	constBufferData_.camera_.mtxView_ = Camera::Ins()->matView_;
	constBufferData_.camera_.mtxViewInv_ = DirectX::XMMatrixInverse(nullptr, constBufferData_.camera_.mtxView_);
	constBufferData_.camera_.mtxProj_ = Camera::Ins()->matPerspective_;
	constBufferData_.camera_.mtxProjInv_ = DirectX::XMMatrixInverse(nullptr, constBufferData_.camera_.mtxProj_);

	// 定数バッファをセット。
	constBuffer_->Write(Engine::Ins()->currentQueueIndex_, &constBufferData_, sizeof(constBufferData_));


	// グローバルルートシグネチャで使うと宣言しているリソースらをセット。
	ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	Engine::Ins()->mainGraphicsCmdList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	Engine::Ins()->mainGraphicsCmdList_->SetComputeRootSignature(pipeline_->GetGlobalRootSig()->GetRootSig().Get());
	// コンピュートキューにも詰む。
	Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->SetComputeRootSignature(pipeline_->GetGlobalRootSig()->GetRootSig().Get());

	// TLASを設定。
	Engine::Ins()->mainGraphicsCmdList_->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(tlas_->GetDescriptorHeapIndex()));

	// 定数バッファをセット
	Engine::Ins()->mainGraphicsCmdList_->SetComputeRootConstantBufferView(1, constBuffer_->GetBuffer(backBufferIndex)->GetGPUVirtualAddress());

	// 出力用UAVを設定。
	lightOutput_[Engine::Ins()->currentQueueIndex_]->SetComputeRootDescriptorTalbe(2);	// ライトの明るさの結果出力用
	colorOutput_[Engine::Ins()->currentQueueIndex_]->SetComputeRootDescriptorTalbe(3);	// テクスチャの色情報出力用
	denoiseMaskOutput_[Engine::Ins()->currentQueueIndex_]->SetComputeRootDescriptorTalbe(4);// デノイズをする際のマスク出力用
	emissiveOutput_[Engine::Ins()->currentQueueIndex_]->SetComputeRootDescriptorTalbe(5);// デノイズをする際のマスク出力用

	// パイプラインを設定。
	Engine::Ins()->mainGraphicsCmdList_->SetPipelineState1(pipeline_->GetStateObject(Engine::Ins()->currentQueueIndex_).Get());

	// レイトレーシングを実行。
	D3D12_DISPATCH_RAYS_DESC rayDesc = pipeline_->GetDispatchRayDesc(Engine::Ins()->currentQueueIndex_);
	Engine::Ins()->mainGraphicsCmdList_->DispatchRays(&rayDesc);

}

void RayEngine::DenoiseCommand()
{

	/*===== デノイズコマンドを積む =====*/

	// ライト情報にデノイズをかける。
	{

		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			lightOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseLightOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseMaskOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get())
		};

		Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->ResourceBarrier(3, barrierToUAV);

		// ライトにデノイズをかける。
		Denoiser::Ins()->Denoise(lightOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseLightOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseMaskOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), 10, 1);

	}

	// ブルームをかける。
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

	// デノイズをかけたライティング情報と色情報を混ぜる。
	Denoiser::Ins()->MixColorAndLuminance(colorOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(),
		denoiseLightOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(),
		emissiveOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseMixTextureOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex());


	// ラジアルブラーをかける。
	{

		D3D12_RESOURCE_BARRIER radialBlurBarrier[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseMixTextureOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			finalOutputTexture_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get())
		};

		Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->ResourceBarrier(2, radialBlurBarrier);

		// ラジアルブラーをかける。
		RadialBlur::Ins()->Blur(denoiseMixTextureOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), finalOutputTexture_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex());

	}

}

void RayEngine::CopyCommand()
{

	/*===== コピーコマンドを積む =====*/


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

	// レンダーターゲットのリソースバリアをもとに戻す。
	D3D12_RESOURCE_BARRIER endBarriers[] = {

	CD3DX12_RESOURCE_BARRIER::Transition(
	Engine::Ins()->swapchain_.backBuffers_[backBufferIndex].Get(),
	D3D12_RESOURCE_STATE_COPY_DEST,
	D3D12_RESOURCE_STATE_PRESENT)

	};

	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(_countof(endBarriers), endBarriers);

}
