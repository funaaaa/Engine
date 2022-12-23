#include "DynamicConstBuffer.h"
#include "FHelper.h"

DynamicConstBuffer::DynamicConstBuffer()
{

	/*===== コンストラクタ =====*/

	buffer_.clear();

}

void DynamicConstBuffer::Generate(UINT BufferSize, const wchar_t* Name)
{

	/*===== バッファ生成処理 =====*/

	// バックバッファの数分バッファを生成する。
	BufferSize = RoundUp(BufferSize, 256);
	UINT count = 2;
	buffer_.resize(count);
	bufferMapAddress_.resize(count);

	// バッファのサイズを保存
	bufferSize_ = BufferSize;

	// バッファを生成する。
	for (UINT i = 0; i < count; ++i) {
		buffer_[i] = CreateBuffer(
			BufferSize,
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_HEAP_TYPE_UPLOAD);

		// 名前をセットする。
		if (buffer_[i]) {
			buffer_[i]->SetName(Name);
		}

		// Mapする。
		buffer_[i]->Map(0, nullptr, &bufferMapAddress_[i]);
	}

}

void DynamicConstBuffer::Write(UINT BufferIndex, const void* Data, UINT Size)
{

	/*===== データ書き込み処理 =====*/

	// 引数で渡されたバッファのサイズが生成された値と違います。
	Size = RoundUp(Size, 256);
	if (Size != bufferSize_) {
		assert(0);
	}

	memcpy(bufferMapAddress_[BufferIndex], Data, bufferSize_);

}

Microsoft::WRL::ComPtr<ID3D12Resource> DynamicConstBuffer::CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType)
{

	/*===== バッファ全般を生成する処理 =====*/

	// バッファを生成。
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = FHelper::CreateBuffer(Size, Flags, InitialState, HeapType);

	// ディスクリプタヒープの生成
	descHeapIndex_ = DescriptorHeapMgr::Ins()->GetHead();

	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), descHeapIndex_, Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)Size;
	Engine::Ins()->device_.dev_->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	DescriptorHeapMgr::Ins()->IncrementHead();

	return resource;

}
