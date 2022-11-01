#include "RaytracingPipeline.h"
#include "DirectXTex/d3dx12.h"
#include "ShaderStorage.h"
#include "HitGroupMgr.h"
#include "RayRootsignature.h"
#include "Engine.h"
#include "BLASRegister.h"
#include <DirectXMath.h>

void RaytracingPipeline::Setting(const std::vector<RayPipelineShaderData>& InputData, int UseHitGroup, int SRVCount, int CBVCount, int UAVCount, int PayloadSize, int AttribSize, int ReflectionCount)
{

	/*===== セッティング処理 =====*/

	// ヒットグループ名を保存。
	hitGroupName_ = HitGroupMgr::Ins()->hitGroupNames[UseHitGroup];

	// 入力されたデータを保存する。
	const int INPUT_COUNT = static_cast<int>(InputData.size());
	for (int index = 0; index < INPUT_COUNT; ++index) {

		// 入力されたデータ構造体。
		RayPipelineShaderData buff;

		// 保存する。
		buff.shaderPath_ = InputData[index].shaderPath_;

		// 保存されているエントリポイントを保存。
		const int RG_ENTRY_COUNT = static_cast<int>(InputData[index].rayGenEnteryPoint_.size());
		for (int entryPointIndex = 0; entryPointIndex < RG_ENTRY_COUNT; ++entryPointIndex) {

			// 保存する。
			buff.rayGenEnteryPoint_.push_back(InputData[index].rayGenEnteryPoint_[entryPointIndex]);

		}
		const int MS_ENTRY_COUNT = static_cast<int>(InputData[index].missEntryPoint_.size());
		for (int entryPointIndex = 0; entryPointIndex < MS_ENTRY_COUNT; ++entryPointIndex) {

			// 保存する。
			buff.missEntryPoint_.push_back(InputData[index].missEntryPoint_[entryPointIndex]);

		}

		// 保存する。
		const int HS_ENTRY_COUNT = static_cast<int>(InputData[index].hitgroupEntryPoint_.size());
		for (int entryPointIndex = 0; entryPointIndex < HS_ENTRY_COUNT; ++entryPointIndex) {
			buff.hitgroupEntryPoint_.push_back(InputData[index].hitgroupEntryPoint_[entryPointIndex]);
		}

		// 保存する。
		shaderData_.push_back(buff);

	}

	// グローバルルートシグネチャを設定。
	globalRootSig_ = std::make_shared<RayRootsignature>();
	// パラメーターt0にTLAS(SRV)を設定。
	for (int index = 0; index < SRVCount; ++index)globalRootSig_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, index);
	// パラメーターb0にカメラ用バッファを設定。
	for (int index = 0; index < CBVCount; ++index) {
		globalRootSig_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, index);
	}
	// パラメーターu0に出力用バッファを設定。
	for (int index = 0; index < UAVCount; ++index)globalRootSig_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, index);

	// ルートシグネチャを生成。
	globalRootSig_->Create(false, L"GlobalRootSig");

	// ステートオブジェクトの設定を保存しておくようの構造体。
	CD3DX12_STATE_OBJECT_DESC subobjects;
	// ステートオブジェクトの状態を設定。
	subobjects.SetStateObjectType(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

	// シェーダーを読み込む。
	const int SHADER_COUNT = static_cast<int>(shaderData_.size());
	for (int index = 0; index < SHADER_COUNT; ++index) {

		shaderCode_.emplace_back();

		// シェーダーをコンパイルする。
		ShaderStorage::Ins()->LoadShaderForDXC(shaderData_[index].shaderPath_, "lib_6_4", "");

		// シェーダーを読み込む。
		shaderCode_[index].BytecodeLength = static_cast<SIZE_T>(ShaderStorage::Ins()->GetShaderBin(shaderData_[index].shaderPath_).size());
		shaderCode_[index].pShaderBytecode = ShaderStorage::Ins()->GetShaderBin(shaderData_[index].shaderPath_).data();

		// シェーダーの各関数レコードの登録。
		auto dxilLib = subobjects.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
		dxilLib->SetDXILLibrary(&shaderCode_[index]);

		// シェーダーのエントリポイントを登録。
		const int RG_ENTRY_COUNT = static_cast<int>(InputData[index].rayGenEnteryPoint_.size());
		for (int entryPointIndex = 0; entryPointIndex < RG_ENTRY_COUNT; ++entryPointIndex) {

			// 保存する。
			dxilLib->DefineExport(shaderData_[index].rayGenEnteryPoint_[entryPointIndex]);

		}
		const int MS_ENTRY_COUNT = static_cast<int>(InputData[index].missEntryPoint_.size());
		for (int entryPointIndex = 0; entryPointIndex < MS_ENTRY_COUNT; ++entryPointIndex) {

			// 保存する。
			dxilLib->DefineExport(shaderData_[index].missEntryPoint_[entryPointIndex]);

		}
		const int HG_ENTRY_COUNT = static_cast<int>(InputData[index].hitgroupEntryPoint_.size());
		for (int entryPointIndex = 0; entryPointIndex < HG_ENTRY_COUNT; ++entryPointIndex) {

			// 保存する。
			dxilLib->DefineExport(shaderData_[index].hitgroupEntryPoint_[entryPointIndex]);

		}

	}

	// ヒットグループの設定。
	auto hitGroup_ = subobjects.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
	hitGroup_->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

	// ClosestHitShaderをエントリポイントを保存。
	if (HitGroupMgr::Ins()->GetCHFlag(UseHitGroup)) {
		hitGroup_->SetClosestHitShaderImport(HitGroupMgr::Ins()->GetCH(UseHitGroup));
	}
	// AnyHitShaderのエントリポイントを保存。
	if (HitGroupMgr::Ins()->GetAHFlag(UseHitGroup)) {
		hitGroup_->SetAnyHitShaderImport(HitGroupMgr::Ins()->GetAH(UseHitGroup));
	}
	// IntersectShaderのエントリポイントを保存。
	if (HitGroupMgr::Ins()->GetISFlag(UseHitGroup)) {
		hitGroup_->SetIntersectionShaderImport(HitGroupMgr::Ins()->GetIS(UseHitGroup));
	}
	// ヒットグループ名を保存。
	hitGroup_->SetHitGroupExport(HitGroupMgr::Ins()->hitGroupNames[UseHitGroup]);

	// グローバルルートシグネチャの設定。
	auto rootSig = subobjects.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	rootSig->SetRootSignature(globalRootSig_->GetRootSig().Get());

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
	pipelineConfig->Config(ReflectionCount);

	// 生成する。
	Engine::Ins()->device_.dev_->CreateStateObject(
		subobjects, IID_PPV_ARGS(stateObject_.ReleaseAndGetAddressOf())
	);

	stateObject_->SetName(L"StateObject");

}

