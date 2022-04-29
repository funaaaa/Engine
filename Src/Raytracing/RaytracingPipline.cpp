#include "RaytracingPipline.h"
#include "DirectXTex/d3dx12.h"
#include "ShaderStorage.h"
#include "HitGroupMgr.h"
#include "RayRootsignature.h"
#include "DirectXBase.h"
#include "BLASRegister.h"
#include <DirectXMath.h>

void RaytracingPipline::Setting(const std::vector<RayPiplineShaderData>& InputData)
{

	/*===== �Z�b�e�B���O���� =====*/

	// ���͂��ꂽ�f�[�^��ۑ�����B
	const int INPUT_COUNT = InputData.size();
	for (int index = 0; index < INPUT_COUNT; ++index) {

		// ���͂��ꂽ�f�[�^�\���́B
		RayPiplineShaderData buff;

		// �ۑ�����B
		buff.shaderPath = InputData[index].shaderPath;

		// �ۑ�����Ă���G���g���|�C���g��ۑ��B
		const int RG_ENTRY_COUNT = InputData[index].rayGenEnteryPoint.size();
		for (int entryPointIndex = 0; entryPointIndex < RG_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			buff.rayGenEnteryPoint.push_back(InputData[index].rayGenEnteryPoint[entryPointIndex]);

		}
		const int MS_ENTRY_COUNT = InputData[index].missEntryPoint.size();
		for (int entryPointIndex = 0; entryPointIndex < MS_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			buff.missEntryPoint.push_back(InputData[index].missEntryPoint[entryPointIndex]);

		}
		const int HG_ENTRY_COUNT = InputData[index].hitgroupEntryPoint.size();
		for (int entryPointIndex = 0; entryPointIndex < HG_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			buff.hitgroupEntryPoint.push_back(InputData[index].hitgroupEntryPoint[entryPointIndex]);

		}

		// �ۑ�����B
		shaderData.push_back(buff);

	}

	// �O���[�o�����[�g�V�O�l�`����ݒ�B
	globalRootSig = std::make_shared<RayRootsignature>();
	// �p�����[�^�[t0��TLAS(SRV)��ݒ�B
	globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0);
	// �p�����[�^�[b0�ɃJ�����p�o�b�t�@��ݒ�B
	globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 0);
	// �p�����[�^�[u0�ɏo�͗p�o�b�t�@��ݒ�B
	globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0);

	// ���[�g�V�O�l�`���𐶐��B
	globalRootSig->Create(false, L"GlobalRootSig");

	// �X�e�[�g�I�u�W�F�N�g�̐ݒ��ۑ����Ă����悤�̍\���́B
	CD3DX12_STATE_OBJECT_DESC subobjects;
	// �X�e�[�g�I�u�W�F�N�g�̏�Ԃ�ݒ�B
	subobjects.SetStateObjectType(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

	// �V�F�[�_�[��ǂݍ��ށB
	const int SHADER_COUNT = shaderData.size();
	for (int index = 0; index < SHADER_COUNT; ++index) {

		shaderCode.emplace_back();

		// �V�F�[�_�[���R���p�C������B
		ShaderStorage::Instance()->LoadShaderForDXC(shaderData[index].shaderPath, "lib_6_4", "");

		// �V�F�[�_�[��ǂݍ��ށB
		shaderCode[index].BytecodeLength = ShaderStorage::Instance()->GetShaderBin(shaderData[index].shaderPath).size();
		shaderCode[index].pShaderBytecode = ShaderStorage::Instance()->GetShaderBin(shaderData[index].shaderPath).data();

		// �V�F�[�_�[�̊e�֐����R�[�h�̓o�^�B
		auto dxilLib = subobjects.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
		dxilLib->SetDXILLibrary(&shaderCode[index]);

		// �V�F�[�_�[�̃G���g���|�C���g��o�^�B
		const int RG_ENTRY_COUNT = InputData[index].rayGenEnteryPoint.size();
		for (int entryPointIndex = 0; entryPointIndex < RG_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			dxilLib->DefineExport(shaderData[index].rayGenEnteryPoint[entryPointIndex]);

		}
		const int MS_ENTRY_COUNT = InputData[index].missEntryPoint.size();
		for (int entryPointIndex = 0; entryPointIndex < MS_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			dxilLib->DefineExport(shaderData[index].missEntryPoint[entryPointIndex]);

		}
		const int HG_ENTRY_COUNT = InputData[index].hitgroupEntryPoint.size();
		for (int entryPointIndex = 0; entryPointIndex < HG_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			dxilLib->DefineExport(shaderData[index].hitgroupEntryPoint[entryPointIndex]);

		}

	}

	// �q�b�g�O���[�v��ݒ�B
	const int HIT_GROUP_COUNT = HitGroupMgr::Instance()->GetHitGroupCount();
	for (int index = 0; index < HIT_GROUP_COUNT; ++index) {

		// �q�b�g�O���[�v�̐ݒ�B
		auto hitGroup = subobjects.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
		hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

		// ClosestHitShader���G���g���|�C���g��ۑ��B
		if (HitGroupMgr::Instance()->GetCHFlag(index)) {
			hitGroup->SetClosestHitShaderImport(HitGroupMgr::Instance()->GetCH(index));
		}
		// AnyHitShader�̃G���g���|�C���g��ۑ��B
		if (HitGroupMgr::Instance()->GetAHFlag(index)) {
			hitGroup->SetAnyHitShaderImport(HitGroupMgr::Instance()->GetAH(index));
		}
		// IntersectShader�̃G���g���|�C���g��ۑ��B
		if (HitGroupMgr::Instance()->GetISFlag(index)) {
			hitGroup->SetIntersectionShaderImport(HitGroupMgr::Instance()->GetIS(index));
		}
		// �q�b�g�O���[�v����ۑ��B
		hitGroup->SetHitGroupExport(HitGroupMgr::Instance()->hitGroupNames[index]);

	}

	// �O���[�o�����[�g�V�O�l�`���̐ݒ�B
	auto rootSig = subobjects.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	rootSig->SetRootSignature(globalRootSig->GetRootSig().Get());

	// HitGroup�̃��[�J�����[�g�V�O�l�`����ݒ�B
	for (int index = 0; index < HIT_GROUP_COUNT; ++index) {

		auto chLocalRootSig = subobjects.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
		chLocalRootSig->SetRootSignature(HitGroupMgr::Instance()->GetLocalRootSig(index)->GetRootSig().Get());
		auto chAssocModel = subobjects.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
		chAssocModel->AddExport(HitGroupMgr::Instance()->hitGroupNames[index]);
		chAssocModel->SetSubobjectToAssociate(*chLocalRootSig);

	}

	// �V�F�[�_�[�̐ݒ�B
	auto shaderConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	shaderConfig->Config(sizeof(DirectX::XMFLOAT3) + sizeof(UINT), sizeof(DirectX::XMFLOAT2));

	// �p�C�v���C���̐ݒ�B
	auto pipelineConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	pipelineConfig->Config(30);

	// ��������B
	HRESULT resultBuff = DirectXBase::Instance()->dev->CreateStateObject(
		subobjects, IID_PPV_ARGS(stateObject.ReleaseAndGetAddressOf())
	);

	stateObject->SetName(L"StateObject");

}

