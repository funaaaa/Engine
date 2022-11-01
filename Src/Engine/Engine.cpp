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
	swapchain_.backBuffers_.resize(2);
	isGameEndReservation_ = false;
	isGameEnd_ = false;
}

#include "RayDenoiser.h"
void Engine::Init() {
#ifdef _DEBUG
	// デバッグレイヤーの有効化
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_.debugController_))))
	{
		debug_.debugController_->EnableDebugLayer();
	}
	//if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_.shaderDebugController_))))
	//{
	//	debug_.shaderDebugController_->SetEnableGPUBasedValidation(true);
	//}
#endif

	// ウィンドウ初期化
	windowsAPI_ = std::make_shared<WindowsAPI>();
	windowsAPI_->Init();

	// DXGIファクトリーの生成
	HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&device_.dxgiFactory_));

	// グラフィックスアダプターの選択
	device_.tmpAdapter_ = nullptr;
	for (int i = 0;
		device_.dxgiFactory_->EnumAdapters1(i, &device_.tmpAdapter_) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		device_.adapters_.push_back(device_.tmpAdapter_);	// 動的配列に追加する
	}
	// 追加されたアダプターオブジェクトを仕分ける
	for (int i = 0; i < device_.adapters_.size(); i++)
	{
		DXGI_ADAPTER_DESC1 adesc;
		device_.adapters_[i]->GetDesc1(&adesc);				// アダプターの情報を取得
		// ソフトウェアデバイスを回避
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}
		std::wstring strDesc = adesc.Description;			// アダプター名
		// Intel UHD Graphics（オンボードグラフィック）を回避
		if (strDesc.find(L"Intel") == std::wstring::npos)
		{
			device_.tmpAdapter_ = device_.adapters_[i];		// 採用
			break;
		}
	}

	// デバイスの生成
	//device_.levels.push_back(D3D_FEATURE_LEVEL_12_1);
	device_.levels_.push_back(D3D_FEATURE_LEVEL_12_0);
	device_.levels_.push_back(D3D_FEATURE_LEVEL_11_1);
	device_.levels_.push_back(D3D_FEATURE_LEVEL_11_0);
	for (auto& index_ : device_.levels_)
	{
		// 採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(device_.tmpAdapter_.Get(), index_, IID_PPV_ARGS(&device_.dev_));
		if (result == S_OK)
		{
			// デバイスを生成できた時点でループを抜ける
			device_.featureLevel_ = index_;
			break;
		}
	}
	device_.dev_->SetName(L"Device");

	// DXR サポートしているか確認
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 options{};
	result = device_.dev_->CheckFeatureSupport(
		D3D12_FEATURE_D3D12_OPTIONS5, &options, sizeof(options));
	if (FAILED(result) ||
		options.RaytracingTier == D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
	{
		throw std::runtime_error("DirectX Raytracing not supported.");
	}

	// コマンドアロケータの生成
	{

		// レイトレ関数やUIの描画に使用するコマンドリスト用のAllocatorを生成。
		result = device_.dev_->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&mainGraphicsCmdAllocator_));
		mainGraphicsCmdAllocator_->SetName(L"MainGraphicsCmdAllocator");

		// デノイズ結果をバックバッファにコピーする際に使用するコマンドリスト用のAllocatorを生成。
		result = device_.dev_->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&copyResourceCmdAllocator_));
		copyResourceCmdAllocator_->SetName(L"CopyResourceCmdAllocator");

		// デノイズに使用するコマンドリスト表用のAllocatorを生成。
		result = Engine::Ins()->device_.dev_->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_COMPUTE,
			IID_PPV_ARGS(&denoiseCmdAllocator_[0]));
		denoiseCmdAllocator_[0]->SetName(L"DenoiseAllocator0");

		// デノイズに使用するコマンドリスト裏用のAllocatorを生成。
		result = Engine::Ins()->device_.dev_->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_COMPUTE,
			IID_PPV_ARGS(&denoiseCmdAllocator_[1]));
		denoiseCmdAllocator_[1]->SetName(L"DenoiseAllocator1");

	}

	// コマンドリストの生成
	{

		// レイトレ関数やUIの描画に使用するコマンドリスト
		result = device_.dev_->CreateCommandList(0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			mainGraphicsCmdAllocator_.Get(), nullptr,
			IID_PPV_ARGS(&mainGraphicsCmdList_));
		mainGraphicsCmdList_->SetName(L"GraphicsCmdList");

		// デノイズ結果をバックバッファにコピーする際に使用するコマンドリスト
		result = device_.dev_->CreateCommandList(0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			copyResourceCmdAllocator_.Get(), nullptr,
			IID_PPV_ARGS(&copyResourceCmdList_));
		copyResourceCmdList_->SetName(L"CopyResourceCmdList");

		// デノイズに使用するコマンドリスト表
		result = Engine::Ins()->device_.dev_->CreateCommandList(0,
			D3D12_COMMAND_LIST_TYPE_COMPUTE,
			denoiseCmdAllocator_[0].Get(), nullptr,
			IID_PPV_ARGS(&denoiseCmdList_[0]));
		denoiseCmdList_[0]->SetName(L"DenoiseComputeCmdList0");

		// デノイズに使用するコマンドリスト裏
		result = Engine::Ins()->device_.dev_->CreateCommandList(0,
			D3D12_COMMAND_LIST_TYPE_COMPUTE,
			denoiseCmdAllocator_[1].Get(), nullptr,
			IID_PPV_ARGS(&denoiseCmdList_[1]));
		denoiseCmdList_[1]->SetName(L"DenoiseComputeCmdList1");

	}

	// コマンドキューの生成
	{

		// グラフィックスキューを生成。
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		result = device_.dev_->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&graphicsCmdQueue_));
		graphicsCmdQueue_->SetName(L"GraphicsCmdQueue");

		// デノイズに使用するコンピュートキューの表を生成。
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		result = Engine::Ins()->device_.dev_->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&computeCmdQueue_));
		computeCmdQueue_->SetName(L"ComputeQueue");

	}

	// フェンスの生成。
	{

		// フェンスの値。
		fenceValue = 1;
		asfenceValue_ = 1;

		// MainGraphicsの処理をCPU側で待つ際に使用するフェンス
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&GPUtoCPUFence_[0]));
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&GPUtoCPUFence_[1]));
		GPUtoCPUFence_[0]->SetName(L"GPUtoCPUFenceVal0");
		GPUtoCPUFence_[1]->SetName(L"GPUtoCPUFenceVal1");

		// MainGraphicsからDenoise表へのフェンスの生成
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&graphicsToCopyFence_[0]));
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&graphicsToCopyFence_[1]));
		graphicsToCopyFence_[0]->SetName(L"MainGraphicsToCopyFence0");
		graphicsToCopyFence_[1]->SetName(L"MainGraphicsToCopyFence1");

		// MainGraphicsからDenoise表へのフェンスの生成
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&graphicsToDenoiseFence_[0]));
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&graphicsToDenoiseFence_[1]));
		graphicsToDenoiseFence_[0]->SetName(L"MainGraphicsToDenoiseFence0");
		graphicsToDenoiseFence_[1]->SetName(L"MainGraphicsToDenoiseFence1");

		// Denoise表からCopyへのフェンスの生成
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&denoiseToCopyFence_[0]));
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&denoiseToCopyFence_[1]));
		denoiseToCopyFence_[0]->SetName(L"DenoiseToCopyFence0");
		denoiseToCopyFence_[1]->SetName(L"DenoiseToCopyFence1");

		// Copy終了監視用のフェンスの生成
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&finishCopyFence_[0]));
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&finishCopyFence_[1]));
		finishCopyFence_[0]->SetName(L"FinishCopyFence0");
		finishCopyFence_[1]->SetName(L"FinishCopyFence1");

		// 加速構造体生成用のフェンス
		result = device_.dev_->CreateFence(asfenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&asFence_));
		asFence_->SetName(L"AsFence");

		++fenceValue;

	}

	// その他GPUの非同期処理の際に必要な変数の初期化。
	{

		// 現在のキューのインデックスと経過フレームを0で初期化。
		currentQueueIndex_ = 1;	// 最初のFの先頭でフラグが切り替わるため、1で初期化しておくことで最初のFのインデックスが0になる。
		pastQueueIndex_ = 0;
		frameIndex_ = 0;

	}

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
	result = device_.dxgiFactory_->CreateSwapChainForHwnd(
		graphicsCmdQueue_.Get(),
		windowsAPI_->hwnd_,
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapChain1);
	// 生成したIDXGISwapChain1のオブジェクトを変換する
	swapChain1.As(&swapchain_.swapchain_);

	// レンダーターゲットビューの生成
	swapchain_.heapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;				// レンダーターゲットビュー
	swapchain_.heapDesc_.NumDescriptors = 2;								// 裏表の２つ
	device_.dev_->CreateDescriptorHeap(&swapchain_.heapDesc_,
		IID_PPV_ARGS(&swapchain_.rtvHeaps_));
	// レンダーターゲットビューを裏表2つ分生成
	for (int i = 0; i < 2; i++)
	{
		// スワップチェーンからバッファを取得
		result = swapchain_.swapchain_->GetBuffer(i, IID_PPV_ARGS(&swapchain_.backBuffers_[i]));
		// デスクリプタヒープのハンドルを取得
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			swapchain_.rtvHeaps_->GetCPUDescriptorHandleForHeapStart(), i, device_.dev_->GetDescriptorHandleIncrementSize(swapchain_.heapDesc_.Type));
		// レンダーターゲットビューの生成
		device_.dev_->CreateRenderTargetView(
			swapchain_.backBuffers_[i].Get(),
			nullptr,
			handle);

	}
	swapchain_.backBuffers_[0]->SetName(L"BackBuffer0");
	swapchain_.backBuffers_[1]->SetName(L"BackBuffer1");

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
	result = device_.dev_->CreateCommittedResource(
		&resourceProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&resrouceClearValue,
		IID_PPV_ARGS(&swapchain_.depthBuffer_)
	);
	swapchain_.depthBuffer_->SetName(L"DepthBuffer");

	// 深度バッファビュー生成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	result = device_.dev_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&swapchain_.dsvHeap_));
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device_.dev_->CreateDepthStencilView(
		swapchain_.depthBuffer_.Get(),
		&dsvDesc,
		swapchain_.dsvHeap_->GetCPUDescriptorHandleForHeapStart()
	);
	swapchain_.dsvHeap_->SetName(L"DsvHeap");

	// DirectInputオブジェクトの生成
	result = DirectInput8Create(
		windowsAPI_->windowClass_.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&input_.dinput_, nullptr);

	// キーボードデバイスの生成
	result = input_.dinput_->CreateDevice(GUID_SysKeyboard, &input_.devkeybord_, NULL);

	// 入力データの形式セット
	result = input_.devkeybord_->SetDataFormat(&c_dfDIKeyboard);

	// 排他制御レベルのセット
	result = input_.devkeybord_->SetCooperativeLevel(
		windowsAPI_->hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	// マウスデバイスの生成
	result = input_.dinput_->CreateDevice(GUID_SysMouse, &input_.devmouse_, NULL);

	// 入力データの形式セット
	result = input_.devmouse_->SetDataFormat(&c_dfDIMouse);

	// 排他制御レベルのセット
	result = input_.devmouse_->SetCooperativeLevel(
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
	blnResult = ImGui_ImplDX12_Init(device_.dev_.Get(), 3, DXGI_FORMAT_R8G8B8A8_UNORM, heapForImgui_.Get(),
		heapForImgui_.Get()->GetCPUDescriptorHandleForHeapStart(), heapForImgui_.Get()->GetGPUDescriptorHandleForHeapStart());

}

#include "RayDenoiser.h"
void Engine::ProcessBeforeDrawing() {

	// 現在のUAVインデックスを切り替える。
	pastQueueIndex_ = currentQueueIndex_;
	currentQueueIndex_ = currentQueueIndex_ ? 0 : 1;

	// 全キーの入力状態を取得する
	Input::Ins()->Update(input_.devkeybord_, input_.devmouse_);

	// レンダーターゲット周りの初期化を行う。
	{

		// レンダーターゲットのリソースバリア変更
		UINT bbIndex = swapchain_.swapchain_->GetCurrentBackBufferIndex();
		CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(swapchain_.backBuffers_[bbIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

		// レンダーターゲットの設定
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			swapchain_.rtvHeaps_->GetCPUDescriptorHandleForHeapStart(), bbIndex, device_.dev_->GetDescriptorHandleIncrementSize(swapchain_.heapDesc_.Type));

		// 深度バッファ用のディスクリプタヒープの先頭アドレスを取得
		D3D12_CPU_DESCRIPTOR_HANDLE dsvH = swapchain_.dsvHeap_->GetCPUDescriptorHandleForHeapStart();
		copyResourceCmdList_->OMSetRenderTargets(1, &rtvH, false, &dsvH);

		// 画面クリア
		// 初期化色
		float clearColor[] = { 0.35f,0.3f,0.2f,0.0f };
		for (int i = 0; i < 4; ++i) {
			clearColor[i] = clearColor[i] / 255.0f;
		}
		//copyResourceCmdList_->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		// 深度バッファのクリアコマンド
		copyResourceCmdList_->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// ビューポート設定
		CD3DX12_VIEWPORT viewportData = CD3DX12_VIEWPORT(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT);
		copyResourceCmdList_->RSSetViewports(1, &viewportData);

		// シザリング矩形設定
		CD3DX12_RECT rectData = CD3DX12_RECT(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		copyResourceCmdList_->RSSetScissorRects(1, &rectData);

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

		// フェンスの値を加算。
		++fenceValue;

	}

	// GraphicsQueueの処理
	if (!isGameEndReservation_) {

		// コマンドリストに追加
		mainGraphicsCmdList_->SetDescriptorHeaps(1, heapForImgui_.GetAddressOf());
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mainGraphicsCmdList_.Get());

		// グラフィックコマンドリストのクローズ
		mainGraphicsCmdList_->Close();

		// グラフィックスキューのコマンドを実行。
		ID3D12CommandList* cmdLists[] = { mainGraphicsCmdList_.Get() };
		graphicsCmdQueue_->ExecuteCommandLists(1, cmdLists);

		// MainGraphicsの実行完了を通知。
		graphicsCmdQueue_->Signal(graphicsToDenoiseFence_[currentQueueIndex_].Get(), fenceValue);

		// 前フレームのデノイズを待つ。
		if (frameIndex_ != 0) {
			graphicsCmdQueue_->Wait(denoiseToCopyFence_[pastQueueIndex_].Get(), fenceValue - 1);
		}

		// コピーコマンドリストのクローズ
		copyResourceCmdList_->Close();

		// コピーを実行。
		ID3D12CommandList* copyCmdLists[] = { copyResourceCmdList_.Get() };
		graphicsCmdQueue_->ExecuteCommandLists(1, copyCmdLists);

		// Copyの実行完了を通知。
		graphicsCmdQueue_->Signal(GPUtoCPUFence_[currentQueueIndex_].Get(), fenceValue);

	}

	// ComputeQueueの処理
	if (!isGameEndReservation_) {

		// コマンドリストを閉じる
		denoiseCmdList_[currentQueueIndex_]->Close();

		// レイトレの処理を待つ。
		computeCmdQueue_->Wait(graphicsToDenoiseFence_[currentQueueIndex_].Get(), fenceValue - 0);

		// デノイズを実行
		ID3D12CommandList* cmdLists[] = { denoiseCmdList_[currentQueueIndex_].Get() };
		computeCmdQueue_->ExecuteCommandLists(1, cmdLists);

		// 実行完了を通知。
		computeCmdQueue_->Signal(denoiseToCopyFence_[currentQueueIndex_].Get(), fenceValue);

	}


	// SwapChainを反転させて、GPUの処理完了を待つ。
	if (!isGameEndReservation_) {


		// 反転。
		swapchain_.swapchain_->Present(1, 0);

		// グラフィックコマンドリストの完了待ち
		UINT64 gpuFence = GPUtoCPUFence_[currentQueueIndex_]->GetCompletedValue();
		if (gpuFence != fenceValue) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			GPUtoCPUFence_[currentQueueIndex_]->SetEventOnCompletion(fenceValue, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		// このフレームで使用したコマンドリストを初期化。
		mainGraphicsCmdAllocator_->Reset();
		mainGraphicsCmdList_->Reset(mainGraphicsCmdAllocator_.Get(), nullptr);

		// 今フレームのコピーのコマンドを初期化。
		copyResourceCmdAllocator_->Reset();
		copyResourceCmdList_->Reset(copyResourceCmdAllocator_.Get(), nullptr);

		// 前フレームのデノイズのコマンドを初期化。
		if (frameIndex_ != 0) {
			denoiseCmdAllocator_[pastQueueIndex_]->Reset();
			denoiseCmdList_[pastQueueIndex_]->Reset(denoiseCmdAllocator_[pastQueueIndex_].Get(), nullptr);
		}

	}
	// ゲームの終了が予約されていたら、前フレームのデノイズが終わるのを待ってからゲーム画面を閉じる。
	else {


		UINT64 gpuFence = denoiseToCopyFence_[pastQueueIndex_]->GetCompletedValue();
		if (gpuFence != fenceValue - 1) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			denoiseToCopyFence_[pastQueueIndex_]->SetEventOnCompletion(fenceValue - 1, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		denoiseCmdAllocator_[pastQueueIndex_]->Reset();
		denoiseCmdList_[pastQueueIndex_]->Reset(denoiseCmdAllocator_[pastQueueIndex_].Get(), nullptr);

		isGameEnd_ = true;

	}

	++frameIndex_;

}

void Engine::SetRenderTarget()
{
	// レンダーターゲットのリソースバリア変更
	UINT bbIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &resourceBarrier);

	resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &resourceBarrier);
	// レンダーターゲットの設定
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		Engine::Ins()->swapchain_.rtvHeaps_->GetCPUDescriptorHandleForHeapStart(), bbIndex, Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(Engine::Ins()->swapchain_.heapDesc_.Type));

	CD3DX12_CPU_DESCRIPTOR_HANDLE backBufferHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(Engine::Ins()->swapchain_.rtvHeaps_->GetCPUDescriptorHandleForHeapStart(),
		Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(Engine::Ins()->swapchain_.heapDesc_.Type));
	D3D12_CPU_DESCRIPTOR_HANDLE dsvStartHandle = Engine::Ins()->swapchain_.dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	Engine::Ins()->mainGraphicsCmdList_->OMSetRenderTargets(1, &backBufferHandle, false, &dsvStartHandle);
	// レンダーターゲットのクリア
	float clearColor[] = { 0.5f,0.5f,0.5f,0.0f };
	Engine::Ins()->mainGraphicsCmdList_->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	// 深度バッファのクリアコマンド
	Engine::Ins()->mainGraphicsCmdList_->ClearDepthStencilView(Engine::Ins()->swapchain_.dsvHeap_->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void Engine::ResourceBarrierAfter()
{
	// レンダーターゲットのリソースバリア変更
	CD3DX12_RESOURCE_BARRIER resrouceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(swapchain_.backBuffers_[swapchain_.swapchain_->GetCurrentBackBufferIndex()].Get(),
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

	device_.dev_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(ret.ReleaseAndGetAddressOf()));

	return ret;
}