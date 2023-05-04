#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Vec.h"

// �{�����[���e�N�X�`���N���X UAV�Ƃ��Ă����g���Ȃ�
class VolumeTextureUAV {

private:

	/*===== �����o�ϐ� =====*/

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc_;					// UAV�̐ݒ�
	Microsoft::WRL::ComPtr<ID3D12Resource> texture_;			// �e�N�X�`���{��
	int uavDescriptorIndex_;									// �f�B�X�N���v�^�q�[�v��̂���UAV�̃C���f�b�N�X


public:

	/*===== �����o�֐� =====*/

	// �Z�b�e�B���O
	void Setting(DXGI_FORMAT Format, LPCWSTR BufferName, Vec3 TextureSize, D3D12_RESOURCE_STATES ResourceState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_SOURCE);

	// �f�B�X�N���v�^���Z�b�g�B
	void SetComputeRootDescriptorTalbe(int RootParamIndex);

	// ���\�[�X�o���A
	void SetResourceBarrier(D3D12_RESOURCE_STATES Dst, D3D12_RESOURCE_STATES Src, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> CmdList = nullptr);

	// �Q�b�^
	Microsoft::WRL::ComPtr<ID3D12Resource> GetRaytracingOutput() { return texture_; }
	int GetUAVIndex() { return uavDescriptorIndex_; }

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTexture3D(UINT Width, UINT Height, UINT Depth, DXGI_FORMAT Format, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType);

};