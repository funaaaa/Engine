#pragma once

#include <DirectXTex/d3dx12.h>
#include <dxgi1_6.h>
#include <dinput.h>
#include <xstring>
#include <vector>
#include <array>
#include <wrl.h>
#include "Singleton.h"
#include <dxgi.h>
#include <Xinput.h>
#include <d3d12.h>
#include <memory>

class WindowsAPI;

class Engine : public Singleton<Engine> {

public:

	// �f�o�b�O�Ɏg�p����e�N���X
	struct Debug {
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController_;
		Microsoft::WRL::ComPtr<ID3D12Debug1> shaderDebugController_;
	};

	// DirectX�̃f�o�C�X�֘A
	struct Device {
		Microsoft::WRL::ComPtr<ID3D12Device5> dev_;
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory_;
		std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapters_;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> tmpAdapter_;
		std::vector<D3D_FEATURE_LEVEL> levels_;
		D3D_FEATURE_LEVEL featureLevel_;
	};

	// SwapChain�֘A
	struct SwapChain {
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeaps_;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc_;
		Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer_;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain_;
	};

	// Input�֘A
	struct DevInput {
		IDirectInput8* dinput_;				// DirectInput�I�u�W�F�N�g
		IDirectInputDevice8* devkeybord_;	// �L�[�{�[�h�I�u�W�F�N�g
		IDirectInputDevice8* devmouse_;		// �}�E�X�I�u�W�F�N�g
	};


public:

	/*===== �����o�ϐ� =====*/

	std::shared_ptr<WindowsAPI> windowsAPI_;	// WindowsAPI�N���X
	Debug debug_;								// �f�o�b�O�p�̃N���X
	Device device_;								// �f�o�C�X�֘A
	SwapChain swapchain_;						// SwapChain�֘A
	DevInput input_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heapForImgui_;	//imgui�p�f�B�X�N���v�^�q�[�v


	/*-- GPU�̔񓯊������ɂ������ĐV���ɍ�����ϐ����� --*/

	// �R�}���h�A���P�[�^�[
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mainGraphicsCmdAllocator_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> copyResourceCmdAllocator_;
	std::array < Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, 2> denoiseCmdAllocator_;

	// �R�}���h���X�g
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> mainGraphicsCmdList_;			// ���C���Ŏg�p����O���t�B�b�N�X�R�}���h���X�g
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> copyResourceCmdList_;			// CopyResource�Ŏg�p����O���t�B�b�N�X�R�}���h���X�g
	std::array < Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>, 2> denoiseCmdList_;// �f�m�C�Y�Ŏg�p����R�}���h���X�g SwapChain�̂悤�ɗ��ƕ\��؂�ւ��Ďg�p����B

	// �L���[
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> graphicsCmdQueue_;				// �O���t�B�b�N�X�L���[ ���C�g���֐���UI�̕`�擙���s���B
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> computeCmdQueue_;				// �R���s���[�g�L���[ ��Ƀf�m�C�Y���s���B

	// �t�F���X
	Microsoft::WRL::ComPtr<ID3D12Fence> GPUtoCPUFence_;							// mainGraphics��CPU�̓������Ƃ邽�߂̃t�F���X
	UINT64 GPUtoCPUFenceVal_;
	Microsoft::WRL::ComPtr<ID3D12Fence> graphicsToCopyFence_;					// copy�̏�����graphics�̏������I�������Ɏ��s���邽�߂̃t�F���X�B
	UINT64 graphicsToCopyFenceVal_;
	Microsoft::WRL::ComPtr<ID3D12Fence> finishCopyFence_;						// copy�I���Ď��p�t�F���X
	UINT64 finishCopyFenceVal_;
	std::array<Microsoft::WRL::ComPtr<ID3D12Fence>, 2> graphicsToDenoiseFence_;	// mainGraphics��denoise�̓������Ƃ邽�߂̃t�F���X SwapChain�̂悤�ɗ��ƕ\��؂�ւ��Ďg�p����B
	std::array<UINT64, 2> graphicsToDenoiseFenceVal_;
	std::array<Microsoft::WRL::ComPtr<ID3D12Fence>, 2> denoiseToCopyFence_;		// denoise��Copy�̓������Ƃ邽�߂̃t�F���X SwapChain�̂悤�ɗ��ƕ\��؂�ւ��Ďg�p����B
	std::array<UINT64, 2> denoiseToCopyFenceVal_;

	// ����Queue�̃C���f�b�N�X�B
	int currentQueueIndex_;
	int frameIndex_;

	// �e�R�}���h���X�g�ɏ������l�ނ��Ƃ��ł��邩�𔻒f����t���O
	std::array<bool, 2> canUseDenoiseCmdList_;	// DenoiseCmdList�ɃR�}���h���l�ނ��Ƃ��ł����Ԃ��̃t���O SwapChain�̂悤�ɗ��ƕ\��؂�ւ��Ďg�p����B
	bool canUseCopyCmdList_;					// CopyCmdList�ɃR�}���h���l�ނ��Ƃ��ł����Ԃ��̃t���O


public:
	// �R���X�g���N�^
	Engine();
	// ����������
	void Init();
	// ���t���[������(�`��O����)
	void ProcessBeforeDrawing();
	// ���t���[������(�`��㏈��)
	void ProcessAfterDrawing();

	// �����_�[�^�[�Q�b�g���Z�b�g
	void SetRenderTarget();
	// �`���̃��\�[�X�o���A
	void ResourceBarrierAfter();

	// imgui�p�q�[�v����
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeaoForImgui();
};