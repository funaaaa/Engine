#include "BaseObject3D.h"
#include "DirectXBase.h"

void BaseObject3D::MapVertexData()
{
	Vertex* vertMap = nullptr;
	vertBuff->Map(0, nullptr, (void**)&vertMap);
	// 全頂点に対して
	for (int i = 0; i < vertex.size(); ++i)
	{
		vertMap[i] = vertex.at(i);   // 座標をコピー
	}
	// マップを解除
	vertBuff->Unmap(0, nullptr);
}

short BaseObject3D::AddConstBuffer(const int& dataSize, const string& structName)
{

	/*-- 定数バッファを追加する --*/

	// 最後尾にデータを追加。
	constBufferData.push_back(ComPtr<ID3D12Resource>());

	// 名前データを保存。
	constBufferDataName.push_back(structName);

	// 最後尾に追加されたデータに値を入れる。
	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((dataSize + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBufferData[constBufferData.size() - 1])
	);

	int offset = isLighting ? 2 : 0;
	// 定数バッファビューにセット
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		constDescHeap->GetCPUDescriptorHandleForHeapStart(), offset + constBufferData.size(), DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = constBufferData[constBufferData.size() - 1]->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBufferData[constBufferData.size() - 1]->GetDesc().Width;
	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	return constBufferData.size() - 1;
}

bool BaseObject3D::TransData(const short& handle, const void* data, const int& dataSize, const string& structName)
{

	/*-- 追加された定数バッファの転送処理 --*/

	// 配列外アクセスを阻止する。
	if (handle >= constBufferData.size() || handle < 0) assert(0);

	// 保存された名前と受け取った名前が違かったら処理を行わない。
	if (constBufferDataName[handle] != structName) assert(0);

	// ここまで来たら転送処理を行う。
	void* constMap = nullptr;
	HRESULT result = constBufferData[handle]->Map(0, nullptr, (void**)&constMap);

	if (result == S_OK) {

		memcpy(constMap, data, dataSize);

	}

	constBufferData[handle]->Unmap(0, nullptr);

	if (result == S_OK) {

		return true;

	}
	else {

		return false;

	}

}
