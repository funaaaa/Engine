#include "BLASRegister.h"
#include "BLAS.h"

void BLASRegister::Setting()
{

	/*===== BLASを準備 =====*/

	for (auto& index : blas_) {

		index = std::make_shared<BLAS>();

	}

}

std::weak_ptr<BLAS> BLASRegister::GenerateObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath, const bool& IsSmoothing, const bool& IsOpaque, const bool& IsNewGenerate)
{

	/*===== BLASを生成 =====*/

	// すでにロード済みかをチェックする。
	bool isLoaded = false;
	int blasIndex_ = 0;
	for (auto& index_ : blas_) {

		if (isLoaded) break;

		// 未生成だったら処理を飛ばす。
		if (!index_->GetIsGenerate()) continue;

		// モデルの名前が同じかどうかをチェックする。
		if (!(index_->GetModelPath() == DirectryPath + ModelName)) continue;

		// テクスチャ名が同じかどうかをチェックする。
		std::vector<LPCWSTR> blasTexture = index_->GetTexturePath();
		for (auto& textureIndex : TexturePath) {

			// テクスチャ名が同じかどうかをチェックする。
			if (!(textureIndex == blasTexture[&textureIndex - &TexturePath[0]])) continue;

			isLoaded = true;
			blasIndex_ = static_cast<int>(&index_ - &blas_[0]);


		}

	}

	// ロードされていたら
	if (isLoaded && !IsNewGenerate) {

		return blas_[blasIndex_];

	}
	else {

		for (auto& index : blas_) {

			if (index->GetIsGenerate()) continue;

			index->GenerateBLASObj(DirectryPath, ModelName, HitGroupName, static_cast<int>(&index - &blas_[0]), TexturePath, IsSmoothing, IsOpaque);

			return index;

		}

		// 要素数をオーバーしました！
		assert(0);

		return std::weak_ptr<BLAS>();

	}

}

std::weak_ptr<BLAS> BLASRegister::GenerateFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, const bool& IsOpaque, const bool& IsNewGenerate)
{

	/*===== BLASを生成 =====*/


	//// すでにロード済みかをチェックする。
	//bool isLoaded = false;
	//int blasIndex_ = 0;
	//for (auto& index_ : blas_) {

	//	if (isLoaded) break;

	//	// 未生成だったら処理を飛ばす。
	//	if (!index_->GetIsGenerate()) continue;

	//	// モデルの名前が同じかどうかをチェックする。
	//	if (!(index_->GetModelPath() == DirectryPath + ModelName)) continue;

	//	isLoaded = true;
	//	blasIndex_ = static_cast<int>(&index_ - &blas_[0]);

	//}

	//// ロードされていたら
	//if (isLoaded && !IsNewGenerate) {

	//	return blas_[blasIndex_];

	//}
	//else {

	for (auto& index : blas_) {

		if (index->GetIsGenerate()) continue;

		index->GenerateBLASFbx(DirectryPath, ModelName, HitGroupName, static_cast<int>(&index - &blas_[0]), IsOpaque);

		return index;

	}

	// 要素数をオーバーしました！
	assert(0);

	return std::weak_ptr<BLAS>();

	//}

}

std::weak_ptr<BLAS> BLASRegister::GenerateData(ModelDataManager::ObjectData Data, const std::wstring& HitGroupName, std::vector<int> TextureHandle, const bool& IsOpaque)
{

	/*===== BLASを生成 =====*/

	for (auto& index : blas_) {

		if (index->GetIsGenerate()) continue;

		index->GenerateBLASData(Data, HitGroupName, static_cast<int>(&index - &blas_[0]), TextureHandle, IsOpaque);

		return index;

	}

	// 要素数をオーバーしました！
	assert(0);

	return std::weak_ptr<BLAS>();

}

uint8_t* BLASRegister::WriteShaderRecord(uint8_t* Dst, const int& Index, UINT RecordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName)
{

	/*===== シェーダーレコードを書き込む =====*/

	return blas_[Index]->WriteShaderRecord(Dst, RecordSize, StateObject, HitGroupName);

}