#include "RaytracingPipeline.h"
#include <DirectXTex/include/d3dx12.h>
#include "ShaderStorage.h"
#include "HitGroupMgr.h"
#include "RayRootsignature.h"
#include "Engine.h"
#include "BLASRegister.h"
#include "FHelper.h"
#include <DirectXMath.h>

void RaytracingPipeline::Setting(const std::vector<RayPipelineShaderData>& InputData, int UseHitGroup, int SRVCount, int CBVCount, int UAVCount, int PayloadSize, int AttribSize, int ReflectionCount)
{

	/*===== �Z�b�e�B���O���� =====*/

	// �q�b�g�O���[�v����ۑ��B
	hitGroupName_ = HitGroupMgr::Ins()->hitGroupNames[UseHitGroup];

	// ���͂��ꂽ�f�[�^��ۑ�����B
	const int INPUT_COUNT = static_cast<int>(InputData.size());
	for (int index = 0; index < INPUT_COUNT; ++index) {

		// ���͂��ꂽ�f�[�^�\���́B
		RayPipelineShaderData buff;

		// �ۑ�����B
		buff.shaderPath_ = InputData[index].shaderPath_;

		// �ۑ�����Ă���G���g���|�C���g��ۑ��B
		const int RG_ENTRY_COUNT = static_cast<int>(InputData[index].rayGenEnteryPoint_.size());
		for (int entryPointIndex = 0; entryPointIndex < RG_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			buff.rayGenEnteryPoint_.push_back(InputData[index].rayGenEnteryPoint_[entryPointIndex]);

		}
		const int MS_ENTRY_COUNT = static_cast<int>(InputData[index].missEntryPoint_.size());
		for (int entryPointIndex = 0; entryPointIndex < MS_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			buff.missEntryPoint_.push_back(InputData[index].missEntryPoint_[entryPointIndex]);

		}

		// �ۑ�����B
		const int HS_ENTRY_COUNT = static_cast<int>(InputData[index].hitgroupEntryPoint_.size());
		for (int entryPointIndex = 0; entryPointIndex < HS_ENTRY_COUNT; ++entryPointIndex) {
			buff.hitgroupEntryPoint_.push_back(InputData[index].hitgroupEntryPoint_[entryPointIndex]);
		}

		// �ۑ�����B
		shaderData_.push_back(buff);

	}

	// �O���[�o�����[�g�V�O�l�`����ݒ�B
	globalRootSig_ = std::make_shared<RayRootsignature>();
	// �p�����[�^�[t0��TLAS(SRV)��ݒ�B
	for (int index = 0; index < SRVCount; ++index)globalRootSig_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, index);
	// �p�����[�^�[b0�ɃJ�����p�o�b�t�@��ݒ�B
	for (int index = 0; index < CBVCount; ++index) {
		globalRootSig_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, index);
	}
	// �p�����[�^�[u0�ɏo�͗p�o�b�t�@��ݒ�B
	for (int index = 0; index < UAVCount; ++index)globalRootSig_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, index);

	// ���[�g�V�O�l�`���𐶐��B
	globalRootSig_->Create(false, L"GlobalRootSig");

	// �X�e�[�g�I�u�W�F�N�g�̐ݒ��ۑ����Ă����悤�̍\���́B
	CD3DX12_STATE_OBJECT_DESC subobjects;
	// �X�e�[�g�I�u�W�F�N�g�̏�Ԃ�ݒ�B
	subobjects.SetStateObjectType(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

	// �V�F�[�_�[��ǂݍ��ށB
	const int SHADER_COUNT = static_cast<int>(shaderData_.size());
	for (int index = 0; index < SHADER_COUNT; ++index) {

		shaderCode_.emplace_back();

		// �V�F�[�_�[���R���p�C������B
		ShaderStorage::Ins()->LoadShaderForDXC(shaderData_[index].shaderPath_, "lib_6_4", "");

		// �V�F�[�_�[��ǂݍ��ށB
		shaderCode_[index].BytecodeLength = static_cast<SIZE_T>(ShaderStorage::Ins()->GetShaderBin(shaderData_[index].shaderPath_).size());
		shaderCode_[index].pShaderBytecode = ShaderStorage::Ins()->GetShaderBin(shaderData_[index].shaderPath_).data();

		// �V�F�[�_�[�̊e�֐����R�[�h�̓o�^�B
		auto dxilLib = subobjects.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
		dxilLib->SetDXILLibrary(&shaderCode_[index]);

		// �V�F�[�_�[�̃G���g���|�C���g��o�^�B
		const int RG_ENTRY_COUNT = static_cast<int>(InputData[index].rayGenEnteryPoint_.size());
		for (int entryPointIndex = 0; entryPointIndex < RG_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			dxilLib->DefineExport(shaderData_[index].rayGenEnteryPoint_[entryPointIndex]);

		}
		const int MS_ENTRY_COUNT = static_cast<int>(InputData[index].missEntryPoint_.size());
		for (int entryPointIndex = 0; entryPointIndex < MS_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			dxilLib->DefineExport(shaderData_[index].missEntryPoint_[entryPointIndex]);

		}
		const int HG_ENTRY_COUNT = static_cast<int>(InputData[index].hitgroupEntryPoint_.size());
		for (int entryPointIndex = 0; entryPointIndex < HG_ENTRY_COUNT; ++entryPointIndex) {

			// �ۑ�����B
			dxilLib->DefineExport(shaderData_[index].hitgroupEntryPoint_[entryPointIndex]);

		}

	}

	// �q�b�g�O���[�v�̐ݒ�B
	auto hitGroup_ = subobjects.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
	hitGroup_->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

	// ClosestHitShader���G���g���|�C���g��ۑ��B
	if (HitGroupMgr::Ins()->GetCHFlag(UseHitGroup)) {
		hitGroup_->SetClosestHitShaderImport(HitGroupMgr::Ins()->GetCH(UseHitGroup));
	}
	// AnyHitShader�̃G���g���|�C���g��ۑ��B
	if (HitGroupMgr::Ins()->GetAHFlag(UseHitGroup)) {
		hitGroup_->SetAnyHitShaderImport(HitGroupMgr::Ins()->GetAH(UseHitGroup));
	}
	// IntersectShader�̃G���g���|�C���g��ۑ��B
	if (HitGroupMgr::Ins()->GetISFlag(UseHitGroup)) {
		hitGroup_->SetIntersectionShaderImport(HitGroupMgr::Ins()->GetIS(UseHitGroup));
	}
	// �q�b�g�O���[�v����ۑ��B
	hitGroup_->SetHitGroupExport(HitGroupMgr::Ins()->hitGroupNames[UseHitGroup]);

	// �O���[�o�����[�g�V�O�l�`���̐ݒ�B
	auto rootSig = subobjects.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	rootSig->SetRootSignature(globalRootSig_->GetRootSig().Get());

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
	Engine::Ins()->device_.dev_->CreateStateObject(
		subobjects, IID_PPV_ARGS(&stateObject_)
	);

	stateObject_->SetName(L"StateObject");

}

