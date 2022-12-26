#include "TLAS.h"
#include "PolygonInstanceRegister.h"
#include "DescriptorHeapMgr.h"

void TLAS::GenerateTLAS()
{

	/*===== TLAS�������� =====*/

	// �C���X�^���X�̍s����v�Z�B
	PolygonInstanceRegister::Ins()->CalWorldMat();


	/*-- �����\���̂�ݒ� --*/
	SettingAccelerationStructure();


	/*-- TLAS�p�̃f�B�X�N���v�^�𐶐����� --*/

	// �f�B�X�N���v�^�q�[�v�̃C���f�b�N�X���擾
	descriptorHeapIndex_ = DescriptorHeapMgr::Ins()->GetHead();

	// �f�B�X�N���v�^�q�[�v��SRV���m��
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.RaytracingAccelerationStructure.Location = tlasBuffer_->GetGPUVirtualAddress();
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	Engine::Ins()->device_.dev_->CreateShaderResourceView(nullptr, &srvDesc,
		basicHeapHandle);

	// �f�B�X�N���v�^�q�[�v���C���N�������g
	DescriptorHeapMgr::Ins()->IncrementHead();
}

void TLAS::Update()
{

	/*===== TLAS�̍X�V���� =====*/

	// �C���X�^���X�̍s����v�Z�B
	PolygonInstanceRegister::Ins()->CalWorldMat();

	// CPU ���珑�����݉\�ȃo�b�t�@�ɏ������ށB
	WriteToMemory(instanceDescMapAddress_, PolygonInstanceRegister::Ins()->GetData(), instanceDescSize_);

	// �X�V�̂��߂̒l��ݒ�B
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc{};
	auto& inputs = asDesc.Inputs;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = PolygonInstanceRegister::MAX_INSTANCE;
	inputs.InstanceDescs = instanceDescBuffer_->GetGPUVirtualAddress();
	// TLAS �̍X�V�������s�����߂̃t���O��ݒ肷��B
	inputs.Flags =
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE |
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;


	// �C���v���[�X�X�V�����s����B
	asDesc.SourceAccelerationStructureData = tlasBuffer_->GetGPUVirtualAddress();
	asDesc.DestAccelerationStructureData = tlasBuffer_->GetGPUVirtualAddress();
	// �X�V�p�̍�ƃo�b�t�@��ݒ肷��B
	asDesc.ScratchAccelerationStructureData = tlasUpdateBuffer_->GetGPUVirtualAddress();

	// �R�}���h���X�g�ɐςށB
	Engine::Ins()->mainGraphicsCmdList_->BuildRaytracingAccelerationStructure(
		&asDesc, 0, nullptr
	);
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::UAV(tlasBuffer_.Get());
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1,&barrier);

}

void TLAS::WriteToMemory(void* MapAddress, const void* PData, size_t DataSize)
{

	/*===== �������ɒl���������ޏ��� =====*/

	// null�`�F�b�N�B
	if (MapAddress == nullptr) assert(0);

	// �}�b�v�������s���B
	memcpy(MapAddress, PData, DataSize);
}

