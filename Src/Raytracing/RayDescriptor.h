#pragma once
#include "Engine.h"

// レイトレーシング用に生成したディスクリプタクラス。レイトレーシング用のデータはすべてこのクラスを使用している。
class RayDescriptor {

private:

	/*===== メンバ変数 =====*/

	D3D12_CPU_DESCRIPTOR_HANDLE hCpu_;	// CPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE hGpu_;	// GPUハンドル


public:

	/*===== メンバ関数 =====*/

	// SRV生成処理
	void CreateStructuredSRV(Microsoft::WRL::ComPtr<ID3D12Resource> Resource, UINT NumElements, UINT FirstElement, UINT Stride, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& DescHeap, UINT Index);

	// アクセッサ
	inline D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle() { return hCpu_; }
	inline D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() { return hGpu_; }

};