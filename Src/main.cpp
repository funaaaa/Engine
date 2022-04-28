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
#include "FbxLoader.h"
#include "Vec.h"

#include "BLASRegister.h"
#include "PorygonInstance.h"
#include "TLAS.h"
#include "RayRootsignature.h"
#include "DynamicConstBuffer.h"
#include "DescriptorHeapMgr.h"

#include <utilapiset.h>

#define COLORHEX(hex) hex / 255.0f

#define SCREEN_VIRTUAL_WIDTH 300

static const wchar_t* hitGroupName = L"hitGroupName";

// fps�X�V
void FPS();

struct KariConstBufferData {

	XMMATRIX mtxView;			// �r���[�s��B
	XMMATRIX mtxProj;			// �v���W�F�N�V�����s��B
	XMMATRIX mtxViewInv;		// �r���[�t�s��B
	XMMATRIX mtxProjInv;		// �v���W�F�N�V�����t�s��B
	XMFLOAT4 lightDirection;	// ���s�����̌����B
	XMFLOAT4 lightColor;		// ���s�����F�B
	XMFLOAT4 ambientColor;		// �����B

};

#include "BLAS.h"
// �X���������{���玝���Ă����֐�
namespace surarin {

	void WriteToHostVisibleMemory(ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize) {
		if (resource == nullptr) {
			return;
		}
		void* mapped = nullptr;
		HRESULT hr = resource->Map(0, nullptr, (void**)&mapped);
		if (SUCCEEDED(hr)) {
			memcpy(mapped, pData, dataSize);
			resource->Unmap(0, nullptr);
		}
	}

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

	ComPtr<ID3D12Resource> CreateDataBuffer(
		size_t requestSize,
		const void* initData,
		D3D12_HEAP_TYPE heapType,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE
	) {

		auto initialState = D3D12_RESOURCE_STATE_COPY_DEST;
		if (heapType == D3D12_HEAP_TYPE_UPLOAD) {
			initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
		}

		auto resource = CreateBuffer(requestSize, flags, initialState, heapType);

		if (initData != nullptr) {
			if (heapType == D3D12_HEAP_TYPE_DEFAULT) {
				WriteToHostVisibleMemory(resource, initData, requestSize);
			}
			if (heapType == D3D12_HEAP_TYPE_UPLOAD) {
				WriteToHostVisibleMemory(resource, initData, requestSize);
			}
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

	UINT WriteShaderIdentifier(void* dst, const void* shaderId)
	{
		memcpy(dst, shaderId, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
		return D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	}

	UINT WriteGPUDescriptor(
		void* dst, const D3D12_GPU_DESCRIPTOR_HANDLE* descriptor)
	{
		memcpy(dst, descriptor, sizeof(descriptor));
		return UINT(sizeof(descriptor));
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

	// �f�B�X�N���v�^�q�[�v���������B
	DescriptorHeapMgr::Instance()->GenerateDescriptorHeap();

	// FBXLoader���������B
	FbxLoader::Instance()->Init();

	// ����BLAS�𐶐��B
	int boneBlas = BLASRegister::Instance()->GenerateFbx("Resource/", "boneTest.fbx", hitGroupName, L"Resource/backGround.png");

	// ����BLAS�𐶐��B
	int monkeyBlas = BLASRegister::Instance()->GenerateObj("Resource/", "monkey.obj", hitGroupName, L"Resource/backGround.png");

	// ����BLAS�𐶐��B
	int groundBlas = BLASRegister::Instance()->GenerateObj("Resource/", "ground.obj", hitGroupName, L"Resource/Fine_Basin.jpg");

	// �O�p�`��Instancec�𐶐��B
	vector<PorygonMeshInstance> porygonInstance;
	porygonInstance.resize(4);

	// �C���X�^���X�𐶐�
	porygonInstance[0].CreateInstance(BLASRegister::Instance()->GetBLASBuffer(boneBlas), 0, 2);
	porygonInstance[1].CreateInstance(BLASRegister::Instance()->GetBLASBuffer(boneBlas), 0, 1);
	porygonInstance[2].CreateInstance(BLASRegister::Instance()->GetBLASBuffer(monkeyBlas), 1, 2);
	porygonInstance[3].CreateInstance(BLASRegister::Instance()->GetBLASBuffer(groundBlas), 2, 2);

	// �ړ�������B
	porygonInstance[0].AddTrans(-2.0f, 0.0f, 0);
	porygonInstance[1].AddTrans(2.0f, 0.0f, 0);
	porygonInstance[2].AddTrans(0.0f, 0.0f, 0);
	porygonInstance[3].AddTrans(0.0f, -1.0f, 0);

	// ������x��]������B
	porygonInstance[0].AddRotate(0.0f, 0.1f, 0);
	porygonInstance[1].AddRotate(0.0f, 0.1f, 0);

	// TLAS�𐶐��B
	TLAS tlas;
	tlas.GenerateTLAS(L"TlasDescriptorHeap");


	// �O���[�o�����[�g�V�O�l�`����ݒ�B
	RayRootsignature globalRootSig;
	// �p�����[�^�[t0��TLAS(SRV)��ݒ�B
	globalRootSig.AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0);
	// �p�����[�^�[b0�ɃJ�����p�o�b�t�@��ݒ�B
	globalRootSig.AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 0);
	// �p�����[�^�[u0�ɏo�͗p�o�b�t�@��ݒ�B
	globalRootSig.AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0);

	// ���[�g�V�O�l�`���𐶐��B
	globalRootSig.Create(false, L"GlobalRootSig");


	// ClosestHit�V�F�[�_�[�p�̃��[�J�����[�g�V�O�l�`���𐶐��B
	RayRootsignature closestHitLocalRootSig;
	// t0�ɃC���f�b�N�X�o�b�t�@(SRV)��ݒ�B
	closestHitLocalRootSig.AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	// t1�ɒ��_�o�b�t�@(SRV)��ݒ�B
	closestHitLocalRootSig.AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	// t2�Ƀe�N�X�`����ݒ�B
	closestHitLocalRootSig.AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);
	// �T���v���[��ǉ��B
	closestHitLocalRootSig.AddStaticSampler(1);
	// ���[�J�����[�g�V�O�l�`���𐶐��B
	closestHitLocalRootSig.Create(true, L"ClosestHitLocalRootSig");

