#include "BLASRegister.h"
#include "BLAS.h"

int BLASRegister::GenerateBLASObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const LPCWSTR& TexturePath)
{

	/*===== BLASを生成 =====*/

	// 最後尾にBLASを追加。
	blas.emplace_back(std::make_shared<BLAS>());

	// 最後尾のインデックスを取得。
	int index = blas.size() - 1;

	// BLASを生成。
	blas[index]->GenerateBLASObj(DirectryPath, ModelName, HitGroupName, TexturePath);

	return index;

}

int BLASRegister::GenerateBLASFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const LPCWSTR& TexturePath)
{

	/*===== BLASを生成 =====*/

	// 最後尾にBLASを追加。
	blas.emplace_back(std::make_shared<BLAS>());

	// 最後尾のインデックスを取得。
	int index = blas.size() - 1;

	// BLASを生成。
	blas[index]->GenerateBLASFbx(DirectryPath, ModelName, HitGroupName, TexturePath);

	return index;

}

void BLASRegister::Update(const int& Index)
{

	/*===== BLASを更新 =====*/

	blas[Index]->Update();

}

void BLASRegister::ComputeSkin(const int& Index)
{

	/*===== BLASのスキニングアニメーションを計算 =====*/

	blas[Index]->ComputeSkin();

}

void BLASRegister::InitAnimation(const int& Index)
{

	/*===== BLASのスキニングアニメーションを初期化 =====*/

	blas[Index]->PlayAnimation();

}

void BLASRegister::PlayAnimation(const int& Index)
{

	/*===== BLASのスキニングアニメーションを開始 =====*/

	blas[Index]->InitAnimation();

}

void BLASRegister::StopAnimation(const int& Index)
{

	/*===== BLASのスキニングアニメーションを停止 =====*/

	blas[Index]->StopAnimation();

}
