#include "Engine.h"
#include <cassert>
#include "Input.h"
#include <stdexcept>
#include <d3d12sdklayers.h>
#include "WindowsAPI.h"
#include "DescriptorHeapMgr.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

Engine::Engine() {
	backBuffers_.resize(2);
}

#include "RayDenoiser.h"
void Engine::Init() {
#ifdef _DEBUG
	// デバッグレイヤーの有効化
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_))))
	{
		debugController_->EnableDebugLayer();
	}
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&shaderDebugController_))))
	{
		shaderDebugController_->SetEnableGPUBasedValidation(true);
	}
#endif

	// ウィンドウ初期化
	windowsAPI_ = std::make_shared<WindowsAPI>();
	windowsAPI_->Init();

	// DXGIファクトリーの生成
	HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory_));

	// グラフィックスアダプターの選択
	tmpAdapter_ = nullptr;
	for (int i = 0;
		dxgiFactory_->EnumAdapters1(i, &tmpAdapter_) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		adapters_.push_back(tmpAdapter_);			// 動的配列に追加する
	}
	// 追加されたアダプターオブジェクトを仕分ける
	for (int i = 0; i < adapters_.size(); i++)
	{
		DXGI_ADAPTER_DESC1 adesc;
		adapters_[i]->GetDesc1(&adesc);			// アダプターの情報を取得
		// ソフトウェアデバイスを回避
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}
		std::wstring strDesc = adesc.Description;				// アダプター名
		// Intel UHD Graphics（オンボードグラフィック）を回避
		if (strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter_ = adapters_[i];						// 採用
			break;
		}
	}

	// デバイスの生成
	//levels.push_back(D3D_FEATURE_LEVEL_12_1);
	levels_.push_back(D3D_FEATURE_LEVEL_12_0);
	levels_.push_back(D3D_FEATURE_LEVEL_11_1);
	levels_.push_back(D3D_FEATURE_LEVEL_11_0);
	for (auto& index_ : levels_)
	{
		// 採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter_.Get(), index_, IID_PPV_ARGS(&dev_));
		if (result == S_OK)
		{
			// デバイスを生成できた時点でループを抜ける
			featureLevel_ = index_;
			break;
		}
	}
	dev_->SetName(L"Device");

	// DXR サポートしているか確認
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 options{};
	result = dev_->CheckFeatureSupport(
		D3D12_FEATURE_D3D12_OPTIONS5, &options, sizeof(options));
	if (FAILED(result) ||
		options.RaytracingTier == D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
	{
		throw std::runtime_error("DirectX Raytracing not supported.");
	}

	// コマンドアロケータの生成
	result = dev_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&mainGraphicsCmdAllocator_));
	mainGraphicsCmdAllocator_->SetName(L"MainGraphicsCmdAllocator");
	result = dev_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&copyResourceCmdAllocator_));
	copyResourceCmdAllocator_->SetName(L"CopyResourceCmdAllocator");

	// コマンドリストの生成
	result = dev_->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mainGraphicsCmdAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&mainGraphicsCmdList_));
	mainGraphicsCmdList_->SetName(L"GraphicsCmdList");
	result = dev_->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		copyResourceCmdAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&copyResourceCmdList));
	copyResourceCmdList->SetName(L"CopyResourceCmdList");

	// コマンドキューの生成
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	result = dev_->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&graphicsCmdQueue_));
	graphicsCmdQueue_->SetName(L"GraphicsCmdQueue");

	// スワップチェーンの生成
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = 1280;
	swapchainDesc.Height = 720;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			// 色情報の書式
	swapchainDesc.SampleDesc.Count = 1;							// マルチサンプルしない
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;			// バックバッファ用
	swapchainDesc.BufferCount = 2;								// バッファ数を２つに設定
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// フリップ後は破棄
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	result = dxgiFactory_->CreateSwapChainForHwnd(
		graphicsCmdQueue_.Get(),
		windowsAPI_->hwnd_,
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapChain1);
	// 生成したIDXGISwapChain1のオブジェクトを変換する
	swapChain1.As(&swapchain_);

	// レンダーターゲットビューの生成
	heapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;				// レンダーターゲットビュー
	heapDesc_.NumDescriptors = 2;								// 裏表の２つ
	dev_->CreateDescriptorHeap(&heapDesc_,
		IID_PPV_ARGS(&rtvHeaps_));
	// レンダーターゲットビューを裏表2つ分生成
	for (int i = 0; i < 2; i++)
	{
		// スワップチェーンからバッファを取得
		result = swapchain_->GetBuffer(i, IID_PPV_ARGS(&backBuffers_[i]));
		// デスクリプタヒープのハンドルを取得
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			rtvHeaps_->GetCPUDescriptorHandleForHeapStart(), i, dev_->GetDescriptorHandleIncrementSize(heapDesc_.Type));
		// レンダーターゲットビューの生成
		dev_->CreateRenderTargetView(
			backBuffers_[i].Get(),
			nullptr,
			handle);

	}
	backBuffers_[0]->SetName(L"BackBuffer0");
	backBuffers_[1]->SetName(L"BackBuffer1");

	// リソース生成
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
	// リソース生成
	CD3DX12_HEAP_PROPERTIES resourceProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_CLEAR_VALUE resrouceClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, static_cast<FLOAT>(1), static_cast<UINT8>(0));
	result = dev_->CreateCommittedResource(
		&resourceProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&resrouceClearValue,
		IID_PPV_ARGS(&depthBuffer_)
	);
	depthBuffer_->SetName(L"DepthBuffer");

	// 深度バッファビュー生成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	result = dev_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap_));
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev_->CreateDepthStencilView(
		depthBuffer_.Get(),
		&dsvDesc,
		dsvHeap_->GetCPUDescriptorHandleForHeapStart()
	);
	dsvHeap_->SetName(L"DsvHeap");

	// GraphicsとDenoiseのフェンスの生成
	graphicsToDenoiseFenceVal_ = 1;
	result = dev_->CreateFence(graphicsToDenoiseFenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&graphicsToDenoiseFence_));
	graphicsToDenoiseFence_->SetName(L"GraphicsWithDenoiseSynchronizeFence");
	// GraphicsとCPUのフェンスの生成
	GPUtoCPUFenceVal_ = 1;
	result = dev_->CreateFence(GPUtoCPUFenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&GPUtoCPUFence_));
	GPUtoCPUFence_->SetName(L"GraphicsWithCPUSynchronizeFence");
	// DenoiseとGraphicsのフェンスの生成
	denoiseToCopyFenceVal_ = 1;
	result = dev_->CreateFence(denoiseToCopyFenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&denoiseToCopyFence_));
	denoiseToCopyFence_->SetName(L"DenoiseToCopyFence");
	// Copy終了監視用のフェンスの生成
	finishCopyFenceVal_ = 1;
	result = dev_->CreateFence(finishCopyFenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&finishCopyFence_));
	finishCopyFence_->SetName(L"FinishCopyFence");

	// DirectInputオブジェクトの生成
	result = DirectInput8Create(
		windowsAPI_->windowClass_.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput_, nullptr);

	// キーボードデバイスの生成
	result = dinput_->CreateDevice(GUID_SysKeyboard, &devkeybord_, NULL);

	// 入力データの形式セット
	result = devkeybord_->SetDataFormat(&c_dfDIKeyboard);

	// 排他制御レベルのセット
	result = devkeybord_->SetCooperativeLevel(
		windowsAPI_->hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	// マウスデバイスの生成
	result = dinput_->CreateDevice(GUID_SysMouse, &devmouse_, NULL);

	// 入力データの形式セット
	result = devmouse_->SetDataFormat(&c_dfDIMouse);

	// 排他制御レベルのセット
	result = devmouse_->SetCooperativeLevel(
		windowsAPI_->hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);


	// imguiの生成
	heapForImgui_ = CreateDescriptorHeaoForImgui();
	// imguiの初期化
	if (ImGui::CreateContext() == nullptr) {
		assert(0);
	}
	// windows用の初期化
	bool blnResult = ImGui_ImplWin32_Init(windowsAPI_->hwnd_);
	if (!blnResult) {
		assert(0);
	}

	// directX12用の初期化
	blnResult = ImGui_ImplDX12_Init(dev_.Get(), 3, DXGI_FORMAT_R8G8B8A8_UNORM, heapForImgui_.Get(),
		heapForImgui_.Get()->GetCPUDescriptorHandleForHeapStart(), heapForImgui_.Get()->GetGPUDescriptorHandleForHeapStart());

	canUseMainGraphicsQueue_ = true;
	canUseDenoiseQueue_ = true;
	canUseCopyQueue_ = true;
	hasFinishedMainGraphicsProcess_ = false;

}

