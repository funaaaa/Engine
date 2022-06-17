#include "DirectXBase.h"
#include <cassert>
#include "struct.h"
#include "Input.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
#include <stdexcept>

DirectXBase::DirectXBase() {
	backBuffers.resize(2);
}

void DirectXBase::Init() {
#ifdef _DEBUG
	//デバッグレイヤーの有効化
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif

	//ウィンドウ初期化
	windowsAPI.Init();

	//DXGIファクトリーの生成
	HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

	//グラフィックスアダプターの選択
	Microsoft::WRL::ComPtr<IDXGIAdapter1> tmpAdapter = nullptr;
	for (int i = 0;
		dxgiFactory->EnumAdapters1(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		adapters.push_back(tmpAdapter);						//動的配列に追加する
	}
	//追加されたアダプターオブジェクトを仕分ける
	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC1 adesc;
		adapters[i]->GetDesc1(&adesc);						//アダプターの情報を取得
		// ソフトウェアデバイスを回避
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}
		wstring strDesc = adesc.Description;				//アダプター名
		// Intel UHD Graphics（オンボードグラフィック）を回避
		if (strDesc.find(L"Intel") == wstring::npos)
		{
			tmpAdapter = adapters[i];						//採用
			break;
		}
	}

	//デバイスの生成
	//levels.push_back(D3D_FEATURE_LEVEL_12_1);
	levels.push_back(D3D_FEATURE_LEVEL_12_0);
	levels.push_back(D3D_FEATURE_LEVEL_11_1);
	levels.push_back(D3D_FEATURE_LEVEL_11_0);
	for (int i = 0; i < levels.size(); i++)
	{
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&dev));
		if (result == S_OK)
		{
			//デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}

	//DXR サポートしているか確認
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 options{};
	result = dev->CheckFeatureSupport(
		D3D12_FEATURE_D3D12_OPTIONS5, &options, sizeof(options));
	if (FAILED(result) ||
		options.RaytracingTier == D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
	{
		throw std::runtime_error("DirectX Raytracing not supported.");
	}


	//コマンドアロケータの生成
	result = dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator));

	//コマンドリストの生成
	result = dev->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(&cmdList));

	//コマンドキューの生成
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	result = dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));

	//スワップチェーンの生成
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = 1280;
	swapchainDesc.Height = 720;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			//色情報の書式
	swapchainDesc.SampleDesc.Count = 1;							//マルチサンプルしない
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;			//バックバッファ用
	swapchainDesc.BufferCount = 2;								//バッファ数を２つに設定
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	//フリップ後は破棄
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	result = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(),
		windowsAPI.hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapChain1);
	//生成したIDXGISwapChain1のオブジェクトを変換する
	swapChain1.As(&swapchain);

	//レンダーターゲットビューの生成
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;				//レンダーターゲットビュー
	heapDesc.NumDescriptors = 2;								//裏表の２つ
	dev->CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(&rtvHeaps));
	//レンダーターゲットビューを裏表2つ分生成
	for (int i = 0; i < 2; i++)
	{
		// スワップチェーンからバッファを取得
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		// デスクリプタヒープのハンドルを取得
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			rtvHeaps->GetCPUDescriptorHandleForHeapStart(), i, dev->GetDescriptorHandleIncrementSize(heapDesc.Type));
		// レンダーターゲットビューの生成
		dev->CreateRenderTargetView(
			backBuffers[i].Get(),
			nullptr,
			handle);
	}

	//リソース生成
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		window_width,
		window_height,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
	//リソース生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1, 0.0f),
		IID_PPV_ARGS(&depthBuffer)
	);

	//深度バッファビュー生成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	//フェンスの生成
	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//DirectInputオブジェクトの生成
	result = DirectInput8Create(
		windowsAPI.windowClass.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);

	//キーボードデバイスの生成
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeybord, NULL);

	//入力データの形式セット
	result = devkeybord->SetDataFormat(&c_dfDIKeyboard);

	//排他制御レベルのセット
	result = devkeybord->SetCooperativeLevel(
		windowsAPI.hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	//マウスデバイスの生成
	result = dinput->CreateDevice(GUID_SysMouse, &devmouse, NULL);

	//入力データの形式セット
	result = devmouse->SetDataFormat(&c_dfDIMouse);

	//排他制御レベルのセット
	result = devmouse->SetCooperativeLevel(
		windowsAPI.hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);


	//imguiの生成
	heapForImgui = CreateDescriptorHeaoForImgui();
	//imguiの初期化
	if (ImGui::CreateContext() == nullptr) {
		assert(0);
	}
	//windows用の初期化
	bool blnResult = ImGui_ImplWin32_Init(windowsAPI.hwnd);
	if (!blnResult) {
		assert(0);
	}
	//directX12用の初期化
	blnResult = ImGui_ImplDX12_Init(dev.Get(), 3, DXGI_FORMAT_R8G8B8A8_UNORM, heapForImgui.Get(),
		heapForImgui.Get()->GetCPUDescriptorHandleForHeapStart(), heapForImgui.Get()->GetGPUDescriptorHandleForHeapStart());

}

