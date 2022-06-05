#pragma once

#pragma region ��邱��
/*
����������
�E�f�o�b�O���C���[�̗L����
�EDXGI�t�@�N�g���[�̐���
�E�O���t�B�b�N�X�A�_�v�^�[�̑I��
�E�f�o�C�X�̐���
�E�R�}���h�A���P�[�^�̐���
�E�R�}���h���X�g�̐���
�E�R�}���h�L���[�̐���
�E�X���b�v�`�F�[���̐���
�E�����_�[�^�[�Q�b�g�r���[�̐���
�E�[�x�o�b�t�@����
�E�[�x�o�b�t�@�r���[����
�E�t�F���X�̐���
���t���[������(�`��O����)
�E�����_�[�^�[�Q�b�g�̃��\�[�X�o���A�ύX
�E�����_�[�^�[�Q�b�g�̐ݒ�
�E��ʃN���A
�E�r���[�|�[�g�ݒ�
�E�V�U�����O��`�ݒ�
���t���[������(�`��㏈��)
�E�����_�[�^�[�Q�b�g�̃��\�[�X�o���A�ύX
�E�O���t�B�b�N�R�}���h���X�g�̃N���[�Y
�E�O���t�B�b�N�R�}���h���X�g�̎��s
�E�O���t�B�b�N�R�}���h���X�g�̊����҂�
�E�R�}���h�A���P�[�^�̃��Z�b�g
�E�R�}���h���X�g�̃��Z�b�g
�E��ʃo�b�t�@���t���b�v
*/
#pragma endregion

#include <DirectXTex/d3dx12.h>
#include <dxgi1_6.h>
#include <dinput.h>
#include <xstring>
#include <vector>
#include <wrl.h>
#include "WindowsAPI.h"
#include "Singleton.h"
#include <dxgi.h>
#include <Xinput.h>
#include <d3d12.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace Microsoft::WRL;

class DirectXBase : public Singleton<DirectXBase> {
public:
	static WindowsAPI windowsAPI;						//WindowsAPI�N���X
	static ComPtr<ID3D12Debug> debugController;
	static ComPtr<ID3D12Device5> dev;
	static ComPtr<IDXGIFactory6> dxgiFactory;
	static vector<ComPtr<IDXGIAdapter1>> adapters;		//�A�_�v�^�[�̗񋓗p
	static ComPtr<IDXGIAdapter1> tmpAdapter;			//����̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	static vector<D3D_FEATURE_LEVEL> levels;			//�Ή����x���̔z��
	static D3D_FEATURE_LEVEL featureLevel;
	static ComPtr<IDXGISwapChain4> swapchain;
	static ComPtr<ID3D12CommandAllocator> cmdAllocator;
	static ComPtr<ID3D12GraphicsCommandList4> cmdList;
	static ComPtr<ID3D12CommandQueue> cmdQueue;
	static ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	static vector<ComPtr<ID3D12Resource>> backBuffers;
	static D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	static ComPtr<ID3D12Resource> depthBuffer;			//�[�x�o�b�t�@
	static ComPtr<ID3D12DescriptorHeap> dsvHeap;		//�[�x�o�b�t�@�p�f�B�X�N���v�^�q�[�v
	static ComPtr<ID3D12Fence> fence;
	static UINT64 fenceVal;
	static IDirectInput8* dinput;						//DirectInput�I�u�W�F�N�g
	static IDirectInputDevice8* devkeybord;				//�L�[�{�[�h�I�u�W�F�N�g
	static IDirectInputDevice8* devmouse;				//�}�E�X�I�u�W�F�N�g


public:
	//�R���X�g���N�^
	friend Singleton<DirectXBase>;
	DirectXBase();
	//����������
	void Init();
	//���t���[������(�`��O����)
	void processBeforeDrawing();
	//���t���[������(�`��㏈��)
	void processAfterDrawing();

	//�����_�[�^�[�Q�b�g���Z�b�g
	void SetRenderTarget();
	//�`���̃��\�[�X�o���A
	void ResourceBarrierAfter();

	//imgui�p�q�[�v����
	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeaoForImgui();
};
class ImGuiWindow : public Singleton<ImGuiWindow> {
public:
	static WindowsAPI windowsAPI;						//WindowsAPI�N���X
	static ComPtr<ID3D12Debug> debugController;
	static ComPtr<ID3D12Device5> dev;
	static ComPtr<IDXGIFactory6> dxgiFactory;
	static vector<ComPtr<IDXGIAdapter1>> adapters;		//�A�_�v�^�[�̗񋓗p
	static ComPtr<IDXGIAdapter1> tmpAdapter;			//����̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	static vector<D3D_FEATURE_LEVEL> levels;			//�Ή����x���̔z��
	static D3D_FEATURE_LEVEL featureLevel;
	static ComPtr<IDXGISwapChain4> swapchain;
	static ComPtr<ID3D12CommandAllocator> cmdAllocator;
	static ComPtr<ID3D12GraphicsCommandList4> cmdList;
	static ComPtr<ID3D12CommandQueue> cmdQueue;
	static ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	static vector<ComPtr<ID3D12Resource>> backBuffers;
	static D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	static ComPtr<ID3D12Resource> depthBuffer;			//�[�x�o�b�t�@
	static ComPtr<ID3D12DescriptorHeap> dsvHeap;		//�[�x�o�b�t�@�p�f�B�X�N���v�^�q�[�v
	static ComPtr<ID3D12Fence> fence;
	static UINT64 fenceVal;
	static IDirectInput8* dinput;						//DirectInput�I�u�W�F�N�g
	static IDirectInputDevice8* devkeybord;				//�L�[�{�[�h�I�u�W�F�N�g
	static IDirectInputDevice8* devmouse;				//�}�E�X�I�u�W�F�N�g
	
	static ComPtr<ID3D12DescriptorHeap> heapForImgui;	//imgui�p�f�B�X�N���v�^�q�[�v

public:
	//�R���X�g���N�^
	friend Singleton<ImGuiWindow>;
	ImGuiWindow();
	//����������
	void Init();
	//���t���[������(�`��O����)
	void processBeforeDrawing();
	//���t���[������(�`��㏈��)
	void processAfterDrawing();

	//�����_�[�^�[�Q�b�g���Z�b�g
	void SetRenderTarget();
	//�`���̃��\�[�X�o���A
	void ResourceBarrierAfter();

	//imgui�p�q�[�v����
	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeaoForImgui();
};