	// �V�F�[�_�[���R���p�C������B
	ShaderStorage::Instance()->LoadShaderForDXC("Resource/ShaderFiles/RayTracing/triangleShaderHeader.hlsl", "lib_6_4", "");


	/*==========  �X�e�[�g�I�u�W�F�N�g�̐���  ==========*/

	// �X�e�[�g�I�u�W�F�N�g�̐ݒ��ۑ����Ă����悤�̍\���́B
	CD3DX12_STATE_OBJECT_DESC subobjects;
	// �X�e�[�g�I�u�W�F�N�g�̏�Ԃ�ݒ�B
	subobjects.SetStateObjectType(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

	// �V�F�[�_�[�̏���ۑ�����p�̕ϐ��B
	D3D12_SHADER_BYTECODE shadercode = {};
	shadercode.BytecodeLength = ShaderStorage::Instance()->GetShaderBin("Resource/ShaderFiles/RayTracing/triangleShaderHeader.hlsl").size();
	shadercode.pShaderBytecode = ShaderStorage::Instance()->GetShaderBin("Resource/ShaderFiles/RayTracing/triangleShaderHeader.hlsl").data();

	// �V�F�[�_�[�̊e�֐����R�[�h�̓o�^�B
	auto dxilLib = subobjects.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
	dxilLib->SetDXILLibrary(&shadercode);
	dxilLib->DefineExport(L"mainRayGen");
	dxilLib->DefineExport(L"mainMS");
	dxilLib->DefineExport(L"shadowMS");
	dxilLib->DefineExport(L"mainCHS");
	dxilLib->DefineExport(L"shadowCHS");
	dxilLib->DefineExport(L"mainAnyHit");

	// �q�b�g�O���[�v�̐ݒ�B
	auto hitGroup = subobjects.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
	hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);
	hitGroup->SetClosestHitShaderImport(L"mainCHS");
	hitGroup->SetAnyHitShaderImport(L"mainAnyHit");
	hitGroup->SetHitGroupExport(hitGroupName);

	// �O���[�o�����[�g�V�O�l�`���̐ݒ�B
	auto rootSig = subobjects.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	rootSig->SetRootSignature(globalRootSig.GetRootSig().Get());

	// ���[�J�����[�g�V�O�l�`���̐ݒ�BClosestHit�V�F�[�_�[�B
	auto chLocalRootSig = subobjects.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
	chLocalRootSig->SetRootSignature(closestHitLocalRootSig.GetRootSig().Get());
	auto chAssocModel = subobjects.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
	chAssocModel->AddExport(hitGroupName);
	chAssocModel->SetSubobjectToAssociate(*chLocalRootSig);

