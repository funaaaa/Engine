#include "RaytracingPipline.h"
#include "DirectXTex/d3dx12.h"
#include "ShaderStorage.h"
#include "HitGroupMgr.h"
#include "RayRootsignature.h"
#include "DirectXBase.h"
#include "BLASRegister.h"
#include <DirectXMath.h>

void RaytracingPipline::Setting(const std::vector<RayPiplineShaderData>& InputData, const int& UseHitGroup, const int& SRVCount, const int& CBVCount, const int& UAVCount, const int& PayloadSize, const int& AttribSize, const int& ReflectionCount)
{

	/*===== �Z�b�e�B���O���� =====*/

	// �q�b�g�O���[�v����ۑ��B
	hitGroupName = HitGroupMgr::Ins()->hitGroupNames[UseHitGroup];

	// ���͂��ꂽ�f�[�^��ۑ�����B
	const int INPUT_COUNT = static_cast<int>(InputData.size());
	for (int index_ = 0; index_ < INPUT_COUNT; ++index_) {

		// ���͂��ꂽ�f�[�^�\���́B
		RayPiplineShaderData buff;

		// �ۑ�����B
		buff.shaderPath_ = InputData[index_].shaderPath_;

		// �ۑ�����Ă���G���g���|�C���g��ۑ��B
		const int RG_ENTRY_COUNT = static_cast<int>(InputData[index_].rayGenEnteryPoint.size());
		for (int entryPointIndex = 0; entryPointIndex < RG_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			buff.rayGenEnteryPoint.push_back(InputData[index_].rayGenEnteryPoint[entryPointIndex]);

		}
		const int MS_ENTRY_COUNT = static_cast<int>(InputData[index_].missEntryPoint.size());
		for (int entryPointIndex = 0; entryPointIndex < MS_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			buff.missEntryPoint.push_back(InputData[index_].missEntryPoint[entryPointIndex]);

		}

		// �ۑ�����B
		const int HS_ENTRY_COUNT = static_cast<int>(InputData[index_].hitgroupEntryPoint.size());
		for (int entryPointIndex = 0; entryPointIndex < HS_ENTRY_COUNT; ++entryPointIndex) {
			buff.hitgroupEntryPoint.push_back(InputData[index_].hitgroupEntryPoint[entryPointIndex]);
		}

		// �ۑ�����B
		shaderData_.push_back(buff);

	}

	// �O���[�o�����[�g�V�O�l�`����ݒ�B
	globalRootSig = std::make_shared<RayRootsignature>();
	// �p�����[�^�[t0��TLAS(SRV)��ݒ�B
	for (int index_ = 0; index_ < SRVCount; ++index_)globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, index_);
	// �p�����[�^�[b0�ɃJ�����p�o�b�t�@��ݒ�B
	for (int index_ = 0; index_ < CBVCount; ++index_) {
		globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, index_);
	}
	// �p�����[�^�[u0�ɏo�͗p�o�b�t�@��ݒ�B
	for (int index_ = 0; index_ < UAVCount; ++index_)globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, index_);

	// ���[�g�V�O�l�`���𐶐��B
	globalRootSig->Create(false, L"GlobalRootSig");

	// �X�e�[�g�I�u�W�F�N�g�̐ݒ��ۑ����Ă����悤�̍\���́B
	CD3DX12_STATE_OBJECT_DESC subobjects;
	// �X�e�[�g�I�u�W�F�N�g�̏�Ԃ�ݒ�B
	subobjects.SetStateObjectType(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

	// �V�F�[�_�[��ǂݍ��ށB
	const int SHADER_COUNT = static_cast<int>(shaderData_.size());
	for (int index_ = 0; index_ < SHADER_COUNT; ++index_) {

		shaderCode.emplace_back();

		// �V�F�[�_�[���R���p�C������B
		ShaderStorage::Ins()->LoadShaderForDXC(shaderData_[index_].shaderPath_, "lib_6_4", "");

		// �V�F�[�_�[��ǂݍ��ށB
		shaderCode[index_].BytecodeLength = static_cast<SIZE_T>(ShaderStorage::Ins()->GetShaderBin(shaderData_[index_].shaderPath_).size());
		shaderCode[index_].pShaderBytecode = ShaderStorage::Ins()->GetShaderBin(shaderData_[index_].shaderPath_).data();

		// �V�F�[�_�[�̊e�֐����R�[�h�̓o�^�B
		auto dxilLib = subobjects.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
		dxilLib->SetDXILLibrary(&shaderCode[index_]);

		// �V�F�[�_�[�̃G���g���|�C���g��o�^�B
		const int RG_ENTRY_COUNT = static_cast<int>(InputData[index_].rayGenEnteryPoint.size());
		for (int entryPointIndex = 0; entryPointIndex < RG_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			dxilLib->DefineExport(shaderData_[index_].rayGenEnteryPoint[entryPointIndex]);

		}
		const int MS_ENTRY_COUNT = static_cast<int>(InputData[index_].missEntryPoint.size());
		for (int entryPointIndex = 0; entryPointIndex < MS_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			dxilLib->DefineExport(shaderData_[index_].missEntryPoint[entryPointIndex]);

		}
		const int HG_ENTRY_COUNT = static_cast<int>(InputData[index_].hitgroupEntryPoint.size());
		for (int entryPointIndex = 0; entryPointIndex < HG_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			dxilLib->DefineExport(shaderData_[index_].hitgroupEntryPoint[entryPointIndex]);

		}

	}

	// �q�b�g�O���[�v�̐ݒ�B
	auto hitGroup = subobjects.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
	hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

	// ClosestHitShader���G���g���|�C���g��ۑ��B
	if (HitGroupMgr::Ins()->GetCHFlag(UseHitGroup)) {
		hitGroup->SetClosestHitShaderImport(HitGroupMgr::Ins()->GetCH(UseHitGroup));
	}
	// AnyHitShader�̃G���g���|�C���g��ۑ��B
	if (HitGroupMgr::Ins()->GetAHFlag(UseHitGroup)) {
		hitGroup->SetAnyHitShaderImport(HitGroupMgr::Ins()->GetAH(UseHitGroup));
	}
	// IntersectShader�̃G���g���|�C���g��ۑ��B
	if (HitGroupMgr::Ins()->GetISFlag(UseHitGroup)) {
		hitGroup->SetIntersectionShaderImport(HitGroupMgr::Ins()->GetIS(UseHitGroup));
	}
	// �q�b�g�O���[�v����ۑ��B
	hitGroup->SetHitGroupExport(HitGroupMgr::Ins()->hitGroupNames[UseHitGroup]);

	// �O���[�o�����[�g�V�O�l�`���̐ݒ�B
	auto rootSig = subobjects.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	rootSig->SetRootSignature(globalRootSig->GetRootSig().Get());

	// HitGroup�̃��[�J�����[�g�V�O�l�`����ݒ�B
	auto chLocalRootSig = subobjects.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
	chLocalRootSig->SetRootSignature(HitGroupMgr::Ins()->GetLocalRootSig(UseHitGroup)->GetRootSig().Get());
	auto chAssocModel = subobjects.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
	chAssocModel->AddExport(HitGroupMgr::Ins()->hitGroupNames[UseHitGroup]);
	chAssocModel->SetSubobjectToAssociate(*chLocalRootSig);

	// �V�F�[�_�[�̐ݒ�B
	auto shaderConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	shaderConfig->Config(PayloadSize, AttribSize);

	// �p�C�v���C���̐ݒ�B
	auto pipelineConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	pipelineConfig->Config(ReflectionCount);

	// ��������B
	DirectXBase::Ins()->dev_->CreateStateObject(
		subobjects, IID_PPV_ARGS(stateObject.ReleaseAndGetAddressOf())
	);

	stateObject->SetName(L"StateObject");

}

