#include "StructuredBuffer.h"
#include <DirectXTex/d3dx12.h>
#include "DirectXBase.h"


void StructuredBuffer::Init(int SizeOfElement, int NumElement, void* InitData)
{
	this->sizeOfElement_ = SizeOfElement;
	this->numElement_ = NumElement;
	auto device = DirectXBase::Ins()->dev_;

	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(SizeOfElement * NumElement);
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	int bufferNo = 0;

	D3D12_HEAP_PROPERTIES prop{};
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	prop.CreationNodeMask = 1;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	prop.Type = D3D12_HEAP_TYPE_CUSTOM;
	prop.VisibleNodeMask = 1;
	device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&buffersOnGPU_)
	);


	// 構造化バッファをCPUからアクセス可能な仮想アドレス空間にマッピングする。
	CD3DX12_RANGE readRange(0, 0);
	buffersOnGPU_->Map(0, &readRange, reinterpret_cast<void**>(&buffersOnCPU_));
	if (InitData != nullptr) {
		memcpy(buffersOnCPU_, InitData, static_cast<size_t>(SizeOfElement * NumElement));
	}

	bufferNo++;
	isInited_ = true;
}
void StructuredBuffer::Update(void* Data)
{
	if (Data == nullptr) return;
	memcpy(buffersOnCPU_, Data, static_cast<size_t>(numElement_ * sizeOfElement_));
}
Microsoft::WRL::ComPtr<ID3D12Resource> StructuredBuffer::GetD3DResoruce()
{
	return buffersOnGPU_;
}
void StructuredBuffer::RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle)
{
	if (!isInited_) {
		return;
	}
	auto device = DirectXBase::Ins()->dev_;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = static_cast<UINT>(numElement_);
	srvDesc.Buffer.StructureByteStride = sizeOfElement_;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	device->CreateShaderResourceView(
		buffersOnGPU_.Get(),
		&srvDesc,
		DescriptorHandle
	);
}