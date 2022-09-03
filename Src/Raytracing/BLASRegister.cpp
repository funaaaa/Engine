#include "BLASRegister.h"
#include "BLAS.h"

void BLASRegister::Setting()
{

	/*===== BLAS������ =====*/

	for (auto& index : blas_) {

		index = std::make_shared<BLAS>();

	}

}

int BLASRegister::GenerateObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath, const bool& IsSmoothing, const bool& IsOpaque, const bool& IsNewGenerate)
{

	/*===== BLAS�𐶐� =====*/

	// ���łɃ��[�h�ς݂����`�F�b�N����B
	bool isLoaded = false;
	int blasIndex_ = 0;
	for (auto& index_ : blas_) {

		if (isLoaded) break;

		// �������������珈�����΂��B
		if (!index_->GetIsGenerate()) continue;

		// ���f���̖��O���������ǂ������`�F�b�N����B
		if (!(index_->GetModelPath() == DirectryPath + ModelName)) continue;

		// �e�N�X�`�������������ǂ������`�F�b�N����B
		std::vector<LPCWSTR> blasTexture = index_->GetTexturePath();
		for (auto& textureIndex : TexturePath) {

			// �e�N�X�`�������������ǂ������`�F�b�N����B
			if (!(textureIndex == blasTexture[&textureIndex - &TexturePath[0]])) continue;

			isLoaded = true;
			blasIndex_ = static_cast<int>(&index_ - &blas_[0]);


		}

	}

	// ���[�h����Ă�����
	if (isLoaded && !IsNewGenerate) {

		return blasIndex_;

	}
	else {

		for (auto& index : blas_) {

			if (index->GetIsGenerate()) continue;

			index->GenerateBLASObj(DirectryPath, ModelName, HitGroupName, TexturePath, IsSmoothing, IsOpaque);

			return static_cast<int>(&index - &blas_[0]);

		}

		// �v�f�����I�[�o�[���܂����I
		assert(0);

		return 0;

	}

}

int BLASRegister::GenerateFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath)
{

	/*===== BLAS�𐶐� =====*/

	for (auto& index : blas_) {

		if (index->GetIsGenerate()) continue;

		index->GenerateBLASFbx(DirectryPath, ModelName, HitGroupName, TexturePath);

		return static_cast<int>(&index - &blas_[0]);

	}

	// �v�f�����I�[�o�[���܂����I
	assert(0);

	return 0;

}

int BLASRegister::GenerateData(ModelDataManager::ObjectData Data, const std::wstring& HitGroupName, std::vector<int> TextureHandle, const bool& IsOpaque)
{

	/*===== BLAS�𐶐� =====*/

	for (auto& index : blas_) {

		if (index->GetIsGenerate()) continue;

		index->GenerateBLASData(Data, HitGroupName, TextureHandle, IsOpaque);

		return static_cast<int>(&index - &blas_[0]);

	}

	// �v�f�����I�[�o�[���܂����I
	assert(0);

	return 0;

}

void BLASRegister::Update(const int& Index)
{

	/*===== �w���BLAS���X�V =====*/

	blas_[Index]->Update();

}

void BLASRegister::ChangeTex(const int& BlasIndex, const int& TexIndex, const int& TexHandle)
{

	/*===== �e�N�X�`����ύX =====*/

	// �͈͊O����Ȃ������`�F�b�N�B
	if (BlasIndex < 0 || blas_.size() <= BlasIndex) assert(0);

	blas_[BlasIndex]->ChangeTex(TexIndex, TexHandle);

}

void BLASRegister::ComputeSkin(const int& Index)
{

	/*===== �w���BLAS�̃X�L�j���O�A�j���[�V�������v�Z =====*/

	blas_[Index]->ComputeSkin();

}

void BLASRegister::InitAnimation(const int& Index)
{

	/*===== �w���BLAS�̃X�L�j���O�A�j���[�V������������ =====*/

	blas_[Index]->InitAnimation();

}

void BLASRegister::PlayAnimation(const int& Index)
{

	/*===== �w���BLAS�̃X�L�j���O�A�j���[�V�������J�n =====*/

	blas_[Index]->PlayAnimation();

}

void BLASRegister::StopAnimation(const int& Index)
{

	/*===== �w���BLAS�̃X�L�j���O�A�j���[�V�������~ =====*/

	blas_[Index]->StopAnimation();

}

uint8_t* BLASRegister::WriteShaderRecord(uint8_t* Dst, const int& Index, UINT RecordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName)
{

	/*===== �V�F�[�_�[���R�[�h���������� =====*/

	return blas_[Index]->WriteShaderRecord(Dst, RecordSize, StateObject, HitGroupName);

}

Microsoft::WRL::ComPtr<ID3D12Resource>& BLASRegister::GetBLASBuffer(const int& Index)
{
	return blas_[Index]->GetBLASBuffer();
}

void BLASRegister::MulVec3Vertex(const int& Index, Vec3 Vec)
{

	/*===== �S�Ă̒��_���ɍs����������� =====*/

	blas_[Index]->MulVec3Vertex(Vec);

}

const Vec3& BLASRegister::GetVertexMin(const int& Index)
{

	/*===== ���_�̍ŏ��̒l���擾 =====*/

	return blas_[Index]->GetVertexMin();

}

const Vec3& BLASRegister::GetVertexMax(const int& Index)
{

	/*===== ���_�̍ő�̒l���擾 =====*/

	return blas_[Index]->GetVertexMax();

}

Vec3 BLASRegister::GetVertexLengthMax(const int& Index)
{
	return blas_[Index]->GetVertexLengthMax();
}

const std::vector<Vec3>& BLASRegister::GetVertex(const int& Index)
{
	return blas_[Index]->GetVertexPos();
}

const std::vector<Vec3>& BLASRegister::GetNormal(const int& Index)
{
	return blas_[Index]->GetVertexNormal();
}

const std::vector<Vec2>& BLASRegister::GetUV(const int& Index)
{
	return blas_[Index]->GetVertexUV();
}

const std::vector<UINT>& BLASRegister::GetVertexIndex(const int& Index)
{
	return blas_[Index]->GetVertexIndex();
}

void BLASRegister::DeleteIndex(const int& Index)
{

	/*===== �w��̗v�f���폜 =====*/

	// �͈͓����`�F�b�N����B
	if (Index < 0 || BLAS_COUNT <= Index) assert(0);

	blas_[Index]->Init();

}
