#include "RayEngine.h"
#include "RaytracingOutput.h"
#include "TLAS.h"
#include "HitGroupMgr.h"

void RayEngine::Setting()
{

	/*===== 生成処理 =====*/

	for (int index = 0; index < 2; ++index) {

		// AO出力用クラスをセット。
		aoOutput_[index] = std::make_shared<RaytracingOutput>();
		aoOutput_[index]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"AOOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		denoiseAOOutput_[index] = std::make_shared<RaytracingOutput>();
		denoiseAOOutput_[index]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseAOOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		// 色出力用クラスをセット。
		colorOutput_[index] = std::make_shared<RaytracingOutput>();
		colorOutput_[index]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"ColorOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		// 明るさ情報出力用クラスをセット。
		lightOutput_[index] = std::make_shared<RaytracingOutput>();
		lightOutput_[index]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"LightOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		denoiseLightOutput_[index] = std::make_shared<RaytracingOutput>();
		denoiseLightOutput_[index]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseLightOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		// GI出力用クラスをセット。
		giOutput_[index] = std::make_shared<RaytracingOutput>();
		giOutput_[index]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"GIOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		denoiseGiOutput_[index] = std::make_shared<RaytracingOutput>();
		denoiseGiOutput_[index]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseGIOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		// デノイズマスク用クラスをセット。
		denoiseMaskOutput_[index] = std::make_shared<RaytracingOutput>();
		denoiseMaskOutput_[index]->Setting(DXGI_FORMAT_R32G32B32A32_FLOAT, L"DenoiseMaskOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		// 最終出力用クラスをセット。
		denoiseMixTextureOutput_[index] = std::make_shared<RaytracingOutput>();
		denoiseMixTextureOutput_[index]->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseMixTextureOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	}

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

	currentUAVIndex_ = 0;
	frameIndex_ = 0;

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

	// 現在のUAVインデックスを切り替える。
	currentUAVIndex_ = currentUAVIndex_ ? 0 : 1;

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
	if (Engine::Ins()->canUseDenoiseQueue_) {
		Engine::Ins()->computeCmdList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		Engine::Ins()->computeCmdList_->SetComputeRootSignature(pipeline_->GetGlobalRootSig()->GetRootSig().Get());
	}

	// TLASを設定。
	Engine::Ins()->mainGraphicsCmdList_->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(RayEngine::Ins()->tlas_->GetDescriptorHeapIndex()));

	// 定数バッファをセット
	Engine::Ins()->mainGraphicsCmdList_->SetComputeRootConstantBufferView(1, constBuffer_->GetBuffer(backBufferIndex)->GetGPUVirtualAddress());

	// 出力用UAVを設定。
	aoOutput_[currentUAVIndex_]->SetComputeRootDescriptorTalbe(2);		// AOの結果出力用
	lightOutput_[currentUAVIndex_]->SetComputeRootDescriptorTalbe(3);	// ライトの明るさの結果出力用
	colorOutput_[currentUAVIndex_]->SetComputeRootDescriptorTalbe(4);	// テクスチャの色情報出力用
	giOutput_[currentUAVIndex_]->SetComputeRootDescriptorTalbe(5);		// giの結果出力用
	denoiseMaskOutput_[currentUAVIndex_]->SetComputeRootDescriptorTalbe(6);// デノイズをする際のマスク出力用

	// パイプラインを設定。
	Engine::Ins()->mainGraphicsCmdList_->SetPipelineState1(pipeline_->GetStateObject().Get());

	// レイトレーシングを実行。
	D3D12_DISPATCH_RAYS_DESC rayDesc = pipeline_->GetDispatchRayDesc();
	Engine::Ins()->mainGraphicsCmdList_->DispatchRays(&rayDesc);


	// DenoiseQueueが実行可能状態だったら。
	if (Engine::Ins()->canUseDenoiseQueue_) {


		// ライト情報にデノイズをかける。
		{

			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				lightOutput_[!currentUAVIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseLightOutput_[!currentUAVIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseMaskOutput_[!currentUAVIndex_]->GetRaytracingOutput().Get())
			};

			Engine::Ins()->computeCmdList_->ResourceBarrier(3, barrierToUAV);

			// ライトにデノイズをかける。
			Denoiser::Ins()->Denoise(lightOutput_[!currentUAVIndex_]->GetUAVIndex(), denoiseLightOutput_[!currentUAVIndex_]->GetUAVIndex(), denoiseMaskOutput_[!currentUAVIndex_]->GetUAVIndex(), 0, 1);

		}

		// AO情報にデノイズをかける。
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				aoOutput_[!currentUAVIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseAOOutput_[!currentUAVIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseMaskOutput_[!currentUAVIndex_]->GetRaytracingOutput().Get())
			};

			Engine::Ins()->computeCmdList_->ResourceBarrier(3, barrierToUAV);

			// AOにデノイズをかける。
			Denoiser::Ins()->Denoise(aoOutput_[!currentUAVIndex_]->GetUAVIndex(), denoiseAOOutput_[!currentUAVIndex_]->GetUAVIndex(), denoiseMaskOutput_[!currentUAVIndex_]->GetUAVIndex(), 1000, 8);
		}


		// GI情報にデノイズをかける。
		{
			D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
				giOutput_[!currentUAVIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseGiOutput_[!currentUAVIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
				denoiseMaskOutput_[!currentUAVIndex_]->GetRaytracingOutput().Get())
			};

			Engine::Ins()->computeCmdList_->ResourceBarrier(3, barrierToUAV);

			// GIにデノイズをかける。
			Denoiser::Ins()->Denoise(giOutput_[!currentUAVIndex_]->GetUAVIndex(), denoiseGiOutput_[!currentUAVIndex_]->GetUAVIndex(), denoiseMaskOutput_[!currentUAVIndex_]->GetUAVIndex(), 100, 1);

		}

		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			colorOutput_[!currentUAVIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseAOOutput_[!currentUAVIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseLightOutput_[!currentUAVIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseGiOutput_[!currentUAVIndex_]->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			denoiseMixTextureOutput_[!currentUAVIndex_]->GetRaytracingOutput().Get())
		};

		Engine::Ins()->computeCmdList_->ResourceBarrier(5, barrierToUAV);

		// デノイズをかけたライティング情報と色情報を混ぜる。
		Denoiser::Ins()->MixColorAndLuminance(colorOutput_[!currentUAVIndex_]->GetUAVIndex(), denoiseAOOutput_[!currentUAVIndex_]->GetUAVIndex(), denoiseLightOutput_[!currentUAVIndex_]->GetUAVIndex(), denoiseGiOutput_[!currentUAVIndex_]->GetUAVIndex(), denoiseMixTextureOutput_[!currentUAVIndex_]->GetUAVIndex());

	}


	// 最初のFはコピーリソースを行わない。 && CopyCmdListが実行可能状態だったら。
	if (frameIndex_ != 0 && Engine::Ins()->canUseCopyQueue_) {

		// バックバッファのインデックスを取得する。
		UINT backBufferIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();

		D3D12_RESOURCE_BARRIER barriers[] = {
			CD3DX12_RESOURCE_BARRIER::Transition(
			Engine::Ins()->swapchain_.backBuffers_[backBufferIndex].Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST),
		};
		Engine::Ins()->copyResourceCmdList->ResourceBarrier(_countof(barriers), barriers);

		denoiseMixTextureOutput_[!currentUAVIndex_]->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, Engine::Ins()->copyResourceCmdList);

		Engine::Ins()->copyResourceCmdList->CopyResource(Engine::Ins()->swapchain_.backBuffers_[backBufferIndex].Get(), denoiseMixTextureOutput_[!currentUAVIndex_]->GetRaytracingOutput().Get());

		denoiseMixTextureOutput_[!currentUAVIndex_]->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, Engine::Ins()->copyResourceCmdList);

		// レンダーターゲットのリソースバリアをもとに戻す。
		D3D12_RESOURCE_BARRIER endBarriers[] = {

		CD3DX12_RESOURCE_BARRIER::Transition(
		Engine::Ins()->swapchain_.backBuffers_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_COMMON)

		};

		Engine::Ins()->copyResourceCmdList->ResourceBarrier(_countof(endBarriers), endBarriers);

	}
	++frameIndex_;

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
