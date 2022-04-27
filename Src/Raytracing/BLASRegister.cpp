#include "BLASRegister.h"
#include "BLAS.h"

int BLASRegister::GenerateBLASObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const LPCWSTR& TexturePath)
{

	/*===== BLAS�𐶐� =====*/

	// �Ō����BLAS��ǉ��B
	blas.emplace_back(std::make_shared<BLAS>());

	// �Ō���̃C���f�b�N�X���擾�B
	int index = blas.size() - 1;

	// BLAS�𐶐��B
	blas[index]->GenerateBLASObj(DirectryPath, ModelName, HitGroupName, TexturePath);

	return index;

}

int BLASRegister::GenerateBLASFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const LPCWSTR& TexturePath)
{

	/*===== BLAS�𐶐� =====*/

	// �Ō����BLAS��ǉ��B
	blas.emplace_back(std::make_shared<BLAS>());

	// �Ō���̃C���f�b�N�X���擾�B
	int index = blas.size() - 1;

	// BLAS�𐶐��B
	blas[index]->GenerateBLASFbx(DirectryPath, ModelName, HitGroupName, TexturePath);

	return index;

}

void BLASRegister::Update(const int& Index)
{

	/*===== BLAS���X�V =====*/

	blas[Index]->Update();

}

void BLASRegister::ComputeSkin(const int& Index)
{

	/*===== BLAS�̃X�L�j���O�A�j���[�V�������v�Z =====*/

	blas[Index]->ComputeSkin();

}

void BLASRegister::InitAnimation(const int& Index)
{

	/*===== BLAS�̃X�L�j���O�A�j���[�V������������ =====*/

	blas[Index]->PlayAnimation();

}

void BLASRegister::PlayAnimation(const int& Index)
{

	/*===== BLAS�̃X�L�j���O�A�j���[�V�������J�n =====*/

	blas[Index]->InitAnimation();

}

void BLASRegister::StopAnimation(const int& Index)
{

	/*===== BLAS�̃X�L�j���O�A�j���[�V�������~ =====*/

	blas[Index]->StopAnimation();

}