void DirectXBase::processBeforeDrawing() {
	//メッセージ確認
	if (PeekMessage(&windowsAPI.msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&windowsAPI.msg);	//キー入力メッセージの処理
		DispatchMessage(&windowsAPI.msg);	//プロシージャにメッセージを送る
	}
	//?ボタンで終了メッセージが来たらゲームループを抜ける
	if (windowsAPI.msg.message == WM_QUIT) {
		exit(true);
	}

	//全キーの入力状態を取得する
	Input::Update(devkeybord, devmouse);

	//レンダーターゲットのリソースバリア変更
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	//レンダーターゲットの設定
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		rtvHeaps->GetCPUDescriptorHandleForHeapStart(), bbIndex, dev->GetDescriptorHandleIncrementSize(heapDesc.Type));

	//深度バッファ用のディスクリプタヒープの先頭アドレスを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	//画面クリア
	//初期化色
	float clearColor[] = { 0.35f,0.3f,0.2f,0.0f };
	for (int i = 0; i < 4; ++i) {
		clearColor[i] = clearColor[i] / 255.0f;
	}
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	//深度バッファのクリアコマンド
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//ビューポート設定
	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, window_width, window_height));

	//シザリング矩形設定
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, window_width, window_height));

	////imgui描画前処理
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

	//ウィンドウ設定
	ImGui::Begin("Light Menu");
	ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);

}

void DirectXBase::processAfterDrawing() {

	ImGui::End();
	ImGui::Render();
	////コマンドリストに追加
	cmdList->SetDescriptorHeaps(1, heapForImgui.GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList.Get());

	//レンダーターゲットのリソースバリア変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[swapchain->GetCurrentBackBufferIndex()].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//グラフィックコマンドリストのクローズ
	cmdList->Close();

	//グラフィックコマンドリストの実行
	ID3D12CommandList* cmdLists[] = { cmdList.Get() }; //コマンドリストの配列
	cmdQueue->ExecuteCommandLists(1, cmdLists);

	//画面バッファをフリップ
	swapchain->Present(1, 0);

	//グラフィックコマンドリストの完了待ち
	cmdQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//コマンドアロケータのリセット
	cmdAllocator->Reset();						//キューをクリア

	//コマンドリストのリセット
	cmdList->Reset(cmdAllocator.Get(), nullptr);		//再びコマンドリストを貯める準備
}

void DirectXBase::SetRenderTarget()
{
	//レンダーターゲットのリソースバリア変更
	UINT bbIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();
	DirectXBase::Ins()->cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DirectXBase::Ins()->backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	DirectXBase::Ins()->cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DirectXBase::Ins()->backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	//レンダーターゲットの設定
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DirectXBase::Ins()->rtvHeaps->GetCPUDescriptorHandleForHeapStart(), bbIndex, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(DirectXBase::Ins()->heapDesc.Type));
	DirectXBase::Ins()->cmdList->OMSetRenderTargets(1, &CD3DX12_CPU_DESCRIPTOR_HANDLE(DirectXBase::Ins()->rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
		DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex(), DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(DirectXBase::Ins()->heapDesc.Type)), false, &DirectXBase::Ins()->dsvHeap->GetCPUDescriptorHandleForHeapStart());
	//レンダーターゲットのクリア
	float clearColor[] = { 0.5f,0.5f,0.5f,0.0f };
	DirectXBase::Ins()->cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	//深度バッファのクリアコマンド
	DirectXBase::Ins()->cmdList->ClearDepthStencilView(DirectXBase::Ins()->dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DirectXBase::ResourceBarrierAfter()
{
	//レンダーターゲットのリソースバリア変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[swapchain->GetCurrentBackBufferIndex()].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXBase::CreateDescriptorHeaoForImgui()
{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> ret;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(ret.ReleaseAndGetAddressOf()));

	return ret;
}