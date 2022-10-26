#include "RayEngine.h"
#include "RaytracingOutput.h"
#include "TLAS.h"
#include "HitGroupMgr.h"

void RayEngine::Setting()
{

	/*===== 生成処理 =====*/

	// AO出力用クラスをセット。
	aoOutput_[0] = std::make_shared<RaytracingOutput>();
	aoOutput_[0]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"AOOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseAOOutput_[0] = std::make_shared<RaytracingOutput>();
	denoiseAOOutput_[0]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseAOOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	aoOutput_[1] = std::make_shared<RaytracingOutput>();
	aoOutput_[1]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"AOOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseAOOutput_[1] = std::make_shared<RaytracingOutput>();
	denoiseAOOutput_[1]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseAOOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// 色出力用クラスをセット。
	colorOutput_[0] = std::make_shared<RaytracingOutput>();
	colorOutput_[0]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"ColorOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	colorOutput_[1] = std::make_shared<RaytracingOutput>();
	colorOutput_[1]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"ColorOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// 明るさ情報出力用クラスをセット。
	lightOutput_[0] = std::make_shared<RaytracingOutput>();
	lightOutput_[0]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"LightOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseLightOutput_[0] = std::make_shared<RaytracingOutput>();
	denoiseLightOutput_[0]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseLightOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	lightOutput_[1] = std::make_shared<RaytracingOutput>();
	lightOutput_[1]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"LightOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseLightOutput_[1] = std::make_shared<RaytracingOutput>();
	denoiseLightOutput_[1]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseLightOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// GI出力用クラスをセット。
	giOutput_[0] = std::make_shared<RaytracingOutput>();
	giOutput_[0]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"GIOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseGiOutput_[0] = std::make_shared<RaytracingOutput>();
	denoiseGiOutput_[0]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseGIOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	giOutput_[1] = std::make_shared<RaytracingOutput>();
	giOutput_[1]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"GIOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseGiOutput_[1] = std::make_shared<RaytracingOutput>();
	denoiseGiOutput_[1]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseGIOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// デノイズマスク用クラスをセット。
	denoiseMaskOutput_[0] = std::make_shared<RaytracingOutput>();
	denoiseMaskOutput_[0]->Setting(DXGI_FORMAT_R32G32B32A32_FLOAT, L"DenoiseMaskOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseMaskOutput_[1] = std::make_shared<RaytracingOutput>();
	denoiseMaskOutput_[1]->Setting(DXGI_FORMAT_R32G32B32A32_FLOAT, L"DenoiseMaskOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// 最終出力用クラスをセット。
	denoiseMixTextureOutput_[0] = std::make_shared<RaytracingOutput>();
	denoiseMixTextureOutput_[0]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseMixTextureOutput0", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseMixTextureOutput_[1] = std::make_shared<RaytracingOutput>();
	denoiseMixTextureOutput_[1]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseMixTextureOutput1", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// デノイズAO用のパイプラインを設定。
	pipelineShaders_.push_back({ "Resource/ShaderFiles/RayTracing/DenoiseAOShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	int payloadSize = sizeof(float) * 4 + sizeof(Vec3) * 4 + sizeof(int) * 2 + sizeof(Vec2);
	pipeline_ = std::make_shared<RaytracingPipeline>();
	pipeline_->Setting(pipelineShaders_, HitGroupMgr::DEF, 1, 1, 5, payloadSize, sizeof(Vec2), 6);

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
#include "Camera.h"
void RayEngine::Draw()
{

	/*===== 描画処理 =====*/

	// カメラ行列を更新。
	auto backBufferIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();
	constBufferData_.camera_.mtxView_ = Camera::Ins()->matView_;
	constBufferData_.camera_.mtxViewInv_ = DirectX::XMMatrixInverse(nullptr, constBufferData_.camera_.mtxView_);
	constBufferData_.camera_.mtxProj_ = Camera::Ins()->matPerspective_;
	constBufferData_.camera_.mtxProjInv_ = DirectX::XMMatrixInverse(nullptr, constBufferData_.camera_.mtxProj_);

	// 定数バッファをセット。
	constBuffer_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), &constBufferData_, sizeof(constBufferData_));


	// グローバルルートシグネチャで使うと宣言しているリソースらをセット。
	ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	Engine::Ins()->mainGraphicsCmdList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	Engine::Ins()->mainGraphicsCmdList_->SetComputeRootSignature(pipeline_->GetGlobalRootSig()->GetRootSig().Get());
	// コンピュートキューにも詰む。
	if (Engine::Ins()->canUseDenoiseCmdList_[Engine::Ins()->currentQueueIndex_]) {
		Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->SetComputeRootSignature(pipeline_->GetGlobalRootSig()->GetRootSig().Get());
	}

	// TLASを設定。
	Engine::Ins()->mainGraphicsCmdList_->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(RayEngine::Ins()->tlas_->GetDescriptorHeapIndex()));

	// 定数バッファをセット
	Engine::Ins()->mainGraphicsCmdList_->SetComputeRootConstantBufferView(1, constBuffer_->GetBuffer(backBufferIndex)->GetGPUVirtualAddress());

	// 出力用UAVを設定。
	aoOutput_[Engine::Ins()->currentQueueIndex_]->SetComputeRootDescriptorTalbe(2);		// AOの結果出力用
	lightOutput_[Engine::Ins()->currentQueueIndex_]->SetComputeRootDescriptorTalbe(3);	// ライトの明るさの結果出力用
	colorOutput_[Engine::Ins()->currentQueueIndex_]->SetComputeRootDescriptorTalbe(4);	// テクスチャの色情報出力用
	giOutput_[Engine::Ins()->currentQueueIndex_]->SetComputeRootDescriptorTalbe(5);		// giの結果出力用
	denoiseMaskOutput_[Engine::Ins()->currentQueueIndex_]->SetComputeRootDescriptorTalbe(6);// デノイズをする際のマスク出力用

	// パイプラインを設定。
	Engine::Ins()->mainGraphicsCmdList_->SetPipelineState1(pipeline_->GetStateObject().Get());

	// レイトレーシングを実行。
	D3D12_DISPATCH_RAYS_DESC rayDesc = pipeline_->GetDispatchRayDesc();
	Engine::Ins()->mainGraphicsCmdList_->DispatchRays(&rayDesc);


	// DenoiseQueueが実行可能状態だったら。
	if (Engine::Ins()->canUseDenoiseCmdList_[Engine::Ins()->currentQueueIndex_]) {


		// ライト情報にデノイズをかける。
		{

			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				lightOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseLightOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseMaskOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get())
			};

			Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->ResourceBarrier(3, barrierToUAV);

			// ライトにデノイズをかける。
			Denoiser::Ins()->Denoise(lightOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseLightOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseMaskOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), 0, 1);

		}

		// AO情報にデノイズをかける。
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				aoOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseAOOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseMaskOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get())
			};

			Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->ResourceBarrier(3, barrierToUAV);

			// AOにデノイズをかける。
			Denoiser::Ins()->Denoise(aoOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseAOOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseMaskOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), 1000, 8);
		}


		// GI情報にデノイズをかける。
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				giOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseGiOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseMaskOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get())
			};

			Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->ResourceBarrier(3, barrierToUAV);

			// GIにデノイズをかける。
			Denoiser::Ins()->Denoise(giOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseGiOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseMaskOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), 100, 1);

		}

		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			colorOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseAOOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseLightOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseGiOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseMixTextureOutput_[Engine::Ins()->currentQueueIndex_]->GetRaytracingOutput().Get())
		};

		Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]->ResourceBarrier(5, barrierToUAV);

		// デノイズをかけたライティング情報と色情報を混ぜる。
		Denoiser::Ins()->MixColorAndLuminance(colorOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseAOOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseLightOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseGiOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex(), denoiseMixTextureOutput_[Engine::Ins()->currentQueueIndex_]->GetUAVIndex());

	}

	// 最初のFはコピーリソースを行わない。
	//if (Engine::Ins()->frameIndex_ != 0) {

		D3D12_RESOURCE_BARRIER barriers[] = {
			CD3DX12_RESOURCE_BARRIER::Transition(
			Engine::Ins()->swapchain_.backBuffers_[backBufferIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_COPY_DEST),
		};
		Engine::Ins()->copyResourceCmdList_->ResourceBarrier(_countof(barriers), barriers);

		denoiseMixTextureOutput_[Engine::Ins()->pastQueueIndex_]->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, Engine::Ins()->copyResourceCmdList_);

		Engine::Ins()->copyResourceCmdList_->CopyResource(Engine::Ins()->swapchain_.backBuffers_[backBufferIndex].Get(), denoiseMixTextureOutput_[Engine::Ins()->pastQueueIndex_]->GetRaytracingOutput().Get());

		denoiseMixTextureOutput_[Engine::Ins()->pastQueueIndex_]->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, Engine::Ins()->copyResourceCmdList_);

		// レンダーターゲットのリソースバリアをもとに戻す。
		D3D12_RESOURCE_BARRIER endBarriers[] = {

		CD3DX12_RESOURCE_BARRIER::Transition(
		Engine::Ins()->swapchain_.backBuffers_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PRESENT)

		};

		Engine::Ins()->copyResourceCmdList_->ResourceBarrier(_countof(endBarriers), endBarriers);


	//}

	// RayDenoiserの描画後処理。
	//Denoiser::Ins()->AfterDraw();

}

