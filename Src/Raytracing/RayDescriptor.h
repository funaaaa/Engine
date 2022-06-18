#pragma once
#include "DirectXBase.h"


class RayDescriptor {

private:

	/*===== �����o�ϐ� =====*/

	D3D12_CPU_DESCRIPTOR_HANDLE hCpu;	// CPU�n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE hGpu;	// GPU�n���h��


public:

	/*===== �����o�֐� =====*/

	// SRV��������
	void CreateStructuredSRV(Microsoft::WRL::ComPtr<ID3D12Resource> resource, UINT numElements, UINT firstElement, UINT stride, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descHeap, UINT index);

	// �A�N�Z�b�^
	inline D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle() { return hCpu; }
	inline D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() { return hGpu; }

};