void RaytracingPipline::ConstructionShaderTable()
{

	/*===== �V�F�[�_�[�e�[�u�����\�z =====*/

	// �V�F�[�_�[�e�[�u���̃T�C�Y���v�Z�B
	const auto ShaderRecordAlignment = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;

	// RayGeneration�V�F�[�_�[�ł̓��[�J�����[�g�V�O�l�`�����g�p�B
	UINT raygenRecordSize = 0;
	raygenRecordSize += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	raygenRecordSize = RoundUp(raygenRecordSize, ShaderRecordAlignment);

	// Miss�V�F�[�_�[�ł̓��[�J�����[�g�V�O�l�`�����g�p�B
	UINT missRecordSize = 0;
	missRecordSize += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	missRecordSize = RoundUp(missRecordSize, ShaderRecordAlignment);

	// �q�b�g�O���[�v�ł́A�ۑ�����Ă���q�b�g�O���[�v�̒�����ő�̃T�C�Y�̂��̂Ńf�[�^���m�ۂ���B�B
	UINT hitgroupRecordSize = 0;
	hitgroupRecordSize += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	hitgroupRecordSize += GetLargestDataSizeInHitGroup();
	hitgroupRecordSize = RoundUp(hitgroupRecordSize, ShaderRecordAlignment);

	// �g�p����e�V�F�[�_�[�̌����A�V�F�[�_�[�e�[�u���̃T�C�Y�����߂�B
	UINT hitgroupCount = HitGroupMgr::Instance()->GetHitGroupCount();
	UINT raygenSize = GetRayGenerationCount() * raygenRecordSize;
	UINT missSize = GetMissCount() * missRecordSize;
	UINT hitGroupSize = hitgroupCount * hitgroupRecordSize;

	// �e�e�[�u���̊J�n�ʒu�ɃA���C�����g���񂪂���̂Œ�������B
	UINT tableAlign = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;
	UINT raygenRegion = RoundUp(raygenRecordSize, tableAlign);
	UINT missRegion = RoundUp(missSize, tableAlign);
	UINT hitgroupRegion = RoundUp(hitGroupSize, tableAlign);

	// �V�F�[�_�[�e�[�u���̃T�C�Y.
	UINT tableSize = raygenRegion + missRegion + hitgroupRegion;

	/*========== �V�F�[�_�[�e�[�u���̍\�z ==========*/

	// �V�F�[�_�[�e�[�u���m�ہB
	shaderTable = CreateBuffer(
		tableSize, D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_HEAP_TYPE_UPLOAD,
		L"ShaderTable");

	stateObject.As(&rtsoProps);

	// �e�V�F�[�_�[���R�[�h����������ł����B
	void* mapped = nullptr;
	shaderTable->Map(0, nullptr, &mapped);
	uint8_t* pStart = static_cast<uint8_t*>(mapped);

	// RayGeneration �p�̃V�F�[�_�[���R�[�h���������݁B
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

	// Miss Shader �p�̃V�F�[�_�[���R�[�h���������݁B
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

	// Hit Group �p�̃V�F�[�_�[���R�[�h���������݁B
	uint8_t* hitgroupStart = pStart + raygenRegion + missRegion;
	{

		uint8_t* pRecord = hitgroupStart;

		// ���̏����͉��̎����B����BLAS�̃f�[�^���������ۂ�BLAS���Ƃɏ������ޏ�����ς���B���l���Ă���̂́AHITGROUP_ID���ƂɊ֐���p�ӂ�������B
		const int BLAS_COUNT = BLASRegister::Instance()->GetBLASCount();
		for (int index = 0; index < BLAS_COUNT; ++index) {

			pRecord = BLASRegister::Instance()->WriteShaderRecord(pRecord, index, hitgroupRecordSize, stateObject);

		}

	}
	shaderTable->Unmap(0, nullptr);

	// ���C���ˎ��̐ݒ��ݒ�B

	// DispatchRays �̂��߂ɏ����Z�b�g���Ă���.
	auto startAddress = shaderTable->GetGPUVirtualAddress();
	// RayGeneration�V�F�[�_�[�̏��
	auto& shaderRecordRG = dispatchRayDesc.RayGenerationShaderRecord;
	shaderRecordRG.StartAddress = startAddress;
	shaderRecordRG.SizeInBytes = raygenSize;
	startAddress += raygenRegion;
	// Miss�V�F�[�_�[�̏��
	auto& shaderRecordMS = dispatchRayDesc.MissShaderTable;
	shaderRecordMS.StartAddress = startAddress;
	shaderRecordMS.SizeInBytes = missSize;
	shaderRecordMS.StrideInBytes = missRecordSize;
	startAddress += missRegion;
	// HitGroup�̏��
	auto& shaderRecordHG = dispatchRayDesc.HitGroupTable;
	shaderRecordHG.StartAddress = startAddress;
	shaderRecordHG.SizeInBytes = hitGroupSize;
	shaderRecordHG.StrideInBytes = hitgroupRecordSize;
	startAddress += hitgroupRegion;
	// ���C�̏��
	dispatchRayDesc.Width = window_width;
	dispatchRayDesc.Height = window_height;
	dispatchRayDesc.Depth = 1;

}

UINT RaytracingPipline::GetLargestDataSizeInHitGroup()
{

	/*===== �S�Ẵq�b�g�O���[�v�̒��ōł��f�[�^�T�C�Y���傫�����̂��擾���� =====*/

	UINT largestDataSize = 0;

	const int HIT_GROUP_COUNT = HitGroupMgr::Instance()->GetHitGroupCount();
	for (int index = 0; index < HIT_GROUP_COUNT; ++index) {

		// �f�[�^�T�C�Y���擾�B
		UINT dataSize = 0;
		const UINT CONST_DATA_SIZE = 4;
		dataSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * HitGroupMgr::Instance()->GetSRVCount(index);
		dataSize += CONST_DATA_SIZE * HitGroupMgr::Instance()->GetCBVCount(index);
		dataSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * HitGroupMgr::Instance()->GetUAVCount(index);

		// �擾�����f�[�^�T�C�Y���ۑ�����Ă���f�[�^�T�C�Y��菬���������珈�����΂��B
		if (dataSize < largestDataSize) continue;

		// �擾�����f�[�^�T�C�Y��ۑ�����B
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
