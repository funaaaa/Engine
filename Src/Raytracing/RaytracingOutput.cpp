#include "RaytracingOutput.h"
#include "DescriptorHeapMgr.h"
#include "DirectXBase.h"

void RaytracingOutput::Setting(DXGI_FORMAT Format)
{

	/*===== 設定 =====*/

	// UAVを設定
	rayTracingOutput = CreateTexture2D(
		window_width, window_height, Format,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// 先頭ハンドルを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	// ディスクリプタヒープにUAVを確保
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	DirectXBase::Ins()->dev->CreateUnorderedAccessView(
		rayTracingOutput.Get(), nullptr, &uavDesc, basicHeapHandle);

	// UAVのディスクリプタヒープのインデックスを取得
	uavDescriptorIndex = DescriptorHeapMgr::Ins()->GetHead();

	// ディスクリプタヒープをインクリメント
	DescriptorHeapMgr::Ins()->IncrementHead();

	rayTracingOutput->SetName(L"RayTracingOutputUAV");

}

void RaytracingOutput::SetComputeRootDescriptorTalbe(const int& RootParamIndex)
{

	DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(RootParamIndex, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(uavDescriptorIndex));

}

void RaytracingOutput::SetResourceBarrier(D3D12_RESOURCE_STATES Dst, D3D12_RESOURCE_STATES Src)
{

	/*===== リソースバリアを遷移 =====*/

	D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::Transition(
			GetRaytracingOutput().Get(),
			Dst,
			Src)
	};

	DirectXBase::Ins()->cmdList->ResourceBarrier(1, barrierToUAV);

}

Microsoft::WRL::ComPtr<ID3D12Resource> RaytracingOutput::CreateTexture2D(UINT width, UINT height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType)
{
	D3D12_HEAP_PROPERTIES heapProps{};
	if (heapType == D3D12_HEAP_TYPE_DEFAULT) {
		heapProps = D3D12_HEAP_PROPERTIES{
		D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
		};
	}
	if (heapType == D3D12_HEAP_TYPE_UPLOAD) {
		heapProps = D3D12_HEAP_PROPERTIES{
		D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
		};
	}
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Alignment = 0;
	resDesc.Width = width;
	resDesc.Height = height;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = format;
	resDesc.SampleDesc = { 1, 0 };
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = flags;

	hr = DirectXBase::Ins()->dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		initialState,
		nullptr,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
	);
	if (FAILED(hr)) {
		OutputDebugStringA("CreateTexture2D failed.\n");
		assert(0);
	}
	return resource;
}