void RaytracingPipline::ConstructionShaderTable(const int& DispatchX, const int& DispatchY)
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
	UINT hitgroupCount = HitGroupMgr::Ins()->GetHitGroupCount();
	UINT raygenSize = GetRayGenerationCount() * raygenRecordSize;
	UINT missSize = GetMissCount() * missRecordSize;
	UINT hitGroupSize = hitgroupCount * hitgroupRecordSize;

	// �e�e�[�u���̊J�n�ʒu�ɃA���C�����g���񂪂���̂Œ�������B
	UINT tableAlign = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;
	UINT raygenRegion = RoundUp(raygenRecordSize, tableAlign);
	UINT missRegion = RoundUp(missSize, tableAlign);
	UINT hitgroupRegion = RoundUp(hitGroupSize, tableAlign);

	// �������ꂽBLAS�̐��B
	const int BLAS_COUNT = BLASRegister::Ins()->GetBLASCount();

	// �V�F�[�_�[�e�[�u���̃T�C�Y�B
	UINT tableSize = raygenRegion + missRegion + hitgroupRegion * BLAS_COUNT;

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

		const int SHADER_DATA_COUNT = static_cast<int>(shaderData_.size());
		for (int index_ = 0; index_ < SHADER_DATA_COUNT; ++index_) {

			const int RG_COUNT = static_cast<int>(shaderData_[index_].rayGenEnteryPoint.size());
			for (int rgIndex = 0; rgIndex < RG_COUNT; ++rgIndex) {

				void* id_ = rtsoProps->GetShaderIdentifier(shaderData_[index_].rayGenEnteryPoint[rgIndex]);
				p += WriteShaderIdentifier(p, id_);

			}

		}

	}

	// Miss Shader �p�̃V�F�[�_�[���R�[�h���������݁B
	uint8_t* missStart = pStart + raygenRegion;
	{
		uint8_t* p = missStart;

		const int SHADER_DATA_COUNT = static_cast<int>(shaderData_.size());
		for (int index_ = 0; index_ < SHADER_DATA_COUNT; ++index_) {

			const int MS_COUNT = static_cast<int>(shaderData_[index_].missEntryPoint.size());
			for (int msIndex = 0; msIndex < MS_COUNT; ++msIndex) {

				void* id_ = rtsoProps->GetShaderIdentifier(shaderData_[index_].missEntryPoint[msIndex]);
				p += WriteShaderIdentifier(p, id_);

			}

		}

	}

	// Hit Group �p�̃V�F�[�_�[���R�[�h���������݁B
	uint8_t* hitgroupStart = pStart + raygenRegion + missRegion;
	{

		uint8_t* pRecord = hitgroupStart;

		// ���̏����͉��̎����B����BLAS�̃f�[�^���������ۂ�BLAS���Ƃɏ������ޏ�����ς���B���l���Ă���̂́AHITGROUP_ID���ƂɊ֐���p�ӂ�������B
		for (int index_ = 0; index_ < BLAS_COUNT; ++index_) {

			pRecord = BLASRegister::Ins()->WriteShaderRecord(pRecord, index_, hitgroupRecordSize, stateObject, hitGroupName);

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
	dispatchRayDesc.Width = DispatchX;
	dispatchRayDesc.Height = DispatchY;
	dispatchRayDesc.Depth = 1;

}

UINT RaytracingPipline::GetLargestDataSizeInHitGroup()
{

	/*===== �S�Ẵq�b�g�O���[�v�̒��ōł��f�[�^�T�C�Y���傫�����̂��擾���� =====*/

	UINT largestDataSize = 0;

	const int HIT_GROUP_COUNT = HitGroupMgr::Ins()->GetHitGroupCount();
	for (int index_ = 0; index_ < HIT_GROUP_COUNT; ++index_) {

		// �f�[�^�T�C�Y���擾�B
		UINT dataSize = 0;
		dataSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * HitGroupMgr::Ins()->GetSRVCount(index_);
		dataSize += sizeof(D3D12_GPU_VIRTUAL_ADDRESS) * HitGroupMgr::Ins()->GetCBVCount(index_);
		dataSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * HitGroupMgr::Ins()->GetUAVCount(index_);

		// �擾�����f�[�^�T�C�Y���ۑ�����Ă���f�[�^�T�C�Y��菬���������珈�����΂��B
		if (dataSize < largestDataSize) continue;

		// �擾�����f�[�^�T�C�Y��ۑ�����B
		largestDataSize = dataSize;

	}

	return largestDataSize;

}

Microsoft::WRL::ComPtr<ID3D12Resource> RaytracingPipline::CreateBuffer(size_t size_, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType, const wchar_t* name_)
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

	hr = DirectXBase::Ins()->dev_->CreateCommittedResource(
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

UINT RaytracingPipline::WriteShaderIdentifier(void* dst, const void* shaderId)
{
	memcpy(dst, shaderId, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	return D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
}

int RaytracingPipline::GetRayGenerationCount()
{
	int count = 0;

	const int SHADER_DATA_COUNT = static_cast<int>(shaderData_.size());
	for (int index_ = 0; index_ < SHADER_DATA_COUNT; ++index_) {

		count += static_cast<int>(shaderData_[index_].rayGenEnteryPoint.size());

	}

	return count;

}

int RaytracingPipline::GetMissCount()
{
	int count = 0;

	const int SHADER_DATA_COUNT = static_cast<int>(shaderData_.size());
	for (int index_ = 0; index_ < SHADER_DATA_COUNT; ++index_) {

		count += static_cast<int>(shaderData_[index_].missEntryPoint.size());

	}

	return count;

}
