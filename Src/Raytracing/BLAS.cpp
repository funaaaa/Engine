#include "BLAS.h"
#include "ModelDataManager.h"
#include "DescriptorHeapMgr.h"
#include <DirectXMath.h>
#include "FbxLoader.h"
#include "TextureManager.h"
#include "FHelper.h"
#include "HitGroupMgr.h"
#include "DoubleRayDescriptor.h"


void BLAS::GenerateBLASObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, int BlasIndex, bool IsOpaque)
{

	/*===== BLAS�𐶐����鏈�� =====*/

	// �p�X��ۑ��B
	hitGroupInfo_.modelPath_ = DirectryPath + ModelName;

	// Blas��Index��ۑ��B
	blasInfo_.blasIndex_ = BlasIndex;


	/*-- �`��f�[�^��ǂݍ��� --*/

	// �ǂݍ��񂾃f�[�^���ꎞ�ۑ�����p�̃o�b�t�@�B
	ModelDataManager::ObjectData dataBuff;

	// ���f�������[�h�B
	ModelDataManager::Ins()->LoadObj(DirectryPath, ModelName, dataBuff, false);

	// �e�����̒l��ۑ��B
	modelInfo_.vertexMax_ = dataBuff.vertexMax_;
	modelInfo_.vertexMin_ = dataBuff.vertexMin_;

	// �}�e���A������ۑ��B
	blasInfo_.material_.baseColor_ = dataBuff.material_.baseColor_;
	blasInfo_.material_.mapParam_ = dataBuff.material_.mapParam_;
	blasInfo_.material_.roughness_ = dataBuff.material_.roughness_;
	blasInfo_.material_.metalness_ = dataBuff.material_.metalness_;
	blasInfo_.material_.specular_ = dataBuff.material_.specular_;

	// �e�N�X�`����ۑ��B
	hitGroupInfo_.baseTextureHandle_ = dataBuff.textureHandle_;

	// ���݂�Queue��Index
	int currentQueueIndex = Engine::Ins()->currentQueueIndex_;

	// �}�e���A���o�b�t�@�𐶐�����B
	CreateMaterialBuffer();

	// ���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�𐶐��B
	CreateVertexBuffer(dataBuff);


	/*-- BLAS�o�b�t�@�𐶐����� --*/

	for (int index = 0; index < 2; ++index) {

		// �`���ݒ肷��p�̍\���̂�ݒ�B
		D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(IsOpaque, index);
		modelInfo_.isOpaque_ = IsOpaque;

		// BLAS�o�b�t�@��ݒ�A�\�z����B
		SettingAccelerationStructure(geomDesc, index);

	}


	// �q�b�g�O���[�v����ۑ�����B
	this->hitGroupInfo_.hitGroupName_ = HitGroupName;

	// �f�o�b�O�Ŏg�p���钸�_�݂̂̃f�[�^�Ɩ@���݂̂̃f�[�^�𐶐�����B
	vertexInfo_.vertexPos_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	vertexInfo_.vertexNormal_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	vertexInfo_.vertexUV_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	int counter = 0;
	for (auto& index_ : vertexInfo_.vertex_) {
		vertexInfo_.vertexPos_[counter] = index_.position_;
		vertexInfo_.vertexNormal_[counter] = index_.normal_;
		vertexInfo_.vertexUV_[counter] = index_.uv_;
		++counter;
	}

	// ���_��ۑ��B
	vertexInfo_.defVertex_ = vertexInfo_.vertex_;

	// �_�[�e�B�t���O
	hitGroupInfo_.isChangeVertex_[0] = true;
	hitGroupInfo_.isChangeVertex_[1] = true;
	hitGroupInfo_.isChangeTexture_[0] = true;
	hitGroupInfo_.isChangeTexture_[1] = true;

	blasInfo_.isGenerate_ = true;

}

