#include "TLAS.h"
#include "PorygonInstanceRegister.h"
#include "DescriptorHeapMgr.h"

void TLAS::GenerateTLAS()
{

	/*===== TLAS生成処理 =====*/

	// インスタンスの行列を計算。
	PorygonInstanceRegister::Ins()->CalWorldMat();


	/*-- 加速構造体を設定 --*/
	SettingAccelerationStructure();


	/*-- TLAS用のディスクリプタを生成する --*/

	// ディスクリプタヒープのインデックスを取得
	descriptorHeapIndex = DescriptorHeapMgr::Ins()->GetHead();

	// ディスクリプタヒープにSRVを確保
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.RaytracingAccelerationStructure.Location = tlasBuffer->GetGPUVirtualAddress();
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	DirectXBase::Ins()->dev->CreateShaderResourceView(nullptr, &srvDesc,
		basicHeapHandle);

	// ディスクリプタヒープをインクリメント
	DescriptorHeapMgr::Ins()->IncrementHead();

}

//void tlas::updateinstancedata()
//{
//
//	// インスタンスの情報を記録したバッファを準備する。
//	size_t sizeofinstancedescs = porygoninstanceregister::instance()->getregistersize() * sizeof(d3d12_raytracing_instance_desc);
//
//	// 生成したバッファにデータを書き込む。
//	writetomemory(instancedescbuffer, porygoninstanceregister::instance()->getdata(), sizeofinstancedescs);
//
//}

void TLAS::Update()
{

	/*===== TLASの更新処理 =====*/

	// インスタンスの行列を計算。
	PorygonInstanceRegister::Ins()->CalWorldMat();

	// Instanceのサイズを取得。
	auto sizeOfInstanceDescs = PorygonInstanceRegister::Ins()->GetRegisterSize();
	sizeOfInstanceDescs *= sizeof(D3D12_RAYTRACING_INSTANCE_DESC);

	// CPU から書き込み可能なバッファに書き込む。
	WriteToMemory(instanceDescBuffer, PorygonInstanceRegister::Ins()->GetData(), sizeOfInstanceDescs);

	// 更新のための値を設定。
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc{};
	auto& inputs = asDesc.Inputs;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = PorygonInstanceRegister::Ins()->GetRegisterSize();
	inputs.InstanceDescs = instanceDescBuffer->GetGPUVirtualAddress();
	// TLAS の更新処理を行うためのフラグを設定する。
	inputs.Flags =
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE |
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;


	// インプレース更新を実行する。
	asDesc.SourceAccelerationStructureData = tlasBuffer->GetGPUVirtualAddress();
	asDesc.DestAccelerationStructureData = tlasBuffer->GetGPUVirtualAddress();
	// 更新用の作業バッファを設定する。
	asDesc.ScratchAccelerationStructureData = tlasUpdateBuffer->GetGPUVirtualAddress();

	// コマンドリストに積む。
	DirectXBase::Ins()->cmdList->BuildRaytracingAccelerationStructure(
		&asDesc, 0, nullptr
	);

	// 実行する。
	/*CreateAccelerationStructure();*/

}

void TLAS::WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize)
{

	/*===== メモリに値を書き込む処理 =====*/

	// nullチェック。
	if (resource == nullptr) return;

	// マップ処理を行う。
	void* mapped = nullptr;
	D3D12_RANGE range{ 0, dataSize };
	HRESULT hr = resource->Map(0, nullptr, (void**)&mapped);

	// マップが成功したら値を書き込む。
	if (SUCCEEDED(hr)) {

		memcpy(mapped, pData, dataSize);
		resource->Unmap(0, nullptr);

	}

}

Microsoft::WRL::ComPtr<ID3D12Resource> TLAS::CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType)
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

	// 生成に失敗したら。
	if (FAILED(hr)) {
		OutputDebugStringA("CreateBuffer failed.\n");
	}

	return resource;

}

void TLAS::SettingAccelerationStructure()
{

	/*===== 加速構造体生成関数 =====*/


	/*-- TLASの生成に必要なメモリ量を求める --*/

	// インスタンスの情報を記録したバッファを準備する。
	size_t sizeOfInstanceDescs = PorygonInstanceRegister::Ins()->GetRegisterSize() * sizeof(D3D12_RAYTRACING_INSTANCE_DESC);
	instanceDescBuffer = CreateBuffer(
		sizeOfInstanceDescs,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_HEAP_TYPE_UPLOAD);

	// 生成したバッファにデータを書き込む。
	WriteToMemory(instanceDescBuffer, PorygonInstanceRegister::Ins()->GetData(), sizeOfInstanceDescs);

	// メモリ量を求めるための設定を行う。
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
	auto& inputs = buildASDesc.Inputs;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = PorygonInstanceRegister::Ins()->GetRegisterSize();
	inputs.InstanceDescs = instanceDescBuffer->GetGPUVirtualAddress();
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;

	// メモリ量を求める関数を実行する。
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO tlasPrebuild{};
	DirectXBase::Ins()->dev->GetRaytracingAccelerationStructurePrebuildInfo(
		&inputs, &tlasPrebuild
	);

	/*-- TLASバッファとスクラッチバッファを生成する --*/

	// スクラッチメモリ(バッファ)を確保。
	scratchBuffer = CreateBuffer(
		tlasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// TLAS用メモリ(バッファ)を確保。
	tlasBuffer = CreateBuffer(
		tlasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// TLAS更新用メモリ(バッファ)を確保。
	tlasUpdateBuffer = CreateBuffer(
		tlasPrebuild.UpdateScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);

	/*-- BLASのアドレスとスクラッチバッファアドレスとTLASのアドレスを指定して確保処理をコマンドリストに積む --*/

	// AccelerationStructure構築。
	buildASDesc.ScratchAccelerationStructureData = scratchBuffer->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = tlasBuffer->GetGPUVirtualAddress();

	// コマンドリストに積んで実行する。
	DirectXBase::Ins()->cmdList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr
	);


	/*-- 実際にバッファを生成する --*/

	CreateAccelerationStructure();

}

void TLAS::CreateAccelerationStructure()
{

	/*===== 実際のバッファを生成する =====*/

	// リソースバリアの設定。
	D3D12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(tlasBuffer.Get());
	DirectXBase::Ins()->cmdList->ResourceBarrier(1, &uavBarrier);
	DirectXBase::Ins()->cmdList->Close();

	// TLASを構築。
	//ID3D12CommandList* pCmdList[] = { DirectXBase::Ins()->cmdList.Get() };

	// 構築用関数を呼ぶ。
	ID3D12CommandList* commandLists[] = { DirectXBase::Ins()->cmdList.Get() };
	DirectXBase::Ins()->cmdQueue->ExecuteCommandLists(1, commandLists);


	/*-- リソースバリアを設定して書き込めないようにする --*/

	//グラフィックコマンドリストの完了待ち
	DirectXBase::Ins()->cmdQueue->Signal(DirectXBase::Ins()->fence.Get(), ++DirectXBase::Ins()->fenceVal);
	if (DirectXBase::Ins()->fence->GetCompletedValue() != DirectXBase::Ins()->fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		DirectXBase::Ins()->fence->SetEventOnCompletion(DirectXBase::Ins()->fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//コマンドアロケータのリセット
	DirectXBase::Ins()->cmdAllocator->Reset();						//キューをクリア

	//コマンドリストのリセット
	DirectXBase::Ins()->cmdList->Reset(DirectXBase::Ins()->cmdAllocator.Get(), nullptr);		//再びコマンドリストを貯める準備

}
