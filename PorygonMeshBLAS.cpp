#include "PorygonMeshBLAS.h"
#include "ModelDataManager.h"
#include "DescriptorHeapMgr.h"
#include <DirectXMath.h>

using namespace DirectX;

void PorygonMeshBlas::GenerateBLAS(const string& directryPath, const string& modelName, const wstring& hitGroupName, const bool& isFbx)
{

	/*===== BLASを生成する処理 =====*/

	/*-- 形状データを読み込む --*/

	// 読み込んだデータを一時保存する用のバッファ。
	Object3DDeliveryData dataBuff;

	// モデルをロード。
	if (isFbx) {
		string path = directryPath;
		path += modelName;
		ModelDataManager::LoadFbx(path.c_str(), dataBuff);
	}
	else {
		ModelDataManager::LoadObj(directryPath, modelName, dataBuff, false);
	}

	// 頂点数を求める。
	vertexCount = dataBuff.vertex.size();

	// 頂点インデックス数を求める。
	indexCount = dataBuff.index.size();

	// 頂点データを変換。
	for (int index = 0; index < vertexCount; ++index) {

		RayVertex buff{};
		buff.normal = dataBuff.vertex[index].normal;
		buff.position = dataBuff.vertex[index].pos;
		buff.uv = dataBuff.vertex[index].uv;

		// データを保存。
		vertex.push_back(buff);

	}

	// 頂点インデックスデータを保存。
	vertIndex = dataBuff.index;

	// 頂点サイズを求める。
	vertexStride = sizeof(RayVertex);

	// 頂点インデックスサイズを求める。
	indexStride = sizeof(UINT);

	// 頂点バッファを生成する。
	vertexBuffer = CreateBuffer(
		vertexStride * vertexCount,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// 確保したバッファに頂点データを書き込む。
	WriteToMemory(vertexBuffer, vertex.data(), vertexStride * vertexCount);

	// 頂点インデックスバッファを生成する。
	indexBuffer = CreateBuffer(
		indexStride * indexCount,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// 確保したインデックスバッファに頂点インデックスデータを書き込む。
	WriteToMemory(indexBuffer, vertIndex.data(), indexStride * indexCount);

	// 頂点インデックスデータでディスクリプタを生成。
	indexDescriptor.CreateStructuredSRV(indexBuffer, indexCount, 0, indexStride, DescriptorHeapMgr::Instance()->GetDescriptorHeap(), DescriptorHeapMgr::Instance()->GetHead());
	DescriptorHeapMgr::Instance()->IncrementHead();

	// 頂点データでディスクリプタを生成。
	vertexDescriptor.CreateStructuredSRV(vertexBuffer, vertexCount, 0, vertexStride, DescriptorHeapMgr::Instance()->GetDescriptorHeap(), DescriptorHeapMgr::Instance()->GetHead());
	DescriptorHeapMgr::Instance()->IncrementHead();


	/*-- BLASバッファを生成する --*/

	// 形状を設定する用の構造体を設定。
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc();

	// BLASバッファを設定、構築する。
	SettingAccelerationStructure(geomDesc);


	// ヒットグループ名を保存する。
	this->hitGroupName = hitGroupName;

}

void PorygonMeshBlas::WriteToMemory(ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize)
{

	/*===== メモリに値を書き込む処理 =====*/

	// nullチェック。
	if (resource == nullptr) return;

	// マップ処理を行う。
	void* mapped = nullptr;
	D3D12_RANGE range{ 0, dataSize };
	HRESULT hr = resource->Map(0, &range, (void**)&mapped);

	// マップが成功したら値を書き込む。
	if (SUCCEEDED(hr)) {

		memcpy(mapped, pData, dataSize);
		resource->Unmap(0, nullptr);

	}

}

ComPtr<ID3D12Resource> PorygonMeshBlas::CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType)
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
	ComPtr<ID3D12Resource> resource;
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
	hr = DirectXBase::Instance()->dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		initialState,
		nullptr,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
	);

	// 生成に失敗したら。
	if (FAILED(hr)) {
		OutputDebugStringA("CreateBuffer failed.\n");
	}

	return resource;

}

