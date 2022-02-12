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

// fps�X�V
void FPS();

// �X���������{���玝���Ă����֐�
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
	/*----------DirectX����������----------*/
	DirectXBase directXBase;							// DirectX��Օ���
	directXBase.Init();									// DirectX��Ղ̏�����
	SoundManager::Instance()->SettingSoundManager();	// �T�E���h�}�l�[�W���[���Z�b�g����

	/*----------�p�C�v���C������----------*/
	PiplineManager::Instance()->Init();

	/*----------�ϐ��錾----------*/
	srand(time(NULL));

	// �O�p�`�f�[�^�����[�h
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
	// �m�ۂ����o�b�t�@�ɒ��_�f�[�^����������.
	surarin::WriteToHostVisibleMemory(vertexBuffer, tri, vbSize);



	/*==========  �@BLAS�̒�`  ==========*/


	// 1�ABLAS�ɕK�v�ȃ������ʂ����߂�B
	{
		/*
		D3D12_RAYTRACING_GEOMETRY_DESC�ɂ���
		�E�O�p�`�W�I���g���A��O�p�`�W�I���g���̂Q��ނ��ݒ�ł���B
		�E����͎O�p�`�W�I���g�����g�p����̂ŁAD3D12_RAYTRACING_GEOMETRY_DESC����triangles�ɒl�����Ă����B
		�E���_�̃T�C�Y�Ȃǂ����Ă������ATriangleStrip�Ȃǂɂ͑Ή����Ă��Ȃ��BTriangleList�̂݁B
		*/
	}
	// �`���ݒ肷��p�̍\���̂�ݒ�B
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc{};
	geomDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geomDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	geomDesc.Triangles.VertexBuffer.StartAddress = vertexBuffer->GetGPUVirtualAddress();
	geomDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(XMFLOAT3);
	geomDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	geomDesc.Triangles.VertexCount = _countof(tri);

	{
		/*
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC�ɂ���
		�E���̍\���̂�AS��TOPLEVEL��BOTTOMLEVEL�̂ǂ���̍\���ɂ��g�p�����B
		�E�����BLAS�𐶐����邽�߁ABOTTOMLEVEL���w�肷��B
		�E�p�ӂ����W�I���g����������B
		�E�t���O�̎�ނɂ��Ă̓X���������{p44���Q�ƁB
		*/
	}
	// AS(���C�Ƃ̔����v���ɍs�����߂ɕK�v�ȃ|���S���f�[�^�\����)�̐ݒ�ɕK�v�Ȋe���ڂ�ݒ�B
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc{};
	auto& inputs = buildASDesc.Inputs;	// D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
	inputs.NumDescs = 1;
	inputs.pGeometryDescs = &geomDesc;

	// �֐����g���ĕK�v�ȃ������ʂ����߂�.
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO blasPrebuild{};
	DirectXBase::Instance()->dev->GetRaytracingAccelerationStructurePrebuildInfo(
		&inputs, &blasPrebuild
	);


	/*-- 2�A�X�N���b�`�o�b�t�@��BLAS�o�b�t�@�𐶐����� --*/

	// �X�N���b�`�o�b�t�@�𐶐�����B
	ComPtr<ID3D12Resource> blasScratch;
	blasScratch = surarin::CreateBuffer(
		blasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// BLAS�̃o�b�t�@�𐶐�����B
	ComPtr<ID3D12Resource> 	triangleBlas;
	triangleBlas = surarin::CreateBuffer(
		blasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);


	/*-- 3�A�X�N���b�`�o�b�t�@��BLAS�o�b�t�@�̃A�h���X���g����BLAS�\�z�R�}���h��ς� --*/

	// Acceleration Structure�̍\�z�B
	buildASDesc.ScratchAccelerationStructureData = blasScratch->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = triangleBlas->GetGPUVirtualAddress();
	// �R�}���h���X�g�ɐς�Ŏ��s����B
	DirectXBase::Instance()->cmdList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr /* pPostBuildInfoDescs */
	);

	// ���\�[�X�o���A�̐ݒ�B
	D3D12_RESOURCE_BARRIER uavBarrier{};
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = triangleBlas.Get();
	DirectXBase::Instance()->cmdList->ResourceBarrier(1, &uavBarrier);
	DirectXBase::Instance()->cmdList->Close();


	/*-- 4�A�R�}���h�����s����BLAS�𐶐����� --*/

	// BLAS���\�z�B
	ID3D12CommandList* pCmdList[] = { DirectXBase::Instance()->cmdList.Get() };
	surarin::ExecuteCommandList(DirectXBase::Instance()->cmdList);


	/*-- 5�AUAV�o���A��ݒ肵�ď������߂Ȃ��悤�ɂ��� --*/

	// �{�֐��𔲂���ƃR�}���h��X�N���b�`�o�b�t�@������ƂȂ邽�ߑҋ@
	//surarin::WaitForIdleGpu();//�O���t�B�b�N�R�}���h���X�g�̎��s
	//ID3D12CommandList* cmdLists[] = { DirectXBase::Instance()->cmdList.Get() }; //�R�}���h���X�g�̔z��
	//DirectXBase::Instance()->cmdQueue->ExecuteCommandLists(1, cmdLists);

	//��ʃo�b�t�@���t���b�v
	//DirectXBase::Instance()->swapchain->Present(1, 0);

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




	/*==========  �ATLAS�̒�`  ==========*/


	// TLAS���`����O�ɁABLAS��3D��ԏ�ɔz�u���邽�߂̐ݒ���s���B

	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc{};
	// �ϊ��s��� float m[3][4]�̂��� XMFLOAT3X4 ���g�p.
	XMStoreFloat3x4(
		reinterpret_cast<XMFLOAT3X4*>(&instanceDesc.Transform),
		XMMatrixIdentity());
	instanceDesc.InstanceID = 0;
	instanceDesc.InstanceMask = 0xFF;
	instanceDesc.InstanceContributionToHitGroupIndex = 0;
	instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	instanceDesc.AccelerationStructure = triangleBlas->GetGPUVirtualAddress();
	// �C���X�^���X�̏����L�^�����o�b�t�@����������.
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


	/*-- 1�ATLAS�ɕK�v�ȃ������ʂ����߂� --*/

	// �������ʂ����߂邽�߂̐ݒ���s���B
	buildASDesc = {};
	inputs = buildASDesc.Inputs; // D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
	inputs.NumDescs = 1;
	// �������ʂ����߂�֐������s����B
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO tlasPrebuild{};
	DirectXBase::Instance()->dev->GetRaytracingAccelerationStructurePrebuildInfo(
		&inputs, &tlasPrebuild
	);



	/*-- 2�A�X�N���b�`�o�b�t�@��TLAS�o�b�t�@���m�ۂ��� --*/

	ComPtr<ID3D12Resource> tlasScratch;
	tlasScratch = surarin::CreateBuffer(
		tlasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);
	// TLAS �p������(�o�b�t�@)���m��.
	// ���\�[�X�X�e�[�g��D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE
	ComPtr<ID3D12Resource> triangleScratch;
	triangleScratch = surarin::CreateBuffer(
		tlasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);


	/*-- 3�ABLAS�̃A�h���X�ƃX�N���b�`�o�b�t�@�A�h���X��TLAS�̃A�h���X���w�肵�Ċm�ۏ������R�}���h���X�g�ɐς� --*/

	// Acceleration Structure �\�z.
	buildASDesc.Inputs.InstanceDescs = instanceDescBuffer->GetGPUVirtualAddress();
	buildASDesc.ScratchAccelerationStructureData = tlasScratch->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = triangleScratch->GetGPUVirtualAddress();
	// �R�}���h���X�g�ɐς�Ŏ��s����.
	DirectXBase::Instance()->cmdList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr /* pPostBuildInfoDescs */
	);


	/*-- 4�A���s���� --*/

	// ���\�[�X�o���A�̐ݒ�.
	uavBarrier = {};
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = triangleScratch.Get();
	DirectXBase::Instance()->cmdList->ResourceBarrier(1, &uavBarrier);
	DirectXBase::Instance()->cmdList->Close();
	// TLAS �\�z.
	surarin::ExecuteCommandList(DirectXBase::Instance()->cmdList);


	/*-- 5�A���\�[�X�o���A��ݒ肵�ď������߂Ȃ��悤�ɂ��� --*/

	// �{�֐��𔲂���ƃX�N���b�`�o�b�t�@��InstanceDesc�̃o�b�t�@������ƂȂ邽�ߑҋ@.
	//surarin::WaitForIdleGpu();
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


	/*-- 6�ATLAS�̃f�B�X�N���v�^����� --*/

	// �f�B�X�N���v�^�̏����B AllocateDescriptor�ł̓f�B�X�N���v�^�[�𐶐����Ċm�ۂ��Ă���̂ŁA�����̃R�[�h���玝���Ă���B
	D3D12_DESCRIPTOR_HEAP_DESC tlasDescHeapDesc{};
	tlasDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	tlasDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			// �V�F�[�_�[���猩����
	tlasDescHeapDesc.NumDescriptors = 32;										// CBV3��
	// �f�B�X�N���v�^�q�[�v�̐���
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> tlasDescriptor = {};
	HRESULT resultBuff = DirectXBase::Instance()->dev->CreateDescriptorHeap(&tlasDescHeapDesc, IID_PPV_ARGS(&tlasDescriptor));
	// �f�B�X�N���v�^�q�[�v��SRV���m��
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.RaytracingAccelerationStructure.Location = triangleScratch->GetGPUVirtualAddress();
	DirectXBase::Instance()->dev->CreateShaderResourceView(nullptr, &srvDesc, tlasDescriptor->GetCPUDescriptorHandleForHeapStart());





	/*==========  �O���[�o�����[�g�V�O�l�`���𐶐�  ==========*/


	vector<D3D12_ROOT_PARAMETER> rootParams;
	rootParams.resize(2);

	// TLAS �� t0 ���W�X�^�Ɋ��蓖�ĂĎg�p����ݒ�.
	D3D12_DESCRIPTOR_RANGE descRangeTLAS{};
	descRangeTLAS.BaseShaderRegister = 0;
	descRangeTLAS.NumDescriptors = 1;
	descRangeTLAS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	// �o�̓o�b�t�@(UAV) �� u0 ���W�X�^�Ɋ��蓖�ĂĎg�p����ݒ�.
	D3D12_DESCRIPTOR_RANGE descRangeOutput{};
	descRangeOutput.BaseShaderRegister = 0;
	descRangeOutput.NumDescriptors = 1;
	descRangeOutput.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

	// TLAS��ݒ�B
	rootParams[0] = D3D12_ROOT_PARAMETER{};
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParams[0].DescriptorTable.NumDescriptorRanges = 1;
	rootParams[0].DescriptorTable.pDescriptorRanges = &descRangeTLAS;

	// �o�͗p�o�b�t�@��ݒ�B
	rootParams[1] = D3D12_ROOT_PARAMETER{};
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;
	rootParams[1].DescriptorTable.pDescriptorRanges = &descRangeOutput;

	// �O���[�o�����[�g�V�O�l�`���ݒ�p�\���̂�ݒ�B
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.NumParameters = UINT(rootParams.size());
	rootSigDesc.pParameters = rootParams.data();

	// ���[�g�V�O�l�`������.
	ComPtr<ID3DBlob> blob, errBlob;
	ComPtr<ID3D12RootSignature> grobalRootsignature;
	D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &blob, &errBlob);
	DirectXBase::Instance()->dev->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
		IID_PPV_ARGS(&grobalRootsignature));





	/*==========  �V�F�[�_�[�̃R���p�C��  ==========*/

	// �V�F�[�_�[���R���p�C������B
	ShaderStorage::Instance()->LoadShaderForDXC("Resource/ShaderFiles/RayTracing/triangleShaderHeader.hlsl", "lib_6_3", "");





	/*==========  �X�e�[�g�I�u�W�F�N�g�̐���  ==========*/

	// �X�e�[�g�I�u�W�F�N�g�̐ݒ��ۑ����Ă����悤�̍\����
	vector<D3D12_STATE_SUBOBJECT> subobjects;


	/*-- 1�A�V�F�[�_�[��ǂݍ���ŃV���{�����擾 --*/

	// �e�V�F�[�_�[�̃G���g���[�|�C���g��ݒ�B
	D3D12_EXPORT_DESC exports[] = {
	{ L"mainRayGen", nullptr, D3D12_EXPORT_FLAG_NONE },
	{ L"mainMS", nullptr, D3D12_EXPORT_FLAG_NONE },
	{ L"mainCHS", nullptr, D3D12_EXPORT_FLAG_NONE },
	};


	vector<char> shaderBin;
	shaderBin = ShaderStorage::Instance()->GetShaderBin("Resource/ShaderFiles/RayTracing/triangleShaderHeader.hlsl");

	// �V�F�[�_�[��Blob���擾���ēo�^����B
	D3D12_DXIL_LIBRARY_DESC dxilLibDesc{};
	dxilLibDesc.DXILLibrary = D3D12_SHADER_BYTECODE{ shaderBin.data(),shaderBin.size() };
	dxilLibDesc.NumExports = _countof(exports);
	dxilLibDesc.pExports = exports;

	// �X�e�[�g�I�u�W�F�N�g�ݒ�ۑ��p�z��Ƀv�b�V���o�b�N
	subobjects.emplace_back(
		D3D12_STATE_SUBOBJECT{
			D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY, &dxilLibDesc
		}
	);


	/*-- 2�A�q�b�g�O���[�v�𐶐� --*/

	D3D12_HIT_GROUP_DESC hitGroupDesc{};
	hitGroupDesc.Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
	hitGroupDesc.ClosestHitShaderImport = L"mainCHS";
	hitGroupDesc.HitGroupExport = hitGroupName;
	subobjects.emplace_back(
		D3D12_STATE_SUBOBJECT{
		D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP, &hitGroupDesc
		}
	);


	/*-- 3�A�O���[�o�����[�g�V�O�l�`���̐ݒ� --*/

	D3D12_GLOBAL_ROOT_SIGNATURE rootSignatureGlobal{};
	rootSignatureGlobal.pGlobalRootSignature = grobalRootsignature.Get();
	subobjects.emplace_back(
		D3D12_STATE_SUBOBJECT{
		D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE, &rootSignatureGlobal
		}
	);


	/*-- 4�A�ċA�Ăяo����e��������̃T�C�Y�̐ݒ� --*/

	// �V�F�[�_�[�Ŏg�p����e�v�f��ݒ�
	D3D12_RAYTRACING_SHADER_CONFIG shaderConfig{};
	shaderConfig.MaxPayloadSizeInBytes = sizeof(XMFLOAT3);
	shaderConfig.MaxAttributeSizeInBytes = sizeof(XMFLOAT2);
	subobjects.emplace_back(
		D3D12_STATE_SUBOBJECT{
		D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG, &shaderConfig
		}
	);

	// �V�F�[�_�[�ł̍ċA�\�i�K��ݒ�
	D3D12_RAYTRACING_PIPELINE_CONFIG pipelineConfig{};
	pipelineConfig.MaxTraceRecursionDepth = 1;
	subobjects.emplace_back(
		D3D12_STATE_SUBOBJECT{
		D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG, &pipelineConfig
		}
	);


	/*-- 5�AID3D12StateObject�𐶐� --*/

	// �X�e�[�g�I�u�W�F�N�g
	ComPtr<ID3D12StateObject> stateObject;

	// �X�e�[�g�I�u�W�F�N�g�𐶐�
	D3D12_STATE_OBJECT_DESC stateObjDesc{};
	stateObjDesc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
	stateObjDesc.NumSubobjects = UINT(subobjects.size());
	stateObjDesc.pSubobjects = subobjects.data();
	resultBuff = DirectXBase::Instance()->dev->CreateStateObject(
		&stateObjDesc, IID_PPV_ARGS(stateObject.ReleaseAndGetAddressOf())
	);





	/*==========  UAV�o�̓o�b�t�@�̏���  ==========*/

	// UAV��ݒ�
	ComPtr<ID3D12Resource> rayTracingOutput = surarin::CreateTexture2D(
		window_width, window_height, DXGI_FORMAT_R8G8B8A8_UNORM,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// �f�B�X�N���v�^�q�[�v�̏���
	D3D12_DESCRIPTOR_HEAP_DESC uavDescHeapDesc{};
	uavDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	uavDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			// �V�F�[�_�[���猩����
	uavDescHeapDesc.NumDescriptors = 32;										// CBV3��
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> uavDescriptor = {};
	resultBuff = DirectXBase::Instance()->dev->CreateDescriptorHeap(&uavDescHeapDesc, IID_PPV_ARGS(&uavDescriptor));

	// �f�B�X�N���v�^�q�[�v��UAV���m��
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	DirectXBase::Instance()->dev->CreateUnorderedAccessView(
		rayTracingOutput.Get(), nullptr, &uavDesc, uavDescriptor->GetCPUDescriptorHandleForHeapStart());





	/*==========  ShaderTable�̐���  ==========*/

	// �e�V�F�[�_�[���R�[�h��ShaderIdentifier��ێ�����B
	UINT recordSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

	// �O���[�o���̃��[�g�V�O�l�`���ȊO�̏��������Ȃ��̂Ń��R�[�h�T�C�Y�͂��ꂾ��.
	// ���Ƃ̓A���C�����g�����ۂ悤�ɂ���.
	recordSize = surarin::RoundUp(recordSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);

	// �V�F�[�_�[�e�[�u���̃T�C�Y�����߂�.
	UINT raygenSize = 1 * recordSize; // ����1�� Ray Generation �V�F�[�_�[�𗘗p���Ă���B
	UINT missSize = 1 * recordSize; // ��1�� Miss �V�F�[�_�[�𗘗p���Ă���B
	UINT hitGroupSize = 1 * recordSize; // ��1�� HitGroup ���g�p���Ă���B

	// �e�e�[�u���̊J�n�ʒu�ɃA���C�����g���񂪂���̂Œ�������B
	auto tableAlign = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;
	UINT raygenRegion = surarin::RoundUp(raygenSize, tableAlign);
	UINT missRegion = surarin::RoundUp(missSize, tableAlign);
	UINT hitgroupRegion = surarin::RoundUp(hitGroupSize, tableAlign);

	// �V�F�[�_�[�e�[�u���m�ہB
	auto tableSize = raygenRegion + missRegion + hitgroupRegion;
	ComPtr<ID3D12Resource> shaderTable = surarin::CreateBuffer(
		tableSize,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_HEAP_TYPE_UPLOAD
	);

	// StateObject����v���p�e�B���擾�B
	ComPtr<ID3D12StateObjectProperties> rtsoProps;
	stateObject.As(&rtsoProps);

	// �e�V�F�[�_�[���R�[�h����������ł����B
	void* mapped = nullptr;
	shaderTable->Map(0, nullptr, &mapped);
	uint8_t* pStart = static_cast<uint8_t*>(mapped);
	// RayGeneration �p�̃V�F�[�_�[���R�[�h���������݁B
	auto rgsStart = pStart;
	{

		uint8_t* p = rgsStart;
		auto id = rtsoProps->GetShaderIdentifier(L"mainRayGen");
		memcpy(p, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
		p += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		// ���[�J�����[�g�V�O�l�`���g�p���ɂ͑��̃f�[�^���������ށB

	}

	// Miss Shader �p�̃V�F�[�_�[���R�[�h���������݁B
	auto missStart = pStart + raygenRegion;
	{

		uint8_t* p = missStart;
		auto id = rtsoProps->GetShaderIdentifier(L"mainMS");
		memcpy(p, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
		p += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		// ���[�J�����[�g�V�O�l�`���g�p���ɂ͑��̃f�[�^���������ށB

	}

	// Hit Group �p�̃V�F�[�_�[���R�[�h���������݁B
	auto hitgroupStart = pStart + raygenRegion + missRegion;
	{

		uint8_t* p = hitgroupStart;
		auto id = rtsoProps->GetShaderIdentifier(hitGroupName);
		memcpy(p, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
		p += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		// ���[�J�����[�g�V�O�l�`���g�p���ɂ͑��̃f�[�^���������ށB

	}

	// �}�b�v�������I����B
	shaderTable->Unmap(0, nullptr);





	/*==========  D3D12_DISPATCH_RAYS_DESC�̐ݒ�  ==========*/

	// ���C�g���[�V���O���J�n����ۂɎg�p����\���́B
	D3D12_DISPATCH_RAYS_DESC dispatchRayDesc = {};

	// RayGeneration�V�F�[�_�[�̃V�F�[�_�[���R�[�h���Z�b�g�B
	auto startAddress = shaderTable->GetGPUVirtualAddress();
	auto& shaderRecordRG = dispatchRayDesc.RayGenerationShaderRecord;
	shaderRecordRG.StartAddress = startAddress;
	shaderRecordRG.SizeInBytes = raygenSize;
	startAddress += raygenRegion;

	// Miss�V�F�[�_�[�̃V�F�[�_�[���R�[�h���Z�b�g�B
	auto& shaderRecordMS = dispatchRayDesc.MissShaderTable;
	shaderRecordMS.StartAddress = startAddress;
	shaderRecordMS.SizeInBytes = missSize;
	shaderRecordMS.StrideInBytes = recordSize;
	startAddress += missRegion;

	// ClosestHit�V�F�[�_�[�̃V�F�[�_�[���R�[�h���Z�b�g�B
	auto& shaderRecordHG = dispatchRayDesc.HitGroupTable;
	shaderRecordHG.StartAddress = startAddress;
	shaderRecordHG.SizeInBytes = hitGroupSize;
	shaderRecordHG.StrideInBytes = recordSize;
	startAddress += hitgroupRegion;

	// ���C�g���[�X�̑傫�������Z�b�g�B
	dispatchRayDesc.Width = window_width;
	dispatchRayDesc.Height = window_height;
	dispatchRayDesc.Depth = 1;


	/*----------�Q�[�����[�v----------*/
	while (true) {
		/*----------���t���[������(�`��O����)----------*/
		directXBase.processBeforeDrawing();



		/*----- �X�V���� -----*/

		// �r���[�s��𐶐��B
		Camera::GenerateMatView();

		FPS();

		//Camera::target = triangle.GetPos();


		/*----- �`�揈�� -----*/

		// ��ʂɕ\������郌���_�[�^�[�Q�b�g�ɖ߂��B
		DirectXBase::Instance()->SetRenderTarget();

		// �O�p�`��`��
		//triangle.Draw();

		// �f�B�X�N���v�^�q�[�v�ݒ�R�}���h
		ID3D12DescriptorHeap* ppHeaps2[] = { tlasDescriptor.Get() };
		DirectXBase::Instance()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);

		// ���[�g�V�O�l�`���ƃ��\�[�X���Z�b�g.
		DirectXBase::Instance()->cmdList->SetComputeRootSignature(
			grobalRootsignature.Get());
		DirectXBase::Instance()->cmdList->SetComputeRootDescriptorTable(
			0, tlasDescriptor->GetGPUDescriptorHandleForHeapStart()); // TLAS �̃Z�b�g.

		// �f�B�X�N���v�^�q�[�v�ݒ�R�}���h
		ppHeaps2[0] = { uavDescriptor.Get() };
		DirectXBase::Instance()->cmdList->SetDescriptorHeaps(_countof(ppHeaps2), ppHeaps2);

		DirectXBase::Instance()->cmdList->SetComputeRootDescriptorTable(
			1, uavDescriptor->GetGPUDescriptorHandleForHeapStart()); // UAV �o�b�t�@�̃Z�b�g.
		// ���C�g���[�V���O���ʃo�b�t�@�� UAV ��Ԃ�.
		auto barrierToUAV = CD3DX12_RESOURCE_BARRIER::Transition(
			rayTracingOutput.Get(),
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);
		DirectXBase::Instance()->cmdList->ResourceBarrier(1, &barrierToUAV);
		// ���C�g���[�V���O���J�n.
		DirectXBase::Instance()->cmdList->SetPipelineState1(stateObject.Get());
		DirectXBase::Instance()->cmdList->DispatchRays(&dispatchRayDesc);

		// �o���A��ݒ肵�e���\�[�X�̏�Ԃ�J�ڂ�����.
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

		// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�����Ƃɖ߂��B
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


// �^�C�g���o�[��FPS�̍X�V
void FPS()
{
	static DWORD prev_time = timeGetTime();	// �O��̎���
	static int frame_count = 0;		// �t���[���J�E���g
	DWORD now_time = timeGetTime();		// ����̃t���[���̎���

	frame_count++;	// �t���[�������J�E���g����

	// �o�ߎ��Ԃ��P�b�𒴂�����J�E���g�Ǝ��Ԃ����Z�b�g
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

��������

�E�O�p�`���`�悳���ł��낤�ꏊ�܂ł͎����ł����B

���_���Q��
1,TransRay�֐�����`����Ă��Ȃ��Ɣ��肳���B
�@��DXR1_0�o�[�W�������Ή����Ă���̂Ŗ����Ƃ��������B
�@��RAY_FLAG�݂����Ȃ����`����Ă��Ȃ�����ɂȂ邪�A�����\���̂͒�`����Ă��锻��ɂȂ�B�����̈Ⴂ�ɂȂɂ����������肻���B
�@�������\���̂͑��̂�ł���`����Ă���̂ŁA�����������C�g���֌W�̊֐����g���Ȃ��H
�@�@�������\���̂𑼂̃v���W�F�N�g�Ɏ����Ă��������`����Ă��Ȃ��Əo���̂ŁA���C�g���֌W�̊֐����̂͒�`����Ă�����ۂ��B
�@���V�F�[�_�[���f��(6.3)����(lib)�������Ă���͂��B���ɂ�����Ȑݒ肪�K�v�Ȃ̂��B

2,����̃R�[�h�ł̓��C�g���p�̃V�F�[�_�[���R���p�C���ł��Ȃ��B
�@���X���������{�ɏ����Ă���V��������g���ĕʂŎ������Ȃ���΂Ȃ�Ȃ��B
�@��Blob��Ԃ��Ƃ���܂Ŏ����Ă�����Ό�͌���̃R�[�h���������������邾���ōςށB

*/