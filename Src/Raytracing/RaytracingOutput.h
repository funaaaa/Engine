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
	void Setting(DXGI_FORMAT Format, LPCWSTR BufferName);

	// �f�B�X�N���v�^���Z�b�g�B
	void SetComputeRootDescriptorTalbe(const int& RootParamIndex);

	// ���\�[�X�o���A
	void SetResourceBarrier(D3D12_RESOURCE_STATES Dst, D3D12_RESOURCE_STATES Src);

	// �Q�b�^
	Microsoft::WRL::ComPtr<ID3D12Resource> GetRaytracingOutput() { return rayTracingOutput; }
	const int& GetUAVIndex() { return uavDescriptorIndex; }

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTexture2D(UINT Width, UINT Height, DXGI_FORMAT Format, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES IinitialState, D3D12_HEAP_TYPE HeapType);

};