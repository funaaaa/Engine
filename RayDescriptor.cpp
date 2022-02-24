#include "RayDescriptor.h"

void RayDescriptor::CreateStructuredSRV(ComPtr<ID3D12Resource> resource, UINT numElements, UINT firstElement, UINT stride, ComPtr<ID3D12DescriptorHeap>& descHeap, UINT index)
{

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = numElements;
	srvDesc.Buffer.FirstElement = firstElement;
	srvDesc.Buffer.StructureByteStride = stride;

	hCpu = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		descHeap->GetCPUDescriptorHandleForHeapStart(), index, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	hGpu = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		descHeap->GetGPUDescriptorHandleForHeapStart(), index, DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	DirectXBase::Instance()->dev->CreateShaderResourceView(resource.Get(), &srvDesc, hCpu);

}