#include "RayDenoiser.h"
void Engine::ProcessBeforeDrawing() {

	// メッセージ確認
	if (PeekMessage(&windowsAPI_->msg_, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&windowsAPI_->msg_);	// キー入力メッセージの処理
		DispatchMessage(&windowsAPI_->msg_);	// プロシージャにメッセージを送る
	}
	// ?ボタンで終了メッセージが来たらゲームループを抜ける
	if (windowsAPI_->msg_.message == WM_QUIT) {
		exit(true);
	}

	// 全キーの入力状態を取得する
	Input::Ins()->Update(devkeybord_, devmouse_);

	// CopyQueueが実行できるときのみレンダーターゲット周りの初期化を行う。
	bool isEndDispatchRayAndDenoise = canUseCopyQueue_ && canUseMainGraphicsQueue_;
	if (canUseCopyQueue_ && isEndDispatchRayAndDenoise) {

		// レンダーターゲットのリソースバリア変更
		UINT bbIndex = swapchain_->GetCurrentBackBufferIndex();
		CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffers_[bbIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		copyResourceCmdList->ResourceBarrier(1, &resourceBarrier);

		// レンダーターゲットの設定
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			rtvHeaps_->GetCPUDescriptorHandleForHeapStart(), bbIndex, dev_->GetDescriptorHandleIncrementSize(heapDesc_.Type));

		// 深度バッファ用のディスクリプタヒープの先頭アドレスを取得
		D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
		copyResourceCmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

		// 画面クリア
		// 初期化色
		float clearColor[] = { 0.35f,0.3f,0.2f,0.0f };
		for (int i = 0; i < 4; ++i) {
			clearColor[i] = clearColor[i] / 255.0f;
		}
		copyResourceCmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		// 深度バッファのクリアコマンド
		copyResourceCmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// ビューポート設定
		CD3DX12_VIEWPORT viewportData = CD3DX12_VIEWPORT(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT);
		copyResourceCmdList->RSSetViewports(1, &viewportData);

		// シザリング矩形設定
		CD3DX12_RECT rectData = CD3DX12_RECT(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		copyResourceCmdList->RSSetScissorRects(1, &rectData);


		// 次にBackBufferに書き込む処理は別のQueueにあるのでステートをCommonに変える。
		resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffers_[bbIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
		copyResourceCmdList->ResourceBarrier(1, &resourceBarrier);

	}

	// imgui描画前処理
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::CreateContext();
	//auto& io = ImGui::GetIO();

	//// Enable docking(available in imgui `docking` branch at the moment)
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	//// Update and Render additional Platform Windows
	//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	//{
	//	ImGui::UpdatePlatformWindows();
	//	ImGui::RenderPlatformWindowsDefault();
	//}

	// ウィンドウ設定
	ImGui::Begin("Menu");
	ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);

}

#include "RayDenoiser.h"
void Engine::ProcessAfterDrawing() {

	ImGui::End();
	ImGui::Render();

	// 各フェンスの値を加算する。
	{

		// GPUからCPU間でのフェンスの値を加算。
		++GPUtoCPUFenceVal_;

		// MainGraphicsQueueからDenoiseQueue間でのフェンスの値を加算。
		++graphicsToDenoiseFenceVal_;

		// DenoiseQueueからCopy間でのフェンスの値を加算。
		++denoiseToCopyFenceVal_;

		// Copyの終了監視用フェンスを加算。
		++finishCopyFenceVal_;

	}

	// DenoiseCmdListの処理
	{

		// DenoiseCmdListが使用可能状態だったら。
		if (canUseDenoiseQueue_) {

			// コンピュートキューのデノイズコマンドを実行。
			Denoiser::Ins()->computeCmdList_->Close();
			ID3D12CommandList* computeCmdLists[] = { Denoiser::Ins()->computeCmdList_.Get() }; // コマンドリストの配列
			Denoiser::Ins()->computeCmdQueue_->Wait(graphicsToDenoiseFence_.Get(), graphicsToDenoiseFenceVal_);	// MainGraphicsの処理が終わってから実行する。
			Denoiser::Ins()->computeCmdQueue_->ExecuteCommandLists(1, computeCmdLists);							// コマンドリストを実行。

			// このコマンドリストを操作不可能にする。
			canUseDenoiseQueue_ = false;

		}

		// デノイズコマンドリストの終了をCopyのフェンスに通知。
		Denoiser::Ins()->computeCmdQueue_->Signal(denoiseToCopyFence_.Get(), denoiseToCopyFenceVal_);

		UINT64 computeFenceValue = graphicsToDenoiseFence_->GetCompletedValue();
		if (computeFenceValue == graphicsToDenoiseFenceVal_ + 1) {

			// コマンドアロケータのリセット
			Denoiser::Ins()->computeCmdAllocator_->Reset();							// キューをクリア

			// コマンドリストのリセット
			Denoiser::Ins()->computeCmdList_->Reset(Denoiser::Ins()->computeCmdAllocator_.Get(), nullptr);	// 再びコマンドリストを貯める準備

			// このコマンドリストを操作可能にする。
			canUseDenoiseQueue_ = true;

		}

	}

	// CopyCmdListの処理
	{

		// CopyCmdListが使用可能状態だったら。
		bool isEndDispatchRayAndDenoise = canUseCopyQueue_ && canUseMainGraphicsQueue_;
		if (canUseCopyQueue_ && isEndDispatchRayAndDenoise) {

			// レンダーターゲットのリソースバリア変更			COMMONとPRESENTはどちらも0だから同じ意味？
			//CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffers_[swapchain_->GetCurrentBackBufferIndex()].Get(),
			//	D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_PRESENT);
			//copyResourceCmdList->ResourceBarrier(1, &resourceBarrier);

			// コピーキューのデノイズコマンドを実行。
			copyResourceCmdList->Close();

			ID3D12CommandList* computeCmdLists[] = { copyResourceCmdList.Get() }; // コマンドリストの配列
			//graphicsCmdQueue_->Wait(graphicsToDenoiseFence_.Get(), graphicsToDenoiseFenceVal_);	// MainGraphicsの処理が終わってから実行する。
			graphicsCmdQueue_->ExecuteCommandLists(1, computeCmdLists);							// コマンドリストを実行。

			// 画面バッファをフリップ
			swapchain_->Present(1, 0);

			// このコマンドリストを操作不可能にする。
			canUseCopyQueue_ = false;

		}

		// コピーコマンドリストの終了をコピー終了監視用のフェンスに通知。
		Denoiser::Ins()->computeCmdQueue_->Signal(finishCopyFence_.Get(), finishCopyFenceVal_);

		UINT64 copyFenceValue = finishCopyFence_->GetCompletedValue();
		if (copyFenceValue == finishCopyFenceVal_ + 1) {

			// コマンドアロケータのリセット
			copyResourceCmdAllocator_->Reset();							// キューをクリア

			// コマンドリストのリセット
			mainGraphicsCmdList_->Reset(copyResourceCmdAllocator_.Get(), nullptr);	// 再びコマンドリストを貯める準備

			// このコマンドリストを操作可能にする。
			canUseCopyQueue_ = true;

		}

	}

	// MainGraphicsCmdListの処理
	{

		// コマンドリストに追加
		mainGraphicsCmdList_->SetDescriptorHeaps(1, heapForImgui_.GetAddressOf());
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mainGraphicsCmdList_.Get());

		// グラフィックコマンドリストのクローズ
		mainGraphicsCmdList_->Close();

		// グラフィックスキューのコマンドを実行。
		ID3D12CommandList* cmdLists[] = { mainGraphicsCmdList_.Get() }; // コマンドリストの配列
		graphicsCmdQueue_->ExecuteCommandLists(1, cmdLists);

		// GPUの実行完了を通知。
		graphicsCmdQueue_->Signal(GPUtoCPUFence_.Get(), GPUtoCPUFenceVal_);
		graphicsCmdQueue_->Signal(graphicsToDenoiseFence_.Get(), graphicsToDenoiseFenceVal_);
		UINT64 graphicsFenceValue = GPUtoCPUFence_->GetCompletedValue();

		// グラフィックコマンドリストの完了待ち
		if (graphicsFenceValue != GPUtoCPUFenceVal_) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			GPUtoCPUFence_->SetEventOnCompletion(GPUtoCPUFenceVal_, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);

		}

		// コマンドアロケータのリセット
		mainGraphicsCmdAllocator_->Reset();							// キューをクリア

		// コマンドリストのリセット
		mainGraphicsCmdList_->Reset(mainGraphicsCmdAllocator_.Get(), nullptr);	// 再びコマンドリストを貯める準備

	}

}

