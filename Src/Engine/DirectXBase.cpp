#include "DirectXBase.h"
#include <cassert>
#include "Input.h"
#include <stdexcept>
#include <d3d12sdklayers.h>
#include "WindowsAPI.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

DirectXBase::DirectXBase() {
	backBuffers_.resize(2);
}

void DirectXBase::Init() {
#ifdef _DEBUG
	// �f�o�b�O���C���[�̗L����
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_))))
	{
		debugController_->EnableDebugLayer();
	}
	//if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&shaderDebugController_))))
	//{
	//	shaderDebugController_->SetEnableGPUBasedValidation(true);
	//}
#endif

	// �E�B���h�E������
	windowsAPI_ = std::make_shared<WindowsAPI>();
	windowsAPI_->Init();

	// DXGI�t�@�N�g���[�̐���
	HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory_));

	// �O���t�B�b�N�X�A�_�v�^�[�̑I��
	tmpAdapter_ = nullptr;
	for (int i = 0;
		dxgiFactory_->EnumAdapters1(i, &tmpAdapter_) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		adapters_.push_back(tmpAdapter_);			// ���I�z��ɒǉ�����
	}
	// �ǉ����ꂽ�A�_�v�^�[�I�u�W�F�N�g���d������
	for (int i = 0; i < adapters_.size(); i++)
	{
		DXGI_ADAPTER_DESC1 adesc;
		adapters_[i]->GetDesc1(&adesc);			// �A�_�v�^�[�̏����擾
		// �\�t�g�E�F�A�f�o�C�X�����
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}
		std::wstring strDesc = adesc.Description;				// �A�_�v�^�[��
		// Intel UHD Graphics�i�I���{�[�h�O���t�B�b�N�j�����
		if (strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter_ = adapters_[i];						// �̗p
			break;
		}
	}

	// �f�o�C�X�̐���
	//levels.push_back(D3D_FEATURE_LEVEL_12_1);
	levels_.push_back(D3D_FEATURE_LEVEL_12_0);
	levels_.push_back(D3D_FEATURE_LEVEL_11_1);
	levels_.push_back(D3D_FEATURE_LEVEL_11_0);
	for (auto& index_ : levels_)
	{
		// �̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter_.Get(), index_, IID_PPV_ARGS(&dev_));
		if (result == S_OK)
		{
			// �f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel_ = index_;
			break;
		}
	}

	// DXR �T�|�[�g���Ă��邩�m�F
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 options{};
	result = dev_->CheckFeatureSupport(
		D3D12_FEATURE_D3D12_OPTIONS5, &options, sizeof(options));
	if (FAILED(result) ||
		options.RaytracingTier == D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
	{
		throw std::runtime_error("DirectX Raytracing not supported.");
	}


	// �R�}���h�A���P�[�^�̐���
	result = dev_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator_));

	// �R�}���h���X�g�̐���
	result = dev_->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&cmdList_));

	// �R�}���h�L���[�̐���
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	result = dev_->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue_));

	// �X���b�v�`�F�[���̐���
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = 1280;
	swapchainDesc.Height = 720;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			// �F���̏���
	swapchainDesc.SampleDesc.Count = 1;							// �}���`�T���v�����Ȃ�
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;			// �o�b�N�o�b�t�@�p
	swapchainDesc.BufferCount = 2;								// �o�b�t�@�����Q�ɐݒ�
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// �t���b�v��͔j��
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	result = dxgiFactory_->CreateSwapChainForHwnd(
		cmdQueue_.Get(),
		windowsAPI_->hwnd_,
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapChain1);
	// ��������IDXGISwapChain1�̃I�u�W�F�N�g��ϊ�����
	swapChain1.As(&swapchain_);

	// �����_�[�^�[�Q�b�g�r���[�̐���
	heapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;				// �����_�[�^�[�Q�b�g�r���[
	heapDesc_.NumDescriptors = 2;								// ���\�̂Q��
	dev_->CreateDescriptorHeap(&heapDesc_,
		IID_PPV_ARGS(&rtvHeaps_));
	// �����_�[�^�[�Q�b�g�r���[�𗠕\2������
	for (int i = 0; i < 2; i++)
	{
		// �X���b�v�`�F�[������o�b�t�@���擾
		result = swapchain_->GetBuffer(i, IID_PPV_ARGS(&backBuffers_[i]));
		// �f�X�N���v�^�q�[�v�̃n���h�����擾
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			rtvHeaps_->GetCPUDescriptorHandleForHeapStart(), i, dev_->GetDescriptorHandleIncrementSize(heapDesc_.Type));
		// �����_�[�^�[�Q�b�g�r���[�̐���
		dev_->CreateRenderTargetView(
			backBuffers_[i].Get(),
			nullptr,
			handle);
	}

	// ���\�[�X����
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
	// ���\�[�X����
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

	// �[�x�o�b�t�@�r���[����
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

	// �t�F���X�̐���
	result = dev_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));

	// DirectInput�I�u�W�F�N�g�̐���
	result = DirectInput8Create(
		windowsAPI_->windowClass_.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput_, nullptr);

	// �L�[�{�[�h�f�o�C�X�̐���
	result = dinput_->CreateDevice(GUID_SysKeyboard, &devkeybord_, NULL);

	// ���̓f�[�^�̌`���Z�b�g
	result = devkeybord_->SetDataFormat(&c_dfDIKeyboard);

	// �r�����䃌�x���̃Z�b�g
	result = devkeybord_->SetCooperativeLevel(
		windowsAPI_->hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	// �}�E�X�f�o�C�X�̐���
	result = dinput_->CreateDevice(GUID_SysMouse, &devmouse_, NULL);

	// ���̓f�[�^�̌`���Z�b�g
	result = devmouse_->SetDataFormat(&c_dfDIMouse);

	// �r�����䃌�x���̃Z�b�g
	result = devmouse_->SetCooperativeLevel(
		windowsAPI_->hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);


	// imgui�̐���
	heapForImgui_ = CreateDescriptorHeaoForImgui();
	// imgui�̏�����
	if (ImGui::CreateContext() == nullptr) {
		assert(0);
	}
	// windows�p�̏�����
	bool blnResult = ImGui_ImplWin32_Init(windowsAPI_->hwnd_);
	if (!blnResult) {
		assert(0);
	}
	// directX12�p�̏�����
	blnResult = ImGui_ImplDX12_Init(dev_.Get(), 3, DXGI_FORMAT_R8G8B8A8_UNORM, heapForImgui_.Get(),
		heapForImgui_.Get()->GetCPUDescriptorHandleForHeapStart(), heapForImgui_.Get()->GetGPUDescriptorHandleForHeapStart());

}