	// �V�F�[�_�[�̐ݒ�B
	auto shaderConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	shaderConfig->Config(sizeof(XMFLOAT3) + sizeof(UINT), sizeof(XMFLOAT2));

	// �p�C�v���C���̐ݒ�B
	auto pipelineConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	pipelineConfig->Config(30);

	// �X�e�[�g�I�u�W�F�N�g
	ComPtr<ID3D12StateObject> stateObject;

	// ��������B
	HRESULT resultBuff = DirectXBase::Instance()->dev->CreateStateObject(
		subobjects, IID_PPV_ARGS(stateObject.ReleaseAndGetAddressOf())
	);

	stateObject->SetName(L"StateObject");

	if (FAILED(resultBuff)) {
		int a = 0;
	}


	/*==========  UAV�o�̓o�b�t�@�̏���  ==========*/

	// UAV��ݒ�
	ComPtr<ID3D12Resource> rayTracingOutput = surarin::CreateTexture2D(
		window_width, window_height, DXGI_FORMAT_R8G8B8A8_UNORM,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_HEAP_TYPE_DEFAULT
	);

	// �擪�n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Instance()->GetDescriptorHeap().Get()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Instance()->GetHead(), DirectXBase::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	// �f�B�X�N���v�^�q�[�v��UAV���m��
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	DirectXBase::Instance()->dev->CreateUnorderedAccessView(
		rayTracingOutput.Get(), nullptr, &uavDesc, basicHeapHandle);

	// UAV�̃f�B�X�N���v�^�q�[�v�̃C���f�b�N�X���擾
	int uavDescriptorIndex = DescriptorHeapMgr::Instance()->GetHead();

	// �f�B�X�N���v�^�q�[�v���C���N�������g
	DescriptorHeapMgr::Instance()->IncrementHead();

	rayTracingOutput->SetName(L"RayTracingOutputUAV");





	/*==========  ShaderTable�̐���  ==========*/			// �V�F�[�_�[�e�[�u���ł͎g�p���郍�[�J�����[�g�V�O�l�`�������ɃT�C�Y�����肷��B

	const auto ShaderRecordAlignment = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;

	// RayGeneration�V�F�[�_�[�ł̓��[�J�����[�g�V�O�l�`�����g�p�B
	UINT raygenRecordSize = 0;
	raygenRecordSize += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	raygenRecordSize = surarin::RoundUp(raygenRecordSize, ShaderRecordAlignment);

	// Miss�V�F�[�_�[�ł̓��[�J�����[�g�V�O�l�`�����g�p�B
	UINT missRecordSize = 0;
	missRecordSize += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	missRecordSize = surarin::RoundUp(missRecordSize, ShaderRecordAlignment);

