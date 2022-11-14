#include "RaytracingOutput.h"
#include "DescriptorHeapMgr.h"
#include "Engine.h"
#include "WindowsAPI.h"

void RaytracingOutput::Setting(DXGI_FORMAT Format, LPCWSTR BufferName, Vec2 TextureSize, D3D12_RESOURCE_STATES ResourceState)
{

	/*===== �ݒ� =====*/

	// UAV��ݒ�
	rayTracingOutput_ = CreateTexture2D(
		static_cast<UINT>(TextureSize.x_), static_cast<UINT>(TextureSize.y_), Format,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		ResourceState,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// �擪�n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	// �f�B�X�N���v�^�q�[�v��UAV���m��
	uavDesc_.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	Engine::Ins()->device_.dev_->CreateUnorderedAccessView(
		rayTracingOutput_.Get(), nullptr, &uavDesc_, basicHeapHandle);

	// UAV�̃f�B�X�N���v�^�q�[�v�̃C���f�b�N�X���擾
	uavDescriptorIndex_ = DescriptorHeapMgr::Ins()->GetHead();

	// �f�B�X�N���v�^�q�[�v���C���N�������g
	DescriptorHeapMgr::Ins()->IncrementHead();

	rayTracingOutput_->SetName(BufferName);

}

void RaytracingOutput::SetComputeRootDescriptorTalbe(int RootParamIndex)
{

	Engine::Ins()->mainGraphicsCmdList_->SetComputeRootDescriptorTable(RootParamIndex, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(uavDescriptorIndex_));

}

void RaytracingOutput::SetResourceBarrier(D3D12_RESOURCE_STATES Dst, D3D12_RESOURCE_STATES Src, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> CmdList)
{

	/*===== ���\�[�X�o���A��J�� =====*/

	D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::Transition(
			GetRaytracingOutput().Get(),
			Dst,
			Src)
	};

	if (CmdList == nullptr) {

		Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, barrierToUAV);

	}
	else {

		CmdList->ResourceBarrier(1, barrierToUAV);

	}

}

Microsoft::WRL::ComPtr<ID3D12Resource> RaytracingOutput::CreateTexture2D(UINT Width, UINT Height, DXGI_FORMAT Format, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType)
{
	D3D12_HEAP_PROPERTIES heapProps{};
	if (HeapType == D3D12_HEAP_TYPE_DEFAULT) {
		heapProps = D3D12_HEAP_PROPERTIES{
		D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
		};
	}
	if (HeapType == D3D12_HEAP_TYPE_UPLOAD) {
		heapProps = D3D12_HEAP_PROPERTIES{
		D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
		};
	}
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Alignment = 0;
	resDesc.Width = Width;
	resDesc.Height = Height;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = Format;
	resDesc.SampleDesc = { 1, 0 };
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = Flags;

	hr = Engine::Ins()->device_.dev_->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		InitialState,
		nullptr,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
	);
	if (FAILED(hr)) {
		OutputDebugStringA("CreateTexture2D failed.\n");
		assert(0);
	}
	return resource;
}
