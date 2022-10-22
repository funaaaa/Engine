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
	std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapters_;//�A�_�v�^�[�̗񋓗p
	Microsoft::WRL::ComPtr<IDXGIAdapter1> tmpAdapter_;			//����̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	std::vector<D3D_FEATURE_LEVEL> levels_;						//�Ή����x���̔z��
	D3D_FEATURE_LEVEL featureLevel_;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> cmdList_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeaps_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc_;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer_;			//�[�x�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;		//�[�x�o�b�t�@�p�f�B�X�N���v�^�q�[�v
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	UINT64 fenceVal_;
	IDirectInput8* dinput_;										//DirectInput�I�u�W�F�N�g
	IDirectInputDevice8* devkeybord_;							//�L�[�{�[�h�I�u�W�F�N�g
	IDirectInputDevice8* devmouse_;								//�}�E�X�I�u�W�F�N�g

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heapForImgui_;	//imgui�p�f�B�X�N���v�^�q�[�v


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