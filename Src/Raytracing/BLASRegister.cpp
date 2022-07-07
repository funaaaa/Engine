#include "BLASRegister.h"
#include "BLAS.h"

int BLASRegister::GenerateObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath, const bool& IsSmoothing, const bool& IsOpaque)
{

	/*===== BLAS�𐶐� =====*/

	// ���łɃ��[�h�ς݂����`�F�b�N����B
	bool isLoaded = false;
	int blasIndex = 0;
	for (auto& index : blas) {

		// ���f���̖��O���������ǂ������`�F�b�N����B
		if (!(index->GetModelPath() == DirectryPath + ModelName)) continue;

		// �e�N�X�`�������������ǂ������`�F�b�N����B
		std::vector<LPCWSTR> blasTexture = index->GetTexturePath();
		for (auto& textureIndex : TexturePath) {

			// �e�N�X�`�������������ǂ������`�F�b�N����B
			if (!(textureIndex == blasTexture[&textureIndex - &TexturePath[0]])) continue;

			isLoaded = true;
			blasIndex = &index - &blas[0];

		}

	}

	// ���[�h����Ă�����
	if (isLoaded) {

		return blasIndex;

	}
	else {

		blas.push_back(std::make_shared<BLAS>());
		blas.back()->GenerateBLASObj(DirectryPath, ModelName, HitGroupName, TexturePath, IsSmoothing, IsOpaque);

		return static_cast<int>(blas.size()) - 1;

	}

}

int BLASRegister::GenerateFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath)
{

	/*===== BLAS�𐶐� =====*/

	blas.push_back(std::make_shared<BLAS>());
	blas.back()->GenerateBLASFbx(DirectryPath, ModelName, HitGroupName, TexturePath);

	return static_cast<int>(blas.size()) - 1;

}

int BLASRegister::GenerateData(ModelDataManager::ObjectData Data, const std::wstring& HitGroupName, std::vector<int> TextureHandle, const bool& IsOpaque)
{

	/*===== BLAS�𐶐� =====*/

	blas.push_back(std::make_shared<BLAS>());
	blas.back()->GenerateBLASData(Data, HitGroupName, TextureHandle, IsOpaque);

	return static_cast<int>(blas.size()) - 1;

}

void BLASRegister::Update(const int& Index)
{

	/*===== �w���BLAS���X�V =====*/

	blas[Index]->Update();

}

void BLASRegister::ComputeSkin(const int& Index)
{

	/*===== �w���BLAS�̃X�L�j���O�A�j���[�V�������v�Z =====*/

	blas[Index]->ComputeSkin();

}

void BLASRegister::InitAnimation(const int& Index)
{

	/*===== �w���BLAS�̃X�L�j���O�A�j���[�V������������ =====*/

	blas[Index]->InitAnimation();

}

void BLASRegister::PlayAnimation(const int& Index)
{

	/*===== �w���BLAS�̃X�L�j���O�A�j���[�V�������J�n =====*/

	blas[Index]->PlayAnimation();

}

void BLASRegister::StopAnimation(const int& Index)
{

	/*===== �w���BLAS�̃X�L�j���O�A�j���[�V�������~ =====*/

	blas[Index]->StopAnimation();

}

uint8_t* BLASRegister::WriteShaderRecord(uint8_t* Dst, const int& Index, UINT RecordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName)
{

	/*===== �V�F�[�_�[���R�[�h���������� =====*/

	return blas[Index]->WriteShaderRecord(Dst, RecordSize, StateObject, HitGroupName);

}

Microsoft::WRL::ComPtr<ID3D12Resource>& BLASRegister::GetBLASBuffer(const int& Index)
{
	return blas[Index]->GetBLASBuffer();
}