#include "FString.h"
void BLAS::GenerateBLASGLTF(const std::wstring& Path, const std::wstring& HitGroupName, int BlasIndex, bool IsOpaque)
{

	/*===== BLAS�𐶐����鏈�� =====*/

	// �p�X��ۑ��B
	hitGroupInfo_.modelPath_ = FString::WStringToString(Path);

	// Blas��Index��ۑ��B
	blasInfo_.blasIndex_ = BlasIndex;


	/*-- �`��f�[�^��ǂݍ��� --*/

	// �ǂݍ��񂾃f�[�^���ꎞ�ۑ�����p�̃o�b�t�@�B
	ModelDataManager::ObjectData dataBuff;

	// ���f�������[�h�B
	ModelDataManager::Ins()->LoadGLTF(Path, dataBuff);

	// �e�����̒l��ۑ��B
	modelInfo_.vertexMax_ = dataBuff.vertexMax_;
	modelInfo_.vertexMin_ = dataBuff.vertexMin_;

	// �}�e���A������ۑ��B
	blasInfo_.material_.baseColor_ = dataBuff.material_.baseColor_;
	blasInfo_.material_.mapParam_ = dataBuff.material_.mapParam_;
	blasInfo_.material_.roughness_ = dataBuff.material_.roughness_;
	blasInfo_.material_.metalness_ = dataBuff.material_.metalness_;
	blasInfo_.material_.specular_ = dataBuff.material_.specular_;

	// �e�N�X�`����ۑ��B
	hitGroupInfo_.baseTextureHandle_ = dataBuff.textureHandle_;

	// �}�e���A���o�b�t�@�𐶐�����B
	CreateMaterialBuffer();

	// ���_�������߂�B
	modelInfo_.vertexCount_ = static_cast<UINT>(dataBuff.vertex_.size());

	// ���_�C���f�b�N�X�������߂�B
	modelInfo_.indexCount_ = static_cast<UINT>(dataBuff.index_.size());

	// ���_�f�[�^��ϊ��B
	for (int index = 0; index < static_cast<int>(modelInfo_.vertexCount_); ++index) {

		RayVertex buff{};
		buff.normal_ = dataBuff.vertex_[index].normal_;
		buff.position_ = dataBuff.vertex_[index].pos_;
		buff.uv_ = dataBuff.vertex_[index].uv_;

		// �f�[�^��ۑ��B
		vertexInfo_.vertex_.push_back(buff);

	}

	// ���_�o�b�t�@�𐶐��B
	CreateVertexBuffer(dataBuff);


	/*-- BLAS�o�b�t�@�𐶐����� --*/

	for (int index = 0; index < 2; ++index) {

		// �`���ݒ肷��p�̍\���̂�ݒ�B
		D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(IsOpaque, index);
		modelInfo_.isOpaque_ = IsOpaque;

		// BLAS�o�b�t�@��ݒ�A�\�z����B
		SettingAccelerationStructure(geomDesc, index);

	}


	// �q�b�g�O���[�v����ۑ�����B
	this->hitGroupInfo_.hitGroupName_ = HitGroupName;

	// �f�o�b�O�Ŏg�p���钸�_�݂̂̃f�[�^�Ɩ@���݂̂̃f�[�^�𐶐�����B
	vertexInfo_.vertexPos_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	vertexInfo_.vertexNormal_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	vertexInfo_.vertexUV_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	int counter = 0;
	for (auto& index_ : vertexInfo_.vertex_) {
		vertexInfo_.vertexPos_[counter] = index_.position_;
		vertexInfo_.vertexNormal_[counter] = index_.normal_;
		vertexInfo_.vertexUV_[counter] = index_.uv_;
		++counter;
	}

	// ���_��ۑ��B
	vertexInfo_.defVertex_ = vertexInfo_.vertex_;

	// �_�[�e�B�t���O
	hitGroupInfo_.isChangeVertex_[0] = true;
	hitGroupInfo_.isChangeVertex_[1] = true;
	hitGroupInfo_.isChangeTexture_[0] = true;
	hitGroupInfo_.isChangeTexture_[1] = true;

	blasInfo_.isGenerate_ = true;

}

