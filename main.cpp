#include "SoundManager.h"
#include "LightManager.h"
#include "PiplineManager.h"
#include "RenderTarget.h"
#include "Sprite.h"
#include "FHelper.h"
#include "Enum.h"
#include "Object3D.h"
#include "MultiPathFunction.h"
#include "Camera.h"
#include "LightCamera.h"
#include "Input.h"
#include "TextureManager.h"

#include <utilapiset.h>

#define COLORHEX(hex) hex / 255.0f

#define SCREEN_VIRTUAL_WIDTH 300

static const wchar_t* hitGroupName = L"hitGroupName";

// fps更新
void FPS();

// スラリンラボから持ってきた関数
namespace surarin {

	ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType, const wchar_t* name = nullptr) {
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

		hr = DirectXBase::Instance()->dev->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			initialState,
			nullptr,
			IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
		);
		if (FAILED(hr)) {
			OutputDebugStringA("CreateBuffer failed.\n");
		}
		if (resource != nullptr && name != nullptr) {
			resource->SetName(name);
		}
		return resource;
	}

	void ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList4> command)
	{
		ID3D12CommandList* commandLists[] = {
			command.Get(),
		};
		DirectXBase::Instance()->cmdQueue->ExecuteCommandLists(1, commandLists);
	}

	void WriteToHostVisibleMemory(ComPtr<ID3D12Resource> resource, const void* pData, size_t dataSize) {
		if (resource == nullptr) {
			return;
		}
		void* mapped = nullptr;
		D3D12_RANGE range{ 0, dataSize };
		HRESULT hr = resource->Map(0, nullptr, (void**)&mapped);
		if (SUCCEEDED(hr)) {
			memcpy(mapped, pData, dataSize);
			resource->Unmap(0, nullptr);
		}
	}

	void WaitForIdleGpu() {
		if (DirectXBase::Instance()->cmdQueue) {
			auto commandList = DirectXBase::Instance()->cmdList;
			auto waitFence = DirectXBase::Instance()->fence;
			UINT64 fenceValue = 1;
			auto event = CreateEvent(nullptr, false, false, nullptr);
			waitFence->SetEventOnCompletion(fenceValue, event);
			DirectXBase::Instance()->cmdQueue->Signal(waitFence.Get(), fenceValue);
			WaitForSingleObject(event, INFINITE);
		}
	}

	ComPtr<ID3D12Resource> CreateTexture2D(UINT width, UINT height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType) {
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
		ComPtr<ID3D12Resource> resource;
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

		hr = DirectXBase::Instance()->dev->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			initialState,
			nullptr,
			IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
		);
		if (FAILED(hr)) {
			OutputDebugStringA("CreateTexture2D failed.\n");
		}
		return resource;
	}

	UINT RoundUp(size_t size, UINT align) {
		return UINT(size + align - 1) & ~(align - 1);
	}

}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	/*----------DirectX初期化処理----------*/
	DirectXBase directXBase;							// DirectX基盤部分
	directXBase.Init();									// DirectX基盤の初期化
	SoundManager::Instance()->SettingSoundManager();	// サウンドマネージャーをセットする

	/*----------パイプライン生成----------*/
	PiplineManager::Instance()->Init();

	/*----------変数宣言----------*/
	srand(time(NULL));

	// 三角形データをロード
	/*Object3D triangle;
	triangle.Generate(XMFLOAT3(0, 0, 0), PROJECTIONID_OBJECT, PIPLINE_OBJECT_NOLIGHT_ALPHA, "Resource/", "triangle.obj", L"Resource/plane.png", FALSE);
	triangle.ChangeScale(100, 100, 100);*/

	XMFLOAT3 tri[] = {
	XMFLOAT3(-0.5f, -0.5f, 0.0f),
	XMFLOAT3(+0.5f, -0.5f, 0.0f),
	XMFLOAT3(0.0f, +0.75f, 0.0f)
	};





	auto vbSize = sizeof(tri);
	ComPtr<ID3D12Resource> vertexBuffer = surarin::CreateBuffer(
		vbSize,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	// 確保したバッファに頂点データを書き込む.
	surarin::WriteToHostVisibleMemory(vertexBuffer, tri, vbSize);



	/*==========  ①BLASの定義  ==========*/


	// 1、BLASに必要なメモリ量を求める。
	{
		/*
		D3D12_RAYTRACING_GEOMETRY_DESCについて
		・三角形ジオメトリ、非三角形ジオメトリの２種類が設定できる。
		・今回は三角形ジオメトリを使用するので、D3D12_RAYTRACING_GEOMETRY_DESC内のtrianglesに値を入れていく。
		・頂点のサイズなどを入れていくが、TriangleStripなどには対応していない。TriangleListのみ。
		*/
	}
	// 形状を設定する用の構造体を設定。
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc{};
	geomDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geomDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	geomDesc.Triangles.VertexBuffer.StartAddress = vertexBuffer->GetGPUVirtualAddress();
	geomDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(XMFLOAT3);
	geomDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	geomDesc.Triangles.VertexCount = _countof(tri);

	{
		/*
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESCについて
		・この構造体はASをTOPLEVELかBOTTOMLEVELのどちらの構成にも使用される。
		・今回はBLASを生成するため、BOTTOMLEVELを指定する。
		・用意したジオメトリ情報を入れる。
		・フラグの種類についてはスラリンラボp44を参照。
		*/
	}
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


	/*-- 2、スクラッチバッファとBLASバッファを生成する --*/

	// スクラッチバッファを生成する。
	ComPtr<ID3D12Resource> blasScratch;
	blasScratch = surarin::CreateBuffer(
		blasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// BLASのバッファを生成する。
	ComPtr<ID3D12Resource> 	triangleBlas;
	triangleBlas = surarin::CreateBuffer(
		blasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);


	/*-- 3、スクラッチバッファとBLASバッファのアドレスを使ってBLAS構築コマンドを積む --*/

	// Acceleration Structureの構築。
	buildASDesc.ScratchAccelerationStructureData = blasScratch->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = triangleBlas->GetGPUVirtualAddress();
	// コマンドリストに積んで実行する。
	DirectXBase::Instance()->cmdList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr /* pPostBuildInfoDescs */
	);

	// リソースバリアの設定。
	D3D12_RESOURCE_BARRIER uavBarrier{};
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = triangleBlas.Get();
	DirectXBase::Instance()->cmdList->ResourceBarrier(1, &uavBarrier);
	DirectXBase::Instance()->cmdList->Close();


	/*-- 4、コマンドを実行してBLASを生成する --*/

	// BLASを構築。
	ID3D12CommandList* pCmdList[] = { DirectXBase::Instance()->cmdList.Get() };
	surarin::ExecuteCommandList(DirectXBase::Instance()->cmdList);


	/*-- 5、UAVバリアを設定して書き込めないようにする --*/

	// 本関数を抜けるとコマンドやスクラッチバッファが解放となるため待機
	//surarin::WaitForIdleGpu();//グラフィックコマンドリストの実行
	//ID3D12CommandList* cmdLists[] = { DirectXBase::Instance()->cmdList.Get() }; //コマンドリストの配列
	//DirectXBase::Instance()->cmdQueue->ExecuteCommandLists(1, cmdLists);

	//画面バッファをフリップ
	//DirectXBase::Instance()->swapchain->Present(1, 0);

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




	/*==========  ②TLASの定義  ==========*/


	// TLASを定義する前に、BLASを3D空間上に配置するための設定を行う。

	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc{};
	// 変換行列は float m[3][4]のため XMFLOAT3X4 を使用.
	XMStoreFloat3x4(
		reinterpret_cast<XMFLOAT3X4*>(&instanceDesc.Transform),
		XMMatrixIdentity());
	instanceDesc.InstanceID = 0;
	instanceDesc.InstanceMask = 0xFF;
	instanceDesc.InstanceContributionToHitGroupIndex = 0;
	instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	instanceDesc.AccelerationStructure = triangleBlas->GetGPUVirtualAddress();
	// インスタンスの情報を記録したバッファを準備する.
	ComPtr<ID3D12Resource> instanceDescBuffer;
	instanceDescBuffer = surarin::CreateBuffer(
		sizeof(instanceDesc),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_HEAP_TYPE_UPLOAD
	);
	surarin::WriteToHostVisibleMemory(
		instanceDescBuffer,
		&instanceDesc, sizeof(instanceDesc)
	);


	/*-- 1、TLASに必要なメモリ量を求める --*/

	// メモリ量を求めるための設定を行う。
	buildASDesc = {};
	inputs = buildASDesc.Inputs; // D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
	inputs.NumDescs = 1;
	// メモリ量を求める関数を実行する。
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO tlasPrebuild{};
	DirectXBase::Instance()->dev->GetRaytracingAccelerationStructurePrebuildInfo(
		&inputs, &tlasPrebuild
	);



	/*-- 2、スクラッチバッファとTLASバッファを確保する --*/

	ComPtr<ID3D12Resource> tlasScratch;
	tlasScratch = surarin::CreateBuffer(
		tlasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);
	// TLAS 用メモリ(バッファ)を確保.
	// リソースステートはD3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE
	ComPtr<ID3D12Resource> triangleScratch;
	triangleScratch = surarin::CreateBuffer(
		tlasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);


	/*-- 3、BLASのアドレスとスクラッチバッファアドレスとTLASのアドレスを指定して確保処理をコマンドリストに積む --*/

	// Acceleration Structure 構築.
	buildASDesc.Inputs.InstanceDescs = instanceDescBuffer->GetGPUVirtualAddress();
	buildASDesc.ScratchAccelerationStructureData = tlasScratch->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = triangleScratch->GetGPUVirtualAddress();
	// コマンドリストに積んで実行する.
	DirectXBase::Instance()->cmdList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr /* pPostBuildInfoDescs */
	);


	/*-- 4、実行する --*/

	// リソースバリアの設定.
	uavBarrier = {};
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = triangleScratch.Get();
	DirectXBase::Instance()->cmdList->ResourceBarrier(1, &uavBarrier);
	DirectXBase::Instance()->cmdList->Close();
	// TLAS 構築.
	surarin::ExecuteCommandList(DirectXBase::Instance()->cmdList);


	/*-- 5、リソースバリアを設定して書き込めないようにする --*/

	// 本関数を抜けるとスクラッチバッファとInstanceDescのバッファが解放となるため待機.
	//surarin::WaitForIdleGpu();
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


	/*-- 6、TLASのディスクリプタを作る --*/

	// ディスクリプタの準備。 AllocateDescriptorではディスクリプターを生成して確保しているので、既存のコードから持ってくる。
	D3D12_DESCRIPTOR_HEAP_DESC tlasDescHeapDesc{};
	tlasDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	tlasDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			// シェーダーから見える
	tlasDescHeapDesc.NumDescriptors = 32;										// CBV3つ
	// ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> tlasDescriptor = {};
	HRESULT resultBuff = DirectXBase::Instance()->dev->CreateDescriptorHeap(&tlasDescHeapDesc, IID_PPV_ARGS(&tlasDescriptor));
	// ディスクリプタヒープにSRVを確保
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.RaytracingAccelerationStructure.Location = triangleScratch->GetGPUVirtualAddress();
	DirectXBase::Instance()->dev->CreateShaderResourceView(nullptr, &srvDesc, tlasDescriptor->GetCPUDescriptorHandleForHeapStart());





	/*==========  グローバルルートシグネチャを生成  ==========*/


	vector<D3D12_ROOT_PARAMETER> rootParams;
	rootParams.resize(2);

	// TLAS を t0 レジスタに割り当てて使用する設定.
	D3D12_DESCRIPTOR_RANGE descRangeTLAS{};
	descRangeTLAS.BaseShaderRegister = 0;
	descRangeTLAS.NumDescriptors = 1;
	descRangeTLAS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	// 出力バッファ(UAV) を u0 レジスタに割り当てて使用する設定.
	D3D12_DESCRIPTOR_RANGE descRangeOutput{};
	descRangeOutput.BaseShaderRegister = 0;
	descRangeOutput.NumDescriptors = 1;
	descRangeOutput.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

	// TLASを設定。
	rootParams[0] = D3D12_ROOT_PARAMETER{};
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParams[0].DescriptorTable.NumDescriptorRanges = 1;
	rootParams[0].DescriptorTable.pDescriptorRanges = &descRangeTLAS;

	// 出力用バッファを設定。
	rootParams[1] = D3D12_ROOT_PARAMETER{};
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;
	rootParams[1].DescriptorTable.pDescriptorRanges = &descRangeOutput;

	// グローバルルートシグネチャ設定用構造体を設定。
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.NumParameters = UINT(rootParams.size());
	rootSigDesc.pParameters = rootParams.data();

	// ルートシグネチャ生成.
	ComPtr<ID3DBlob> blob, errBlob;
	ComPtr<ID3D12RootSignature> grobalRootsignature;
	D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &blob, &errBlob);
	DirectXBase::Instance()->dev->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
		IID_PPV_ARGS(&grobalRootsignature));





	/*==========  シェーダーのコンパイル  ==========*/

	// シェーダーをコンパイルする。
	ShaderStorage::Instance()->LoadShaderForDXC("Resource/ShaderFiles/RayTracing/triangleShaderHeader.hlsl", "lib_6_3", "");





	/*==========  ステートオブジェクトの生成  ==========*/

	// ステートオブジェクトの設定を保存しておくようの構造体
	vector<D3D12_STATE_SUBOBJECT> subobjects;


	/*-- 1、シェーダーを読み込んでシンボルを取得 --*/

	// 各シェーダーのエントリーポイントを設定。
	D3D12_EXPORT_DESC exports[] = {
	{ L"mainRayGen", nullptr, D3D12_EXPORT_FLAG_NONE },
	{ L"mainMS", nullptr, D3D12_EXPORT_FLAG_NONE },
	{ L"mainCHS", nullptr, D3D12_EXPORT_FLAG_NONE },
	};


	vector<char> shaderBin;
	shaderBin = ShaderStorage::Instance()->GetShaderBin("Resource/ShaderFiles/RayTracing/triangleShaderHeader.hlsl");

	// シェーダーのBlobを取得して登録する。
	D3D12_DXIL_LIBRARY_DESC dxilLibDesc{};
	dxilLibDesc.DXILLibrary = D3D12_SHADER_BYTECODE{ shaderBin.data(),shaderBin.size() };
	dxilLibDesc.NumExports = _countof(exports);
	dxilLibDesc.pExports = exports;

	// ステートオブジェクト設定保存用配列にプッシュバック
	subobjects.emplace_back(
		D3D12_STATE_SUBOBJECT{
			D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY, &dxilLibDesc
		}
	);


	/*-- 2、ヒットグループを生成 --*/

	D3D12_HIT_GROUP_DESC hitGroupDesc{};
	hitGroupDesc.Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
	hitGroupDesc.ClosestHitShaderImport = L"mainCHS";
	hitGroupDesc.HitGroupExport = hitGroupName;
	subobjects.emplace_back(
		D3D12_STATE_SUBOBJECT{
		D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP, &hitGroupDesc
		}
	);


	/*-- 3、グローバルルートシグネチャの設定 --*/

	D3D12_GLOBAL_ROOT_SIGNATURE rootSignatureGlobal{};
	rootSignatureGlobal.pGlobalRootSignature = grobalRootsignature.Get();
	subobjects.emplace_back(
		D3D12_STATE_SUBOBJECT{
		D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE, &rootSignatureGlobal
		}
	);


	/*-- 4、再帰呼び出しや各種引数情報のサイズの設定 --*/

	// シェーダーで使用する各要素を設定
	D3D12_RAYTRACING_SHADER_CONFIG shaderConfig{};
	shaderConfig.MaxPayloadSizeInBytes = sizeof(XMFLOAT3);
	shaderConfig.MaxAttributeSizeInBytes = sizeof(XMFLOAT2);
	subobjects.emplace_back(
		D3D12_STATE_SUBOBJECT{
		D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG, &shaderConfig
		}
	);

	// シェーダーでの再帰可能段階を設定
	D3D12_RAYTRACING_PIPELINE_CONFIG pipelineConfig{};
	pipelineConfig.MaxTraceRecursionDepth = 1;
	subobjects.emplace_back(
		D3D12_STATE_SUBOBJECT{
		D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG, &pipelineConfig
		}
	);


	/*-- 5、ID3D12StateObjectを生成 --*/

	// ステートオブジェクト
	ComPtr<ID3D12StateObject> stateObject;

	// ステートオブジェクトを生成
	D3D12_STATE_OBJECT_DESC stateObjDesc{};
	stateObjDesc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
	stateObjDesc.NumSubobjects = UINT(subobjects.size());
	stateObjDesc.pSubobjects = subobjects.data();
	resultBuff = DirectXBase::Instance()->dev->CreateStateObject(
		&stateObjDesc, IID_PPV_ARGS(stateObject.ReleaseAndGetAddressOf())
	);





	/*==========  UAV出力バッファの準備  ==========*/

	// UAVを設定
	ComPtr<ID3D12Resource> rayTracingOutput = surarin::CreateTexture2D(
		window_width, window_height, DXGI_FORMAT_R8G8B8A8_UNORM,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// ディスクリプタヒープの準備
	D3D12_DESCRIPTOR_HEAP_DESC uavDescHeapDesc{};
	uavDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	uavDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			// シェーダーから見える
	uavDescHeapDesc.NumDescriptors = 32;										// CBV3つ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> uavDescriptor = {};
	resultBuff = DirectXBase::Instance()->dev->CreateDescriptorHeap(&uavDescHeapDesc, IID_PPV_ARGS(&uavDescriptor));

	// ディスクリプタヒープにUAVを確保
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	DirectXBase::Instance()->dev->CreateUnorderedAccessView(
		rayTracingOutput.Get(), nullptr, &uavDesc, uavDescriptor->GetCPUDescriptorHandleForHeapStart());





	/*==========  ShaderTableの生成  ==========*/

	// 各シェーダーレコードはShaderIdentifierを保持する。
	UINT recordSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

	// グローバルのルートシグネチャ以外の情報を持たないのでレコードサイズはこれだけ.
	// あとはアライメント制約を保つようにする.
	recordSize = surarin::RoundUp(recordSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);

	// シェーダーテーブルのサイズを求める.
	UINT raygenSize = 1 * recordSize; // 今は1つの Ray Generation シェーダーを利用している。
	UINT missSize = 1 * recordSize; // 今1つの Miss シェーダーを利用している。
	UINT hitGroupSize = 1 * recordSize; // 今1つの HitGroup を使用している。

	// 各テーブルの開始位置にアライメント制約があるので調整する。
	auto tableAlign = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;
	UINT raygenRegion = surarin::RoundUp(raygenSize, tableAlign);
	UINT missRegion = surarin::RoundUp(missSize, tableAlign);
	UINT hitgroupRegion = surarin::RoundUp(hitGroupSize, tableAlign);

	// シェーダーテーブル確保。
	auto tableSize = raygenRegion + missRegion + hitgroupRegion;
	ComPtr<ID3D12Resource> shaderTable = surarin::CreateBuffer(
		tableSize,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_HEAP_TYPE_UPLOAD
	);

	// StateObjectからプロパティを取得。
	ComPtr<ID3D12StateObjectProperties> rtsoProps;
	stateObject.As(&rtsoProps);

	// 各シェーダーレコードを書き込んでいく。
	void* mapped = nullptr;
	shaderTable->Map(0, nullptr, &mapped);
	uint8_t* pStart = static_cast<uint8_t*>(mapped);
	// RayGeneration 用のシェーダーレコードを書き込み。
	auto rgsStart = pStart;
	{

		uint8_t* p = rgsStart;
		auto id = rtsoProps->GetShaderIdentifier(L"mainRayGen");
		memcpy(p, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
		p += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		// ローカルルートシグネチャ使用時には他のデータを書き込む。

	}

	// Miss Shader 用のシェーダーレコードを書き込み。
	auto missStart = pStart + raygenRegion;
	{

		uint8_t* p = missStart;
		auto id = rtsoProps->GetShaderIdentifier(L"mainMS");
		memcpy(p, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
		p += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		// ローカルルートシグネチャ使用時には他のデータを書き込む。

	}

	// Hit Group 用のシェーダーレコードを書き込み。
	auto hitgroupStart = pStart + raygenRegion + missRegion;
	{

		uint8_t* p = hitgroupStart;
		auto id = rtsoProps->GetShaderIdentifier(hitGroupName);
		memcpy(p, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
		p += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		// ローカルルートシグネチャ使用時には他のデータを書き込む。

	}

	// マップ処理を終える。
	shaderTable->Unmap(0, nullptr);





	/*==========  D3D12_DISPATCH_RAYS_DESCの設定  ==========*/

	// レイトレーシングを開始する際に使用する構造体。
	D3D12_DISPATCH_RAYS_DESC dispatchRayDesc = {};

	// RayGenerationシェーダーのシェーダーレコードをセット。
	auto startAddress = shaderTable->GetGPUVirtualAddress();
	auto& shaderRecordRG = dispatchRayDesc.RayGenerationShaderRecord;
	shaderRecordRG.StartAddress = startAddress;
	shaderRecordRG.SizeInBytes = raygenSize;
	startAddress += raygenRegion;

	// Missシェーダーのシェーダーレコードをセット。
	auto& shaderRecordMS = dispatchRayDesc.MissShaderTable;
	shaderRecordMS.StartAddress = startAddress;
	shaderRecordMS.SizeInBytes = missSize;
	shaderRecordMS.StrideInBytes = recordSize;
	startAddress += missRegion;

	// ClosestHitシェーダーのシェーダーレコードをセット。
	auto& shaderRecordHG = dispatchRayDesc.HitGroupTable;
	shaderRecordHG.StartAddress = startAddress;
	shaderRecordHG.SizeInBytes = hitGroupSize;
	shaderRecordHG.StrideInBytes = recordSize;
	startAddress += hitgroupRegion;

	// レイトレースの大きさ情報をセット。
	dispatchRayDesc.Width = window_width;
	dispatchRayDesc.Height = window_height;
	dispatchRayDesc.Depth = 1;


	/*----------ゲームループ----------*/
	while (true) {
		/*----------毎フレーム処理(描画前処理)----------*/
		directXBase.processBeforeDrawing();



		/*----- 更新処理 -----*/

		// ビュー行列を生成。
		Camera::GenerateMatView();

		FPS();

		//Camera::target = triangle.GetPos();


		/*----- 描画処理 -----*/

		// 画面に表示されるレンダーターゲットに戻す。
		DirectXBase::Instance()->SetRenderTarget();

		// 三角形を描画
		//triangle.Draw();

		// ディスクリプタヒープ設定コマンド
		ID3D12DescriptorHeap* ppHeaps2[] = { tlasDescriptor.Get() };
		DirectXBase::Instance()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);

		// ルートシグネチャとリソースをセット.
		DirectXBase::Instance()->cmdList->SetComputeRootSignature(
			grobalRootsignature.Get());
		DirectXBase::Instance()->cmdList->SetComputeRootDescriptorTable(
			0, tlasDescriptor->GetGPUDescriptorHandleForHeapStart()); // TLAS のセット.

		// ディスクリプタヒープ設定コマンド
		ppHeaps2[0] = { uavDescriptor.Get() };
		DirectXBase::Instance()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);

		DirectXBase::Instance()->cmdList->SetComputeRootDescriptorTable(
			1, uavDescriptor->GetGPUDescriptorHandleForHeapStart()); // UAV バッファのセット.
		// レイトレーシング結果バッファを UAV 状態へ.
		auto barrierToUAV = CD3DX12_RESOURCE_BARRIER::Transition(
			rayTracingOutput.Get(),
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);
		DirectXBase::Instance()->cmdList->ResourceBarrier(1, &barrierToUAV);
		// レイトレーシングを開始.
		DirectXBase::Instance()->cmdList->SetPipelineState1(stateObject.Get());
		DirectXBase::Instance()->cmdList->DispatchRays(&dispatchRayDesc);

		// バリアを設定し各リソースの状態を遷移させる.
		D3D12_RESOURCE_BARRIER barriers[] = {
		CD3DX12_RESOURCE_BARRIER::Transition(
		rayTracingOutput.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE),
		CD3DX12_RESOURCE_BARRIER::Transition(
		DirectXBase::Instance()->backBuffers[DirectXBase::Instance()->swapchain->GetCurrentBackBufferIndex()].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_COPY_DEST),
		};
		DirectXBase::Instance()->cmdList->ResourceBarrier(_countof(barriers), barriers);
		DirectXBase::Instance()->cmdList->CopyResource(DirectXBase::Instance()->backBuffers[DirectXBase::Instance()->swapchain->GetCurrentBackBufferIndex()].Get(), rayTracingOutput.Get());

		// レンダーターゲットのリソースバリアをもとに戻す。
		D3D12_RESOURCE_BARRIER endBarriers[] = {

		CD3DX12_RESOURCE_BARRIER::Transition(
		DirectXBase::Instance()->backBuffers[DirectXBase::Instance()->swapchain->GetCurrentBackBufferIndex()].Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_RENDER_TARGET)

		};
		DirectXBase::Instance()->cmdList->ResourceBarrier(_countof(endBarriers), endBarriers);

		directXBase.processAfterDrawing();

	}

	return 0;
}


// タイトルバーのFPSの更新
void FPS()
{
	static DWORD prev_time = timeGetTime();	// 前回の時間
	static int frame_count = 0;		// フレームカウント
	DWORD now_time = timeGetTime();		// 今回のフレームの時間

	frame_count++;	// フレーム数をカウントする

	// 経過時間が１秒を超えたらカウントと時間をリセット
	if (now_time - prev_time >= 1000)
	{
		wchar_t fps[1000];
		_itow_s(frame_count, fps, 10);
		wchar_t moji[] = L"FPS";
		wcscat_s(fps, moji);
		SetWindowText(DirectXBase::Instance()->windowsAPI.hwnd, fps);
		//OutputDebugString(fps);

		prev_time = now_time;
		frame_count = 0;
	}
}

/*

実装メモ

・三角形が描画されるであろう場所までは実装できた。

問題点が２つ
1,TransRay関数が定義されていないと判定される。
　→DXR1_0バージョンが対応しているので無いとおかしい。
　→RAY_FLAGみたいなやつも定義されていない判定になるが、加速構造体は定義されている判定になる。ここの違いになにか原因がありそう。
　→加速構造体は他のやつでも定義されているので、そもそもレイトレ関係の関数が使えない？
　　→加速構造体を他のプロジェクトに持っていったら定義されていないと出たので、レイトレ関係の関数自体は定義されているっぽい。
　→シェーダーモデル(6.3)や種類(lib)も合っているはず。他にも特殊な設定が必要なのか。

2,現状のコードではレイトレ用のシェーダーがコンパイルできない。
　→スラリンラボに書いてある新しいやつを使って別で実装しなければならない。
　→Blobを返すところまで持っていければ後は現状のコードを少し書き換えるだけで済む。

*/