#pragma once
#include "DirectXBase.h"


class RayDescriptor {

private:

	/*===== メンバ変数 =====*/

	D3D12_CPU_DESCRIPTOR_HANDLE hCpu;	// CPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE hGpu;	// GPUハンドル


public:

	/*===== メンバ関数 =====*/

	// SRV生成処理
	void CreateStructuredSRV(Microsoft::WRL::ComPtr<ID3D12Resource> resource, UINT numElements, UINT firstElement, UINT stride, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descHeap, UINT index);

	// アクセッタ
	inline D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle() { return hCpu; }
	inline D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() { return hGpu; }

};