void RaytracingPipeline::ConstructionShaderTable(int DispatchX, int DispatchY)
{

	/*===== �V�F�[�_�[�e�[�u�����\�z =====*/

	// �V�F�[�_�[�e�[�u���̃T�C�Y���v�Z�B
	const auto ShaderRecordAlignment = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;

	// RayGeneration�V�F�[�_�[�ł̓��[�J�����[�g�V�O�l�`�����g�p�B
	raygenRecordSize_ += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	raygenRecordSize_ = RoundUp(raygenRecordSize_, ShaderRecordAlignment);

	// Miss�V�F�[�_�[�ł̓��[�J�����[�g�V�O�l�`�����g�p�B
	missRecordSize_ += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	missRecordSize_ = RoundUp(missRecordSize_, ShaderRecordAlignment);

	// �q�b�g�O���[�v�ł́A�ۑ�����Ă���q�b�g�O���[�v�̒�����ő�̃T�C�Y�̂��̂Ńf�[�^���m�ۂ���B
	hitgroupRecordSize_ += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	hitgroupRecordSize_ += GetLargestDataSizeInHitGroup();
	hitgroupRecordSize_ = RoundUp(hitgroupRecordSize_, ShaderRecordAlignment);

	// �g�p����e�V�F�[�_�[�̌����A�V�F�[�_�[�e�[�u���̃T�C�Y�����߂�B
	hitgroupCount = HitGroupMgr::Ins()->GetHitGroupCount();
	raygenSize_ = GetRayGenerationCount() * raygenRecordSize_;
	missSize_ = GetMissCount() * missRecordSize_;
	hitGroupSize_ = hitgroupCount * hitgroupRecordSize_;

	// �e�e�[�u���̊J�n�ʒu�ɃA���C�����g���񂪂���̂Œ�������B
	tableAlign_ = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;
	raygenRegion_ = RoundUp(raygenRecordSize_, tableAlign_);
	missRegion_ = RoundUp(missSize_, tableAlign_);
	hitgroupRegion_ = RoundUp(hitGroupSize_, tableAlign_);

	// �������ꂽBLAS�̐��B
	const int BLAS_COUNT = BLASRegister::Ins()->GetBLASCount() - 1;

	// �V�F�[�_�[�e�[�u���̃T�C�Y�B
	tableSize_ = raygenRegion_ + missRegion_ + hitgroupRegion_ * BLAS_COUNT;

	/*========== �V�F�[�_�[�e�[�u���̍\�z ==========*/

	// �V�F�[�_�[�e�[�u���m�ہB
	shaderTable_[0] = FHelper::CreateBuffer(
		tableSize_, D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_HEAP_TYPE_UPLOAD,
		L"ShaderTable0");
	shaderTalbeMapAddress_[0] = nullptr;
	shaderTable_[0]->Map(0, nullptr, &shaderTalbeMapAddress_[0]);
	shaderTable_[1] = FHelper::CreateBuffer(
		tableSize_, D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_HEAP_TYPE_UPLOAD,
		L"ShaderTable1");
	shaderTalbeMapAddress_[1] = nullptr;
	shaderTable_[1]->Map(0, nullptr, &shaderTalbeMapAddress_[1]);

	stateObject_.As(&rtsoProps_);

	// �V�F�[�_�[�e�[�u�����������݁A���C��ݒ肷��B
	WriteShadetTalbeAndSettingRay(0, DispatchX, DispatchY);
	WriteShadetTalbeAndSettingRay(1, DispatchX, DispatchY);

}

