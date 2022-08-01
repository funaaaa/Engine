#pragma once
#include "DirectXBase.h"


class RayDescriptor {

private:

	/*===== �����o�ϐ� =====*/

	D3D12_CPU_DESCRIPTOR_HANDLE hCpu_;	// CPU�n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE hGpu_;	// GPU�n���h��


public:

	/*===== �����o�֐� =====*/

	// SRV��������
	void CreateStructuredSRV(Microsoft::WRL::ComPtr<ID3D12Resource> Resource, UINT NumElements, UINT FirstElement, UINT Stride, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& DescHeap, UINT Index);

	// �A�N�Z�b�T
	inline D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle() { return hCpu_; }
	inline D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() { return hGpu_; }

};