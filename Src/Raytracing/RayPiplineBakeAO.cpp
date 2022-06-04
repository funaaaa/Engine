#include "RayPiplineBakeAO.h"
#include "RaytracingOutput.h"
#include "HitGroupMgr.h"
#include "BLASRegister.h"
#include "WindowsAPI.h"

void RayPiplineBakeAO::ConstructionShaderTableBakeAO(std::vector<std::shared_ptr<RaytracingOutput>> BakeTargets)
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

		const int BAKE_TARGET_SIZE = BakeTargets.size();
		for (int index = 0; index < BAKE_TARGET_SIZE; ++index) {

			pRecord = BLASRegister::Ins()->WriteShaderRecordSpecifyUAV(pRecord, index, hitgroupRecordSize, stateObject, hitGroupName, BakeTargets[index]->GetUAVIndex());

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