void RaytracingPipeline::MapHitGroupInfo()
{

	/*===== HitGroup�̏���]�� =====*/

	// ���݂�Queue�̃C���f�b�N�X
	int currentQueueIndex = Engine::Ins()->currentQueueIndex_;

	uint8_t* pStart = static_cast<uint8_t*>(shaderTalbeMapAddress_[currentQueueIndex]);

	// Hit Group �p�̃V�F�[�_�[���R�[�h���������݁B
	uint8_t* hitgroupStart = pStart + raygenRegion_ + missRegion_;
	{

		uint8_t* pRecord = hitgroupStart;

		// ���̏����͉��̎����B����BLAS�̃f�[�^���������ۂ�BLAS���Ƃɏ������ޏ�����ς���B���l���Ă���̂́AHITGROUP_ID���ƂɊ֐���p�ӂ�������B
		pRecord = BLASRegister::Ins()->WriteShaderRecord(pRecord, hitgroupRecordSize_, stateObject_, hitGroupName_, Engine::Ins()->currentQueueIndex_);

	}

}

UINT RaytracingPipeline::GetLargestDataSizeInHitGroup()
{

	/*===== �S�Ẵq�b�g�O���[�v�̒��ōł��f�[�^�T�C�Y���傫�����̂��擾���� =====*/

	UINT largestDataSize = 0;

	const int HIT_GROUP_COUNT = HitGroupMgr::Ins()->GetHitGroupCount();
	for (int index = 0; index < HIT_GROUP_COUNT; ++index) {

		// �f�[�^�T�C�Y���擾�B
		UINT dataSize = 0;
		dataSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * HitGroupMgr::Ins()->GetSRVCount(index);
		dataSize += sizeof(D3D12_GPU_VIRTUAL_ADDRESS) * HitGroupMgr::Ins()->GetCBVCount(index);
		dataSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * HitGroupMgr::Ins()->GetUAVCount(index);

		// �擾�����f�[�^�T�C�Y���ۑ�����Ă���f�[�^�T�C�Y��菬���������珈�����΂��B
		if (dataSize < largestDataSize) continue;

		// �擾�����f�[�^�T�C�Y��ۑ�����B
		largestDataSize = dataSize;

	}

	return largestDataSize;

}