void Engine::SetRenderTarget()
{
	// レンダーターゲットのリソースバリア変更
	UINT bbIndex = Engine::Ins()->swapchain_->GetCurrentBackBufferIndex();
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &resourceBarrier);

	resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &resourceBarrier);
	// レンダーターゲットの設定
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		Engine::Ins()->rtvHeaps_->GetCPUDescriptorHandleForHeapStart(), bbIndex, Engine::Ins()->dev_->GetDescriptorHandleIncrementSize(Engine::Ins()->heapDesc_.Type));

	CD3DX12_CPU_DESCRIPTOR_HANDLE backBufferHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(Engine::Ins()->rtvHeaps_->GetCPUDescriptorHandleForHeapStart(),
		Engine::Ins()->swapchain_->GetCurrentBackBufferIndex(), Engine::Ins()->dev_->GetDescriptorHandleIncrementSize(Engine::Ins()->heapDesc_.Type));
	D3D12_CPU_DESCRIPTOR_HANDLE dsvStartHandle = Engine::Ins()->dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	Engine::Ins()->mainGraphicsCmdList_->OMSetRenderTargets(1, &backBufferHandle, false, &dsvStartHandle);
	// レンダーターゲットのクリア
	float clearColor[] = { 0.5f,0.5f,0.5f,0.0f };
	Engine::Ins()->mainGraphicsCmdList_->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	// 深度バッファのクリアコマンド
	Engine::Ins()->mainGraphicsCmdList_->ClearDepthStencilView(Engine::Ins()->dsvHeap_->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void Engine::ResourceBarrierAfter()
{
	// レンダーターゲットのリソースバリア変更
	CD3DX12_RESOURCE_BARRIER resrouceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffers_[swapchain_->GetCurrentBackBufferIndex()].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	mainGraphicsCmdList_->ResourceBarrier(1, &resrouceBarrier);
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Engine::CreateDescriptorHeaoForImgui()
{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> ret;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	dev_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(ret.ReleaseAndGetAddressOf()));

	return ret;
}