void BLAS::GenerateBLASData(const ModelDataManager::ObjectData& ModelData, int BlasIndex, bool IsOpaque)
{

	/*===== BLAS�𐶐����鏈�� =====*/

	// �p�X��ۑ��B
	hitGroupInfo_.modelPath_ = "SelfModel";

	// Blas��Index��ۑ��B
	blasInfo_.blasIndex_ = BlasIndex;


	/*-- �`��f�[�^��ǂݍ��� --*/

	// �e�����̒l��ۑ��B
	modelInfo_.vertexMax_ = ModelData.vertexMax_;
	modelInfo_.vertexMin_ = ModelData.vertexMin_;

	// �}�e���A������ۑ��B
	blasInfo_.material_.baseColor_ = ModelData.material_.baseColor_;
	blasInfo_.material_.mapParam_ = ModelData.material_.mapParam_;
	blasInfo_.material_.roughness_ = ModelData.material_.roughness_;
	blasInfo_.material_.metalness_ = ModelData.material_.metalness_;
	blasInfo_.material_.specular_ = ModelData.material_.specular_;

	// �e�N�X�`����ۑ��B
	hitGroupInfo_.baseTextureHandle_ = ModelData.textureHandle_;

	// �}�e���A���o�b�t�@�𐶐�����B
	CreateMaterialBuffer();

	// ���_�o�b�t�@�𐶐��B
	CreateVertexBuffer(ModelData);


	/*-- BLAS�o�b�t�@�𐶐����� --*/

	for (int index = 0; index < 2; ++index) {

		// �`���ݒ肷��p�̍\���̂�ݒ�B
		D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(IsOpaque, index);
		modelInfo_.isOpaque_ = IsOpaque;

		// BLAS�o�b�t�@��ݒ�A�\�z����B
		SettingAccelerationStructure(geomDesc, index);

	}


	// �q�b�g�O���[�v����ۑ�����B
	this->hitGroupInfo_.hitGroupName_ = HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::Ins()->DEF];

	// �f�o�b�O�Ŏg�p���钸�_�݂̂̃f�[�^�Ɩ@���݂̂̃f�[�^�𐶐�����B
	vertexInfo_.vertexPos_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	vertexInfo_.vertexNormal_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	vertexInfo_.vertexUV_.resize(static_cast<unsigned __int64>(vertexInfo_.vertex_.size()));
	int counter = 0;
	for (auto& index_ : vertexInfo_.vertex_) {
		vertexInfo_.vertexPos_[counter] = index_.position_;
		vertexInfo_.vertexNormal_[counter] = index_.normal_;
		vertexInfo_.vertexUV_[counter] = index_.uv_;
		++counter;
	}

	// ���_��ۑ��B
	vertexInfo_.defVertex_ = vertexInfo_.vertex_;

	// �_�[�e�B�t���O
	hitGroupInfo_.isChangeVertex_[0] = true;
	hitGroupInfo_.isChangeVertex_[1] = true;
	hitGroupInfo_.isChangeTexture_[0] = true;
	hitGroupInfo_.isChangeTexture_[1] = true;

	blasInfo_.isGenerate_ = true;

}

void BLAS::Update()
{

	/*===== BLAS�̍X�V =====*/

	// ���݂�Queue�̃C���f�b�N�X
	int currentQueueIndex = Engine::Ins()->currentQueueIndex_;

	// �������Ă��Ȃ������珈�����΂��B
	if (!blasInfo_.isGenerate_) return;

	// ���_���������ށB ���̂Ƃ���͒��_��������������\��͂Ȃ����A��X���̂���������ށB�_�[�e�B�t���O�݂����Ȃ̂�p�ӂ������B
	blasInfo_.vertexBuffer_->WriteData(currentQueueIndex, sizeof(modelInfo_.vertexStride_ * modelInfo_.vertexCount_), vertexInfo_.vertex_.data());

	// �X�V�̂��߂̒l��ݒ�B
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = GetGeometryDesc(modelInfo_.isOpaque_, currentQueueIndex);
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
	asDesc.SourceAccelerationStructureData = blasInfo_.blasBuffer_[currentQueueIndex]->GetGPUVirtualAddress();
	asDesc.DestAccelerationStructureData = blasInfo_.blasBuffer_[currentQueueIndex]->GetGPUVirtualAddress();
	// �X�V�p�̍�ƃo�b�t�@��ݒ肷��B
	asDesc.ScratchAccelerationStructureData = blasInfo_.updateBuffer_[currentQueueIndex]->GetGPUVirtualAddress();

	// �R�}���h���X�g�ɐςށB
	Engine::Ins()->mainGraphicsCmdList_->BuildRaytracingAccelerationStructure(
		&asDesc, 0, nullptr
	);

	CreateAccelerationStructure(currentQueueIndex);

}

