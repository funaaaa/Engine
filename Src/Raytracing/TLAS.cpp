#include "TLAS.h"
#include "PorygonInstanceRegister.h"
#include "DescriptorHeapMgr.h"

void TLAS::GenerateTLAS()
{

	/*===== TLAS�������� =====*/

	// �C���X�^���X�̍s����v�Z�B
	PorygonInstanceRegister::Ins()->CalWorldMat();


	/*-- �����\���̂�ݒ� --*/
	SettingAccelerationStructure();


	/*-- TLAS�p�̃f�B�X�N���v�^�𐶐����� --*/

	// �f�B�X�N���v�^�q�[�v�̃C���f�b�N�X���擾
	descriptorHeapIndex = DescriptorHeapMgr::Ins()->GetHead();

	// �f�B�X�N���v�^�q�[�v��SRV���m��
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.RaytracingAccelerationStructure.Location = tlasBuffer->GetGPUVirtualAddress();
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap().Get()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	DirectXBase::Ins()->dev->CreateShaderResourceView(nullptr, &srvDesc,
		basicHeapHandle);

	// �f�B�X�N���v�^�q�[�v���C���N�������g
	DescriptorHeapMgr::Ins()->IncrementHead();

}

//void tlas::updateinstancedata()
//{
//
//	// �C���X�^���X�̏����L�^�����o�b�t�@����������B
//	size_t sizeofinstancedescs = porygoninstanceregister::instance()->getregistersize() * sizeof(d3d12_raytracing_instance_desc);
//
//	// ���������o�b�t�@�Ƀf�[�^���������ށB
//	writetomemory(instancedescbuffer, porygoninstanceregister::instance()->getdata(), sizeofinstancedescs);
//
//}

void TLAS::Update()
{

	/*===== TLAS�̍X�V���� =====*/

	// �C���X�^���X�̍s����v�Z�B
	PorygonInstanceRegister::Ins()->CalWorldMat();

	// Instance�̃T�C�Y���擾�B
	auto sizeOfInstanceDescs = PorygonInstanceRegister::Ins()->GetRegisterSize();
	sizeOfInstanceDescs *= sizeof(D3D12_RAYTRACING_INSTANCE_DESC);

	// CPU ���珑�����݉\�ȃo�b�t�@�ɏ������ށB
	WriteToMemory(instanceDescBuffer, PorygonInstanceRegister::Ins()->GetData(), sizeOfInstanceDescs);

	// �X�V�̂��߂̒l��ݒ�B
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc{};
	auto& inputs = asDesc.Inputs;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = PorygonInstanceRegister::Ins()->GetRegisterSize();
	inputs.InstanceDescs = instanceDescBuffer->GetGPUVirtualAddress();
	// TLAS �̍X�V�������s�����߂̃t���O��ݒ肷��B
	inputs.Flags =
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE |
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;


	// �C���v���[�X�X�V�����s����B
	asDesc.SourceAccelerationStructureData = tlasBuffer->GetGPUVirtualAddress();
	asDesc.DestAccelerationStructureData = tlasBuffer->GetGPUVirtualAddress();
	// �X�V�p�̍�ƃo�b�t�@��ݒ肷��B
	asDesc.ScratchAccelerationStructureData = tlasUpdateBuffer->GetGPUVirtualAddress();

	// �R�}���h���X�g�ɐςށB
	DirectXBase::Ins()->cmdList->BuildRaytracingAccelerationStructure(
		&asDesc, 0, nullptr
	);

	// ���s����B
	/*CreateAccelerationStructure();*/

}

void TLAS::WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize)
{

	/*===== �������ɒl���������ޏ��� =====*/

	// null�`�F�b�N�B
	if (resource == nullptr) return;

	// �}�b�v�������s���B
	void* mapped = nullptr;
	D3D12_RANGE range{ 0, dataSize };
	HRESULT hr = resource->Map(0, nullptr, (void**)&mapped);

	// �}�b�v������������l���������ށB
	if (SUCCEEDED(hr)) {

		memcpy(mapped, pData, dataSize);
		resource->Unmap(0, nullptr);

	}

}

Microsoft::WRL::ComPtr<ID3D12Resource> TLAS::CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType)
{

	/*===== �o�b�t�@�S�ʂ𐶐����鏈�� =====*/

	// ��������ݒ�p�\���̂�ݒ肷��B
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

	// ���ۂɃo�b�t�@�𐶐�����B
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

	// �o�b�t�@�������߂��o���B
	hr = DirectXBase::Ins()->dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		initialState,
		nullptr,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
	);

	// �����Ɏ��s������B
	if (FAILED(hr)) {
		OutputDebugStringA("CreateBuffer failed.\n");
	}

	return resource;

}

