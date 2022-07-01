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

class DirectXBase : public Singleton<DirectXBase> {

public:

	std::shared_ptr<WindowsAPI> windowsAPI;						//WindowsAPI�N���X
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	Microsoft::WRL::ComPtr<ID3D12Device5> dev;
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
	std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapters;//�A�_�v�^�[�̗񋓗p
	Microsoft::WRL::ComPtr<IDXGIAdapter1> tmpAdapter;			//����̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	std::vector<D3D_FEATURE_LEVEL> levels;						//�Ή����x���̔z��
	D3D_FEATURE_LEVEL featureLevel;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> cmdList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;			//�[�x�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;		//�[�x�o�b�t�@�p�f�B�X�N���v�^�q�[�v
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal;
	IDirectInput8* dinput;										//DirectInput�I�u�W�F�N�g
	IDirectInputDevice8* devkeybord;							//�L�[�{�[�h�I�u�W�F�N�g
	IDirectInputDevice8* devmouse;								//�}�E�X�I�u�W�F�N�g

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heapForImgui;	//imgui�p�f�B�X�N���v�^�q�[�v


public:
	//�R���X�g���N�^
	friend Singleton<DirectXBase>;
	DirectXBase();
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