void BLAS::InitAnimation()
{
	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Ins()->GetFbxModel(modelInfo_.modelIndex_).hasAnimation_) {

		FbxLoader::Ins()->GetFbxModel(modelInfo_.modelIndex_).InitAnimation();

	}
}

void BLAS::PlayAnimation()
{

	/*===== �A�j���[�V���������� =====*/

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Ins()->GetFbxModel(modelInfo_.modelIndex_).hasAnimation_) {

		FbxLoader::Ins()->GetFbxModel(modelInfo_.modelIndex_).PlayAnimation();

	}

}

void BLAS::StopAnimation()
{

	/*===== �A�j���[�V�������~������ =====*/

	// ���f�����A�j���[�V�����������Ă�����B
	if (FbxLoader::Ins()->GetFbxModel(modelInfo_.modelIndex_).hasAnimation_) {

		FbxLoader::Ins()->GetFbxModel(modelInfo_.modelIndex_).StopAnimation();

	}

}
void BLAS::IsChangeMaterial()
{

	/*===== �}�e���A���������������ۂ̏��� =====*/

	// ���݂�Queue�̃C���f�b�N�X
	int currentQueueIndex = Engine::Ins()->currentQueueIndex_;

	// �m�ۂ����o�b�t�@�Ƀ}�e���A���f�[�^���������ށB
	blasInfo_.materialBuffer_->WriteData(currentQueueIndex, sizeof(ModelDataManager::GPUMaterial), &blasInfo_.material_);

	hitGroupInfo_.isChangeVertex_[0] = true;
	hitGroupInfo_.isChangeVertex_[1] = true;

}
void BLAS::ChangeBaseTexture(int Index)
{
	hitGroupInfo_.baseTextureHandle_ = Index;
	hitGroupInfo_.isChangeTexture_[0] = true;
	hitGroupInfo_.isChangeTexture_[1] = true;
}
void BLAS::ChangeMapTexture(int Index, MAP_PARAM MapParam)
{
	hitGroupInfo_.mapTextureHandle_ = Index;
	blasInfo_.material_.mapParam_ = static_cast<int>(MapParam);
	hitGroupInfo_.isChangeTexture_[0] = true;
	hitGroupInfo_.isChangeTexture_[1] = true;
	IsChangeMaterial();
}
#include "HitGroupMgr.h"
#include <assert.h>
BLAS::BLAS()
{

	blasInfo_.isGenerate_ = false;
	hitGroupInfo_.baseTextureHandle_ = -1;
	hitGroupInfo_.mapTextureHandle_ = -1;
	blasInfo_.blasIndex_ = -1;
	modelInfo_.indexCount_ = 0;
	blasInfo_.indexDescriptor_ = {};
	modelInfo_.indexStride_ = {};
	hitGroupInfo_.isChangeVertex_[0] = true;
	hitGroupInfo_.isChangeVertex_[1] = true;
	hitGroupInfo_.isChangeTexture_[0] = true;
	hitGroupInfo_.isChangeTexture_[1] = true;
	modelInfo_.isOpaque_ = false;
	blasInfo_.materialDescriptor_ = {};
	modelInfo_.modelIndex_ = 0;
	modelInfo_.vertexCount_ = 0;
	blasInfo_.vertexDescriptor_ = {};
	modelInfo_.vertexStride_ = 0;
}

