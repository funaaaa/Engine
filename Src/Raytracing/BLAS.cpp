#include "BLAS.h"
#include "ModelDataManager.h"
#include "DescriptorHeapMgr.h"
#include <DirectXMath.h>
#include "FbxLoader.h"
#include "RWStructuredBuffer.h"
#include "StructuredBuffer.h"
#include "FString.h"
#include "TextureManager.h"

using namespace DirectX;

void BLAS::GenerateBLASObj(const string& directryPath, const string& modelName, const wstring& hitGroupName, const LPCWSTR& TexturePath)
{

	/*===== BLAS�𐶐����鏈�� =====*/

	/*-- �`��f�[�^��ǂݍ��� --*/

	// �e�N�X�`����ǂݍ��ށB
	textureHandle = TextureManager::Instance()->LoadTextureInDescriptorHeapMgr(TexturePath);

	// �ǂݍ��񂾃f�[�^���ꎞ�ۑ�����p�̃o�b�t�@�B
	Object3DDeliveryData dataBuff;

	// ���f�������[�h�B
	ModelDataManager::LoadObj(directryPath, modelName, dataBuff, false);

	// ���_�������߂�B
	vertexCount = dataBuff.vertex.size();

	// ���_�C���f�b�N�X�������߂�B
	indexCount = dataBuff.index.size();

	// ���_�f�[�^��ϊ��B
	for (int index = 0; index < vertexCount; ++index) {

		RayVertex buff{};
		buff.normal = dataBuff.vertex[index].normal;
		buff.position = dataBuff.vertex[index].pos;
		buff.uv = dataBuff.vertex[index].uv;

		// �f�[�^��ۑ��B
		vertex.push_back(buff);

	}

	// ���_�C���f�b�N�X�f�[�^��ۑ��B
	vertIndex = dataBuff.index;

	// ���_�T�C�Y�����߂�B
	vertexStride = sizeof(RayVertex);

	// ���_�C���f�b�N�X�T�C�Y�����߂�B
	indexStride = sizeof(UINT);

	// ���_�o�b�t�@�𐶐�����B
	vertexBuffer = CreateBuffer(
		vertexStride * vertexCount,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// �m�ۂ����o�b�t�@�ɒ��_�f�[�^���������ށB
	WriteToMemory(vertexBuffer, vertex.data(), vertexStride * vertexCount);

	// ���_�C���f�b�N�X�o�b�t�@�𐶐�����B
	indexBuffer = CreateBuffer(
		indexStride * indexCount,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// �m�ۂ����C���f�b�N�X�o�b�t�@�ɒ��_�C���f�b�N�X�f�[�^���������ށB
	WriteToMemory(indexBuffer, vertIndex.data(), indexStride * indexCount);

	// ���_�C���f�b�N�X�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	indexDescriptor.CreateStructuredSRV(indexBuffer, indexCount, 0, indexStride, DescriptorHeapMgr::Instance()->GetDescriptorHeap(), DescriptorHeapMgr::Instance()->GetHead());
	DescriptorHeapMgr::Instance()->IncrementHead();

	// ���_�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	vertexDescriptor.CreateStructuredSRV(vertexBuffer, vertexCount, 0, vertexStride, DescriptorHeapMgr::Instance()->GetDescriptorHeap(), DescriptorHeapMgr::Instance()->GetHead());
	DescriptorHeapMgr::Instance()->IncrementHead();


	/*-- BLAS�o�b�t�@�𐶐����� --*/

	// �`���ݒ肷��p�̍\���̂�ݒ�B
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc();

	// BLAS�o�b�t�@��ݒ�A�\�z����B
	SettingAccelerationStructure(geomDesc);


	// �q�b�g�O���[�v����ۑ�����B
	this->hitGroupName = hitGroupName;

}

void BLAS::GenerateBLASFbx(const string& directryPath, const string& modelName, const wstring& hitGroupName, const LPCWSTR& TexturePath)
{

	/*===== BLAS�𐶐����鏈�� =====*/

	/*-- �`��f�[�^��ǂݍ��� --*/

	// �e�N�X�`����ǂݍ��ށB
	textureHandle = TextureManager::Instance()->LoadTextureInDescriptorHeapMgr(TexturePath);

	// �ǂݍ��񂾃f�[�^���ꎞ�ۑ�����p�̃o�b�t�@�B
	Object3DDeliveryData dataBuff;

	// ���f�������[�h�B
	modelIndex = FbxLoader::Instance()->LoadModelFromFile(directryPath, modelName);

	dataBuff = FbxLoader::Instance()->ConvertObject3DDeliveryData(modelIndex);

	// ���_�������߂�B
	vertexCount = dataBuff.vertex.size();

	// ���_�C���f�b�N�X�������߂�B
	indexCount = dataBuff.index.size();

	Object3DDeliveryData fbxData;
	ModelDataManager::LoadFbx((directryPath + modelName).c_str(), fbxData);

	// ���_�f�[�^��ϊ��B
	for (int index = 0; index < vertexCount; ++index) {

		RayVertex buff{};
		buff.normal = dataBuff.vertex[index].normal;
		buff.position = dataBuff.vertex[index].pos;
		buff.uv = dataBuff.vertex[index].uv;

		// �f�[�^��ۑ��B
		vertex.push_back(buff);

	}

	// ���_�C���f�b�N�X�f�[�^��ۑ��B
	vertIndex = dataBuff.index;

	// ���_�T�C�Y�����߂�B
	vertexStride = sizeof(RayVertex);

	// ���_�C���f�b�N�X�T�C�Y�����߂�B
	indexStride = sizeof(UINT);

	// ���_�o�b�t�@�𐶐�����B
	vertexBuffer = CreateBuffer(
		vertexStride * vertexCount,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// �m�ۂ����o�b�t�@�ɒ��_�f�[�^���������ށB
	WriteToMemory(vertexBuffer, vertex.data(), vertexStride * vertexCount);

	// ���_�C���f�b�N�X�o�b�t�@�𐶐�����B
	indexBuffer = CreateBuffer(
		indexStride * indexCount,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// �m�ۂ����C���f�b�N�X�o�b�t�@�ɒ��_�C���f�b�N�X�f�[�^���������ށB
	WriteToMemory(indexBuffer, vertIndex.data(), indexStride * indexCount);

	// ���_�C���f�b�N�X�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	indexDescriptor.CreateStructuredSRV(indexBuffer, indexCount, 0, indexStride, DescriptorHeapMgr::Instance()->GetDescriptorHeap(), DescriptorHeapMgr::Instance()->GetHead());
	DescriptorHeapMgr::Instance()->IncrementHead();

	// ���_�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	vertexDescriptor.CreateStructuredSRV(vertexBuffer, vertexCount, 0, vertexStride, DescriptorHeapMgr::Instance()->GetDescriptorHeap(), DescriptorHeapMgr::Instance()->GetHead());
	DescriptorHeapMgr::Instance()->IncrementHead();


	/*-- BLAS�o�b�t�@�𐶐����� --*/

	// �`���ݒ肷��p�̍\���̂�ݒ�B
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc();

	// BLAS�o�b�t�@��ݒ�A�\�z����B
	SettingAccelerationStructure(geomDesc);

	// �q�b�g�O���[�v����ۑ�����B
	this->hitGroupName = hitGroupName;

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Instance()->GetFbxModel(modelIndex).hasAnimation) {

		// ���͗p�\���̂����T�C�Y�B
		skinComputeInput.resize(vertex.size());

		// �X�L�j���O�A�j���[�V�����R���s���[�g�V�F�[�_�[�Ŏg�p������͗p�\���̂��Z�b�g�B
		FbxLoader::Instance()->GetSkinComputeInput(modelIndex, skinComputeInput);

		// �X�L�j���O�A�j���[�V�����Ŏg�p����R���s���[�g�V�F�[�_�[�����[�h���Ă����B
		skinComput.Init(L"resource/ShaderFiles/RayTracing/ComputeSkin.hlsl", sizeof(FbxLoader::SkinComputeInput), skinComputeInput.size(), skinComputeInput.data(), sizeof(RayVertex), vertex.size(), vertex.data());

	}

}

void BLAS::Update()
{

	/*===== BLAS�̍X�V =====*/

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Instance()->GetFbxModel(modelIndex).hasAnimation) {

		// �A�j���[�V�����̍X�V����
		auto& model = FbxLoader::Instance()->GetFbxModel(modelIndex);
		if (model.isPlay) {

			model.currentTime += model.frameTime;

			// �Ō�܂ōĐ�������擪�ɖ߂��B
			if (model.endTime < model.currentTime) {

				model.currentTime = model.startTime;

			}

			// �X�L�j���O�A�j���[�V�����R���s���[�g�V�F�[�_�[�Ŏg�p������͗p�\���̂��Z�b�g�B
			FbxLoader::Instance()->GetSkinComputeInput(modelIndex, skinComputeInput);

		}

	};

	// ���_���������ށB ���̂Ƃ���͒��_��������������\��͂Ȃ����A��X���̂���������ށB�_�[�e�B�t���O�݂����Ȃ̂�p�ӂ������B
	WriteToMemory(vertexBuffer, vertex.data(), vertexStride * vertexCount);

	// �X�V�̂��߂̒l��ݒ�B
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc();
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc{};
	auto& inputs = asDesc.Inputs;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = 1;
	inputs.pGeometryDescs = &geomDesc;
	// BLAS �̍X�V�������s�����߂̃t���O��ݒ肷��B
	inputs.Flags =
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE |
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;


	// �C���v���[�X�X�V�����s����B
	asDesc.SourceAccelerationStructureData = blasBuffer->GetGPUVirtualAddress();
	asDesc.DestAccelerationStructureData = blasBuffer->GetGPUVirtualAddress();
	// �X�V�p�̍�ƃo�b�t�@��ݒ肷��B
	asDesc.ScratchAccelerationStructureData = updateBuffer->GetGPUVirtualAddress();

	// �R�}���h���X�g�ɐςށB
	DirectXBase::Instance()->cmdList->BuildRaytracingAccelerationStructure(
		&asDesc, 0, nullptr
	);

	CreateAccelerationStructure();

}

void BLAS::ComputeSkin()
{

	/*===== ���_�f�[�^���X�L�j���O�s������ɏ��������鏈�� =====*/

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Instance()->GetFbxModel(modelIndex).hasAnimation) {

		// ���͍\���̂��X�V�B
		skinComput.UpdateInputSB(skinComputeInput.data());

		// �v�Z�J�n�B
		skinComput.Dispatch(1, 1, 1);

		// ���ʂ����B
		memcpy(vertex.data(), skinComput.outputSB->buffersOnCPU, sizeof(RayVertex) * vertex.size());

	}

}

void BLAS::InitAnimation()
{
	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Instance()->GetFbxModel(modelIndex).hasAnimation) {

		FbxLoader::Instance()->GetFbxModel(modelIndex).InitAnimation();

	}
}

