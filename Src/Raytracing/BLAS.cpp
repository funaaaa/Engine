#include "BLAS.h"
#include "ModelDataManager.h"
#include "DescriptorHeapMgr.h"
#include <DirectXMath.h>
#include "FbxLoader.h"
#include "RWStructuredBuffer.h"
#include "StructuredBuffer.h"
#include "FString.h"
#include "TextureManager.h"

using namespace DirectX;

void BLAS::GenerateBLASObj(const string& directryPath, const string& modelName, const wstring& hitGroupName, const LPCWSTR& TexturePath)
{

	/*===== BLASを生成する処理 =====*/

	/*-- 形状データを読み込む --*/

	// テクスチャを読み込む。
	textureHandle = TextureManager::Instance()->LoadTextureInDescriptorHeapMgr(TexturePath);

	// 読み込んだデータを一時保存する用のバッファ。
	Object3DDeliveryData dataBuff;

	// モデルをロード。
	ModelDataManager::LoadObj(directryPath, modelName, dataBuff, false);

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

void BLAS::GenerateBLASFbx(const string& directryPath, const string& modelName, const wstring& hitGroupName, const LPCWSTR& TexturePath)
{

	/*===== BLASを生成する処理 =====*/

	/*-- 形状データを読み込む --*/

	// テクスチャを読み込む。
	textureHandle = TextureManager::Instance()->LoadTextureInDescriptorHeapMgr(TexturePath);

	// 読み込んだデータを一時保存する用のバッファ。
	Object3DDeliveryData dataBuff;

	// モデルをロード。
	modelIndex = FbxLoader::Instance()->LoadModelFromFile(directryPath, modelName);

	dataBuff = FbxLoader::Instance()->ConvertObject3DDeliveryData(modelIndex);

	// 頂点数を求める。
	vertexCount = dataBuff.vertex.size();

	// 頂点インデックス数を求める。
	indexCount = dataBuff.index.size();

	Object3DDeliveryData fbxData;
	ModelDataManager::LoadFbx((directryPath + modelName).c_str(), fbxData);

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

	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Instance()->GetFbxModel(modelIndex).hasAnimation) {

		// 入力用構造体をリサイズ。
		skinComputeInput.resize(vertex.size());

		// スキニングアニメーションコンピュートシェーダーで使用する入力用構造体をセット。
		FbxLoader::Instance()->GetSkinComputeInput(modelIndex, skinComputeInput);

		// スキニングアニメーションで使用するコンピュートシェーダーをロードしておく。
		skinComput.Init(L"resource/ShaderFiles/RayTracing/ComputeSkin.hlsl", sizeof(FbxLoader::SkinComputeInput), skinComputeInput.size(), skinComputeInput.data(), sizeof(RayVertex), vertex.size(), vertex.data());

	}

}

void BLAS::Update()
{

	/*===== BLASの更新 =====*/

	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Instance()->GetFbxModel(modelIndex).hasAnimation) {

		// アニメーションの更新処理
		auto& model = FbxLoader::Instance()->GetFbxModel(modelIndex);
		if (model.isPlay) {

			model.currentTime += model.frameTime;

			// 最後まで再生したら先頭に戻す。
			if (model.endTime < model.currentTime) {

				model.currentTime = model.startTime;

			}

			// スキニングアニメーションコンピュートシェーダーで使用する入力用構造体をセット。
			FbxLoader::Instance()->GetSkinComputeInput(modelIndex, skinComputeInput);

		}

	};

	// 頂点を書き込む。 今のところは頂点しか書き換える予定はないが、後々他のやつも書き込む。ダーティフラグみたいなのを用意したい。
	WriteToMemory(vertexBuffer, vertex.data(), vertexStride * vertexCount);

	// 更新のための値を設定。
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc();
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc{};
	auto& inputs = asDesc.Inputs;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = 1;
	inputs.pGeometryDescs = &geomDesc;
	// BLAS の更新処理を行うためのフラグを設定する。
	inputs.Flags =
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE |
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;


	// インプレース更新を実行する。
	asDesc.SourceAccelerationStructureData = blasBuffer->GetGPUVirtualAddress();
	asDesc.DestAccelerationStructureData = blasBuffer->GetGPUVirtualAddress();
	// 更新用の作業バッファを設定する。
	asDesc.ScratchAccelerationStructureData = updateBuffer->GetGPUVirtualAddress();

	// コマンドリストに積む。
	DirectXBase::Instance()->cmdList->BuildRaytracingAccelerationStructure(
		&asDesc, 0, nullptr
	);

	CreateAccelerationStructure();

}