D3D12_RAYTRACING_GEOMETRY_DESC PorygonMeshBlas::GetGeometryDesc()
{

	/*===== BLAS生成時に設定を取得する用関数 =====*/

	// 形状データのフラグを設定。
	auto geometryDesc = D3D12_RAYTRACING_GEOMETRY_DESC{};
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

	// 形状データの細かい項目を設定。
	auto& triangles = geometryDesc.Triangles;
	triangles.VertexBuffer.StartAddress = vertexBuffer->GetGPUVirtualAddress();
	triangles.VertexBuffer.StrideInBytes = vertexStride;
	triangles.VertexCount = vertexCount;
	triangles.IndexBuffer = indexBuffer->GetGPUVirtualAddress();
	triangles.IndexCount = indexCount;
	triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	triangles.IndexFormat = DXGI_FORMAT_R32_UINT;

	return geometryDesc;

}

void PorygonMeshBlas::SettingAccelerationStructure(const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc)
{

	/*===== 加速構造体の設定用関数 =====*/

	// AS(レイとの判定を迅速に行うために必要なポリゴンデータ構造体)の設定に必要な各項目を設定。
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc{};
	auto& inputs = buildASDesc.Inputs;	// D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
	inputs.NumDescs = 1;
	inputs.pGeometryDescs = &geomDesc;

	// 関数を使って必要なメモリ量を求める.
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO blasPrebuild{};
	DirectXBase::Instance()->dev->GetRaytracingAccelerationStructurePrebuildInfo(
		&inputs, &blasPrebuild
	);

	// スクラッチバッファを生成する。
	scratchBuffer = CreateBuffer(
		blasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// BLASのバッファを生成する。
	blasBuffer = CreateBuffer(
		blasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// BLASを構築する。
	CreateAccelerationStructure(buildASDesc);

}

void PorygonMeshBlas::CreateAccelerationStructure(D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& buildASDesc)
{

	/*===== BLAS構築処理 =====*/

	// AccelerationStructureの構築。
	buildASDesc.ScratchAccelerationStructureData = scratchBuffer->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = blasBuffer->GetGPUVirtualAddress();
	// コマンドリストに積んで実行する。
	DirectXBase::Instance()->cmdList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr /* pPostBuildInfoDescs */
	);

	// リソースバリアの設定。
	D3D12_RESOURCE_BARRIER uavBarrier{};
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = blasBuffer.Get();
	DirectXBase::Instance()->cmdList->ResourceBarrier(1, &uavBarrier);
	DirectXBase::Instance()->cmdList->Close();

	// BLASを構築。
	ID3D12CommandList* pCmdList[] = { DirectXBase::Instance()->cmdList.Get() };
	// 構築用関数を呼ぶ。
	ID3D12CommandList* commandLists[] = { DirectXBase::Instance()->cmdList.Get() };
	DirectXBase::Instance()->cmdQueue->ExecuteCommandLists(1, commandLists);

	//グラフィックコマンドリストの完了待ち
	DirectXBase::Instance()->cmdQueue->Signal(DirectXBase::Instance()->fence.Get(), ++DirectXBase::Instance()->fenceVal);
	if (DirectXBase::Instance()->fence->GetCompletedValue() != DirectXBase::Instance()->fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		DirectXBase::Instance()->fence->SetEventOnCompletion(DirectXBase::Instance()->fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//コマンドアロケータのリセット
	DirectXBase::Instance()->cmdAllocator->Reset();						//キューをクリア

	//コマンドリストのリセット
	DirectXBase::Instance()->cmdList->Reset(DirectXBase::Instance()->cmdAllocator.Get(), nullptr);		//再びコマンドリストを貯める準備

}