void RaytracingPipeline::WriteShadetTalbeAndSettingRay(int Index, int DispatchX, int DispatchY)
{

	/*===== �V�F�[�_�[�e�[�u�����������݁A���C��ݒ肷�� =====*/

	// �e�V�F�[�_�[���R�[�h����������ł����B
	uint8_t* pStart = static_cast<uint8_t*>(shaderTalbeMapAddress_[Index]);

	// RayGeneration �p�̃V�F�[�_�[���R�[�h���������݁B
	uint8_t* rgsStart = pStart;
	{
		uint8_t* p = rgsStart;

		const int SHADER_DATA_COUNT = static_cast<int>(shaderData_.size());
		for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

			const int RG_COUNT = static_cast<int>(shaderData_[index].rayGenEnteryPoint_.size());
			for (int rgIndex = 0; rgIndex < RG_COUNT; ++rgIndex) {

				void* id = rtsoProps_->GetShaderIdentifier(shaderData_[index].rayGenEnteryPoint_[rgIndex]);
				p += WriteShaderIdentifier(p, id);

			}

		}

	}

	// Miss Shader �p�̃V�F�[�_�[���R�[�h���������݁B
	uint8_t* missStart = pStart + raygenRegion_;
	{
		uint8_t* p = missStart;

		const int SHADER_DATA_COUNT = static_cast<int>(shaderData_.size());
		for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

			const int MS_COUNT = static_cast<int>(shaderData_[index].missEntryPoint_.size());
			for (int msIndex = 0; msIndex < MS_COUNT; ++msIndex) {

				void* id = rtsoProps_->GetShaderIdentifier(shaderData_[index].missEntryPoint_[msIndex]);
				p += WriteShaderIdentifier(p, id);

			}

		}

	}

	// Hit Group �p�̃V�F�[�_�[���R�[�h���������݁B
	uint8_t* hitgroupStart = pStart + raygenRegion_ + missRegion_;
	{

		uint8_t* pRecord = hitgroupStart;

		// ���̏����͉��̎����B����BLAS�̃f�[�^���������ۂ�BLAS���Ƃɏ������ޏ�����ς���B���l���Ă���̂́AHITGROUP_ID���ƂɊ֐���p�ӂ�������B
		pRecord = BLASRegister::Ins()->WriteShaderRecord(pRecord, hitgroupRecordSize_, stateObject_, hitGroupName_, Index);

	}

	// ���C���ˎ��̐ݒ��ݒ�B

	// DispatchRays �̂��߂ɏ����Z�b�g���Ă���.
	auto startAddress = shaderTable_[Index]->GetGPUVirtualAddress();
	// RayGeneration�V�F�[�_�[�̏��
	auto& shaderRecordRG = dispatchRayDesc_[Index].RayGenerationShaderRecord;
	shaderRecordRG.StartAddress = startAddress;
	shaderRecordRG.SizeInBytes = raygenSize_;
	startAddress += raygenRegion_;
	// Miss�V�F�[�_�[�̏��
	auto& shaderRecordMS = dispatchRayDesc_[Index].MissShaderTable;
	shaderRecordMS.StartAddress = startAddress;
	shaderRecordMS.SizeInBytes = missSize_;
	shaderRecordMS.StrideInBytes = missRecordSize_;
	startAddress += missRegion_;
	// HitGroup�̏��
	auto& shaderRecordHG = dispatchRayDesc_[Index].HitGroupTable;
	shaderRecordHG.StartAddress = startAddress;
	shaderRecordHG.SizeInBytes = hitGroupSize_;
	shaderRecordHG.StrideInBytes = hitgroupRecordSize_;
	startAddress += hitgroupRegion_;
	// ���C�̏��
	dispatchRayDesc_[Index].Width = DispatchX;
	dispatchRayDesc_[Index].Height = DispatchY;
	dispatchRayDesc_[Index].Depth = 1;

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