uint8_t* BLAS::WriteShaderRecord(uint8_t* Dst, UINT recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName, int Index)
{

	/*===== �V�F�[�_�[���R�[�h���������� =====*/

	// ��������Ă��Ȃ��������΂��B
	if (!blasInfo_.isGenerate_) {


		Dst = Dst + recordSize;
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

	// �V�F�[�_�[���ʎq���������ށB
	memcpy(Dst, mode_, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	Dst += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;


	// ���_�֌W�̃f�[�^���ύX����Ă�����B
	if (hitGroupInfo_.isChangeVertex_[Index]) {
		// ����̃v���O�����ł͈ȉ��̏����Ńf�B�X�N���v�^���L�^�B
		// [0] : �C���f�b�N�X�o�b�t�@
		// [1] : ���_�o�b�t�@
		// �� ���[�J�����[�g�V�O�l�`���̏����ɍ��킹��K�v������B
		Dst += WriteGPUDescriptor(Dst, &blasInfo_.indexDescriptor_->GetDescriptor(Index).lock()->GetGPUHandle());
		Dst += WriteGPUDescriptor(Dst, &blasInfo_.vertexDescriptor_->GetDescriptor(Index).lock()->GetGPUHandle());

		// �}�e���A���p�̃o�b�t�@���Z�b�g�B
		Dst += WriteGPUDescriptor(Dst, &blasInfo_.materialDescriptor_->GetDescriptor(Index).lock()->GetGPUHandle());

		hitGroupInfo_.isChangeVertex_[Index] = false;

	}
	else {

		Dst += static_cast<UINT>((sizeof(D3D12_GPU_DESCRIPTOR_HANDLE*)));
		Dst += static_cast<UINT>((sizeof(D3D12_GPU_DESCRIPTOR_HANDLE*)));
		Dst += static_cast<UINT>((sizeof(D3D12_GPU_DESCRIPTOR_HANDLE*)));

	}

	// �q�b�g�O���[�v������q�b�g�O���[�v��ID���擾����B
	int hitGroupID = HitGroupMgr::Ins()->GetHitGroupID(HitGroupName);

	// ���_�A�C���f�b�N�X�A�}�e���A���̃I�t�Z�b�g
	const int OFFSET_VERTEX_INDEX_MATERIAL = 3;

	// �q�b�g�O���[�vID����SRV�̐����擾�B
	int srvCount = HitGroupMgr::Ins()->GetHitGroupSRVCount(hitGroupID) - OFFSET_VERTEX_INDEX_MATERIAL;

	// �e�N�X�`���֌W���ύX����Ă�����B
	if (hitGroupInfo_.isChangeTexture_[Index]) {

		// �����̓e�N�X�`���̃T�C�Y�ł͂Ȃ��A�p�C�v���C���ɃZ�b�g���ꂽSRV�̐��������Ă��Ă�����g���B
		// ���̎��_��SRV�̐��ƃe�N�X�`���̐��������Ă��Ȃ�������assert���o���B
		for (int index = 0; index < srvCount; ++index) {

			// 0�Ԗڂ͊�{�e�N�X�`���B
			if (index == 0) {

				// ��{�̃e�N�X�`�����ݒ肳��Ă��Ȃ��B
				if (hitGroupInfo_.baseTextureHandle_ == -1) assert(0);

				CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(hitGroupInfo_.baseTextureHandle_);
				Dst += WriteGPUDescriptor(Dst, &texDescHandle);

			}
			// 1�Ԗڂ͖@���}�b�v�e�N�X�`���B
			else if (index == 1) {

				if (hitGroupInfo_.mapTextureHandle_ != -1) {

					CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(hitGroupInfo_.mapTextureHandle_);
					Dst += WriteGPUDescriptor(Dst, &texDescHandle);

				}
				else {

					// �@���}�b�v���ݒ肳��Ă��Ȃ�������A�������̌��Ԃ𖄂߂邽�ߒʏ�̃e�N�X�`�����������ށB
					CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(hitGroupInfo_.baseTextureHandle_);
					Dst += WriteGPUDescriptor(Dst, &texDescHandle);

				}

			}

		}

		hitGroupInfo_.isChangeTexture_[Index] = false;

	}
	else {

		Dst += static_cast<UINT>(sizeof(D3D12_GPU_DESCRIPTOR_HANDLE*)) * static_cast<UINT>(srvCount);

	}

	// �g�p����UAV�̐����擾�B
	int uavCount = HitGroupMgr::Ins()->GetHitGroupUAVCount(hitGroupID);
	for (int index = 0; index < uavCount; ++index) {

		// �e�N�X�`�������݂��Ă�����B
		if (0 < hitGroupInfo_.uavHandle_.size()) {

			CD3DX12_GPU_DESCRIPTOR_HANDLE texDescHandle = DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(hitGroupInfo_.uavHandle_[index]);
			Dst += WriteGPUDescriptor(Dst, &texDescHandle);

		}
		else {

			Dst += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE*);

		}

	}

	Dst = entryBegin + recordSize;
	return Dst;

}

void BLAS::Init()
{

	/*===== ���������� =====*/

	hitGroupInfo_.texturePath_.clear();
	vertexInfo_.defVertex_.clear();
	vertexInfo_.vertex_.clear();
	vertexInfo_.vertexPos_.clear();
	vertexInfo_.vertexNormal_.clear();
	vertexInfo_.vertexUV_.clear();
	vertexInfo_.vertIndex_.clear();
	blasInfo_.isGenerate_ = false;
	hitGroupInfo_.baseTextureHandle_ = -1;
	hitGroupInfo_.mapTextureHandle_ = -1;

}

Vec3 BLAS::GetVertexLengthMax()
{

	/*===== �e�����̍ő�̒�����Ԃ� =====*/

	Vec3 vertexLength;

	if (fabs(modelInfo_.vertexMin_.x_) < fabs(modelInfo_.vertexMax_.x_)) {
		vertexLength.x_ = fabs(modelInfo_.vertexMax_.x_);
	}
	else {
		vertexLength.x_ = fabs(modelInfo_.vertexMin_.x_);
	}

	if (fabs(modelInfo_.vertexMin_.y_) < fabs(modelInfo_.vertexMax_.y_)) {
		vertexLength.y_ = fabs(modelInfo_.vertexMax_.y_);
	}
	else {
		vertexLength.y_ = fabs(modelInfo_.vertexMin_.y_);
	}

	if (fabs(modelInfo_.vertexMin_.z_) < fabs(modelInfo_.vertexMax_.z_)) {
		vertexLength.z_ = fabs(modelInfo_.vertexMax_.z_);
	}
	else {
		vertexLength.z_ = fabs(modelInfo_.vertexMin_.z_);
	}

	return vertexLength;
}

void BLAS::MulVec3Vertex(Vec3 Vec)
{

	/*===== ���ׂĂ̒��_�ɍs����������� ====*/

	// ���݂�Queue�̃C���f�b�N�X
	int currentQueueIndex = Engine::Ins()->currentQueueIndex_;

	for (auto& index_ : vertexInfo_.vertex_) {

		index_.position_ = vertexInfo_.defVertex_[&index_ - &vertexInfo_.vertex_[0]].position_ * Vec;

	}

	// �m�ۂ����o�b�t�@�ɒ��_�f�[�^���������ށB
	blasInfo_.vertexBuffer_->WriteData(currentQueueIndex, static_cast<size_t>(modelInfo_.vertexStride_ * modelInfo_.vertexCount_), vertexInfo_.vertex_.data());

}

void BLAS::AssignUV(const std::vector<RayVertex>& UV)
{

	/*===== �w���UV�������� =====*/

	// ���݂�Queue�̃C���f�b�N�X
	int currentQueueIndex = 0;

	for (auto& index : vertexInfo_.vertex_) {

		index.subUV_ = UV[static_cast<int>(&index - &vertexInfo_.vertex_[0])].uv_;

	}

	// �m�ۂ����o�b�t�@�ɒ��_�f�[�^���������ށB
	blasInfo_.vertexBuffer_->WriteData(currentQueueIndex, static_cast<size_t>(modelInfo_.vertexStride_ * modelInfo_.vertexCount_), vertexInfo_.vertex_.data());

	// ���݂�Queue�̃C���f�b�N�X
	currentQueueIndex = 1;

	// �m�ۂ����o�b�t�@�ɒ��_�f�[�^���������ށB
	blasInfo_.vertexBuffer_->WriteData(currentQueueIndex, static_cast<size_t>(modelInfo_.vertexStride_ * modelInfo_.vertexCount_), vertexInfo_.vertex_.data());

	// ���_�������������t���O�𗧂Ă�B
	hitGroupInfo_.isChangeVertex_[0] = true;
	hitGroupInfo_.isChangeVertex_[1] = true;

}

void BLAS::ChangeVertexPosition(int Index, const Vec3& Pos)
{

	/*===== ���_���̍��W�����݂̂�ύX =====*/

	vertexInfo_.vertex_[Index].position_ = Pos;
	hitGroupInfo_.isChangeVertex_[0] = true;
	hitGroupInfo_.isChangeVertex_[1] = true;

}

D3D12_RAYTRACING_GEOMETRY_DESC BLAS::GetGeometryDesc(bool IsOpaque, int Index)
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
	triangles.VertexBuffer.StartAddress = blasInfo_.vertexBuffer_->GetBuffer(Index)->GetGPUVirtualAddress();
	triangles.VertexBuffer.StrideInBytes = modelInfo_.vertexStride_;
	triangles.VertexCount = modelInfo_.vertexCount_;
	triangles.IndexBuffer = blasInfo_.indexBuffer_->GetBuffer(Index)->GetGPUVirtualAddress();
	triangles.IndexCount = modelInfo_.indexCount_;
	triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	triangles.IndexFormat = DXGI_FORMAT_R32_UINT;

	return geometryDesc;

}

