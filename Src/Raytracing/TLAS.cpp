#include "TLAS.h"
#include "PolygonInstanceRegister.h"
#include "DescriptorHeapMgr.h"

void TLAS::GenerateTLAS()
{

	/*===== TLAS生成処理 =====*/

	// インスタンスの行列を計算。
	PolygonInstanceRegister::Ins()->CalWorldMat();


	/*-- 加速構造体を設定 --*/
	SettingAccelerationStructure();


	/*-- TLAS用のディスクリプタを生成する --*/

	// ディスクリプタヒープのインデックスを取得
	descriptorHeapIndex_ = DescriptorHeapMgr::Ins()->GetHead();

	// ディスクリプタヒープにSRVを確保
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.RaytracingAccelerationStructure.Location = tlasBuffer_->GetGPUVirtualAddress();
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	Engine::Ins()->device_.dev_->CreateShaderResourceView(nullptr, &srvDesc,
		basicHeapHandle);

	// ディスクリプタヒープをインクリメント
	DescriptorHeapMgr::Ins()->IncrementHead();

}

void TLAS::Update()
{

	/*===== TLASの更新処理 =====*/

	// インスタンスの行列を計算。
	PolygonInstanceRegister::Ins()->CalWorldMat();

	// Instanceのサイズを取得。
	auto sizeOfInstanceDescs = PolygonInstanceRegister::MAX_INSTANCE;
	sizeOfInstanceDescs *= sizeof(D3D12_RAYTRACING_INSTANCE_DESC);

	// CPU から書き込み可能なバッファに書き込む。
	WriteToMemory(instanceDescBuffer_, PolygonInstanceRegister::Ins()->GetData(), sizeOfInstanceDescs);

	// 更新のための値を設定。
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc{};
	auto& inputs = asDesc.Inputs;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = PolygonInstanceRegister::MAX_INSTANCE;
	inputs.InstanceDescs = instanceDescBuffer_->GetGPUVirtualAddress();
	// TLAS の更新処理を行うためのフラグを設定する。
	inputs.Flags =
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE |
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;


	// インプレース更新を実行する。
	asDesc.SourceAccelerationStructureData = tlasBuffer_->GetGPUVirtualAddress();
	asDesc.DestAccelerationStructureData = tlasBuffer_->GetGPUVirtualAddress();
	// 更新用の作業バッファを設定する。
	asDesc.ScratchAccelerationStructureData = tlasUpdateBuffer_->GetGPUVirtualAddress();

	// コマンドリストに積む。
	Engine::Ins()->mainGraphicsCmdList_->BuildRaytracingAccelerationStructure(
		&asDesc, 0, nullptr
	);

	// 実行する。
	/*CreateAccelerationStructure();*/

}

void TLAS::WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* PData, size_t DataSize)
{

	/*===== メモリに値を書き込む処理 =====*/

	// nullチェック。
	if (Resource == nullptr) return;

	// マップ処理を行う。
	void* mapped = nullptr;
	D3D12_RANGE range{ 0, DataSize };
	HRESULT hr = Resource->Map(0, nullptr, (void**)&mapped);

	// マップが成功したら値を書き込む。
	if (SUCCEEDED(hr)) {

		memcpy(mapped, PData, DataSize);
		Resource->Unmap(0, nullptr);

	}

}

Microsoft::WRL::ComPtr<ID3D12Resource> TLAS::CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType)
{

	/*===== バッファ全般を生成する処理 =====*/

	// 引数から設定用構造体を設定する。
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

	// 実際にバッファを生成する。
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = 0;
	resDesc.Width = Size;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc = { 1, 0 };
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = Flags;

	// バッファ生成命令を出す。
	hr = Engine::Ins()->device_.dev_->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		InitialState,
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
	size_t sizeOfInstanceDescs = PolygonInstanceRegister::MAX_INSTANCE * sizeof(D3D12_RAYTRACING_INSTANCE_DESC);
	instanceDescBuffer_ = CreateBuffer(
		sizeOfInstanceDescs,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_HEAP_TYPE_UPLOAD);
	instanceDescBuffer_->SetName(L"instanceDescBuffer_");

	// 生成したバッファにデータを書き込む。
	WriteToMemory(instanceDescBuffer_, PolygonInstanceRegister::Ins()->GetData(), sizeOfInstanceDescs);

	// メモリ量を求めるための設定を行う。
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
	auto& inputs = buildASDesc.Inputs;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = PolygonInstanceRegister::MAX_INSTANCE;
	inputs.InstanceDescs = instanceDescBuffer_->GetGPUVirtualAddress();
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;

	// メモリ量を求める関数を実行する。
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO tlasPrebuild{};
	Engine::Ins()->device_.dev_->GetRaytracingAccelerationStructurePrebuildInfo(
		&inputs, &tlasPrebuild
	);

	/*-- TLASバッファとスクラッチバッファを生成する --*/

	// スクラッチメモリ(バッファ)を確保。
	scratchBuffer_ = CreateBuffer(
		tlasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);
	scratchBuffer_->SetName(L"TLASScratchBuffer");

	// TLAS用メモリ(バッファ)を確保。
	tlasBuffer_ = CreateBuffer(
		tlasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);
	tlasBuffer_->SetName(L"tlasBuffer_");

	// TLAS更新用メモリ(バッファ)を確保。
	tlasUpdateBuffer_ = CreateBuffer(
		tlasPrebuild.UpdateScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);
	tlasUpdateBuffer_->SetName(L"tlasUpdateBuffer_");

	/*-- BLASのアドレスとスクラッチバッファアドレスとTLASのアドレスを指定して確保処理をコマンドリストに積む --*/

	// AccelerationStructure構築。
	buildASDesc.ScratchAccelerationStructureData = scratchBuffer_->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = tlasBuffer_->GetGPUVirtualAddress();

	// コマンドリストに積んで実行する。
	Engine::Ins()->mainGraphicsCmdList_->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr
	);


	/*-- 実際にバッファを生成する --*/

	CreateAccelerationStructure();

}

void TLAS::CreateAccelerationStructure()
{

	/*===== 実際のバッファを生成する =====*/

	// リソースバリアの設定。
	D3D12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(tlasBuffer_.Get());
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &uavBarrier);
	Engine::Ins()->mainGraphicsCmdList_->Close();

	// TLASを構築。
	//ID3D12CommandList* pCmdList[] = { Engine::Ins()->cmdList.Get() };

	// 構築用関数を呼ぶ。
	ID3D12CommandList* commandLists[] = { Engine::Ins()->mainGraphicsCmdList_.Get() };
	Engine::Ins()->graphicsCmdQueue_->ExecuteCommandLists(1, commandLists);
	Engine::Ins()->graphicsCmdQueue_->Signal(Engine::Ins()->asFence_.Get(), ++Engine::Ins()->asfenceValue_);


	/*-- リソースバリアを設定して書き込めないようにする --*/

	// グラフィックコマンドリストの完了待ち
	UINT64 gpuFence = Engine::Ins()->asFence_->GetCompletedValue();
	if (gpuFence != Engine::Ins()->asfenceValue_) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		Engine::Ins()->asFence_->SetEventOnCompletion(Engine::Ins()->asfenceValue_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//コマンドアロケータのリセット
	Engine::Ins()->mainGraphicsCmdAllocator_->Reset();						//キューをクリア

	//コマンドリストのリセット
	Engine::Ins()->mainGraphicsCmdList_->Reset(Engine::Ins()->mainGraphicsCmdAllocator_.Get(), nullptr);		//再びコマンドリストを貯める準備

}
