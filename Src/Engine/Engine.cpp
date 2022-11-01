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
	// �f�o�b�O���C���[�̗L����
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_.debugController_))))
	{
		debug_.debugController_->EnableDebugLayer();
	}
	//if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_.shaderDebugController_))))
	//{
	//	debug_.shaderDebugController_->SetEnableGPUBasedValidation(true);
	//}
#endif

	// �E�B���h�E������
	windowsAPI_ = std::make_shared<WindowsAPI>();
	windowsAPI_->Init();

	// DXGI�t�@�N�g���[�̐���
	HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&device_.dxgiFactory_));

	// �O���t�B�b�N�X�A�_�v�^�[�̑I��
	device_.tmpAdapter_ = nullptr;
	for (int i = 0;
		device_.dxgiFactory_->EnumAdapters1(i, &device_.tmpAdapter_) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		device_.adapters_.push_back(device_.tmpAdapter_);	// ���I�z��ɒǉ�����
	}
	// �ǉ����ꂽ�A�_�v�^�[�I�u�W�F�N�g���d������
	for (int i = 0; i < device_.adapters_.size(); i++)
	{
		DXGI_ADAPTER_DESC1 adesc;
		device_.adapters_[i]->GetDesc1(&adesc);				// �A�_�v�^�[�̏����擾
		// �\�t�g�E�F�A�f�o�C�X�����
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}
		std::wstring strDesc = adesc.Description;			// �A�_�v�^�[��
		// Intel UHD Graphics�i�I���{�[�h�O���t�B�b�N�j�����
		if (strDesc.find(L"Intel") == std::wstring::npos)
		{
			device_.tmpAdapter_ = device_.adapters_[i];		// �̗p
			break;
		}
	}

	// �f�o�C�X�̐���
	//device_.levels.push_back(D3D_FEATURE_LEVEL_12_1);
	device_.levels_.push_back(D3D_FEATURE_LEVEL_12_0);
	device_.levels_.push_back(D3D_FEATURE_LEVEL_11_1);
	device_.levels_.push_back(D3D_FEATURE_LEVEL_11_0);
	for (auto& index_ : device_.levels_)
	{
		// �̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(device_.tmpAdapter_.Get(), index_, IID_PPV_ARGS(&device_.dev_));
		if (result == S_OK)
		{
			// �f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			device_.featureLevel_ = index_;
			break;
		}
	}
	device_.dev_->SetName(L"Device");

	// DXR �T�|�[�g���Ă��邩�m�F
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 options{};
	result = device_.dev_->CheckFeatureSupport(
		D3D12_FEATURE_D3D12_OPTIONS5, &options, sizeof(options));
	if (FAILED(result) ||
		options.RaytracingTier == D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
	{
		throw std::runtime_error("DirectX Raytracing not supported.");
	}

	// �R�}���h�A���P�[�^�̐���
	{

		// ���C�g���֐���UI�̕`��Ɏg�p����R�}���h���X�g�p��Allocator�𐶐��B
		result = device_.dev_->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&mainGraphicsCmdAllocator_));
		mainGraphicsCmdAllocator_->SetName(L"MainGraphicsCmdAllocator");

		// �f�m�C�Y���ʂ��o�b�N�o�b�t�@�ɃR�s�[����ۂɎg�p����R�}���h���X�g�p��Allocator�𐶐��B
		result = device_.dev_->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&copyResourceCmdAllocator_));
		copyResourceCmdAllocator_->SetName(L"CopyResourceCmdAllocator");

		// �f�m�C�Y�Ɏg�p����R�}���h���X�g�\�p��Allocator�𐶐��B
		result = Engine::Ins()->device_.dev_->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_COMPUTE,
			IID_PPV_ARGS(&denoiseCmdAllocator_[0]));
		denoiseCmdAllocator_[0]->SetName(L"DenoiseAllocator0");

		// �f�m�C�Y�Ɏg�p����R�}���h���X�g���p��Allocator�𐶐��B
		result = Engine::Ins()->device_.dev_->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_COMPUTE,
			IID_PPV_ARGS(&denoiseCmdAllocator_[1]));
		denoiseCmdAllocator_[1]->SetName(L"DenoiseAllocator1");

	}

	// �R�}���h���X�g�̐���
	{

		// ���C�g���֐���UI�̕`��Ɏg�p����R�}���h���X�g
		result = device_.dev_->CreateCommandList(0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			mainGraphicsCmdAllocator_.Get(), nullptr,
			IID_PPV_ARGS(&mainGraphicsCmdList_));
		mainGraphicsCmdList_->SetName(L"GraphicsCmdList");

		// �f�m�C�Y���ʂ��o�b�N�o�b�t�@�ɃR�s�[����ۂɎg�p����R�}���h���X�g
		result = device_.dev_->CreateCommandList(0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			copyResourceCmdAllocator_.Get(), nullptr,
			IID_PPV_ARGS(&copyResourceCmdList_));
		copyResourceCmdList_->SetName(L"CopyResourceCmdList");

		// �f�m�C�Y�Ɏg�p����R�}���h���X�g�\
		result = Engine::Ins()->device_.dev_->CreateCommandList(0,
			D3D12_COMMAND_LIST_TYPE_COMPUTE,
			denoiseCmdAllocator_[0].Get(), nullptr,
			IID_PPV_ARGS(&denoiseCmdList_[0]));
		denoiseCmdList_[0]->SetName(L"DenoiseComputeCmdList0");

		// �f�m�C�Y�Ɏg�p����R�}���h���X�g��
		result = Engine::Ins()->device_.dev_->CreateCommandList(0,
			D3D12_COMMAND_LIST_TYPE_COMPUTE,
			denoiseCmdAllocator_[1].Get(), nullptr,
			IID_PPV_ARGS(&denoiseCmdList_[1]));
		denoiseCmdList_[1]->SetName(L"DenoiseComputeCmdList1");

	}

	// �R�}���h�L���[�̐���
	{

		// �O���t�B�b�N�X�L���[�𐶐��B
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		result = device_.dev_->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&graphicsCmdQueue_));
		graphicsCmdQueue_->SetName(L"GraphicsCmdQueue");

		// �f�m�C�Y�Ɏg�p����R���s���[�g�L���[�̕\�𐶐��B
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		result = Engine::Ins()->device_.dev_->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&computeCmdQueue_));
		computeCmdQueue_->SetName(L"ComputeQueue");

	}

	// �t�F���X�̐����B
	{

		// �t�F���X�̒l�B
		fenceValue = 1;
		asfenceValue_ = 1;

		// MainGraphics�̏�����CPU���ő҂ۂɎg�p����t�F���X
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&GPUtoCPUFence_[0]));
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&GPUtoCPUFence_[1]));
		GPUtoCPUFence_[0]->SetName(L"GPUtoCPUFenceVal0");
		GPUtoCPUFence_[1]->SetName(L"GPUtoCPUFenceVal1");

		// MainGraphics����Denoise�\�ւ̃t�F���X�̐���
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&graphicsToCopyFence_[0]));
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&graphicsToCopyFence_[1]));
		graphicsToCopyFence_[0]->SetName(L"MainGraphicsToCopyFence0");
		graphicsToCopyFence_[1]->SetName(L"MainGraphicsToCopyFence1");

		// MainGraphics����Denoise�\�ւ̃t�F���X�̐���
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&graphicsToDenoiseFence_[0]));
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&graphicsToDenoiseFence_[1]));
		graphicsToDenoiseFence_[0]->SetName(L"MainGraphicsToDenoiseFence0");
		graphicsToDenoiseFence_[1]->SetName(L"MainGraphicsToDenoiseFence1");

		// Denoise�\����Copy�ւ̃t�F���X�̐���
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&denoiseToCopyFence_[0]));
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&denoiseToCopyFence_[1]));
		denoiseToCopyFence_[0]->SetName(L"DenoiseToCopyFence0");
		denoiseToCopyFence_[1]->SetName(L"DenoiseToCopyFence1");

		// Copy�I���Ď��p�̃t�F���X�̐���
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&finishCopyFence_[0]));
		result = device_.dev_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&finishCopyFence_[1]));
		finishCopyFence_[0]->SetName(L"FinishCopyFence0");
		finishCopyFence_[1]->SetName(L"FinishCopyFence1");

		// �����\���̐����p�̃t�F���X
		result = device_.dev_->CreateFence(asfenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&asFence_));
		asFence_->SetName(L"AsFence");

		++fenceValue;

	}

	// ���̑�GPU�̔񓯊������̍ۂɕK�v�ȕϐ��̏������B
	{

		// ���݂̃L���[�̃C���f�b�N�X�ƌo�߃t���[����0�ŏ������B
		currentQueueIndex_ = 1;	// �ŏ���F�̐擪�Ńt���O���؂�ւ�邽�߁A1�ŏ��������Ă������Ƃōŏ���F�̃C���f�b�N�X��0�ɂȂ�B
		pastQueueIndex_ = 0;
		frameIndex_ = 0;

	}

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
	result = device_.dxgiFactory_->CreateSwapChainForHwnd(
		graphicsCmdQueue_.Get(),
		windowsAPI_->hwnd_,
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapChain1);
	// ��������IDXGISwapChain1�̃I�u�W�F�N�g��ϊ�����
	swapChain1.As(&swapchain_.swapchain_);

	// �����_�[�^�[�Q�b�g�r���[�̐���
	swapchain_.heapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;				// �����_�[�^�[�Q�b�g�r���[
	swapchain_.heapDesc_.NumDescriptors = 2;								// ���\�̂Q��
	device_.dev_->CreateDescriptorHeap(&swapchain_.heapDesc_,
		IID_PPV_ARGS(&swapchain_.rtvHeaps_));
	// �����_�[�^�[�Q�b�g�r���[�𗠕\2������
	for (int i = 0; i < 2; i++)
	{
		// �X���b�v�`�F�[������o�b�t�@���擾
		result = swapchain_.swapchain_->GetBuffer(i, IID_PPV_ARGS(&swapchain_.backBuffers_[i]));
		// �f�X�N���v�^�q�[�v�̃n���h�����擾
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			swapchain_.rtvHeaps_->GetCPUDescriptorHandleForHeapStart(), i, device_.dev_->GetDescriptorHandleIncrementSize(swapchain_.heapDesc_.Type));
		// �����_�[�^�[�Q�b�g�r���[�̐���
		device_.dev_->CreateRenderTargetView(
			swapchain_.backBuffers_[i].Get(),
			nullptr,
			handle);

	}
	swapchain_.backBuffers_[0]->SetName(L"BackBuffer0");
	swapchain_.backBuffers_[1]->SetName(L"BackBuffer1");

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
	result = device_.dev_->CreateCommittedResource(
		&resourceProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&resrouceClearValue,
		IID_PPV_ARGS(&swapchain_.depthBuffer_)
	);
	swapchain_.depthBuffer_->SetName(L"DepthBuffer");

	// �[�x�o�b�t�@�r���[����
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

	// DirectInput�I�u�W�F�N�g�̐���
	result = DirectInput8Create(
		windowsAPI_->windowClass_.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&input_.dinput_, nullptr);

	// �L�[�{�[�h�f�o�C�X�̐���
	result = input_.dinput_->CreateDevice(GUID_SysKeyboard, &input_.devkeybord_, NULL);

	// ���̓f�[�^�̌`���Z�b�g
	result = input_.devkeybord_->SetDataFormat(&c_dfDIKeyboard);

	// �r�����䃌�x���̃Z�b�g
	result = input_.devkeybord_->SetCooperativeLevel(
		windowsAPI_->hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	// �}�E�X�f�o�C�X�̐���
	result = input_.dinput_->CreateDevice(GUID_SysMouse, &input_.devmouse_, NULL);

	// ���̓f�[�^�̌`���Z�b�g
	result = input_.devmouse_->SetDataFormat(&c_dfDIMouse);

	// �r�����䃌�x���̃Z�b�g
	result = input_.devmouse_->SetCooperativeLevel(
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
	blnResult = ImGui_ImplDX12_Init(device_.dev_.Get(), 3, DXGI_FORMAT_R8G8B8A8_UNORM, heapForImgui_.Get(),
		heapForImgui_.Get()->GetCPUDescriptorHandleForHeapStart(), heapForImgui_.Get()->GetGPUDescriptorHandleForHeapStart());

}

#include "RayDenoiser.h"
void Engine::ProcessBeforeDrawing() {

	// ���݂�UAV�C���f�b�N�X��؂�ւ���B
	pastQueueIndex_ = currentQueueIndex_;
	currentQueueIndex_ = currentQueueIndex_ ? 0 : 1;

	// �S�L�[�̓��͏�Ԃ��擾����
	Input::Ins()->Update(input_.devkeybord_, input_.devmouse_);

	// �����_�[�^�[�Q�b�g����̏��������s���B
	{

		// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�ύX
		UINT bbIndex = swapchain_.swapchain_->GetCurrentBackBufferIndex();
		CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(swapchain_.backBuffers_[bbIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);

		// �����_�[�^�[�Q�b�g�̐ݒ�
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			swapchain_.rtvHeaps_->GetCPUDescriptorHandleForHeapStart(), bbIndex, device_.dev_->GetDescriptorHandleIncrementSize(swapchain_.heapDesc_.Type));

		// �[�x�o�b�t�@�p�̃f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
		D3D12_CPU_DESCRIPTOR_HANDLE dsvH = swapchain_.dsvHeap_->GetCPUDescriptorHandleForHeapStart();
		copyResourceCmdList_->OMSetRenderTargets(1, &rtvH, false, &dsvH);

		// ��ʃN���A
		// �������F
		float clearColor[] = { 0.35f,0.3f,0.2f,0.0f };
		for (int i = 0; i < 4; ++i) {
			clearColor[i] = clearColor[i] / 255.0f;
		}
		//copyResourceCmdList_->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		// �[�x�o�b�t�@�̃N���A�R�}���h
		copyResourceCmdList_->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// �r���[�|�[�g�ݒ�
		CD3DX12_VIEWPORT viewportData = CD3DX12_VIEWPORT(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT);
		copyResourceCmdList_->RSSetViewports(1, &viewportData);

		// �V�U�����O��`�ݒ�
		CD3DX12_RECT rectData = CD3DX12_RECT(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		copyResourceCmdList_->RSSetScissorRects(1, &rectData);

	}

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
	ImGui::Begin("Menu");
	ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);

}

#include "RayDenoiser.h"
void Engine::ProcessAfterDrawing() {

	ImGui::End();
	ImGui::Render();

	// �e�t�F���X�̒l�����Z����B
	{

		// �t�F���X�̒l�����Z�B
		++fenceValue;

	}

	// GraphicsQueue�̏���
	if (!isGameEndReservation_) {

		// �R�}���h���X�g�ɒǉ�
		mainGraphicsCmdList_->SetDescriptorHeaps(1, heapForImgui_.GetAddressOf());
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mainGraphicsCmdList_.Get());

		// �O���t�B�b�N�R�}���h���X�g�̃N���[�Y
		mainGraphicsCmdList_->Close();

		// �O���t�B�b�N�X�L���[�̃R�}���h�����s�B
		ID3D12CommandList* cmdLists[] = { mainGraphicsCmdList_.Get() };
		graphicsCmdQueue_->ExecuteCommandLists(1, cmdLists);

		// MainGraphics�̎��s������ʒm�B
		graphicsCmdQueue_->Signal(graphicsToDenoiseFence_[currentQueueIndex_].Get(), fenceValue);

		// �O�t���[���̃f�m�C�Y��҂B
		if (frameIndex_ != 0) {
			graphicsCmdQueue_->Wait(denoiseToCopyFence_[pastQueueIndex_].Get(), fenceValue - 1);
		}

		// �R�s�[�R�}���h���X�g�̃N���[�Y
		copyResourceCmdList_->Close();

		// �R�s�[�����s�B
		ID3D12CommandList* copyCmdLists[] = { copyResourceCmdList_.Get() };
		graphicsCmdQueue_->ExecuteCommandLists(1, copyCmdLists);

		// Copy�̎��s������ʒm�B
		graphicsCmdQueue_->Signal(GPUtoCPUFence_[currentQueueIndex_].Get(), fenceValue);

	}

	// ComputeQueue�̏���
	if (!isGameEndReservation_) {

		// �R�}���h���X�g�����
		denoiseCmdList_[currentQueueIndex_]->Close();

		// ���C�g���̏�����҂B
		computeCmdQueue_->Wait(graphicsToDenoiseFence_[currentQueueIndex_].Get(), fenceValue - 0);

		// �f�m�C�Y�����s
		ID3D12CommandList* cmdLists[] = { denoiseCmdList_[currentQueueIndex_].Get() };
		computeCmdQueue_->ExecuteCommandLists(1, cmdLists);

		// ���s������ʒm�B
		computeCmdQueue_->Signal(denoiseToCopyFence_[currentQueueIndex_].Get(), fenceValue);

	}


	// SwapChain�𔽓]�����āAGPU�̏���������҂B
	if (!isGameEndReservation_) {


		// ���]�B
		swapchain_.swapchain_->Present(1, 0);

		// �O���t�B�b�N�R�}���h���X�g�̊����҂�
		UINT64 gpuFence = GPUtoCPUFence_[currentQueueIndex_]->GetCompletedValue();
		if (gpuFence != fenceValue) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			GPUtoCPUFence_[currentQueueIndex_]->SetEventOnCompletion(fenceValue, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		// ���̃t���[���Ŏg�p�����R�}���h���X�g���������B
		mainGraphicsCmdAllocator_->Reset();
		mainGraphicsCmdList_->Reset(mainGraphicsCmdAllocator_.Get(), nullptr);

		// ���t���[���̃R�s�[�̃R�}���h���������B
		copyResourceCmdAllocator_->Reset();
		copyResourceCmdList_->Reset(copyResourceCmdAllocator_.Get(), nullptr);

		// �O�t���[���̃f�m�C�Y�̃R�}���h���������B
		if (frameIndex_ != 0) {
			denoiseCmdAllocator_[pastQueueIndex_]->Reset();
			denoiseCmdList_[pastQueueIndex_]->Reset(denoiseCmdAllocator_[pastQueueIndex_].Get(), nullptr);
		}

	}
	// �Q�[���̏I�����\�񂳂�Ă�����A�O�t���[���̃f�m�C�Y���I���̂�҂��Ă���Q�[����ʂ����B
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
	// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�ύX
	UINT bbIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &resourceBarrier);

	resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &resourceBarrier);
	// �����_�[�^�[�Q�b�g�̐ݒ�
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		Engine::Ins()->swapchain_.rtvHeaps_->GetCPUDescriptorHandleForHeapStart(), bbIndex, Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(Engine::Ins()->swapchain_.heapDesc_.Type));

	CD3DX12_CPU_DESCRIPTOR_HANDLE backBufferHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(Engine::Ins()->swapchain_.rtvHeaps_->GetCPUDescriptorHandleForHeapStart(),
		Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(Engine::Ins()->swapchain_.heapDesc_.Type));
	D3D12_CPU_DESCRIPTOR_HANDLE dsvStartHandle = Engine::Ins()->swapchain_.dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	Engine::Ins()->mainGraphicsCmdList_->OMSetRenderTargets(1, &backBufferHandle, false, &dsvStartHandle);
	// �����_�[�^�[�Q�b�g�̃N���A
	float clearColor[] = { 0.5f,0.5f,0.5f,0.0f };
	Engine::Ins()->mainGraphicsCmdList_->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	// �[�x�o�b�t�@�̃N���A�R�}���h
	Engine::Ins()->mainGraphicsCmdList_->ClearDepthStencilView(Engine::Ins()->swapchain_.dsvHeap_->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void Engine::ResourceBarrierAfter()
{
	// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�ύX
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