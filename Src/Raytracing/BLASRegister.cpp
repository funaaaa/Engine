#include "BLASRegister.h"
#include "BLAS.h"

const int& BLASRegister::GenerateObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const LPCWSTR& TexturePath)
{

	/*===== BLAS�𐶐� =====*/

	blas.push_back(std::make_shared<BLAS>());
	blas.back()->GenerateBLASObj(DirectryPath, ModelName, HitGroupName, TexturePath);

	return blas.size() - 1;

}

const int& BLASRegister::GenerateFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const LPCWSTR& TexturePath)
{

	/*===== BLAS�𐶐� =====*/

	blas.push_back(std::make_shared<BLAS>());
	blas.back()->GenerateBLASFbx(DirectryPath, ModelName, HitGroupName, TexturePath);

	return blas.size() - 1;

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

uint8_t* BLASRegister::WriteShaderRecord(uint8_t* Dst, const int& Index, UINT RecordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject)
{

	/*===== �V�F�[�_�[���R�[�h���������� =====*/

	return blas[Index]->WriteShaderRecord(Dst, RecordSize, StateObject);

}

Microsoft::WRL::ComPtr<ID3D12Resource>& BLASRegister::GetBLASBuffer(const int& Index)
{
	return blas[Index]->GetBLASBuffer();
}