void BLAS::SettingAccelerationStructure(const D3D12_RAYTRACING_GEOMETRY_DESC& GeomDesc, int Index)
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
	blasInfo_.scratchBuffer_[Index] = FHelper::CreateBuffer(
		blasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_HEAP_TYPE_DEFAULT
	);
	D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::Transition(blasInfo_.scratchBuffer_[Index].Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_UNORDERED_ACCESS) };
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, barrierToUAV);
	blasInfo_.scratchBuffer_[Index]->SetName(L"BLASScratchBuffer");

	// BLAS�̃o�b�t�@�𐶐�����B
	blasInfo_.blasBuffer_[Index] = FHelper::CreateBuffer(
		blasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);
	blasInfo_.blasBuffer_[Index]->SetName(L"BLASBuffer");

	// �X�V�p�o�b�t�@�𐶐�����B
	blasInfo_.updateBuffer_[Index] = FHelper::CreateBuffer(
		blasPrebuild.UpdateScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_HEAP_TYPE_DEFAULT
	);
	barrierToUAV[0] = { CD3DX12_RESOURCE_BARRIER::Transition(blasInfo_.updateBuffer_[Index].Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_UNORDERED_ACCESS) };
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, barrierToUAV);
	blasInfo_.updateBuffer_[Index]->SetName(L"BLASUpdateBuffer");

	// AccelerationStructure�̍\�z�B
	buildASDesc.ScratchAccelerationStructureData = blasInfo_.scratchBuffer_[Index]->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = blasInfo_.blasBuffer_[Index]->GetGPUVirtualAddress();
	// �R�}���h���X�g�ɐς�Ŏ��s����B
	Engine::Ins()->mainGraphicsCmdList_->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr /* pPostBuildInfoDescs */
	);

	// BLAS���\�z����B
	CreateAccelerationStructure(Index);

}