void BLAS::PlayAnimation()
{

	/*===== �A�j���[�V���������� =====*/

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Instance()->GetFbxModel(modelIndex).hasAnimation) {

		FbxLoader::Instance()->GetFbxModel(modelIndex).PlayAnimation();

	}

}

void BLAS::StopAnimation()
{

	/*===== �A�j���[�V�������~������ =====*/

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Instance()->GetFbxModel(modelIndex).hasAnimation) {

		FbxLoader::Instance()->GetFbxModel(modelIndex).StopAnimation();

	}

}

void BLAS::WriteToMemory(ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize)
{

	/*===== �������ɒl���������ޏ��� =====*/

	// null�`�F�b�N�B
	if (resource == nullptr) return;

	// �}�b�v�������s���B
	void* mapped = nullptr;
	D3D12_RANGE range{ 0, dataSize };
	HRESULT hr = resource->Map(0, &range, (void**)&mapped);

	// �}�b�v������������l���������ށB
	if (SUCCEEDED(hr)) {

		memcpy(mapped, pData, dataSize);
		resource->Unmap(0, nullptr);

	}

}

ComPtr<ID3D12Resource> BLAS::CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType)
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

	// �o�b�t�@�������߂��o���B
	hr = DirectXBase::Instance()->dev->CreateCommittedResource(
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

D3D12_RAYTRACING_GEOMETRY_DESC BLAS::GetGeometryDesc()
{

	/*===== BLAS�������ɐݒ���擾����p�֐� =====*/

	// �`��f�[�^�̃t���O��ݒ�B
	auto geometryDesc = D3D12_RAYTRACING_GEOMETRY_DESC{};
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION;

	// �`��f�[�^�ׂ̍������ڂ�ݒ�B
	auto& triangles = geometryDesc.Triangles;
	triangles.VertexBuffer.StartAddress = vertexBuffer->GetGPUVirtualAddress();
	triangles.VertexBuffer.StrideInBytes = vertexStride;
	triangles.VertexCount = vertexCount;
	triangles.IndexBuffer = indexBuffer->GetGPUVirtualAddress();
	triangles.IndexCount = indexCount;
	triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	triangles.IndexFormat = DXGI_FORMAT_R32_UINT;

	return geometryDesc;

}

void BLAS::SettingAccelerationStructure(const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc)
{

	/*===== �����\���̂̐ݒ�p�֐� =====*/

	// AS(���C�Ƃ̔����v���ɍs�����߂ɕK�v�ȃ|���S���f�[�^�\����)�̐ݒ�ɕK�v�Ȋe���ڂ�ݒ�B
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc{};
	auto& inputs = buildASDesc.Inputs;	// D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	inputs.NumDescs = 1;
	inputs.pGeometryDescs = &geomDesc;

	// �֐����g���ĕK�v�ȃ������ʂ����߂�.
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO blasPrebuild{};
	DirectXBase::Instance()->dev->GetRaytracingAccelerationStructurePrebuildInfo(
		&inputs, &blasPrebuild
	);

	// �X�N���b�`�o�b�t�@�𐶐�����B
	scratchBuffer = CreateBuffer(
		blasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// BLAS�̃o�b�t�@�𐶐�����B
	blasBuffer = CreateBuffer(
		blasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// �X�V�p�o�b�t�@�𐶐�����B
	updateBuffer = CreateBuffer(
		blasPrebuild.UpdateScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// AccelerationStructure�̍\�z�B
	buildASDesc.ScratchAccelerationStructureData = scratchBuffer->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = blasBuffer->GetGPUVirtualAddress();
	// �R�}���h���X�g�ɐς�Ŏ��s����B
	DirectXBase::Instance()->cmdList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr /* pPostBuildInfoDescs */
	);

	// BLAS���\�z����B
	CreateAccelerationStructure();

}

void BLAS::CreateAccelerationStructure()
{

	/*===== BLAS�\�z���� =====*/

	// ���\�[�X�o���A�̐ݒ�B
	D3D12_RESOURCE_BARRIER uavBarrier{};
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = blasBuffer.Get();
	DirectXBase::Instance()->cmdList->ResourceBarrier(1, &uavBarrier);
	DirectXBase::Instance()->cmdList->Close();

	// BLAS���\�z�B
	ID3D12CommandList* pCmdList[] = { DirectXBase::Instance()->cmdList.Get() };
	// �\�z�p�֐����ĂԁB
	ID3D12CommandList* commandLists[] = { DirectXBase::Instance()->cmdList.Get() };
	DirectXBase::Instance()->cmdQueue->ExecuteCommandLists(1, commandLists);

	//�O���t�B�b�N�R�}���h���X�g�̊����҂�
	DirectXBase::Instance()->cmdQueue->Signal(DirectXBase::Instance()->fence.Get(), ++DirectXBase::Instance()->fenceVal);
	if (DirectXBase::Instance()->fence->GetCompletedValue() != DirectXBase::Instance()->fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		DirectXBase::Instance()->fence->SetEventOnCompletion(DirectXBase::Instance()->fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//�R�}���h�A���P�[�^�̃��Z�b�g
	DirectXBase::Instance()->cmdAllocator->Reset();						//�L���[���N���A

	//�R�}���h���X�g�̃��Z�b�g
	DirectXBase::Instance()->cmdList->Reset(DirectXBase::Instance()->cmdAllocator.Get(), nullptr);		//�ĂуR�}���h���X�g�𒙂߂鏀��

}

/*

������
�E�����̎�������ʂ�I�����B
�E�R���s���[�g�V�F�[�_�[����l�����Ƒ����Ă���ۂ��H
�E�������`�����������B�ł������̍Ō�̕��ɒ����������邩��A���������sole�Ŏ��邩���B
 ��FbxLoader.cpp��sort�ŏo�Ă�����������t�ɂ���ƌ`�͂�����ۂ��Ȃ�B�A�j���[�V�����͔��f����Ȃ��B
�E�A�j���[�V�������Đ�����Ȃ��B
�@���^�C�}�[�͕ς���Ă���̂ŁA�E�F�C�g���̕ϐ����������ς���Ă��邩�𒲂ׂ�B
�@���`�͌�񂵁A�܂��̓^�C�}�[�𔽉f������B

*/