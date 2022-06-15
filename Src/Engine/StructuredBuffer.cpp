#include "StructuredBuffer.h"
#include <DirectXTex/d3dx12.h>
#include "DirectXBase.h"


void StructuredBuffer::Init(int sizeOfElement, int numElement, void* initData)
{
	this->sizeOfElement = sizeOfElement;
	this->numElement = numElement;
	auto device = DirectXBase::Ins()->dev;

	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(sizeOfElement * numElement);
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	int bufferNo = 0;

	D3D12_HEAP_PROPERTIES prop{};
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	prop.CreationNodeMask = 1;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	prop.Type = D3D12_HEAP_TYPE_CUSTOM;
	prop.VisibleNodeMask = 1;
	//	for (auto& buffer : buffersOnGPU) {
	auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			IID_PPV_ARGS(&buffersOnGPU)
		);


	//構造化バッファをCPUからアクセス可能な仮想アドレス空間にマッピングする。
	//マップ、アンマップのオーバーヘッドを軽減するためにはこのインスタンスが生きている間は行わない。
		CD3DX12_RANGE readRange(0, 0);        //     intend to read from this resource on the CPU.
		buffersOnGPU->Map(0, &readRange, reinterpret_cast<void**>(&buffersOnCPU));
	if (initData != nullptr) {
		memcpy(buffersOnCPU, initData, sizeOfElement * numElement);
		//buffersOnGPU->Unmap(0, &readRange);
	}

	bufferNo++;
	//	}
	isInited = true;
}
void StructuredBuffer::Update(void* data)
{
	if (data == nullptr) return;
	memcpy(buffersOnCPU, data, numElement * sizeOfElement);
}
ID3D12Resource* StructuredBuffer::GetD3DResoruce()
{
	auto backBufferIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();
	return buffersOnGPU;
}
void StructuredBuffer::RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo)
{
	if (!isInited) {
		return;
	}
	auto device = DirectXBase::Ins()->dev;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = static_cast<UINT>(numElement);
	srvDesc.Buffer.StructureByteStride = sizeOfElement;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	device->CreateShaderResourceView(
		buffersOnGPU,
		&srvDesc,
		descriptorHandle
	);
}