void BLAS::ComputeSkin()
{

	/*===== 頂点データをスキニング行列を元に書き換える処理 =====*/

	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Instance()->GetFbxModel(modelIndex).hasAnimation) {

		// 入力構造体を更新。
		skinComput.UpdateInputSB(skinComputeInput.data());

		// 計算開始。
		skinComput.Dispatch(1, 1, 1);

		// 結果を代入。
		memcpy(vertex.data(), skinComput.outputSB->buffersOnCPU, sizeof(RayVertex) * vertex.size());

	}

}

void BLAS::InitAnimation()
{
	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Instance()->GetFbxModel(modelIndex).hasAnimation) {

		FbxLoader::Instance()->GetFbxModel(modelIndex).InitAnimation();

	}
}

void BLAS::PlayAnimation()
{

	/*===== アニメーションさせる =====*/

	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Instance()->GetFbxModel(modelIndex).hasAnimation) {

		FbxLoader::Instance()->GetFbxModel(modelIndex).PlayAnimation();

	}

}

void BLAS::StopAnimation()
{

	/*===== アニメーションを停止させる =====*/

	// モデルがアニメーションを持っていたら。
	if (FbxLoader::Instance()->GetFbxModel(modelIndex).hasAnimation) {

		FbxLoader::Instance()->GetFbxModel(modelIndex).StopAnimation();

	}

}

void BLAS::WriteToMemory(ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize)
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

ComPtr<ID3D12Resource> BLAS::CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType)
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

D3D12_RAYTRACING_GEOMETRY_DESC BLAS::GetGeometryDesc()
{

	/*===== BLAS生成時に設定を取得する用関数 =====*/

	// 形状データのフラグを設定。
	auto geometryDesc = D3D12_RAYTRACING_GEOMETRY_DESC{};
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION;

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

void BLAS::SettingAccelerationStructure(const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc)
{

	/*===== 加速構造体の設定用関数 =====*/

	// AS(レイとの判定を迅速に行うために必要なポリゴンデータ構造体)の設定に必要な各項目を設定。
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc{};
	auto& inputs = buildASDesc.Inputs;	// D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
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

	// 更新用バッファを生成する。
	updateBuffer = CreateBuffer(
		blasPrebuild.UpdateScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// AccelerationStructureの構築。
	buildASDesc.ScratchAccelerationStructureData = scratchBuffer->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = blasBuffer->GetGPUVirtualAddress();
	// コマンドリストに積んで実行する。
	DirectXBase::Instance()->cmdList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr /* pPostBuildInfoDescs */
	);

	// BLASを構築する。
	CreateAccelerationStructure();

}

void BLAS::CreateAccelerationStructure()
{

	/*===== BLAS構築処理 =====*/

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

/*

◯メモ
・資料の実装を一通り終えた。
・コンピュートシェーダーから値ちゃんと送られてるっぽい？
・しかし形がおかしい。でも資料の最後の方に直し方があるから、ワンちゃんsoleで治るかも。
 →FbxLoader.cppでsortで出てくる条件式を逆にすると形はそれっぽくなる。アニメーションは反映されない。
・アニメーションが再生されない。
　→タイマーは変わっているので、ウェイト等の変数も正しく変わっているかを調べる。
　→形は後回し、まずはタイマーを反映させる。

*/