void DirectXBase::ProcessBeforeDrawing() {
	// ���b�Z�[�W�m�F
	if (PeekMessage(&windowsAPI_->msg_, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&windowsAPI_->msg_);	// �L�[���̓��b�Z�[�W�̏���
		DispatchMessage(&windowsAPI_->msg_);	// �v���V�[�W���Ƀ��b�Z�[�W�𑗂�
	}
	// ?�{�^���ŏI�����b�Z�[�W��������Q�[�����[�v�𔲂���
	if (windowsAPI_->msg_.message == WM_QUIT) {
		exit(true);
	}

	// �S�L�[�̓��͏�Ԃ��擾����
	Input::Ins()->Update(devkeybord_, devmouse_);

	// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�ύX
	UINT bbIndex = swapchain_->GetCurrentBackBufferIndex();
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	cmdList_->ResourceBarrier(1, &resourceBarrier);

	// �����_�[�^�[�Q�b�g�̐ݒ�
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		rtvHeaps_->GetCPUDescriptorHandleForHeapStart(), bbIndex, dev_->GetDescriptorHandleIncrementSize(heapDesc_.Type));

	// �[�x�o�b�t�@�p�̃f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	cmdList_->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	// ��ʃN���A
	// �������F
	float clearColor[] = { 0.35f,0.3f,0.2f,0.0f };
	for (int i = 0; i < 4; ++i) {
		clearColor[i] = clearColor[i] / 255.0f;
	}
	cmdList_->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	// �[�x�o�b�t�@�̃N���A�R�}���h
	cmdList_->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// �r���[�|�[�g�ݒ�
	CD3DX12_VIEWPORT viewportData = CD3DX12_VIEWPORT(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT);
	cmdList_->RSSetViewports(1, &viewportData);

	// �V�U�����O��`�ݒ�
	CD3DX12_RECT rectData = CD3DX12_RECT(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	cmdList_->RSSetScissorRects(1, &rectData);

	// imgui�`��O����
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

	// �E�B���h�E�ݒ�
	ImGui::Begin("Debug Menu");
	ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);

}