void RaytracingPipeline::ConstructionShaderTable(int DispatchX, int DispatchY)
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
	hitgroupRecordSize_ = 0;
	hitgroupRecordSize_ += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	hitgroupRecordSize_ += GetLargestDataSizeInHitGroup();
	hitgroupRecordSize_ = RoundUp(hitgroupRecordSize_, ShaderRecordAlignment);

	// 使用する各シェーダーの個数より、シェーダーテーブルのサイズを求める。
	UINT hitgroupCount = HitGroupMgr::Ins()->GetHitGroupCount();
	UINT raygenSize = GetRayGenerationCount() * raygenRecordSize;
	UINT missSize = GetMissCount() * missRecordSize;
	UINT hitGroupSize = hitgroupCount * hitgroupRecordSize_;

	// 各テーブルの開始位置にアライメント制約があるので調整する。
	UINT tableAlign = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;
	raygenRegion_ = RoundUp(raygenRecordSize, tableAlign);
	missRegion_ = RoundUp(missSize, tableAlign);
	UINT hitgroupRegion = RoundUp(hitGroupSize, tableAlign);

	// 生成されたBLASの数。
	const int BLAS_COUNT = BLASRegister::Ins()->GetBLASCount() - 1;

	// シェーダーテーブルのサイズ。
	UINT tableSize = raygenRegion_ + missRegion_ + hitgroupRegion * BLAS_COUNT;

	/*========== シェーダーテーブルの構築 ==========*/

	// シェーダーテーブル確保。
	shaderTable_ = CreateBuffer(
		tableSize, D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_HEAP_TYPE_UPLOAD,
		L"ShaderTable");

	stateObject_.As(&rtsoProps_);

	// 各シェーダーレコードを書き込んでいく。
	void* mapped = nullptr;
	shaderTable_->Map(0, nullptr, &mapped);
	uint8_t* pStart = static_cast<uint8_t*>(mapped);

	// RayGeneration 用のシェーダーレコードを書き込み。
	uint8_t* rgsStart = pStart;
	{
		uint8_t* p = rgsStart;

		const int SHADER_DATA_COUNT = static_cast<int>(shaderData_.size());
		for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

			const int RG_COUNT = static_cast<int>(shaderData_[index].rayGenEnteryPoint_.size());
			for (int rgIndex = 0; rgIndex < RG_COUNT; ++rgIndex) {

				void* id_ = rtsoProps_->GetShaderIdentifier(shaderData_[index].rayGenEnteryPoint_[rgIndex]);
				p += WriteShaderIdentifier(p, id_);

			}

		}

	}

	// Miss Shader 用のシェーダーレコードを書き込み。
	uint8_t* missStart = pStart + raygenRegion_;
	{
		uint8_t* p = missStart;

		const int SHADER_DATA_COUNT = static_cast<int>(shaderData_.size());
		for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

			const int MS_COUNT = static_cast<int>(shaderData_[index].missEntryPoint_.size());
			for (int msIndex = 0; msIndex < MS_COUNT; ++msIndex) {

				void* id_ = rtsoProps_->GetShaderIdentifier(shaderData_[index].missEntryPoint_[msIndex]);
				p += WriteShaderIdentifier(p, id_);

			}

		}

	}

	// Hit Group 用のシェーダーレコードを書き込み。
	uint8_t* hitgroupStart = pStart + raygenRegion_ + missRegion_;
	{

		uint8_t* pRecord = hitgroupStart;

		// この処理は仮の実装。送るBLASのデータが増えた際はBLASごとに書き込む処理を変える。今考えているのは、HITGROUP_IDごとに関数を用意する実装。
		pRecord = BLASRegister::Ins()->WriteShaderRecord(pRecord, hitgroupRecordSize_, stateObject_, hitGroupName_);

	}
	shaderTable_->Unmap(0, nullptr);

	// レイ発射時の設定を設定。

	// DispatchRays のために情報をセットしておく.
	auto startAddress = shaderTable_->GetGPUVirtualAddress();
	// RayGenerationシェーダーの情報
	auto& shaderRecordRG = dispatchRayDesc_.RayGenerationShaderRecord;
	shaderRecordRG.StartAddress = startAddress;
	shaderRecordRG.SizeInBytes = raygenSize;
	startAddress += raygenRegion_;
	// Missシェーダーの情報
	auto& shaderRecordMS = dispatchRayDesc_.MissShaderTable;
	shaderRecordMS.StartAddress = startAddress;
	shaderRecordMS.SizeInBytes = missSize;
	shaderRecordMS.StrideInBytes = missRecordSize;
	startAddress += missRegion_;
	// HitGroupの情報
	auto& shaderRecordHG = dispatchRayDesc_.HitGroupTable;
	shaderRecordHG.StartAddress = startAddress;
	shaderRecordHG.SizeInBytes = hitGroupSize;
	shaderRecordHG.StrideInBytes = hitgroupRecordSize_;
	startAddress += hitgroupRegion;
	// レイの情報
	dispatchRayDesc_.Width = DispatchX;
	dispatchRayDesc_.Height = DispatchY;
	dispatchRayDesc_.Depth = 1;

}