void BLAS::CreateAccelerationStructure(int Index)
{

	/*===== BLAS�\�z���� =====*/

	// ���\�[�X�o���A�̐ݒ�B
	D3D12_RESOURCE_BARRIER uavBarrier{};
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = blasInfo_.blasBuffer_[Index].Get();
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &uavBarrier);

}

void BLAS::CreateMaterialBuffer()
{

	/*===== �}�e���A����ݒ� =====*/

	// �}�e���A���p�o�b�t�@�𐶐�
	blasInfo_.materialBuffer_ = std::make_shared<DoubleResourceWrapper>();
	blasInfo_.materialBuffer_->CreateBuffer(sizeof(ModelDataManager::GPUMaterial), &blasInfo_.material_, L"MaterialBuffer");

	// �}�e���A���f�[�^�Ńf�B�X�N���v�^�𐶐��B
	blasInfo_.materialDescriptor_ = std::make_shared<DoubleRayDescriptor>();
	blasInfo_.materialDescriptor_->CreateSRV(blasInfo_.materialBuffer_, 1, 0, sizeof(ModelDataManager::GPUMaterial));

}

void BLAS::CreateVertexBuffer(const ModelDataManager::ObjectData& DataBuff)
{

	/*===== ���_�ƃC���f�b�N�X�o�b�t�@�𐶐� =====*/

	// ���_�������߂�B
	modelInfo_.vertexCount_ = static_cast<UINT>(DataBuff.vertex_.size());

	// ���_�C���f�b�N�X�������߂�B
	modelInfo_.indexCount_ = static_cast<UINT>(DataBuff.index_.size());

	// ���_�f�[�^��ϊ��B
	for (int index = 0; index < static_cast<int>(modelInfo_.vertexCount_); ++index) {

		RayVertex buff{};
		buff.normal_ = DataBuff.vertex_[index].normal_;
		buff.position_ = DataBuff.vertex_[index].pos_;
		buff.uv_ = DataBuff.vertex_[index].uv_;
		buff.subUV_ = DataBuff.vertex_[index].uv_;

		// �f�[�^��ۑ��B
		vertexInfo_.vertex_.push_back(buff);

	}

	// ���_�C���f�b�N�X�f�[�^��ۑ��B
	vertexInfo_.vertIndex_ = DataBuff.index_;

	// ���_�T�C�Y�����߂�B
	modelInfo_.vertexStride_ = sizeof(RayVertex);

	// ���_�C���f�b�N�X�T�C�Y�����߂�B
	modelInfo_.indexStride_ = sizeof(UINT);

	// ���_�o�b�t�@�𐶐��B
	blasInfo_.vertexBuffer_ = std::make_shared<DoubleResourceWrapper>();
	blasInfo_.vertexBuffer_->CreateBuffer(static_cast<size_t>(modelInfo_.vertexStride_ * modelInfo_.vertexCount_), vertexInfo_.vertex_.data(), L"VertexBuffer");

	// ���_�C���f�b�N�X�o�b�t�@�𐶐��B
	blasInfo_.indexBuffer_ = std::make_shared<DoubleResourceWrapper>();
	blasInfo_.indexBuffer_->CreateBuffer(static_cast<size_t>(modelInfo_.indexStride_ * modelInfo_.indexCount_), vertexInfo_.vertIndex_.data(), L"VertexIndexBuffer");

	// ���_�C���f�b�N�X�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	blasInfo_.indexDescriptor_ = std::make_shared<DoubleRayDescriptor>();
	blasInfo_.indexDescriptor_->CreateSRV(blasInfo_.indexBuffer_, modelInfo_.indexCount_, 0, modelInfo_.indexStride_);

	// ���_�f�[�^�Ńf�B�X�N���v�^�𐶐��B
	blasInfo_.vertexDescriptor_ = std::make_shared<DoubleRayDescriptor>();
	blasInfo_.vertexDescriptor_->CreateSRV(blasInfo_.vertexBuffer_, modelInfo_.vertexCount_, 0, modelInfo_.vertexStride_);

}