	// �q�b�g�O���[�v�ł́AShaderIndentifer�ƃ��[�J�����[�g�V�O�l�`���ɂ��VB/IB(SRV)���g�p�B
	UINT hitgroupRecordSize = 0;
	hitgroupRecordSize += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	hitgroupRecordSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);	// ���_�o�b�t�@�r���[
	hitgroupRecordSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);	// �C���f�b�N�X�o�b�t�@�r���[
	hitgroupRecordSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);	// �e�N�X�`��
	hitgroupRecordSize = surarin::RoundUp(hitgroupRecordSize, ShaderRecordAlignment);

	// �g�p����e�V�F�[�_�[�̌����A�V�F�[�_�[�e�[�u���̃T�C�Y�����߂�B
	UINT hitgroupCount = 1;
	UINT raygenSize = 1 * raygenRecordSize;
	UINT missSize = 2 * missRecordSize;
	UINT hitGroupSize = hitgroupCount * hitgroupRecordSize;

	// �e�e�[�u���̊J�n�ʒu�ɃA���C�����g���񂪂���̂Œ�������B
	UINT tableAlign = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;
	UINT raygenRegion = surarin::RoundUp(raygenRecordSize, tableAlign);
	UINT missRegion = surarin::RoundUp(missSize, tableAlign);
	UINT hitgroupRegion = surarin::RoundUp(hitGroupSize, tableAlign);

	// �V�F�[�_�[�e�[�u���̃T�C�Y.
	UINT tableSize = raygenRegion + missRegion + hitgroupRegion;



	/*========== �V�F�[�_�[�e�[�u���̍\�z ==========*/

	// �V�F�[�_�[�e�[�u���m�ہB
	ComPtr<ID3D12Resource> shaderTable = surarin::CreateBuffer(
		tableSize, D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_HEAP_TYPE_UPLOAD,
		L"ShaderTable");

	ComPtr<ID3D12StateObjectProperties> rtsoProps;
	stateObject.As(&rtsoProps);

	// �e�V�F�[�_�[���R�[�h����������ł����B
	void* mapped = nullptr;
	shaderTable->Map(0, nullptr, &mapped);
	uint8_t* pStart = static_cast<uint8_t*>(mapped);

	// RayGeneration �p�̃V�F�[�_�[���R�[�h���������݁B
	uint8_t* rgsStart = pStart;
	{
		uint8_t* p = rgsStart;
		void* id = rtsoProps->GetShaderIdentifier(L"mainRayGen");
		p += surarin::WriteShaderIdentifier(p, id);

	}

	// Miss Shader �p�̃V�F�[�_�[���R�[�h���������݁B
	uint8_t* missStart = pStart + raygenRegion;
	{
		uint8_t* p = missStart;
		auto id = rtsoProps->GetShaderIdentifier(L"mainMS");
		p += surarin::WriteShaderIdentifier(p, id);
		// Shadow Miss Shader �p�̃V�F�[�_�[���R�[�h���������݁B
		id = rtsoProps->GetShaderIdentifier(L"shadowMS");
		p += surarin::WriteShaderIdentifier(p, id);
	}

	// Hit Group �p�̃V�F�[�_�[���R�[�h���������݁B
	uint8_t* hitgroupStart = pStart + raygenRegion + missRegion;
	{

		uint8_t* pRecord = hitgroupStart;
		// moneky�ɑΉ�����V�F�[�_�[���R�[�h����������
		pRecord = BLASRegister::Instance()->WriteShaderRecord(pRecord, boneBlas, hitgroupRecordSize, stateObject);
		// skydome �ɑΉ�����V�F�[�_�[���R�[�h����������
		pRecord = BLASRegister::Instance()->WriteShaderRecord(pRecord, monkeyBlas, hitgroupRecordSize, stateObject);
		// ground �ɑΉ�����V�F�[�_�[���R�[�h����������
		pRecord = BLASRegister::Instance()->WriteShaderRecord(pRecord, groundBlas, hitgroupRecordSize, stateObject);

	}
	shaderTable->Unmap(0, nullptr);


	/*==========  D3D12_DISPATCH_RAYS_DESC�̐ݒ�  ==========*/

	D3D12_DISPATCH_RAYS_DESC dispatchRayDesc = {};

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



	// ���̒萔�o�b�t�@��錾
	KariConstBufferData constBufferData;
	constBufferData.ambientColor = { 1,1,1,1 };
	constBufferData.lightColor = { 1,1,1,1 };
	constBufferData.lightDirection = { 1,1,0,0 };
	constBufferData.mtxProj = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),				//��p(60�x)
		(float)window_width / window_height,	//�A�X�y�N�g��
		0.1f, 1000000.0f							//�O�[�A���[
	);
	constBufferData.mtxProjInv = XMMatrixInverse(nullptr, constBufferData.mtxProj);
	Vec3 eye = { 0,0,-10 };
	Vec3 target = { 0,0,0 };
	Vec3 up = { 0,1,0 };
	constBufferData.mtxView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
	constBufferData.mtxViewInv = XMMatrixInverse(nullptr, constBufferData.mtxView);

	DynamicConstBuffer constBuff;
	constBuff.Generate(sizeof(KariConstBufferData), L"constBuffer");

	// �J�������������B
	Camera::Instance()->Init();


	/*----------�Q�[�����[�v----------*/
	while (true) {
		/*----------���t���[������(�`��O����)----------*/
		directXBase.processBeforeDrawing();



		/*----- �X�V���� -----*/

		// �r���[�s��𐶐��B
		Camera::Instance()->GenerateMatView();

		FPS();

		//Camera::target = triangle.GetPos();

		// �X�L�j���O�A�j���[�V����������B
		BLASRegister::Instance()->ComputeSkin(boneBlas);

		float speed = 0.1f;
		float rot = 0.05f;
		if (Input::isKey(DIK_W)) Camera::Instance()->Move(speed);
		if (Input::isKey(DIK_S)) Camera::Instance()->Move(-speed);
		if (Input::isKey(DIK_A)) Camera::Instance()->MoveRight(speed);
		if (Input::isKey(DIK_D)) Camera::Instance()->MoveRight(-speed);
		if (Input::isKey(DIK_UP)) Camera::Instance()->eye.y += speed;
		if (Input::isKey(DIK_DOWN)) Camera::Instance()->eye.y -= speed;

		if (Input::isKey(DIK_I)) porygonInstance[0].AddTrans(0.0f, 0.0f, -0.1f);
		if (Input::isKey(DIK_K)) porygonInstance[0].AddTrans(0.0f, 0.0f, 0.1f);
		if (Input::isKey(DIK_J)) porygonInstance[0].AddTrans(0.1f, 0.0f, 0.0f);
		if (Input::isKey(DIK_L)) porygonInstance[0].AddTrans(-0.1f, 0.0f, 0.0f);

		if (Input::isKey(DIK_1)) {

			BLASRegister::Instance()->InitAnimation(boneBlas);

		}
		if (Input::isKey(DIK_2)) {

			BLASRegister::Instance()->PlayAnimation(boneBlas);

		}
		if (Input::isKey(DIK_3)) {

			BLASRegister::Instance()->StopAnimation(boneBlas);

		}

		BLASRegister::Instance()->Update(boneBlas);

		// TLAS���X�V�B
		tlas.Update();

		eye = Camera::Instance()->eye;
		target = Camera::Instance()->target;
		up = Camera::Instance()->up;

		// �J�������X�V�B
		Camera::Instance()->Update();


		/*----- �`�揈�� -----*/

		// ��ʂɕ\������郌���_�[�^�[�Q�b�g�ɖ߂��B
		//DirectXBase::Instance()->SetRenderTarget();

		auto frameIndex = DirectXBase::Instance()->swapchain->GetCurrentBackBufferIndex();
		constBufferData.mtxView = XMMatrixLookAtLH(eye.ConvertXMVECTOR(), target.ConvertXMVECTOR(), up.ConvertXMVECTOR());
		constBufferData.mtxViewInv = XMMatrixInverse(nullptr, constBufferData.mtxView);
		// �萔�o�b�t�@�̒��g���X�V����B
		constBuff.Write(frameIndex, &constBufferData, sizeof(KariConstBufferData));
		auto sceneConstantBuffer = constBuff.GetBuffer(frameIndex);

		// �O���[�o�����[�g�V�O�l�`���Ŏg���Ɛ錾���Ă��郊�\�[�X����Z�b�g�B
		ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Instance()->GetDescriptorHeap().Get() };
		DirectXBase::Instance()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		DirectXBase::Instance()->cmdList->SetComputeRootSignature(globalRootSig.GetRootSig().Get());
		DirectXBase::Instance()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Instance()->GetGPUHandleIncrement(tlas.GetDescriptorHeapIndex()));
		DirectXBase::Instance()->cmdList->SetComputeRootDescriptorTable(2, DescriptorHeapMgr::Instance()->GetGPUHandleIncrement(uavDescriptorIndex));
		DirectXBase::Instance()->cmdList->SetComputeRootConstantBufferView(1, sceneConstantBuffer->GetGPUVirtualAddress());


		// ���C�g���[�V���O���ʃo�b�t�@��UAV��Ԃ�
		auto barrierToUAV = CD3DX12_RESOURCE_BARRIER::Transition(
			rayTracingOutput.Get(),
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);
		DirectXBase::Instance()->cmdList->ResourceBarrier(1, &barrierToUAV);

		DirectXBase::Instance()->cmdList->SetPipelineState1(stateObject.Get());

		DirectXBase::Instance()->cmdList->DispatchRays(&dispatchRayDesc);

		// �o�b�N�o�b�t�@�̃C���f�b�N�X���擾����B
		UINT backBufferIndex = DirectXBase::Instance()->swapchain->GetCurrentBackBufferIndex();

		// �o���A��ݒ肵�e���\�[�X�̏�Ԃ�J�ڂ�����.
		D3D12_RESOURCE_BARRIER barriers[] = {
		CD3DX12_RESOURCE_BARRIER::Transition(
		rayTracingOutput.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE),
		CD3DX12_RESOURCE_BARRIER::Transition(
		DirectXBase::Instance()->backBuffers[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_COPY_DEST),
		};
		DirectXBase::Instance()->cmdList->ResourceBarrier(_countof(barriers), barriers);
		DirectXBase::Instance()->cmdList->CopyResource(DirectXBase::Instance()->backBuffers[backBufferIndex].Get(), rayTracingOutput.Get());

		// �����_�[�^�[�Q�b�g�̃��\�[�X�o���A�����Ƃɖ߂��B
		D3D12_RESOURCE_BARRIER endBarriers[] = {

		CD3DX12_RESOURCE_BARRIER::Transition(
		DirectXBase::Instance()->backBuffers[backBufferIndex].Get(),
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