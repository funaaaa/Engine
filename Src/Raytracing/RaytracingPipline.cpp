#include "RaytracingPipline.h"
#include "DirectXTex/d3dx12.h"
#include "ShaderStorage.h"
#include "HitGroupMgr.h"
#include "RayRootsignature.h"
#include "DirectXBase.h"
#include "BLASRegister.h"
#include <DirectXMath.h>

void RaytracingPipline::Setting(const std::vector<RayPiplineShaderData>& InputData, const int& UseHitGroup, const int& SRVCount, const int& CBVCount, const int& UAVCount, const int& PayloadSize, const int& AttribSize)
{

	/*===== セッティング処理 =====*/

	// ヒットグループ名を保存。
	hitGroupName = HitGroupMgr::Ins()->hitGroupNames[UseHitGroup];

	// 入力されたデータを保存する。
	const int INPUT_COUNT = InputData.size();
	for (int index = 0; index < INPUT_COUNT; ++index) {

		// 入力されたデータ構造体。
		RayPiplineShaderData buff;

		// 保存する。
		buff.shaderPath = InputData[index].shaderPath;

		// 保存されているエントリポイントを保存。
		const int RG_ENTRY_COUNT = InputData[index].rayGenEnteryPoint.size();
		for (int entryPointIndex = 0; entryPointIndex < RG_ENTRY_COUNT; ++entryPointIndex) {

			// 保存する。
			buff.rayGenEnteryPoint.push_back(InputData[index].rayGenEnteryPoint[entryPointIndex]);

		}
		const int MS_ENTRY_COUNT = InputData[index].missEntryPoint.size();
		for (int entryPointIndex = 0; entryPointIndex < MS_ENTRY_COUNT; ++entryPointIndex) {

			// 保存する。
			buff.missEntryPoint.push_back(InputData[index].missEntryPoint[entryPointIndex]);

		}

		// 保存する。
		const int HS_ENTRY_COUNT = InputData[index].hitgroupEntryPoint.size();
		for (int entryPointIndex = 0; entryPointIndex < HS_ENTRY_COUNT; ++entryPointIndex) {
			buff.hitgroupEntryPoint.push_back(InputData[index].hitgroupEntryPoint[entryPointIndex]);
		}

		// 保存する。
		shaderData.push_back(buff);

	}

	// グローバルルートシグネチャを設定。
	globalRootSig = std::make_shared<RayRootsignature>();
	// パラメーターt0にTLAS(SRV)を設定。
	for (int index = 0; index < SRVCount; ++index)globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, index);
	// パラメーターb0にカメラ用バッファを設定。
	for (int index = 0; index < CBVCount; ++index)globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, index);
	// パラメーターu0に出力用バッファを設定。
	for (int index = 0; index < UAVCount; ++index)globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, index);

	// ルートシグネチャを生成。
	globalRootSig->Create(false, L"GlobalRootSig");

	// ステートオブジェクトの設定を保存しておくようの構造体。
	CD3DX12_STATE_OBJECT_DESC subobjects;
	// ステートオブジェクトの状態を設定。
	subobjects.SetStateObjectType(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

	// シェーダーを読み込む。
	const int SHADER_COUNT = shaderData.size();
	for (int index = 0; index < SHADER_COUNT; ++index) {

		shaderCode.emplace_back();

		// シェーダーをコンパイルする。
		ShaderStorage::Ins()->LoadShaderForDXC(shaderData[index].shaderPath, "lib_6_4", "");

		// シェーダーを読み込む。
		shaderCode[index].BytecodeLength = ShaderStorage::Ins()->GetShaderBin(shaderData[index].shaderPath).size();
		shaderCode[index].pShaderBytecode = ShaderStorage::Ins()->GetShaderBin(shaderData[index].shaderPath).data();

		// シェーダーの各関数レコードの登録。
		auto dxilLib = subobjects.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
		dxilLib->SetDXILLibrary(&shaderCode[index]);

		// シェーダーのエントリポイントを登録。
		const int RG_ENTRY_COUNT = InputData[index].rayGenEnteryPoint.size();
		for (int entryPointIndex = 0; entryPointIndex < RG_ENTRY_COUNT; ++entryPointIndex) {

			// 保存する。
			dxilLib->DefineExport(shaderData[index].rayGenEnteryPoint[entryPointIndex]);

		}
		const int MS_ENTRY_COUNT = InputData[index].missEntryPoint.size();
		for (int entryPointIndex = 0; entryPointIndex < MS_ENTRY_COUNT; ++entryPointIndex) {

			// 保存する。
			dxilLib->DefineExport(shaderData[index].missEntryPoint[entryPointIndex]);

		}
		const int HG_ENTRY_COUNT = InputData[index].hitgroupEntryPoint.size();
		for (int entryPointIndex = 0; entryPointIndex < HG_ENTRY_COUNT; ++entryPointIndex) {

			// 保存する。
			dxilLib->DefineExport(shaderData[index].hitgroupEntryPoint[entryPointIndex]);

		}

	}

	// ヒットグループの設定。
	auto hitGroup = subobjects.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
	hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

	// ClosestHitShaderをエントリポイントを保存。
	if (HitGroupMgr::Ins()->GetCHFlag(UseHitGroup)) {
		hitGroup->SetClosestHitShaderImport(HitGroupMgr::Ins()->GetCH(UseHitGroup));
	}
	// AnyHitShaderのエントリポイントを保存。
	if (HitGroupMgr::Ins()->GetAHFlag(UseHitGroup)) {
		hitGroup->SetAnyHitShaderImport(HitGroupMgr::Ins()->GetAH(UseHitGroup));
	}
	// IntersectShaderのエントリポイントを保存。
	if (HitGroupMgr::Ins()->GetISFlag(UseHitGroup)) {
		hitGroup->SetIntersectionShaderImport(HitGroupMgr::Ins()->GetIS(UseHitGroup));
	}
	// ヒットグループ名を保存。
	hitGroup->SetHitGroupExport(HitGroupMgr::Ins()->hitGroupNames[UseHitGroup]);

	// グローバルルートシグネチャの設定。
	auto rootSig = subobjects.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	rootSig->SetRootSignature(globalRootSig->GetRootSig().Get());

	// HitGroupのローカルルートシグネチャを設定。
	auto chLocalRootSig = subobjects.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
	chLocalRootSig->SetRootSignature(HitGroupMgr::Ins()->GetLocalRootSig(UseHitGroup)->GetRootSig().Get());
	auto chAssocModel = subobjects.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
	chAssocModel->AddExport(HitGroupMgr::Ins()->hitGroupNames[UseHitGroup]);
	chAssocModel->SetSubobjectToAssociate(*chLocalRootSig);

	// シェーダーの設定。
	auto shaderConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	shaderConfig->Config(PayloadSize, AttribSize);

	// パイプラインの設定。
	auto pipelineConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	pipelineConfig->Config(30);

	// 生成する。
	HRESULT resultBuff = DirectXBase::Ins()->dev->CreateStateObject(
		subobjects, IID_PPV_ARGS(stateObject.ReleaseAndGetAddressOf())
	);

	stateObject->SetName(L"StateObject");

}