void RaytracingPipeline::MapHitGroupInfo()
{

	/*===== HitGroupの情報を転送 =====*/

	// 生成されたBLASの数。
	const int BLAS_COUNT = BLASRegister::Ins()->GetBLASCount();

	void* mapped = nullptr;
	HRESULT result = shaderTable_->Map(0, nullptr, &mapped);
	if (result != S_OK) {

		int a = 0;
		++a;

	}
	uint8_t* pStart = static_cast<uint8_t*>(mapped);

	// Hit Group 用のシェーダーレコードを書き込み。
	uint8_t* hitgroupStart = pStart + raygenRegion_ + missRegion_;
	{

		uint8_t* pRecord = hitgroupStart;

		// この処理は仮の実装。送るBLASのデータが増えた際はBLASごとに書き込む処理を変える。今考えているのは、HITGROUP_IDごとに関数を用意する実装。
		pRecord = BLASRegister::Ins()->WriteShaderRecord(pRecord, hitgroupRecordSize_, stateObject_, hitGroupName_);

	}
	shaderTable_->Unmap(0, nullptr);

}

UINT RaytracingPipeline::GetLargestDataSizeInHitGroup()
{

	/*===== 全てのヒットグループの中で最もデータサイズが大きいものを取得する =====*/

	UINT largestDataSize = 0;

	const int HIT_GROUP_COUNT = HitGroupMgr::Ins()->GetHitGroupCount();
	for (int index = 0; index < HIT_GROUP_COUNT; ++index) {

		// データサイズを取得。
		UINT dataSize = 0;
		dataSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * HitGroupMgr::Ins()->GetSRVCount(index);
		dataSize += sizeof(D3D12_GPU_VIRTUAL_ADDRESS) * HitGroupMgr::Ins()->GetCBVCount(index);
		dataSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * HitGroupMgr::Ins()->GetUAVCount(index);

		// 取得したデータサイズが保存されているデータサイズより小さかったら処理を飛ばす。
		if (dataSize < largestDataSize) continue;

		// 取得したデータサイズを保存する。
		largestDataSize = dataSize;

	}

	return largestDataSize;

}

Microsoft::WRL::ComPtr<ID3D12Resource> RaytracingPipeline::CreateBuffer(size_t size_, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType, const wchar_t* name_)
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
	resDesc.Width = size_;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc = { 1, 0 };
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = flags;

	hr = Engine::Ins()->device_.dev_->CreateCommittedResource(
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
	if (resource != nullptr && name_ != nullptr) {
		resource->SetName(name_);
	}
	return resource;
}

UINT RaytracingPipeline::WriteShaderIdentifier(void* dst, const void* shaderId)
{
	memcpy(dst, shaderId, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	return D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
}

int RaytracingPipeline::GetRayGenerationCount()
{
	int count = 0;

	const int SHADER_DATA_COUNT = static_cast<int>(shaderData_.size());
	for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

		count += static_cast<int>(shaderData_[index].rayGenEnteryPoint_.size());

	}

	return count;

}

int RaytracingPipeline::GetMissCount()
{
	int count = 0;

	const int SHADER_DATA_COUNT = static_cast<int>(shaderData_.size());
	for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

		count += static_cast<int>(shaderData_[index].missEntryPoint_.size());

	}

	return count;

}
