#include "DynamicConstBuffer.h"

DynamicConstBuffer::DynamicConstBuffer()
{

	/*===== コンストラクタ =====*/

	buffer.clear();

}

void DynamicConstBuffer::Generate(UINT bufferSize, const wchar_t* name)
{

	/*===== バッファ生成処理 =====*/

	// バックバッファの数分バッファを生成する。
	bufferSize = RoundUp(bufferSize, 256);
	UINT count = 2;
	buffer.resize(count);

	// バッファを生成する。
	for (UINT i = 0; i < count; ++i) {
		buffer[i] = CreateBuffer(
			bufferSize,
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_HEAP_TYPE_UPLOAD);

		// 名前をセットする。
		if (buffer[i]) {
			buffer[i]->SetName(name);
		}
	}

}

void DynamicConstBuffer::Write(UINT bufferIndex, const void* data, UINT size)
{

	/*===== データ書き込み処理 =====*/

	auto& buff = buffer[bufferIndex];
	void* dst = nullptr;
	buff->Map(0, nullptr, &dst);
	if (dst) {
		memcpy(dst, data, size);
		buff->Unmap(0, nullptr);
	}

}

Microsoft::WRL::ComPtr<ID3D12Resource> DynamicConstBuffer::CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType)
{

	/*===== バッファ全般を生成する処理 =====*/

	// 引数から設定用構造体を設定する。
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

	// 実際にバッファを生成する。
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = 0;
	resDesc.Width = size;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc = { 1, 0 };
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = flags;

	// バッファ生成命令を出す。
	hr = DirectXBase::Ins()->dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		initialState,
		nullptr,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
	);

	// ディスクリプタヒープの生成
	descHeapIndex = DescriptorHeapMgr::Ins()->GetHead();

	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), descHeapIndex, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)size;
	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	DescriptorHeapMgr::Ins()->IncrementHead();

	// 生成に失敗したら。
	if (FAILED(hr)) {
		OutputDebugStringA("CreateBuffer failed.\n");
	}

	return resource;

}