void RaytracingPipline::ConstructionShaderTable()
{

	/*===== シェーダーテーブルを構築 =====*/

	// シェーダーテーブルのサイズを計算。
	const auto ShaderRecordAlignment = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;

	// RayGenerationシェーダーではローカルルートシグネチャ未使用。
	UINT raygenRecordSize = 0;
	raygenRecordSize += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	raygenRecordSize = RoundUp(raygenRecordSize, ShaderRecordAlignment);

	// Missシェーダーではローカルルートシグネチャ未使用。
	UINT missRecordSize = 0;
	missRecordSize += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	missRecordSize = RoundUp(missRecordSize, ShaderRecordAlignment);

	// ヒットグループでは、保存されているヒットグループの中から最大のサイズのものでデータを確保する。。
	UINT hitgroupRecordSize = 0;
	hitgroupRecordSize += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	hitgroupRecordSize += GetLargestDataSizeInHitGroup();
	hitgroupRecordSize = RoundUp(hitgroupRecordSize, ShaderRecordAlignment);

	// 使用する各シェーダーの個数より、シェーダーテーブルのサイズを求める。
	UINT hitgroupCount = HitGroupMgr::Ins()->GetHitGroupCount();
	UINT raygenSize = GetRayGenerationCount() * raygenRecordSize;
	UINT missSize = GetMissCount() * missRecordSize;
	UINT hitGroupSize = hitgroupCount * hitgroupRecordSize;

	// 各テーブルの開始位置にアライメント制約があるので調整する。
	UINT tableAlign = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;
	UINT raygenRegion = RoundUp(raygenRecordSize, tableAlign);
	UINT missRegion = RoundUp(missSize, tableAlign);
	UINT hitgroupRegion = RoundUp(hitGroupSize, tableAlign);

	// 生成されたBLASの数。
	const int BLAS_COUNT = BLASRegister::Ins()->GetBLASCount();

	// シェーダーテーブルのサイズ。
	UINT tableSize = raygenRegion + missRegion + hitgroupRegion * BLAS_COUNT;

	/*========== シェーダーテーブルの構築 ==========*/

	// シェーダーテーブル確保。
	shaderTable = CreateBuffer(
		tableSize, D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_HEAP_TYPE_UPLOAD,
		L"ShaderTable");

	stateObject.As(&rtsoProps);

	// 各シェーダーレコードを書き込んでいく。
	void* mapped = nullptr;
	shaderTable->Map(0, nullptr, &mapped);
	uint8_t* pStart = static_cast<uint8_t*>(mapped);

	// RayGeneration 用のシェーダーレコードを書き込み。
	uint8_t* rgsStart = pStart;
	{
		uint8_t* p = rgsStart;

		const int SHADER_DATA_COUNT = shaderData.size();
		for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

			const int RG_COUNT = shaderData[index].rayGenEnteryPoint.size();
			for (int rgIndex = 0; rgIndex < RG_COUNT; ++rgIndex) {

				void* id = rtsoProps->GetShaderIdentifier(shaderData[index].rayGenEnteryPoint[rgIndex]);
				p += WriteShaderIdentifier(p, id);

			}

		}

	}

	// Miss Shader 用のシェーダーレコードを書き込み。
	uint8_t* missStart = pStart + raygenRegion;
	{
		uint8_t* p = missStart;

		const int SHADER_DATA_COUNT = shaderData.size();
		for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

			const int MS_COUNT = shaderData[index].missEntryPoint.size();
			for (int msIndex = 0; msIndex < MS_COUNT; ++msIndex) {

				void* id = rtsoProps->GetShaderIdentifier(shaderData[index].missEntryPoint[msIndex]);
				p += WriteShaderIdentifier(p, id);

			}

		}

	}

	// Hit Group 用のシェーダーレコードを書き込み。
	uint8_t* hitgroupStart = pStart + raygenRegion + missRegion;
	{

		uint8_t* pRecord = hitgroupStart;

		// この処理は仮の実装。送るBLASのデータが増えた際はBLASごとに書き込む処理を変える。今考えているのは、HITGROUP_IDごとに関数を用意する実装。
		for (int index = 0; index < BLAS_COUNT; ++index) {

			pRecord = BLASRegister::Ins()->WriteShaderRecord(pRecord, index, hitgroupRecordSize, stateObject, hitGroupName);

		}

	}
	shaderTable->Unmap(0, nullptr);

	// レイ発射時の設定を設定。

	// DispatchRays のために情報をセットしておく.
	auto startAddress = shaderTable->GetGPUVirtualAddress();
	// RayGenerationシェーダーの情報
	auto& shaderRecordRG = dispatchRayDesc.RayGenerationShaderRecord;
	shaderRecordRG.StartAddress = startAddress;
	shaderRecordRG.SizeInBytes = raygenSize;
	startAddress += raygenRegion;
	// Missシェーダーの情報
	auto& shaderRecordMS = dispatchRayDesc.MissShaderTable;
	shaderRecordMS.StartAddress = startAddress;
	shaderRecordMS.SizeInBytes = missSize;
	shaderRecordMS.StrideInBytes = missRecordSize;
	startAddress += missRegion;
	// HitGroupの情報
	auto& shaderRecordHG = dispatchRayDesc.HitGroupTable;
	shaderRecordHG.StartAddress = startAddress;
	shaderRecordHG.SizeInBytes = hitGroupSize;
	shaderRecordHG.StrideInBytes = hitgroupRecordSize;
	startAddress += hitgroupRegion;
	// レイの情報
	dispatchRayDesc.Width = window_width;
	dispatchRayDesc.Height = window_height;
	dispatchRayDesc.Depth = 1;

}