void DirectXBase::ProcessAfterDrawing() {

	ImGui::End();
	ImGui::Render();
	// �R�}���h���X�g�ɒǉ�
	cmdList_->SetDescriptorHeaps(1, heapForImgui_.GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList_.Get());

	// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�ύX
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffers_[swapchain_->GetCurrentBackBufferIndex()].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	cmdList_->ResourceBarrier(1, &resourceBarrier);

	// �O���t�B�b�N�R�}���h���X�g�̃N���[�Y
	cmdList_->Close();

	//�O���t�B�b�N�R�}���h���X�g�̎��s
	ID3D12CommandList* cmdLists[] = { cmdList_.Get() }; // �R�}���h���X�g�̔z��
	cmdQueue_->ExecuteCommandLists(1, cmdLists);

	// ��ʃo�b�t�@���t���b�v
	swapchain_->Present(1, 0);

	// �O���t�B�b�N�R�}���h���X�g�̊����҂�
	cmdQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() != fenceVal_) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceVal_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// �R�}���h�A���P�[�^�̃��Z�b�g
	cmdAllocator_->Reset();							// �L���[���N���A

	// �R�}���h���X�g�̃��Z�b�g
	cmdList_->Reset(cmdAllocator_.Get(), nullptr);	// �ĂуR�}���h���X�g�𒙂߂鏀��
}

void DirectXBase::SetRenderTarget()
{
	// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�ύX
	UINT bbIndex = DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex();
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(DirectXBase::Ins()->backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	DirectXBase::Ins()->cmdList_->ResourceBarrier(1, &resourceBarrier);

	resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(DirectXBase::Ins()->backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	DirectXBase::Ins()->cmdList_->ResourceBarrier(1, &resourceBarrier);
	// �����_�[�^�[�Q�b�g�̐ݒ�
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DirectXBase::Ins()->rtvHeaps_->GetCPUDescriptorHandleForHeapStart(), bbIndex, DirectXBase::Ins()->dev_->GetDescriptorHandleIncrementSize(DirectXBase::Ins()->heapDesc_.Type));

	CD3DX12_CPU_DESCRIPTOR_HANDLE backBufferHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(DirectXBase::Ins()->rtvHeaps_->GetCPUDescriptorHandleForHeapStart(),
		DirectXBase::Ins()->swapchain_->GetCurrentBackBufferIndex(), DirectXBase::Ins()->dev_->GetDescriptorHandleIncrementSize(DirectXBase::Ins()->heapDesc_.Type));
	D3D12_CPU_DESCRIPTOR_HANDLE dsvStartHandle = DirectXBase::Ins()->dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	DirectXBase::Ins()->cmdList_->OMSetRenderTargets(1, &backBufferHandle, false, &dsvStartHandle);
	// �����_�[�^�[�Q�b�g�̃N���A
	float clearColor[] = { 0.5f,0.5f,0.5f,0.0f };
	DirectXBase::Ins()->cmdList_->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	// �[�x�o�b�t�@�̃N���A�R�}���h
	DirectXBase::Ins()->cmdList_->ClearDepthStencilView(DirectXBase::Ins()->dsvHeap_->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DirectXBase::ResourceBarrierAfter()
{
	// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�ύX
	CD3DX12_RESOURCE_BARRIER resrouceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffers_[swapchain_->GetCurrentBackBufferIndex()].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	cmdList_->ResourceBarrier(1, &resrouceBarrier);
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXBase::CreateDescriptorHeaoForImgui()
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