void TLAS::SettingAccelerationStructure()
{

	/*===== �����\���̐����֐� =====*/


	/*-- TLAS�̐����ɕK�v�ȃ������ʂ����߂� --*/

	// �C���X�^���X�̏����L�^�����o�b�t�@����������B
	size_t sizeOfInstanceDescs = PorygonInstanceRegister::Ins()->GetRegisterSize() * sizeof(D3D12_RAYTRACING_INSTANCE_DESC);
	instanceDescBuffer = CreateBuffer(
		sizeOfInstanceDescs,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_HEAP_TYPE_UPLOAD);

	// ���������o�b�t�@�Ƀf�[�^���������ށB
	WriteToMemory(instanceDescBuffer, PorygonInstanceRegister::Ins()->GetData(), sizeOfInstanceDescs);

	// �������ʂ����߂邽�߂̐ݒ���s���B
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
	auto& inputs = buildASDesc.Inputs;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = PorygonInstanceRegister::Ins()->GetRegisterSize();
	inputs.InstanceDescs = instanceDescBuffer->GetGPUVirtualAddress();
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;

	// �������ʂ����߂�֐������s����B
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO tlasPrebuild{};
	DirectXBase::Ins()->dev->GetRaytracingAccelerationStructurePrebuildInfo(
		&inputs, &tlasPrebuild
	);

	/*-- TLAS�o�b�t�@�ƃX�N���b�`�o�b�t�@�𐶐����� --*/

	// �X�N���b�`������(�o�b�t�@)���m�ہB
	scratchBuffer = CreateBuffer(
		tlasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// TLAS�p������(�o�b�t�@)���m�ہB
	tlasBuffer = CreateBuffer(
		tlasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// TLAS�X�V�p������(�o�b�t�@)���m�ہB
	tlasUpdateBuffer = CreateBuffer(
		tlasPrebuild.UpdateScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);

	/*-- BLAS�̃A�h���X�ƃX�N���b�`�o�b�t�@�A�h���X��TLAS�̃A�h���X���w�肵�Ċm�ۏ������R�}���h���X�g�ɐς� --*/

	// AccelerationStructure�\�z�B
	buildASDesc.ScratchAccelerationStructureData = scratchBuffer->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = tlasBuffer->GetGPUVirtualAddress();

	// �R�}���h���X�g�ɐς�Ŏ��s����B
	DirectXBase::Ins()->cmdList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr
	);


	/*-- ���ۂɃo�b�t�@�𐶐����� --*/

	CreateAccelerationStructure();

}

void TLAS::CreateAccelerationStructure()
{

	/*===== ���ۂ̃o�b�t�@�𐶐����� =====*/

	// ���\�[�X�o���A�̐ݒ�B
	D3D12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(tlasBuffer.Get());
	DirectXBase::Ins()->cmdList->ResourceBarrier(1, &uavBarrier);
	DirectXBase::Ins()->cmdList->Close();

	// TLAS���\�z�B
	//ID3D12CommandList* pCmdList[] = { DirectXBase::Ins()->cmdList.Get() };

	// �\�z�p�֐����ĂԁB
	ID3D12CommandList* commandLists[] = { DirectXBase::Ins()->cmdList.Get() };
	DirectXBase::Ins()->cmdQueue->ExecuteCommandLists(1, commandLists);


	/*-- ���\�[�X�o���A��ݒ肵�ď������߂Ȃ��悤�ɂ��� --*/

	//�O���t�B�b�N�R�}���h���X�g�̊����҂�
	DirectXBase::Ins()->cmdQueue->Signal(DirectXBase::Ins()->fence.Get(), ++DirectXBase::Ins()->fenceVal);
	if (DirectXBase::Ins()->fence->GetCompletedValue() != DirectXBase::Ins()->fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		DirectXBase::Ins()->fence->SetEventOnCompletion(DirectXBase::Ins()->fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//�R�}���h�A���P�[�^�̃��Z�b�g
	DirectXBase::Ins()->cmdAllocator->Reset();						//�L���[���N���A

	//�R�}���h���X�g�̃��Z�b�g
	DirectXBase::Ins()->cmdList->Reset(DirectXBase::Ins()->cmdAllocator.Get(), nullptr);		//�ĂуR�}���h���X�g�𒙂߂鏀��

}
