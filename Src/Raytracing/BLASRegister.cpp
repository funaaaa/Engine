#include "BLASRegister.h"
#include "BLAS.h"

int BLASRegister::GenerateObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath, const bool& IsSmoothing, const bool& IsOpaque)
{

	/*===== BLASを生成 =====*/

	// すでにロード済みかをチェックする。
	bool isLoaded = false;
	int blasIndex = 0;
	for (auto& index : blas) {

		// モデルの名前が同じかどうかをチェックする。
		if (!(index->GetModelPath() == DirectryPath + ModelName)) continue;

		// テクスチャ名が同じかどうかをチェックする。
		std::vector<LPCWSTR> blasTexture = index->GetTexturePath();
		for (auto& textureIndex : TexturePath) {

			// テクスチャ名が同じかどうかをチェックする。
			if (!(textureIndex == blasTexture[&textureIndex - &TexturePath[0]])) continue;

			isLoaded = true;
			blasIndex = &index - &blas[0];

		}

	}

	// ロードされていたら
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

	/*===== BLASを生成 =====*/

	blas.push_back(std::make_shared<BLAS>());
	blas.back()->GenerateBLASFbx(DirectryPath, ModelName, HitGroupName, TexturePath);

	return static_cast<int>(blas.size()) - 1;

}

int BLASRegister::GenerateData(ModelDataManager::ObjectData Data, const std::wstring& HitGroupName, std::vector<int> TextureHandle, const bool& IsOpaque)
{

	/*===== BLASを生成 =====*/

	blas.push_back(std::make_shared<BLAS>());
	blas.back()->GenerateBLASData(Data, HitGroupName, TextureHandle, IsOpaque);

	return static_cast<int>(blas.size()) - 1;

}

void BLASRegister::Update(const int& Index)
{

	/*===== 指定のBLASを更新 =====*/

	blas[Index]->Update();

}

void BLASRegister::ComputeSkin(const int& Index)
{

	/*===== 指定のBLASのスキニングアニメーションを計算 =====*/

	blas[Index]->ComputeSkin();

}

void BLASRegister::InitAnimation(const int& Index)
{

	/*===== 指定のBLASのスキニングアニメーションを初期化 =====*/

	blas[Index]->InitAnimation();

}

void BLASRegister::PlayAnimation(const int& Index)
{

	/*===== 指定のBLASのスキニングアニメーションを開始 =====*/

	blas[Index]->PlayAnimation();

}

void BLASRegister::StopAnimation(const int& Index)
{

	/*===== 指定のBLASのスキニングアニメーションを停止 =====*/

	blas[Index]->StopAnimation();

}

uint8_t* BLASRegister::WriteShaderRecord(uint8_t* Dst, const int& Index, UINT RecordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName)
{

	/*===== シェーダーレコードを書き込む =====*/

	return blas[Index]->WriteShaderRecord(Dst, RecordSize, StateObject, HitGroupName);

}

Microsoft::WRL::ComPtr<ID3D12Resource>& BLASRegister::GetBLASBuffer(const int& Index)
{
	return blas[Index]->GetBLASBuffer();
}
