#pragma once

#include <DirectXTex/d3dx12.h>
#include <dxgi1_6.h>
#include <dinput.h>
#include <xstring>
#include <vector>
#include <wrl.h>
#include "Singleton.h"
#include <dxgi.h>
#include <Xinput.h>
#include <d3d12.h>
#include <memory>

class WindowsAPI;

class Engine : public Singleton<Engine> {

public:

	std::shared_ptr<WindowsAPI> windowsAPI_;						//WindowsAPI�N���X
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController_;
	Microsoft::WRL::ComPtr<ID3D12Debug1> shaderDebugController_;
	Microsoft::WRL::ComPtr<ID3D12Device5> dev_;
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory_;
	std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapters_;//  �A�_�v�^�[�̗񋓗p
	Microsoft::WRL::ComPtr<IDXGIAdapter1> tmpAdapter_;			// ����̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	std::vector<D3D_FEATURE_LEVEL> levels_;						// �Ή����x���̔z��
	D3D_FEATURE_LEVEL featureLevel_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeaps_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc_;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer_;			// �[�x�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;		// �[�x�o�b�t�@�p�f�B�X�N���v�^�q�[�v
	IDirectInput8* dinput_;										// DirectInput�I�u�W�F�N�g
	IDirectInputDevice8* devkeybord_;							// �L�[�{�[�h�I�u�W�F�N�g
	IDirectInputDevice8* devmouse_;								// �}�E�X�I�u�W�F�N�g

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heapForImgui_;	//imgui�p�f�B�X�N���v�^�q�[�v

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain_;

	// GPU�̔񓯊������ɕK�v�ȕϐ��B
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mainGraphicsCmdAllocator_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> copyResourceCmdAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> mainGraphicsCmdList_;	// ���C���Ŏg�p����O���t�B�b�N�X�R�}���h���X�g
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> copyResourceCmdList;		// CopyResource�Ŏg�p����O���t�B�b�N�X�R�}���h���X�g
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> graphicsCmdQueue_;				// �O���t�B�b�N�X�L���[
	Microsoft::WRL::ComPtr<ID3D12Fence> graphicsToDenoiseFence_;	// mainGraphics��denoise�̓������Ƃ邽�߂̃t�F���X
	UINT64 graphicsToDenoiseFenceVal_;
	Microsoft::WRL::ComPtr<ID3D12Fence> GPUtoCPUFence_;				// mainGraphics��CPU�̓������Ƃ邽�߂̃t�F���X
	UINT64 GPUtoCPUFenceVal_;
	Microsoft::WRL::ComPtr<ID3D12Fence> denoiseToCopyFence_;		// denoise��Copy�̓������Ƃ邽�߂̃t�F���X
	UINT64 denoiseToCopyFenceVal_;
	Microsoft::WRL::ComPtr<ID3D12Fence> finishCopyFence_;			// copy�I���Ď��p�t�F���X
	UINT64 finishCopyFenceVal_;
	bool canUseMainGraphicsQueue_;			// MainGraphicsCmdList�ɃR�}���h���l�ނ��Ƃ��ł����Ԃ��̃t���O
	bool canUseDenoiseQueue_;				// DenoiseCmdList�ɃR�}���h���l�ނ��Ƃ��ł����Ԃ��̃t���O
	bool canUseCopyQueue_;					// CopyCmdList�ɃR�}���h���l�ނ��Ƃ��ł����Ԃ��̃t���O
	bool hasFinishedMainGraphicsProcess_;	// ���̃t���[����MainGraphicsCmdList�̏������I��������ǂ����̃t���O


public:
	//�R���X�g���N�^
	friend Singleton<Engine>;
	Engine();
	//����������
	void Init();
	//���t���[������(�`��O����)
	void ProcessBeforeDrawing();
	//���t���[������(�`��㏈��)
	void ProcessAfterDrawing();

	//�����_�[�^�[�Q�b�g���Z�b�g
	void SetRenderTarget();
	//�`���̃��\�[�X�o���A
	void ResourceBarrierAfter();

	//imgui�p�q�[�v����
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeaoForImgui();
};