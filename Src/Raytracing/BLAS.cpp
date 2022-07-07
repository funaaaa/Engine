#include "BLAS.h"
#include "ModelDataManager.h"
#include "DescriptorHeapMgr.h"
#include <DirectXMath.h>
#include "FbxLoader.h"
#include "RWStructuredBuffer.h"
#include "StructuredBuffer.h"
#include "TextureManager.h"

void BLAS::GenerateBLASObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath, const bool& IsSmoothing, const bool& IsOpaque)
{

	/*===== BLAS�𐶐����鏈�� =====*/

	// �p�X��ۑ��B
	texturePath = TexturePath;
	modelPath = DirectryPath + ModelName;

	// �e�N�X�`����ǂݍ��ށB
	const int TEXTURE_PATH_COUNT = static_cast<int>(TexturePath.size());
	for (auto& index : TexturePath) {

		textureHandle.emplace_back(TextureManager::Ins()->LoadTexture(index));

	}


	/*-- �`��f�[�^��ǂݍ��� --*/

	// �ǂݍ��񂾃f�[�^���ꎞ�ۑ�����p�̃o�b�t�@�B
	ModelDataManager::ObjectData dataBuff;

	// ���f�������[�h�B
	ModelDataManager::Ins()->LoadObj(DirectryPath, ModelName, dataBuff, IsSmoothing);

	// �}�e���A������ۑ��B
	material = dataBuff.material;

	// �}�e���A���p�萔�o�b�t�@�𐶐��B
	materialBuffer = CreateBuffer(
		static_cast<size_t>(sizeof(ModelDataManager::Material)),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// �m�ۂ����o�b�t�@�Ƀ}�e���A���f�[�^���������ށB
	WriteToMemory(materialBuffer, &material, static_cast<size_t>(sizeof(ModelDataManager::Material)));

	// �}�e���A���f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> materialDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	materialDescriptor.CreateStructuredSRV(materialBuffer, 1, 0, sizeof(ModelDataManager::Material), materialDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// ���_�������߂�B
	vertexCount = static_cast<UINT>(dataBuff.vertex.size());

	// ���_�C���f�b�N�X�������߂�B
	indexCount = static_cast<UINT>(dataBuff.index.size());

	// ���_�f�[�^��ϊ��B
	for (int index = 0; index < static_cast<int>(vertexCount); ++index) {

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
		static_cast<size_t>(vertexStride * vertexCount),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// �m�ۂ����o�b�t�@�ɒ��_�f�[�^���������ށB
	WriteToMemory(vertexBuffer, vertex.data(), static_cast<size_t>(vertexStride * vertexCount));

	// ���_�C���f�b�N�X�o�b�t�@�𐶐�����B
	indexBuffer = CreateBuffer(
		static_cast<size_t>(indexStride * indexCount),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// �m�ۂ����C���f�b�N�X�o�b�t�@�ɒ��_�C���f�b�N�X�f�[�^���������ށB
	WriteToMemory(indexBuffer, vertIndex.data(), static_cast<size_t>(indexStride * indexCount));

	// ���_�C���f�b�N�X�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	indexDescriptor.CreateStructuredSRV(indexBuffer, indexCount, 0, indexStride, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// ���_�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> vertexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	vertexDescriptor.CreateStructuredSRV(vertexBuffer, vertexCount, 0, vertexStride, vertexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();


	/*-- BLAS�o�b�t�@�𐶐����� --*/

	// �`���ݒ肷��p�̍\���̂�ݒ�B
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(IsOpaque);
	isOpaque = IsOpaque;

	// BLAS�o�b�t�@��ݒ�A�\�z����B
	SettingAccelerationStructure(geomDesc);


	// �q�b�g�O���[�v����ۑ�����B
	this->hitGroupName = HitGroupName;

	// �f�o�b�O�Ŏg�p���钸�_�݂̂̃f�[�^�Ɩ@���݂̂̃f�[�^�𐶐�����B
	vertexPos.resize(static_cast<unsigned __int64>(vertex.size()));
	vertexNormal.resize(static_cast<unsigned __int64>(vertex.size()));
	int counter = 0;
	for (auto& index : vertex) {
		vertexPos[counter] = index.position;
		vertexNormal[counter] = index.normal;
		++counter;
	}

}

void BLAS::GenerateBLASFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath)
{

	/*===== BLAS�𐶐����鏈�� =====*/

	// �e�N�X�`����ǂݍ��ށB
	const int TEXTURE_PATH_COUNT = static_cast<int>(TexturePath.size());
	for (int index = 0; index < TEXTURE_PATH_COUNT; ++index) {

		textureHandle.emplace_back(TextureManager::Ins()->LoadTexture(TexturePath[index]));

	}

	/*-- �`��f�[�^��ǂݍ��� --*/

	// �ǂݍ��񂾃f�[�^���ꎞ�ۑ�����p�̃o�b�t�@�B
	std::vector<FbxLoader::Vertex> modelVertexData;
	std::vector<UINT> modelIndexData;

	// ���f�������[�h�B
	modelIndex = FbxLoader::Ins()->LoadModelFromFile(DirectryPath, ModelName);

	FbxLoader::Ins()->GetFbxData(modelIndex, modelVertexData, modelIndexData);

	// �}�e���A���p�萔�o�b�t�@�𐶐��B
	materialBuffer = CreateBuffer(
		static_cast<size_t>(sizeof(ModelDataManager::Material)),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// �m�ۂ����o�b�t�@�Ƀ}�e���A���f�[�^���������ށB
	WriteToMemory(materialBuffer, &material, static_cast<size_t>(sizeof(ModelDataManager::Material)));

	// �}�e���A���f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> materialDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	materialDescriptor.CreateStructuredSRV(materialBuffer, 1, 0, sizeof(ModelDataManager::Material), materialDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// ���_�������߂�B
	vertexCount = static_cast<UINT>(modelVertexData.size());

	// ���_�C���f�b�N�X�������߂�B
	indexCount = static_cast<UINT>(modelIndexData.size());

	// ���_�f�[�^��ϊ��B
	for (int index = 0; index < static_cast<int>(vertexCount); ++index) {

		RayVertex buff{};
		buff.normal = modelVertexData[index].normal;
		buff.position = modelVertexData[index].pos;
		buff.uv = modelVertexData[index].uv;

		// �f�[�^��ۑ��B
		vertex.push_back(buff);

	}

	// ���_�C���f�b�N�X�f�[�^��ۑ��B
	vertIndex = modelIndexData;

	// ���_�T�C�Y�����߂�B
	vertexStride = sizeof(RayVertex);

	// ���_�C���f�b�N�X�T�C�Y�����߂�B
	indexStride = sizeof(UINT);

	// ���_�o�b�t�@�𐶐�����B
	vertexBuffer = CreateBuffer(
		static_cast<size_t>(vertexStride * vertexCount),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// �m�ۂ����o�b�t�@�ɒ��_�f�[�^���������ށB
	WriteToMemory(vertexBuffer, vertex.data(), static_cast<size_t>(vertexStride * vertexCount));

	// ���_�C���f�b�N�X�o�b�t�@�𐶐�����B
	indexBuffer = CreateBuffer(
		static_cast<size_t>(indexStride * indexCount),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// �m�ۂ����C���f�b�N�X�o�b�t�@�ɒ��_�C���f�b�N�X�f�[�^���������ށB
	WriteToMemory(indexBuffer, vertIndex.data(), static_cast<size_t>(indexStride * indexCount));

	// ���_�C���f�b�N�X�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	indexDescriptor.CreateStructuredSRV(indexBuffer, indexCount, 0, indexStride, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// ���_�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> vertexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	vertexDescriptor.CreateStructuredSRV(vertexBuffer, vertexCount, 0, vertexStride, vertexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();


	/*-- BLAS�o�b�t�@�𐶐����� --*/

	// �`���ݒ肷��p�̍\���̂�ݒ�B
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(true);
	isOpaque = true;

	// BLAS�o�b�t�@��ݒ�A�\�z����B
	SettingAccelerationStructure(geomDesc);

	// �q�b�g�O���[�v����ۑ�����B
	this->hitGroupName = HitGroupName;

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Ins()->GetFbxModel(modelIndex).hasAnimation) {

		// ���͗p�\���̂����T�C�Y�B
		skinComputeInput.resize(vertex.size());

		// �X�L�j���O�A�j���[�V�����R���s���[�g�V�F�[�_�[�Ŏg�p������͗p�\���̂��Z�b�g�B
		FbxLoader::Ins()->GetSkinComputeInput(modelIndex, skinComputeInput);

		// �X�L�j���O�A�j���[�V�����Ŏg�p����R���s���[�g�V�F�[�_�[�����[�h���Ă����B
		skinComput.Init(L"resource/ShaderFiles/RayTracing/ComputeSkin.hlsl", sizeof(FbxLoader::SkinComputeInput), static_cast<int>(skinComputeInput.size()), skinComputeInput.data(), sizeof(RayVertex), static_cast<int>(vertex.size()), vertex.data());

	}

	// �f�o�b�O�Ŏg�p���钸�_�݂̂̃f�[�^�Ɩ@���݂̂̃f�[�^�𐶐�����B
	vertexPos.resize(static_cast<unsigned __int64>(vertex.size()));
	vertexNormal.resize(static_cast<unsigned __int64>(vertex.size()));
	int counter = 0;
	for (auto& index : vertex) {
		vertexPos[counter] = index.position;
		vertexNormal[counter] = index.normal;
		++counter;
	}

}

void BLAS::GenerateBLASData(ModelDataManager::ObjectData Data, const std::wstring& HitGroupName, std::vector<int> TextureHandle, const bool& IsOpaque)
{

	/*===== BLAS�𐶐����鏈�� =====*/

	// �e�N�X�`����ǂݍ��ށB
	textureHandle = TextureHandle;

	/*-- �`��f�[�^��ǂݍ��� --*/

	// �}�e���A���p�萔�o�b�t�@�𐶐��B
	materialBuffer = CreateBuffer(
		static_cast<size_t>(sizeof(ModelDataManager::Material)),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// �m�ۂ����o�b�t�@�Ƀ}�e���A���f�[�^���������ށB
	WriteToMemory(materialBuffer, &material, static_cast<size_t>(sizeof(ModelDataManager::Material)));

	// �}�e���A���f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> materialDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	materialDescriptor.CreateStructuredSRV(materialBuffer, 1, 0, sizeof(ModelDataManager::Material), materialDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// ���_�������߂�B
	vertexCount = static_cast<UINT>(Data.vertex.size());

	// ���_�C���f�b�N�X�������߂�B
	indexCount = static_cast<UINT>(Data.index.size());

	// ���_�f�[�^��ϊ��B
	for (int index = 0; index < static_cast<int>(vertexCount); ++index) {

		RayVertex buff{};
		buff.normal = Data.vertex[index].normal;
		buff.position = Data.vertex[index].pos;
		buff.uv = Data.vertex[index].uv;

		// �f�[�^��ۑ��B
		vertex.push_back(buff);

	}

	// ���_�C���f�b�N�X�f�[�^��ۑ��B
	vertIndex = Data.index;

	// ���_�T�C�Y�����߂�B
	vertexStride = sizeof(RayVertex);

	// ���_�C���f�b�N�X�T�C�Y�����߂�B
	indexStride = sizeof(UINT);

	// ���_�o�b�t�@�𐶐�����B
	vertexBuffer = CreateBuffer(
		static_cast<size_t>(vertexStride * vertexCount),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// �m�ۂ����o�b�t�@�ɒ��_�f�[�^���������ށB
	WriteToMemory(vertexBuffer, vertex.data(), static_cast<size_t>(vertexStride * vertexCount));

	// ���_�C���f�b�N�X�o�b�t�@�𐶐�����B
	indexBuffer = CreateBuffer(
		static_cast<size_t>(indexStride * indexCount),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// �m�ۂ����C���f�b�N�X�o�b�t�@�ɒ��_�C���f�b�N�X�f�[�^���������ށB
	WriteToMemory(indexBuffer, vertIndex.data(), static_cast<size_t>(indexStride * indexCount));

	// ���_�C���f�b�N�X�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	indexDescriptor.CreateStructuredSRV(indexBuffer, indexCount, 0, indexStride, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// ���_�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> vertexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	vertexDescriptor.CreateStructuredSRV(vertexBuffer, vertexCount, 0, vertexStride, vertexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();


	/*-- BLAS�o�b�t�@�𐶐����� --*/

	// �`���ݒ肷��p�̍\���̂�ݒ�B
	isOpaque = IsOpaque;
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(isOpaque);

	// BLAS�o�b�t�@��ݒ�A�\�z����B
	SettingAccelerationStructure(geomDesc);


	// �q�b�g�O���[�v����ۑ�����B
	this->hitGroupName = HitGroupName;

	// �f�o�b�O�Ŏg�p���钸�_�݂̂̃f�[�^�Ɩ@���݂̂̃f�[�^�𐶐�����B
	vertexPos.resize(static_cast<unsigned __int64>(vertex.size()));
	vertexNormal.resize(static_cast<unsigned __int64>(vertex.size()));
	int counter = 0;
	for (auto& index : vertex) {
		vertexPos[counter] = index.position;
		vertexNormal[counter] = index.normal;
		++counter;
	}

}

void BLAS::Update()
{

	/*===== BLAS�̍X�V =====*/

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Ins()->GetFbxModel(modelIndex).hasAnimation) {

		// �A�j���[�V�����̍X�V����
		auto& model = FbxLoader::Ins()->GetFbxModel(modelIndex);
		if (model.isPlay) {

			model.currentTime += model.frameTime;

			// �Ō�܂ōĐ�������擪�ɖ߂��B
			if (model.endTime < model.currentTime) {

				model.currentTime = model.startTime;

			}

			// �X�L�j���O�A�j���[�V�����R���s���[�g�V�F�[�_�[�Ŏg�p������͗p�\���̂��Z�b�g�B
			FbxLoader::Ins()->GetSkinComputeInput(modelIndex, skinComputeInput);

		}

	};

	// ���_���������ށB ���̂Ƃ���͒��_��������������\��͂Ȃ����A��X���̂���������ށB�_�[�e�B�t���O�݂����Ȃ̂�p�ӂ������B
	WriteToMemory(vertexBuffer, vertex.data(), static_cast<size_t>(vertexStride * vertexCount));

	// �X�V�̂��߂̒l��ݒ�B
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(isOpaque);
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
	DirectXBase::Ins()->cmdList->BuildRaytracingAccelerationStructure(
		&asDesc, 0, nullptr
	);

	CreateAccelerationStructure();

}

void BLAS::ComputeSkin()
{

	/*===== ���_�f�[�^���X�L�j���O�s������ɏ��������鏈�� =====*/

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Ins()->GetFbxModel(modelIndex).hasAnimation) {

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
	if (FbxLoader::Ins()->GetFbxModel(modelIndex).hasAnimation) {

		FbxLoader::Ins()->GetFbxModel(modelIndex).InitAnimation();

	}
}

void BLAS::PlayAnimation()
{

	/*===== �A�j���[�V���������� =====*/

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Ins()->GetFbxModel(modelIndex).hasAnimation) {

		FbxLoader::Ins()->GetFbxModel(modelIndex).PlayAnimation();

	}

}

void BLAS::StopAnimation()
{

	/*===== �A�j���[�V�������~������ =====*/

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Ins()->GetFbxModel(modelIndex).hasAnimation) {

		FbxLoader::Ins()->GetFbxModel(modelIndex).StopAnimation();

	}

}
#include "HitGroupMgr.h"
#include <assert.h>
uint8_t* BLAS::WriteShaderRecord(uint8_t* Dst, UINT recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName)
{

	/*===== �V�F�[�_�[���R�[�h���������� =====*/

	Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> rtsoProps;
	StateObject.As(&rtsoProps);
	auto entryBegin = Dst;
	auto shader = GetHitGroupName();

	// �ۑ�����Ă���q�b�g�O���[�v���ƈ���Ă����珑�����܂Ȃ��B
	//if (HitGroupName == shader) {

	auto id = rtsoProps->GetShaderIdentifier(shader.c_str());
	if (id == nullptr) {
		throw std::logic_error("Not found ShaderIdentifier");
	}

	// �V�F�[�_�[���ʎq���������ށB
	memcpy(Dst, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	Dst += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

	// ����̃v���O�����ł͈ȉ��̏����Ńf�B�X�N���v�^���L�^�B
	// [0] : �C���f�b�N�X�o�b�t�@
	// [1] : ���_�o�b�t�@
	// �� ���[�J�����[�g�V�O�l�`���̏����ɍ��킹��K�v������B
	Dst += WriteGPUDescriptor(Dst, &indexDescriptor.GetGPUHandle());
	Dst += WriteGPUDescriptor(Dst, &vertexDescriptor.GetGPUHandle());

	// �}�e���A���p�̃o�b�t�@���Z�b�g�B
	Dst += WriteGPUDescriptor(Dst, &materialDescriptor.GetGPUHandle());

	// �q�b�g�O���[�v������q�b�g�O���[�v��ID���擾����B
	int hitGroupID = HitGroupMgr::Ins()->GetHitGroupID(HitGroupName);

	// ���_�A�C���f�b�N�X�A�}�e���A���̃I�t�Z�b�g
	const int OFFSET_VERTEX_INDEX_MATERIAL = 3;

	// �q�b�g�O���[�vID����SRV�̐����擾�B
	int srvCount = HitGroupMgr::Ins()->GetHitGroupUAVCount(hitGroupID) + HitGroupMgr::Ins()->GetHitGroupSRVCount(hitGroupID) - OFFSET_VERTEX_INDEX_MATERIAL;

	// �����̓e�N�X�`���̃T�C�Y�ł͂Ȃ��A�p�C�v���C���ɃZ�b�g���ꂽSRV�̐��������Ă��Ă�����g���B
	// ���̎��_��SRV�̐��ƃe�N�X�`���̐��������Ă��Ȃ�������assert���o���B
	for (int index = 0; index < srvCount; ++index) {

		// ���̃C���f�b�N�X�̃e�N�X�`�������݂��Ă��Ȃ�������
		if (textureHandle.size() <= index) {

			// ��������ɃY���������Ă��܂��̂Ő擪�̃e�N�X�`�����������ށB
			CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(textureHandle[0]);
			WriteGPUDescriptor(Dst, &texDescHandle);

		}
		else {

			CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(textureHandle[index]);
			Dst += WriteGPUDescriptor(Dst, &texDescHandle);

		}
	}

	Dst = entryBegin + recordSize;
	return Dst;

	//}


	// �q�b�g�O���[�v�������������Ƃ��̏����B���͈���������̂ŏ����B
	//else {

	//	// �V�F�[�_�[���ʎq���������ށB
	//	auto idBuff = rtsoProps->GetShaderIdentifier(HitGroupName);
	//	memcpy(Dst, idBuff, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	//	Dst += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

	//	// ����̃v���O�����ł͈ȉ��̏����Ńf�B�X�N���v�^���L�^�B
	//	// [0] : �C���f�b�N�X�o�b�t�@
	//	// [1] : ���_�o�b�t�@
	//	// �� ���[�J�����[�g�V�O�l�`���̏����ɍ��킹��K�v������B
	//	Dst += WriteGPUDescriptor(Dst, &indexDescriptor.GetGPUHandle());
	//	Dst += WriteGPUDescriptor(Dst, &vertexDescriptor.GetGPUHandle());

	//	// �}�e���A���p�̃o�b�t�@���Z�b�g�B
	//	Dst += WriteGPUDescriptor(Dst, &materialDescriptor.GetGPUHandle());

	//	// �q�b�g�O���[�v������q�b�g�O���[�v��ID���擾����B
	//	int hitGroupID = HitGroupMgr::Ins()->GetHitGroupID(HitGroupName);

	//	// ���_�A�C���f�b�N�X�A�}�e���A���̃I�t�Z�b�g
	//	const int OFFSET_VERTEX_INDEX_MATERIAL = 3;

	//	// �q�b�g�O���[�vID����SRV�̐����擾�B
	//	int srvCount = HitGroupMgr::Ins()->GetHitGroupUAVCount(hitGroupID) + HitGroupMgr::Ins()->GetHitGroupSRVCount(hitGroupID) - OFFSET_VERTEX_INDEX_MATERIAL;

	//	// �����̓e�N�X�`���̃T�C�Y�ł͂Ȃ��A�p�C�v���C���ɃZ�b�g���ꂽSRV�̐��������Ă��Ă�����g���B
	//	// ���̎��_��SRV�̐��ƃe�N�X�`���̐��������Ă��Ȃ�������assert���o���B
	//	for (int index = 0; index < srvCount; ++index) {

	//		// ���̃C���f�b�N�X�̃e�N�X�`�������݂��Ă��Ȃ�������
	//		if (textureHandle.size() <= index) {

	//			// ��������ɃY���������Ă��܂��̂Ő擪�̃e�N�X�`�����������ށB
	//			CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(textureHandle[0]);
	//			WriteGPUDescriptor(Dst, &texHandle);

	//		}
	//		else {

	//			CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(textureHandle[index]);
	//			Dst += WriteGPUDescriptor(Dst, &texHandle);

	//		}
	//	}

	//	Dst = entryBegin + recordSize;
	//	return Dst;

	//}

}

void BLAS::WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* pData, size_t DataSize)
{

	/*===== �������ɒl���������ޏ��� =====*/

	// null�`�F�b�N�B
	if (Resource == nullptr) return;

	// �}�b�v�������s���B
	void* mapped = nullptr;
	D3D12_RANGE range{ 0, DataSize };
	HRESULT hr = Resource->Map(0, &range, (void**)&mapped);

	// �}�b�v������������l���������ށB
	if (SUCCEEDED(hr)) {

		memcpy(mapped, pData, DataSize);
		Resource->Unmap(0, nullptr);

	}

}

Microsoft::WRL::ComPtr<ID3D12Resource> BLAS::CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType)
{

	/*===== �o�b�t�@�S�ʂ𐶐����鏈�� =====*/

	// ��������ݒ�p�\���̂�ݒ肷��B
	D3D12_HEAP_PROPERTIES heapProps{};
	if (HeapType == D3D12_HEAP_TYPE_DEFAULT) {
		heapProps = D3D12_HEAP_PROPERTIES{
		D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
		};
	}
	if (HeapType == D3D12_HEAP_TYPE_UPLOAD) {
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
	resDesc.Width = Size;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc = { 1, 0 };
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = Flags;

	// �o�b�t�@�������߂��o���B
	hr = DirectXBase::Ins()->dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		InitialState,
		nullptr,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
	);

	// �����Ɏ��s������B
	if (FAILED(hr)) {
		OutputDebugStringA("CreateBuffer failed.\n");
	}

	return resource;

}

D3D12_RAYTRACING_GEOMETRY_DESC BLAS::GetGeometryDesc(const bool& IsOpaque)
{

	/*===== BLAS�������ɐݒ���擾����p�֐� =====*/

	// �`��f�[�^�̃t���O��ݒ�B
	auto geometryDesc = D3D12_RAYTRACING_GEOMETRY_DESC{};
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	// �s�����t���O�B�����ŕs�����ɂ���ƃV�F�[�_�[����AnyHitShader��On�ɂ��Ă��Ă��Ă΂�Ȃ��Ȃ�炵���I
	if (IsOpaque) {
		geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	}
	else {
		geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION;
	}

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

void BLAS::SettingAccelerationStructure(const D3D12_RAYTRACING_GEOMETRY_DESC& GeomDesc)
{

	/*===== �����\���̂̐ݒ�p�֐� =====*/

	// AS(���C�Ƃ̔����v���ɍs�����߂ɕK�v�ȃ|���S���f�[�^�\����)�̐ݒ�ɕK�v�Ȋe���ڂ�ݒ�B
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc{};
	auto& inputs = buildASDesc.Inputs;	// D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	inputs.NumDescs = 1;
	inputs.pGeometryDescs = &GeomDesc;

	// �֐����g���ĕK�v�ȃ������ʂ����߂�.
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO blasPrebuild{};
	DirectXBase::Ins()->dev->GetRaytracingAccelerationStructurePrebuildInfo(
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
	DirectXBase::Ins()->cmdList->BuildRaytracingAccelerationStructure(
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
	DirectXBase::Ins()->cmdList->ResourceBarrier(1, &uavBarrier);
	DirectXBase::Ins()->cmdList->Close();

	// BLAS���\�z�B
	//ID3D12CommandList* pCmdList[] = { DirectXBase::Ins()->cmdList.Get() };
	// �\�z�p�֐����ĂԁB
	ID3D12CommandList* commandLists[] = { DirectXBase::Ins()->cmdList.Get() };
	DirectXBase::Ins()->cmdQueue->ExecuteCommandLists(1, commandLists);

	// �O���t�B�b�N�R�}���h���X�g�̊����҂�
	DirectXBase::Ins()->cmdQueue->Signal(DirectXBase::Ins()->fence.Get(), ++DirectXBase::Ins()->fenceVal);
	if (DirectXBase::Ins()->fence->GetCompletedValue() != DirectXBase::Ins()->fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		DirectXBase::Ins()->fence->SetEventOnCompletion(DirectXBase::Ins()->fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// �R�}���h�A���P�[�^�̃��Z�b�g
	DirectXBase::Ins()->cmdAllocator->Reset();	//�L���[���N���A

	// �R�}���h���X�g�̃��Z�b�g
	DirectXBase::Ins()->cmdList->Reset(DirectXBase::Ins()->cmdAllocator.Get(), nullptr);//�ĂуR�}���h���X�g�𒙂߂鏀��

}