void TLAS::SettingAccelerationStructure()
{

	/*===== �����\���̐����֐� =====*/


	/*-- TLAS�̐����ɕK�v�ȃ������ʂ����߂� --*/

	// �C���X�^���X�̏����L�^�����o�b�t�@����������B
	instanceDescSize_ = PolygonInstanceRegister::MAX_INSTANCE * sizeof(D3D12_RAYTRACING_INSTANCE_DESC);
	instanceDescBuffer_ = FHelper::CreateBuffer(
		instanceDescSize_,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_HEAP_TYPE_UPLOAD);
	instanceDescBuffer_->SetName(L"InstanceDescBuffer");
	instanceDescBuffer_->Map(0, nullptr, &instanceDescMapAddress_);

	// ���������o�b�t�@�Ƀf�[�^���������ށB
	WriteToMemory(instanceDescMapAddress_, PolygonInstanceRegister::Ins()->GetData(), instanceDescSize_);

	// �������ʂ����߂邽�߂̐ݒ���s���B
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
	auto& inputs = buildASDesc.Inputs;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = PolygonInstanceRegister::MAX_INSTANCE;
	inputs.InstanceDescs = instanceDescBuffer_->GetGPUVirtualAddress();
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;

	// �������ʂ����߂�֐������s����B
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO tlasPrebuild{};
	Engine::Ins()->device_.dev_->GetRaytracingAccelerationStructurePrebuildInfo(
		&inputs, &tlasPrebuild
	);

	/*-- TLAS�o�b�t�@�ƃX�N���b�`�o�b�t�@�𐶐����� --*/

	// �X�N���b�`������(�o�b�t�@)���m�ہB
	scratchBuffer_ = FHelper::CreateBuffer(
		tlasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_HEAP_TYPE_DEFAULT
	);
	D3D12_RESOURCE_BARRIER barrier = { CD3DX12_RESOURCE_BARRIER::Transition(scratchBuffer_.Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_UNORDERED_ACCESS) };
	scratchBuffer_->SetName(L"TlasScratchBuffer");

	// TLAS�p������(�o�b�t�@)���m�ہB
	tlasBuffer_ = FHelper::CreateBuffer(
		tlasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);
	tlasBuffer_->SetName(L"TlasBuffer");

	// TLAS�X�V�p������(�o�b�t�@)���m�ہB
	tlasUpdateBuffer_ = FHelper::CreateBuffer(
		tlasPrebuild.UpdateScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_HEAP_TYPE_DEFAULT
	);
	barrier = { CD3DX12_RESOURCE_BARRIER::Transition(tlasUpdateBuffer_.Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_UNORDERED_ACCESS) };
	tlasUpdateBuffer_->SetName(L"TlasUpdateBuffer");

	/*-- BLAS�̃A�h���X�ƃX�N���b�`�o�b�t�@�A�h���X��TLAS�̃A�h���X���w�肵�Ċm�ۏ������R�}���h���X�g�ɐς� --*/

	// AccelerationStructure�\�z�B
	buildASDesc.ScratchAccelerationStructureData = scratchBuffer_->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = tlasBuffer_->GetGPUVirtualAddress();

	// �R�}���h���X�g�ɐς�Ŏ��s����B
	Engine::Ins()->mainGraphicsCmdList_->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr
	);


	/*-- ���ۂɃo�b�t�@�𐶐����� --*/

	CreateAccelerationStructure();

}

void TLAS::CreateAccelerationStructure()
{

	/*===== ���ۂ̃o�b�t�@�𐶐����� =====*/

	// ���\�[�X�o���A�̐ݒ�B
	D3D12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(tlasBuffer_.Get());
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &uavBarrier);
	Engine::Ins()->mainGraphicsCmdList_->Close();

	// TLAS���\�z�B
	//ID3D12CommandList* pCmdList[] = { Engine::Ins()->cmdList.Get() };

	// �\�z�p�֐����ĂԁB
	ID3D12CommandList* commandLists[] = { Engine::Ins()->mainGraphicsCmdList_.Get() };
	Engine::Ins()->graphicsCmdQueue_->ExecuteCommandLists(1, commandLists);
	Engine::Ins()->graphicsCmdQueue_->Signal(Engine::Ins()->asFence_.Get(), ++Engine::Ins()->asfenceValue_);


	/*-- ���\�[�X�o���A��ݒ肵�ď������߂Ȃ��悤�ɂ��� --*/

	// �O���t�B�b�N�R�}���h���X�g�̊����҂�
	UINT64 gpuFence = Engine::Ins()->asFence_->GetCompletedValue();
	if (gpuFence != Engine::Ins()->asfenceValue_) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		Engine::Ins()->asFence_->SetEventOnCompletion(Engine::Ins()->asfenceValue_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//�R�}���h�A���P�[�^�̃��Z�b�g
	Engine::Ins()->mainGraphicsCmdAllocator_->Reset();						//�L���[���N���A

	//�R�}���h���X�g�̃��Z�b�g
	Engine::Ins()->mainGraphicsCmdList_->Reset(Engine::Ins()->mainGraphicsCmdAllocator_.Get(), nullptr);		//�ĂуR�}���h���X�g�𒙂߂鏀��

}