UINT RaytracingPipline::GetLargestDataSizeInHitGroup()
{

	/*===== 全てのヒットグループの中で最もデータサイズが大きいものを取得する =====*/

	UINT largestDataSize = 0;

	const int HIT_GROUP_COUNT = HitGroupMgr::Ins()->GetHitGroupCount();
	for (int index = 0; index < HIT_GROUP_COUNT; ++index) {

		// データサイズを取得。
		UINT dataSize = 0;
		const UINT CONST_DATA_SIZE = 4;
		dataSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * HitGroupMgr::Ins()->GetSRVCount(index);
		dataSize += CONST_DATA_SIZE * HitGroupMgr::Ins()->GetCBVCount(index);
		dataSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * HitGroupMgr::Ins()->GetUAVCount(index);

		// 取得したデータサイズが保存されているデータサイズより小さかったら処理を飛ばす。
		if (dataSize < largestDataSize) continue;

		// 取得したデータサイズを保存する。
		largestDataSize = dataSize;

	}

	return largestDataSize;

}

Microsoft::WRL::ComPtr<ID3D12Resource> RaytracingPipline::CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType, const wchar_t* name)
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

	hr = DirectXBase::Ins()->dev->CreateCommittedResource(
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

UINT RaytracingPipline::WriteShaderIdentifier(void* dst, const void* shaderId)
{
	memcpy(dst, shaderId, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	return D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
}

const int& RaytracingPipline::GetRayGenerationCount()
{
	int count = 0;

	const int SHADER_DATA_COUNT = shaderData.size();
	for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

		count += shaderData[index].rayGenEnteryPoint.size();

	}

	return count;

}

const int& RaytracingPipline::GetMissCount()
{
	int count = 0;

	const int SHADER_DATA_COUNT = shaderData.size();
	for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

		count += shaderData[index].missEntryPoint.size();

	}

	return count;

}
