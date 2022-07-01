#include "RayDescriptor.h"

void RayDescriptor::CreateStructuredSRV(Microsoft::WRL::ComPtr<ID3D12Resource> Resource, UINT NumElements, UINT FirstElement, UINT Stride, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descHeap, UINT Index)
{

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = NumElements;
	srvDesc.Buffer.FirstElement = FirstElement;
	srvDesc.Buffer.StructureByteStride = Stride;

	hCpu = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		descHeap->GetCPUDescriptorHandleForHeapStart(), Index, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	hGpu = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		descHeap->GetGPUDescriptorHandleForHeapStart(), Index, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	DirectXBase::Ins()->dev->CreateShaderResourceView(Resource.Get(), &srvDesc, hCpu);

}