void RayEngine::NoiseDraw()
{

	/*===== 描画処理 =====*/

	//// カメラ行列を更新。
	//auto frameIndex = Engine::Ins()->swapchain_->GetCurrentBackBufferIndex();
	//constBufferData_.camera_.mtxView_ = Camera::Ins()->matView_;
	//constBufferData_.camera_.mtxViewInv_ = DirectX::XMMatrixInverse(nullptr, constBufferData_.camera_.mtxView_);
	//constBufferData_.camera_.mtxProj_ = Camera::Ins()->matPerspective_;
	//constBufferData_.camera_.mtxProjInv_ = DirectX::XMMatrixInverse(nullptr, constBufferData_.camera_.mtxProj_);

	//// 定数バッファをセット。
	//constBuffer_->Write(Engine::Ins()->swapchain_->GetCurrentBackBufferIndex(), &constBufferData_, sizeof(constBufferData_));


	//// グローバルルートシグネチャで使うと宣言しているリソースらをセット。
	//ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get() };
	//Engine::Ins()->cmdList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	//Engine::Ins()->cmdList_->SetComputeRootSignature(pipeline_->GetGlobalRootSig()->GetRootSig().Get());

	//// TLASを設定。
	//Engine::Ins()->cmdList_->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(RayEngine::Ins()->tlas_->GetDescriptorHeapIndex()));

	//// 定数バッファをセット
	//Engine::Ins()->cmdList_->SetComputeRootConstantBufferView(1, constBuffer_->GetBuffer(frameIndex)->GetGPUVirtualAddress());

	//// バリアを設定し各リソースの状態を遷移させる.
	//aoOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	//denoiseAOOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	//lightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	//denoiseLightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	//colorOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	//giOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	//denoiseGiOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	//denoiseMaskOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	//// 出力用UAVを設定。
	//aoOutput_->SetComputeRootDescriptorTalbe(2);		// AOの結果出力用
	//lightOutput_->SetComputeRootDescriptorTalbe(3);	// ライトの明るさの結果出力用
	//colorOutput_->SetComputeRootDescriptorTalbe(4);	// テクスチャの色情報出力用
	//giOutput_->SetComputeRootDescriptorTalbe(5);		// giの結果出力用
	//denoiseMaskOutput_->SetComputeRootDescriptorTalbe(6);// デノイズをする際のマスク出力用

	//// パイプラインを設定。
	//Engine::Ins()->cmdList_->SetPipelineState1(pipeline_->GetStateObject().Get());

	//// レイトレーシングを実行。
	//D3D12_DISPATCH_RAYS_DESC rayDesc = pipeline_->GetDispatchRayDesc();
	//Engine::Ins()->cmdList_->DispatchRays(&rayDesc);

	//denoiseMixTextureOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	//D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
	//	colorOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
	//	aoOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
	//	lightOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
	//	giOutput_->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
	//	denoiseMixTextureOutput_->GetRaytracingOutput().Get())
	//};

	//Engine::Ins()->cmdList_->ResourceBarrier(5, barrierToUAV);

	//// デノイズをかけたライティング情報と色情報を混ぜる。
	//Denoiser::Ins()->MixColorAndLuminance(colorOutput_->GetUAVIndex(), aoOutput_->GetUAVIndex(), lightOutput_->GetUAVIndex(), giOutput_->GetUAVIndex(), denoiseMixTextureOutput_->GetUAVIndex());
	//denoiseMixTextureOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	//// バックバッファのインデックスを取得する。
	//UINT backBufferIndex = Engine::Ins()->swapchain_->GetCurrentBackBufferIndex();

	//D3D12_RESOURCE_BARRIER barriers[] = {
	//	CD3DX12_RESOURCE_BARRIER::Transition(
	//	Engine::Ins()->backBuffers_[backBufferIndex].Get(),
	//	D3D12_RESOURCE_STATE_RENDER_TARGET,
	//	D3D12_RESOURCE_STATE_COPY_DEST),
	//};
	//Engine::Ins()->cmdList_->ResourceBarrier(_countof(barriers), barriers);

	//Engine::Ins()->cmdList_->CopyResource(Engine::Ins()->backBuffers_[backBufferIndex].Get(), denoiseMixTextureOutput_->GetRaytracingOutput().Get());

	//// レンダーターゲットのリソースバリアをもとに戻す。
	//D3D12_RESOURCE_BARRIER endBarriers[] = {

	//CD3DX12_RESOURCE_BARRIER::Transition(
	//Engine::Ins()->backBuffers_[backBufferIndex].Get(),
	//D3D12_RESOURCE_STATE_COPY_DEST,
	//D3D12_RESOURCE_STATE_RENDER_TARGET)

	//};

	//Engine::Ins()->cmdList_->ResourceBarrier(_countof(endBarriers), endBarriers);

	//// バリアを設定し各リソースの状態を遷移させる.
	//aoOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	//denoiseAOOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	//lightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	//denoiseLightOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	//colorOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	//giOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	//denoiseGiOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	//denoiseMaskOutput_->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

}
