#pragma once
#include <d3d12.h>
#include <wrl.h>

// ���C�g���o��UAV�N���X
class RaytracingOutput {

private:

	/*===== �����o�ϐ� =====*/

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;					// UAV�̐ݒ�
	Microsoft::WRL::ComPtr<ID3D12Resource> rayTracingOutput;	// UAV
	int uavDescriptorIndex;										// �f�B�X�N���v�^�q�[�v��̂���UAV�̃C���f�b�N�X


public:

	/*===== �����o�֐� =====*/

	// �Z�b�e�B���O
	void Setting();

	// �f�B�X�N���v�^���Z�b�g�B
	void SetComputeRootDescriptorTalbe(const int& RootParamIndex);

	// �Q�b�^
	Microsoft::WRL::ComPtr<ID3D12Resource> GetRaytracingOutput() { return rayTracingOutput; }

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTexture2D(UINT width, UINT height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);

};