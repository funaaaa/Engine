#include "BLAS.h"
#include "ModelDataManager.h"
#include "DescriptorHeapMgr.h"
#include <DirectXMath.h>
#include "FbxLoader.h"
#include "RWStructuredBuffer.h"
#include "StructuredBuffer.h"
#include "TextureManager.h"
#include "FHelper.h"


void BLAS::GenerateBLASObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, int BlasIndex, bool IsOpaque)
{

	/*===== BLAS�𐶐����鏈�� =====*/

	// �p�X��ۑ��B
	modelPath_ = DirectryPath + ModelName;

	// Blas��Index��ۑ��B
	blasIndex_ = BlasIndex;


	/*-- �`��f�[�^��ǂݍ��� --*/

	// �ǂݍ��񂾃f�[�^���ꎞ�ۑ�����p�̃o�b�t�@�B
	ModelDataManager::ObjectData dataBuff;

	// ���f�������[�h�B
	ModelDataManager::Ins()->LoadObj(DirectryPath, ModelName, dataBuff, false);

	// �e�����̒l��ۑ��B
	vertexMax_ = dataBuff.vertexMax_;
	vertexMin_ = dataBuff.vertexMin_;

	// �}�e���A������ۑ��B
	material_.baseColor_ = dataBuff.material_.baseColor_;
	material_.mapParam_ = dataBuff.material_.mapParam_;
	material_.roughness_ = dataBuff.material_.roughness_;
	material_.metalness_ = dataBuff.material_.metalness_;
	material_.specular_ = dataBuff.material_.specular_;

	// �e�N�X�`����ۑ��B
	baseTextureHandle_ = dataBuff.textureHandle_;

	// �}�e���A���o�b�t�@�𐶐�����B
	materialBuffer_ = CreateBuffer(
		static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);
	materialBuffer_->SetName(L"MaterialBuffer");

	// �A�b�v���[�h�p�}�e���A���o�b�t�@�𐶐�����B
	materialUploadBuffer_ = CreateBuffer(
		static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	materialUploadBuffer_->SetName(L"MaterialUploadBuffer");

	// �m�ۂ����o�b�t�@�Ƀ}�e���A���f�[�^���������ށB
	WriteToMemory(materialUploadBuffer_, &material_, static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)));

	Engine::Ins()->mainGraphicsCmdList_->CopyResource(materialBuffer_.Get(), materialUploadBuffer_.Get());
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(materialBuffer_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	// �}�e���A���f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> materialDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	materialDescriptor_.CreateStructuredSRV(materialBuffer_, 1, 0, sizeof(ModelDataManager::GPUMaterial), materialDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// ���_�������߂�B
	vertexCount_ = static_cast<UINT>(dataBuff.vertex_.size());

	// ���_�C���f�b�N�X�������߂�B
	indexCount_ = static_cast<UINT>(dataBuff.index_.size());

	// ���_�f�[�^��ϊ��B
	for (int index = 0; index < static_cast<int>(vertexCount_); ++index) {

		RayVertex buff{};
		buff.normal_ = dataBuff.vertex_[index].normal_;
		buff.position_ = dataBuff.vertex_[index].pos_;
		buff.uv_ = dataBuff.vertex_[index].uv_;
		buff.subUV_ = dataBuff.vertex_[index].uv_;

		// �f�[�^��ۑ��B
		vertex_.push_back(buff);

	}

	// ���_�C���f�b�N�X�f�[�^��ۑ��B
	vertIndex_ = dataBuff.index_;

	// ���_�T�C�Y�����߂�B
	vertexStride_ = sizeof(RayVertex);

	// ���_�C���f�b�N�X�T�C�Y�����߂�B
	indexStride_ = sizeof(UINT);

	// ���_�o�b�t�@�𐶐�����B
	vertexBuffer_ = CreateBuffer(
		static_cast<size_t>(vertexStride_ * vertexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);
	vertexBuffer_->SetName(L"VertexBuffer");

	// �A�b�v���[�h�p���_�o�b�t�@�𐶐�����B
	vertexUploadBuffer_ = CreateBuffer(
		static_cast<size_t>(vertexStride_ * vertexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	vertexUploadBuffer_->SetName(L"VertexUploadBuffer");

	// �m�ۂ����o�b�t�@�ɒ��_�f�[�^���������ށB
	WriteToMemory(vertexUploadBuffer_, vertex_.data(), static_cast<size_t>(vertexStride_ * vertexCount_));

	Engine::Ins()->mainGraphicsCmdList_->CopyResource(vertexBuffer_.Get(), vertexUploadBuffer_.Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);


	// ���_�C���f�b�N�X�o�b�t�@�𐶐�����B
	indexBuffer_ = CreateBuffer(
		static_cast<size_t>(indexStride_ * indexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);
	indexBuffer_->SetName(L"IndexBuffer");

	// �A�b�v���[�h�p���_�C���f�b�N�X�o�b�t�@�𐶐�����B
	indexUploadBuffer_ = CreateBuffer(
		static_cast<size_t>(indexStride_ * indexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	indexUploadBuffer_->SetName(L"IndexUploadBuffer");

	// �m�ۂ����C���f�b�N�X�o�b�t�@�ɒ��_�C���f�b�N�X�f�[�^���������ށB
	WriteToMemory(indexUploadBuffer_, vertIndex_.data(), static_cast<size_t>(indexStride_ * indexCount_));

	Engine::Ins()->mainGraphicsCmdList_->CopyResource(indexBuffer_.Get(), indexUploadBuffer_.Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(indexBuffer_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	// ���_�C���f�b�N�X�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	indexDescriptor_.CreateStructuredSRV(indexBuffer_, indexCount_, 0, indexStride_, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// ���_�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> vertexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	vertexDescriptor_.CreateStructuredSRV(vertexBuffer_, vertexCount_, 0, vertexStride_, vertexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();


	/*-- BLAS�o�b�t�@�𐶐����� --*/

	// �`���ݒ肷��p�̍\���̂�ݒ�B
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(IsOpaque);
	isOpaque_ = IsOpaque;

	// BLAS�o�b�t�@��ݒ�A�\�z����B
	SettingAccelerationStructure(geomDesc);


	// �q�b�g�O���[�v����ۑ�����B
	this->hitGroupName_ = HitGroupName;

	// �f�o�b�O�Ŏg�p���钸�_�݂̂̃f�[�^�Ɩ@���݂̂̃f�[�^�𐶐�����B
	vertexPos_.resize(static_cast<unsigned __int64>(vertex_.size()));
	vertexNormal_.resize(static_cast<unsigned __int64>(vertex_.size()));
	vertexUV_.resize(static_cast<unsigned __int64>(vertex_.size()));
	int counter = 0;
	for (auto& index_ : vertex_) {
		vertexPos_[counter] = index_.position_;
		vertexNormal_[counter] = index_.normal_;
		vertexUV_[counter] = index_.uv_;
		++counter;
	}

	// ���_��ۑ��B
	defVertex_ = vertex_;

	// �_�[�e�B�t���O
	isChangeVertex_ = true;
	isChangeTexture_ = true;

	isGenerate_ = true;

}

void BLAS::GenerateBLASFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, int BlasIndex, bool IsOpaque)
{

	/*===== BLAS�𐶐����鏈�� =====*/

	// Blas��Index��ۑ��B
	blasIndex_ = BlasIndex;

	/*-- �`��f�[�^��ǂݍ��� --*/

	// �ǂݍ��񂾃f�[�^���ꎞ�ۑ�����p�̃o�b�t�@�B
	std::vector<FbxLoader::Vertex> modelVertexData;
	std::vector<UINT> modelIndexData;

	// ���f�������[�h�B
	modelIndex_ = FbxLoader::Ins()->LoadModelFromFile(DirectryPath, ModelName);

	FbxLoader::Ins()->GetFbxData(modelIndex_, modelVertexData, modelIndexData);

	// �e�N�X�`�����擾�B
	baseTextureHandle_ = FbxLoader::Ins()->GetTextureIndex(modelIndex_);

	// �}�e���A���p�萔�o�b�t�@�𐶐��B
	materialBuffer_ = CreateBuffer(
		static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// �m�ۂ����o�b�t�@�Ƀ}�e���A���f�[�^���������ށB
	WriteToMemory(materialBuffer_, &material_, static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)));

	// �}�e���A���f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> materialDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	materialDescriptor_.CreateStructuredSRV(materialBuffer_, 1, 0, sizeof(ModelDataManager::GPUMaterial), materialDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// ���_�������߂�B
	vertexCount_ = static_cast<UINT>(modelVertexData.size());

	// ���_�C���f�b�N�X�������߂�B
	indexCount_ = static_cast<UINT>(modelIndexData.size());

	// ���_�f�[�^��ϊ��B
	for (int index = 0; index < static_cast<int>(vertexCount_); ++index) {

		RayVertex buff{};
		buff.normal_ = modelVertexData[index].normal_;
		buff.position_ = modelVertexData[index].pos_;
		buff.uv_ = modelVertexData[index].uv_;

		// �f�[�^��ۑ��B
		vertex_.push_back(buff);

	}

	// ���_�C���f�b�N�X�f�[�^��ۑ��B
	vertIndex_ = modelIndexData;

	// ���_�T�C�Y�����߂�B
	vertexStride_ = sizeof(RayVertex);

	// ���_�C���f�b�N�X�T�C�Y�����߂�B
	indexStride_ = sizeof(UINT);

	// ���_�o�b�t�@�𐶐�����B
	vertexBuffer_ = CreateBuffer(
		static_cast<size_t>(vertexStride_ * vertexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

	// �m�ۂ����o�b�t�@�ɒ��_�f�[�^���������ށB
	WriteToMemory(vertexBuffer_, vertex_.data(), static_cast<size_t>(vertexStride_ * vertexCount_));

	// ���_�C���f�b�N�X�o�b�t�@�𐶐�����B
	indexBuffer_ = CreateBuffer(
		static_cast<size_t>(indexStride_ * indexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	indexBuffer_->SetName(L"IndexBuffer");

	// �m�ۂ����C���f�b�N�X�o�b�t�@�ɒ��_�C���f�b�N�X�f�[�^���������ށB
	WriteToMemory(indexBuffer_, vertIndex_.data(), static_cast<size_t>(indexStride_ * indexCount_));

	// ���_�C���f�b�N�X�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	indexDescriptor_.CreateStructuredSRV(indexBuffer_, indexCount_, 0, indexStride_, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// ���_�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> vertexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	vertexDescriptor_.CreateStructuredSRV(vertexBuffer_, vertexCount_, 0, vertexStride_, vertexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();


	/*-- BLAS�o�b�t�@�𐶐����� --*/

	// �`���ݒ肷��p�̍\���̂�ݒ�B
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(IsOpaque);
	isOpaque_ = IsOpaque;

	// BLAS�o�b�t�@��ݒ�A�\�z����B
	SettingAccelerationStructure(geomDesc);

	// �q�b�g�O���[�v����ۑ�����B
	this->hitGroupName_ = HitGroupName;

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Ins()->GetFbxModel(modelIndex_).hasAnimation_) {

		// ���͗p�\���̂����T�C�Y�B
		skinComputeInput_.resize(vertex_.size());

		// �X�L�j���O�A�j���[�V�����R���s���[�g�V�F�[�_�[�Ŏg�p������͗p�\���̂��Z�b�g�B
		FbxLoader::Ins()->GetSkinComputeInput(modelIndex_, skinComputeInput_);

		// �X�L�j���O�A�j���[�V�����Ŏg�p����R���s���[�g�V�F�[�_�[�����[�h���Ă����B
		skinComput_.Init(L"resource/ShaderFiles/RayTracing/ComputeSkin.hlsl", sizeof(FbxLoader::SkinComputeInput), static_cast<int>(skinComputeInput_.size()), skinComputeInput_.data(), sizeof(RayVertex), static_cast<int>(vertex_.size()), vertex_.data());

	}

	// �f�o�b�O�Ŏg�p���钸�_�݂̂̃f�[�^�Ɩ@���݂̂̃f�[�^�𐶐�����B
	vertexPos_.resize(static_cast<unsigned __int64>(vertex_.size()));
	vertexNormal_.resize(static_cast<unsigned __int64>(vertex_.size()));
	vertexUV_.resize(static_cast<unsigned __int64>(vertex_.size()));
	int counter = 0;
	for (auto& index_ : vertex_) {
		vertexPos_[counter] = index_.position_;
		vertexNormal_[counter] = index_.normal_;
		vertexUV_[counter] = index_.uv_;
		++counter;
	}

	// ���_��ۑ��B
	defVertex_ = vertex_;

	// �_�[�e�B�t���O
	isChangeVertex_ = true;
	isChangeTexture_ = true;

	isGenerate_ = true;

	// �ŏ��ƍő�̒��_��ۑ��B
	ModelDataManager::ObjectData dataBuff;
	for (auto& index : vertexPos_) {

		ModelDataManager::Ins()->SaveVertexMinMaxInfo(dataBuff, index);

	}
	vertexMin_ = dataBuff.vertexMin_;
	vertexMax_ = dataBuff.vertexMax_;

}

#include "FString.h"
void BLAS::GenerateBLASGLTF(const std::wstring& Path, const std::wstring& HitGroupName, int BlasIndex, bool IsOpaque)
{

	/*===== BLAS�𐶐����鏈�� =====*/

	// �p�X��ۑ��B
	modelPath_ = FString::WStringToString(Path);

	// Blas��Index��ۑ��B
	blasIndex_ = BlasIndex;


	/*-- �`��f�[�^��ǂݍ��� --*/

	// �ǂݍ��񂾃f�[�^���ꎞ�ۑ�����p�̃o�b�t�@�B
	ModelDataManager::ObjectData dataBuff;

	// ���f�������[�h�B
	ModelDataManager::Ins()->LoadGLTF(Path, dataBuff);

	// �e�����̒l��ۑ��B
	vertexMax_ = dataBuff.vertexMax_;
	vertexMin_ = dataBuff.vertexMin_;

	// �}�e���A������ۑ��B
	material_.baseColor_ = dataBuff.material_.baseColor_;
	material_.mapParam_ = dataBuff.material_.mapParam_;
	material_.roughness_ = dataBuff.material_.roughness_;
	material_.metalness_ = dataBuff.material_.metalness_;
	material_.specular_ = dataBuff.material_.specular_;

	// �e�N�X�`����ۑ��B
	baseTextureHandle_ = dataBuff.textureHandle_;

	// �}�e���A���o�b�t�@�𐶐�����B
	materialBuffer_ = CreateBuffer(
		static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);
	materialBuffer_->SetName(L"MaterialBuffer");

	// �A�b�v���[�h�p�}�e���A���o�b�t�@�𐶐�����B
	materialUploadBuffer_ = CreateBuffer(
		static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	materialUploadBuffer_->SetName(L"MaterialUploadBuffer");

	// �m�ۂ����o�b�t�@�Ƀ}�e���A���f�[�^���������ށB
	WriteToMemory(materialUploadBuffer_, &material_, static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)));

	Engine::Ins()->mainGraphicsCmdList_->CopyResource(materialBuffer_.Get(), materialUploadBuffer_.Get());
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(materialBuffer_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	// �}�e���A���f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> materialDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	materialDescriptor_.CreateStructuredSRV(materialBuffer_, 1, 0, sizeof(ModelDataManager::GPUMaterial), materialDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// ���_�������߂�B
	vertexCount_ = static_cast<UINT>(dataBuff.vertex_.size());

	// ���_�C���f�b�N�X�������߂�B
	indexCount_ = static_cast<UINT>(dataBuff.index_.size());

	// ���_�f�[�^��ϊ��B
	for (int index = 0; index < static_cast<int>(vertexCount_); ++index) {

		RayVertex buff{};
		buff.normal_ = dataBuff.vertex_[index].normal_;
		buff.position_ = dataBuff.vertex_[index].pos_;
		buff.uv_ = dataBuff.vertex_[index].uv_;

		// �f�[�^��ۑ��B
		vertex_.push_back(buff);

	}

	// ���_�C���f�b�N�X�f�[�^��ۑ��B
	vertIndex_ = dataBuff.index_;

	// ���_�T�C�Y�����߂�B
	vertexStride_ = sizeof(RayVertex);

	// ���_�C���f�b�N�X�T�C�Y�����߂�B
	indexStride_ = sizeof(UINT);

	// ���_�o�b�t�@�𐶐�����B
	vertexBuffer_ = CreateBuffer(
		static_cast<size_t>(vertexStride_ * vertexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);
	vertexBuffer_->SetName(L"VertexBuffer");

	// �A�b�v���[�h�p���_�o�b�t�@�𐶐�����B
	vertexUploadBuffer_ = CreateBuffer(
		static_cast<size_t>(vertexStride_ * vertexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	vertexUploadBuffer_->SetName(L"VertexUploadBuffer");

	// �m�ۂ����o�b�t�@�ɒ��_�f�[�^���������ށB
	WriteToMemory(vertexUploadBuffer_, vertex_.data(), static_cast<size_t>(vertexStride_ * vertexCount_));

	Engine::Ins()->mainGraphicsCmdList_->CopyResource(vertexBuffer_.Get(), vertexUploadBuffer_.Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);


	// ���_�C���f�b�N�X�o�b�t�@�𐶐�����B
	indexBuffer_ = CreateBuffer(
		static_cast<size_t>(indexStride_ * indexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);
	indexBuffer_->SetName(L"IndexBuffer");

	// �A�b�v���[�h�p���_�C���f�b�N�X�o�b�t�@�𐶐�����B
	indexUploadBuffer_ = CreateBuffer(
		static_cast<size_t>(indexStride_ * indexCount_),
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
	indexUploadBuffer_->SetName(L"IndexUploadBuffer");

	// �m�ۂ����C���f�b�N�X�o�b�t�@�ɒ��_�C���f�b�N�X�f�[�^���������ށB
	WriteToMemory(indexUploadBuffer_, vertIndex_.data(), static_cast<size_t>(indexStride_ * indexCount_));

	Engine::Ins()->mainGraphicsCmdList_->CopyResource(indexBuffer_.Get(), indexUploadBuffer_.Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(indexBuffer_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	// ���_�C���f�b�N�X�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> indexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	indexDescriptor_.CreateStructuredSRV(indexBuffer_, indexCount_, 0, indexStride_, indexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();

	// ���_�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> vertexDescHeap = DescriptorHeapMgr::Ins()->GetDescriptorHeap();
	vertexDescriptor_.CreateStructuredSRV(vertexBuffer_, vertexCount_, 0, vertexStride_, vertexDescHeap, DescriptorHeapMgr::Ins()->GetHead());
	DescriptorHeapMgr::Ins()->IncrementHead();


	/*-- BLAS�o�b�t�@�𐶐����� --*/

	// �`���ݒ肷��p�̍\���̂�ݒ�B
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(IsOpaque);
	isOpaque_ = IsOpaque;

	// BLAS�o�b�t�@��ݒ�A�\�z����B
	SettingAccelerationStructure(geomDesc);


	// �q�b�g�O���[�v����ۑ�����B
	this->hitGroupName_ = HitGroupName;

	// �f�o�b�O�Ŏg�p���钸�_�݂̂̃f�[�^�Ɩ@���݂̂̃f�[�^�𐶐�����B
	vertexPos_.resize(static_cast<unsigned __int64>(vertex_.size()));
	vertexNormal_.resize(static_cast<unsigned __int64>(vertex_.size()));
	vertexUV_.resize(static_cast<unsigned __int64>(vertex_.size()));
	int counter = 0;
	for (auto& index_ : vertex_) {
		vertexPos_[counter] = index_.position_;
		vertexNormal_[counter] = index_.normal_;
		vertexUV_[counter] = index_.uv_;
		++counter;
	}

	// ���_��ۑ��B
	defVertex_ = vertex_;

	// �_�[�e�B�t���O
	isChangeVertex_ = true;
	isChangeTexture_ = true;

	isGenerate_ = true;

}

void BLAS::Update()
{

	/*===== BLAS�̍X�V =====*/

	// �������Ă��Ȃ������珈�����΂��B
	if (!isGenerate_) return;

	// ���̏�����FBX�̃A�j���[�V�������K�v�ɂȂ����ۂɖ߂��B
	{
		//// ���f�����A�j���[�V�����������Ă�����B
		//if (FbxLoader::Ins()->GetFbxModel(modelIndex_).hasAnimation) {

		//	// �A�j���[�V�����̍X�V����
		//	auto& model = FbxLoader::Ins()->GetFbxModel(modelIndex_);
		//	if (model.isPlay) {

		//		model.currentTime += model.frameTime;

		//		// �Ō�܂ōĐ�������擪�ɖ߂��B
		//		if (model.endTime < model.currentTime) {

		//			model.currentTime = model.startTime;

		//		}

		//		// �X�L�j���O�A�j���[�V�����R���s���[�g�V�F�[�_�[�Ŏg�p������͗p�\���̂��Z�b�g�B
		//		FbxLoader::Ins()->GetSkinComputeInput(modelIndex_, skinComputeInput_);

		//	}

		//};
	}

	// ���_���������ށB ���̂Ƃ���͒��_��������������\��͂Ȃ����A��X���̂���������ށB�_�[�e�B�t���O�݂����Ȃ̂�p�ӂ������B
	WriteToMemory(vertexUploadBuffer_, vertex_.data(), static_cast<size_t>(vertexStride_ * vertexCount_));

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer_.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	Engine::Ins()->mainGraphicsCmdList_->CopyResource(vertexBuffer_.Get(), vertexUploadBuffer_.Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	// �X�V�̂��߂̒l��ݒ�B
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(isOpaque_);
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
	asDesc.SourceAccelerationStructureData = blasBuffer_->GetGPUVirtualAddress();
	asDesc.DestAccelerationStructureData = blasBuffer_->GetGPUVirtualAddress();
	// �X�V�p�̍�ƃo�b�t�@��ݒ肷��B
	asDesc.ScratchAccelerationStructureData = updateBuffer_->GetGPUVirtualAddress();

	// �R�}���h���X�g�ɐςށB
	Engine::Ins()->mainGraphicsCmdList_->BuildRaytracingAccelerationStructure(
		&asDesc, 0, nullptr
	);

	CreateAccelerationStructure();

}

void BLAS::ComputeSkin()
{

	/*===== ���_�f�[�^���X�L�j���O�s������ɏ��������鏈�� =====*/

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Ins()->GetFbxModel(modelIndex_).hasAnimation_) {

		// ���͍\���̂��X�V�B
		skinComput_.UpdateInputSB(skinComputeInput_.data());

		// �v�Z�J�n�B
		skinComput_.Dispatch(1, 1, 1);

		// ���ʂ����B
		memcpy(vertex_.data(), skinComput_.outputSB_->buffersOnCPU_, sizeof(RayVertex) * vertex_.size());

	}

}

void BLAS::InitAnimation()
{
	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Ins()->GetFbxModel(modelIndex_).hasAnimation_) {

		FbxLoader::Ins()->GetFbxModel(modelIndex_).InitAnimation();

	}
}

void BLAS::PlayAnimation()
{

	/*===== �A�j���[�V���������� =====*/

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Ins()->GetFbxModel(modelIndex_).hasAnimation_) {

		FbxLoader::Ins()->GetFbxModel(modelIndex_).PlayAnimation();

	}

}

void BLAS::StopAnimation()
{

	/*===== �A�j���[�V�������~������ =====*/

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Ins()->GetFbxModel(modelIndex_).hasAnimation_) {

		FbxLoader::Ins()->GetFbxModel(modelIndex_).StopAnimation();

	}

}
void BLAS::IsChangeMaterial()
{

	/*===== �}�e���A���������������ۂ̏��� =====*/

	// �m�ۂ����o�b�t�@�Ƀ}�e���A���f�[�^���������ށB
	WriteToMemory(materialUploadBuffer_, &material_, static_cast<size_t>(sizeof(ModelDataManager::GPUMaterial)));

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(materialBuffer_.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	Engine::Ins()->mainGraphicsCmdList_->CopyResource(materialBuffer_.Get(), materialUploadBuffer_.Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(materialBuffer_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	isChangeVertex_ = true;

}
void BLAS::ChangeBaseTexture(int Index)
{
	baseTextureHandle_ = Index;
	isChangeTexture_ = true;
}
void BLAS::ChangeMapTexture(int Index, MAP_PARAM MapParam)
{
	mapTextureHandle_ = Index;
	material_.mapParam_ = static_cast<int>(MapParam);
	isChangeTexture_ = true;
	IsChangeMaterial();
}
#include "HitGroupMgr.h"
#include <assert.h>
BLAS::BLAS()
{

	isGenerate_ = false;
	baseTextureHandle_ = -1;
	mapTextureHandle_ = -1;
	blasIndex_ = -1;
	indexCount_ = 0;
	indexDescriptor_ = {};
	indexStride_ = {};
	isChangeTexture_ = false;
	isChangeVertex_ = false;
	isOpaque_ = false;
	materialDescriptor_ = {};
	modelIndex_ = 0;
	vertexCount_ = 0;
	vertexDescriptor_ = {};
	vertexStride_ = 0;
}

uint8_t* BLAS::WriteShaderRecord(uint8_t* Dst, UINT recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName)
{

	/*===== �V�F�[�_�[���R�[�h���������� =====*/

	// ��������Ă��Ȃ��������΂��B
	if (!isGenerate_) {

		return Dst;

	}

	Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> rtsoProps_;
	StateObject.As(&rtsoProps_);
	auto entryBegin = Dst;
	auto shader = GetHitGroupName();

	// �ۑ�����Ă���q�b�g�O���[�v���ƈ���Ă����珑�����܂Ȃ��B
	//if (HitGroupName == shader) {

	auto mode_ = rtsoProps_->GetShaderIdentifier(shader.c_str());
	if (mode_ == nullptr) {
		throw std::logic_error("Not found ShaderIdentifier");
	}

	// ���_�֌W�̃f�[�^���ύX����Ă�����B
	if (isChangeVertex_) {

		// �V�F�[�_�[���ʎq���������ށB
		memcpy(Dst, mode_, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
		Dst += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

		// ����̃v���O�����ł͈ȉ��̏����Ńf�B�X�N���v�^���L�^�B
		// [0] : �C���f�b�N�X�o�b�t�@
		// [1] : ���_�o�b�t�@
		// �� ���[�J�����[�g�V�O�l�`���̏����ɍ��킹��K�v������B
		Dst += WriteGPUDescriptor(Dst, &indexDescriptor_.GetGPUHandle());
		Dst += WriteGPUDescriptor(Dst, &vertexDescriptor_.GetGPUHandle());

		// �}�e���A���p�̃o�b�t�@���Z�b�g�B
		Dst += WriteGPUDescriptor(Dst, &materialDescriptor_.GetGPUHandle());

		isChangeVertex_ = false;

	}
	else {

		Dst += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		Dst += static_cast<UINT>((sizeof(D3D12_GPU_DESCRIPTOR_HANDLE)));
		Dst += static_cast<UINT>((sizeof(D3D12_GPU_DESCRIPTOR_HANDLE)));
		Dst += static_cast<UINT>((sizeof(D3D12_GPU_DESCRIPTOR_HANDLE)));

	}

	// �q�b�g�O���[�v������q�b�g�O���[�v��ID���擾����B
	int hitGroupID = HitGroupMgr::Ins()->GetHitGroupID(HitGroupName);

	// ���_�A�C���f�b�N�X�A�}�e���A���̃I�t�Z�b�g
	const int OFFSET_VERTEX_INDEX_MATERIAL = 3;

	// �q�b�g�O���[�vID����SRV�̐����擾�B
	int srvCount = HitGroupMgr::Ins()->GetHitGroupSRVCount(hitGroupID) - OFFSET_VERTEX_INDEX_MATERIAL;

	// �e�N�X�`���֌W���ύX����Ă�����B
	if (isChangeTexture_) {

		// �����̓e�N�X�`���̃T�C�Y�ł͂Ȃ��A�p�C�v���C���ɃZ�b�g���ꂽSRV�̐��������Ă��Ă�����g���B
		// ���̎��_��SRV�̐��ƃe�N�X�`���̐��������Ă��Ȃ�������assert���o���B
		for (int index = 0; index < srvCount; ++index) {

			// 0�Ԗڂ͊�{�e�N�X�`���B
			if (index == 0) {

				// ��{�̃e�N�X�`�����ݒ肳��Ă��Ȃ��B
				if (baseTextureHandle_ == -1) assert(0);

				CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(baseTextureHandle_);
				Dst += WriteGPUDescriptor(Dst, &texDescHandle);

			}
			// 1�Ԗڂ͖@���}�b�v�e�N�X�`���B
			else if (index == 1) {

				if (mapTextureHandle_ != -1) {

					CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(mapTextureHandle_);
					Dst += WriteGPUDescriptor(Dst, &texDescHandle);

				}
				else {

					// �@���}�b�v���ݒ肳��Ă��Ȃ�������A�������̌��Ԃ𖄂߂邽�ߒʏ�̃e�N�X�`�����������ށB
					CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(baseTextureHandle_);
					Dst += WriteGPUDescriptor(Dst, &texDescHandle);

				}

			}
			// ����ȍ~�͒ʏ�e�N�X�`������������Ń������̌��Ԃ𖄂߂�B
			else {

				CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(baseTextureHandle_);
				Dst += WriteGPUDescriptor(Dst, &texDescHandle);

			}

		}

		isChangeTexture_ = false;

	}
	else {

		Dst += static_cast<UINT>(sizeof(D3D12_GPU_DESCRIPTOR_HANDLE*)) * static_cast<UINT>(srvCount);

	}

	// �g�p����UAV�̐����擾�B
	int uavCount = HitGroupMgr::Ins()->GetHitGroupUAVCount(hitGroupID);
	for (int index = 0; index < uavCount; ++index) {

		// �e�N�X�`�������݂��Ă�����B
		if (0 < uavHandle_.size()) {

			CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(uavHandle_[index]);
			Dst += WriteGPUDescriptor(Dst, &texDescHandle);

		}
		else {

			Dst += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);

		}

	}

	Dst = entryBegin + recordSize;
	return Dst;

}

void BLAS::Init()
{

	/*===== ���������� =====*/

	texturePath_.clear();
	defVertex_.clear();
	vertex_.clear();
	vertexPos_.clear();
	vertexNormal_.clear();
	vertexUV_.clear();
	vertIndex_.clear();
	isGenerate_ = false;
	baseTextureHandle_ = -1;
	mapTextureHandle_ = -1;

}

Vec3 BLAS::GetVertexLengthMax()
{

	/*===== �e�����̍ő�̒�����Ԃ� =====*/

	Vec3 vertexLength;

	if (fabs(vertexMin_.x_) < fabs(vertexMax_.x_)) {
		vertexLength.x_ = fabs(vertexMax_.x_);
	}
	else {
		vertexLength.x_ = fabs(vertexMin_.x_);
	}

	if (fabs(vertexMin_.y_) < fabs(vertexMax_.y_)) {
		vertexLength.y_ = fabs(vertexMax_.y_);
	}
	else {
		vertexLength.y_ = fabs(vertexMin_.y_);
	}

	if (fabs(vertexMin_.z_) < fabs(vertexMax_.z_)) {
		vertexLength.z_ = fabs(vertexMax_.z_);
	}
	else {
		vertexLength.z_ = fabs(vertexMin_.z_);
	}

	return vertexLength;
}

void BLAS::MulVec3Vertex(Vec3 Vec)
{

	/*===== ���ׂĂ̒��_�ɍs����������� ====*/

	for (auto& index_ : vertex_) {

		index_.position_ = defVertex_[&index_ - &vertex_[0]].position_ * Vec;

	}

	// �m�ۂ����o�b�t�@�ɒ��_�f�[�^���������ށB
	WriteToMemory(vertexBuffer_, vertex_.data(), static_cast<size_t>(vertexStride_ * vertexCount_));

}

void BLAS::AssignUV(const std::vector<RayVertex>& UV)
{

	/*===== �w���UV�������� =====*/

	for (auto& index : vertex_) {

		index.subUV_ = UV[static_cast<int>(&index - &vertex_[0])].uv_;

	}

	// �m�ۂ����o�b�t�@�ɒ��_�f�[�^���������ށB
	WriteToMemory(vertexUploadBuffer_, vertex_.data(), static_cast<size_t>(vertexStride_ * vertexCount_));

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer_.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);
	Engine::Ins()->mainGraphicsCmdList_->CopyResource(vertexBuffer_.Get(), vertexUploadBuffer_.Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

	// ���_�������������t���O�𗧂Ă�B
	isChangeVertex_ = true;

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
	hr = Engine::Ins()->device_.dev_->CreateCommittedResource(
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

D3D12_RAYTRACING_GEOMETRY_DESC BLAS::GetGeometryDesc(bool IsOpaque)
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
	triangles.VertexBuffer.StartAddress = vertexBuffer_->GetGPUVirtualAddress();
	triangles.VertexBuffer.StrideInBytes = vertexStride_;
	triangles.VertexCount = vertexCount_;
	triangles.IndexBuffer = indexBuffer_->GetGPUVirtualAddress();
	triangles.IndexCount = indexCount_;
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
	Engine::Ins()->device_.dev_->GetRaytracingAccelerationStructurePrebuildInfo(
		&inputs, &blasPrebuild
	);

	// �X�N���b�`�o�b�t�@�𐶐�����B
	scratchBuffer_ = CreateBuffer(
		blasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);
	scratchBuffer_->SetName(L"BLASScratchBuffer");

	// BLAS�̃o�b�t�@�𐶐�����B
	blasBuffer_ = CreateBuffer(
		blasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);
	blasBuffer_->SetName(L"BLASBuffer");

	// �X�V�p�o�b�t�@�𐶐�����B
	updateBuffer_ = CreateBuffer(
		blasPrebuild.UpdateScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_HEAP_TYPE_DEFAULT
	);
	updateBuffer_->SetName(L"BLASUpdateBuffer");

	// AccelerationStructure�̍\�z�B
	buildASDesc.ScratchAccelerationStructureData = scratchBuffer_->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = blasBuffer_->GetGPUVirtualAddress();
	// �R�}���h���X�g�ɐς�Ŏ��s����B
	Engine::Ins()->mainGraphicsCmdList_->BuildRaytracingAccelerationStructure(
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
	uavBarrier.UAV.pResource = blasBuffer_.Get();
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &uavBarrier);
	Engine::Ins()->mainGraphicsCmdList_->Close();

	// BLAS���\�z�B
	//ID3D12CommandList* pCmdList[] = { Engine::Ins()->cmdList.Get() };
	// �\�z�p�֐����ĂԁB
	ID3D12CommandList* commandLists[] = { Engine::Ins()->mainGraphicsCmdList_.Get() };
	Engine::Ins()->graphicsCmdQueue_->ExecuteCommandLists(1, commandLists);
	Engine::Ins()->graphicsCmdQueue_->Signal(Engine::Ins()->asFence_.Get(), ++Engine::Ins()->asfenceValue_);


	// �O���t�B�b�N�R�}���h���X�g�̊����҂�
	UINT64 gpuFence = Engine::Ins()->asFence_->GetCompletedValue();
	if (gpuFence != Engine::Ins()->asfenceValue_) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		Engine::Ins()->asFence_->SetEventOnCompletion(Engine::Ins()->asfenceValue_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// �R�}���h�A���P�[�^�̃��Z�b�g
	Engine::Ins()->mainGraphicsCmdAllocator_->Reset();	//�L���[���N���A

	// �R�}���h���X�g�̃��Z�b�g
	Engine::Ins()->mainGraphicsCmdList_->Reset(Engine::Ins()->mainGraphicsCmdAllocator_.Get(), nullptr);//�ĂуR�}���h���X�g�𒙂߂鏀��

}