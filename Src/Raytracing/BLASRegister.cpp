#include "BLASRegister.h"
#include "BLAS.h"

void BLASRegister::Setting()
{

	/*===== BLASを準備 =====*/

	for (auto& index : blas_) {

		index = std::make_shared<BLAS>();

	}

}

std::weak_ptr<BLAS> BLASRegister::GenerateObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, bool IsOpaque, bool IsNewGenerate)
{

	/*===== BLASを生成 =====*/

	// すでにロード済みかをチェックする。
	bool isLoaded = false;
	int blasIndex_ = 0;
	for (auto& index_ : blas_) {

		// 未生成だったら処理を飛ばす。
		if (!index_->GetIsGenerate()) continue;

		// モデルの名前が同じかどうかをチェックする。
		if (!(index_->GetModelPath() == DirectryPath + ModelName)) continue;

		isLoaded = true;
		blasIndex_ = static_cast<int>(&index_ - &blas_[0]);
		break;

	}

	// ロードされていたら
	if (isLoaded && !IsNewGenerate) {

		return blas_[blasIndex_];

	}
	else {

		for (auto& index : blas_) {

			if (index->GetIsGenerate()) continue;

			index->GenerateBLASObj(DirectryPath, ModelName, HitGroupName, static_cast<int>(&index - &blas_[0]), IsOpaque);

			return index;

		}

		// 要素数をオーバーしました！
		assert(0);

		return std::weak_ptr<BLAS>();

	}

}

std::weak_ptr<BLAS> BLASRegister::GenerateFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, bool IsOpaque, bool IsNewGenerate)
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

#include "FString.h"
std::weak_ptr<BLAS> BLASRegister::GenerateGLTF(const std::wstring& Path, const std::wstring& HitGroupName, bool IsOpaque, bool IsNewGenerate) {


	/*===== BLASを生成 =====*/

	// すでにロード済みかをチェックする。
	bool isLoaded = false;
	int blasIndex_ = 0;
	for (auto& index_ : blas_) {

		if (isLoaded) break;

		// 未生成だったら処理を飛ばす。
		if (!index_->GetIsGenerate()) continue;

		// モデルの名前が同じかどうかをチェックする。
		if (!(index_->GetModelPath() == FString::WStringToString(Path))) continue;

		isLoaded = true;
		blasIndex_ = static_cast<int>(&index_ - &blas_[0]);

	}

	// ロードされていたら
	if (isLoaded && !IsNewGenerate) {

		return blas_[blasIndex_];

	}
	else {

		for (auto& index : blas_) {

			if (index->GetIsGenerate()) continue;

			index->GenerateBLASGLTF(Path, HitGroupName, static_cast<int>(&index - &blas_[0]), IsOpaque);

			return index;

		}

		// 要素数をオーバーしました！
		assert(0);

		return std::weak_ptr<BLAS>();

	}


}

uint8_t* BLASRegister::WriteShaderRecord(uint8_t* Dst, UINT RecordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName)
{

	/*===== シェーダーレコードを書き込む =====*/

	for (auto& index : blas_) {
		Dst = index->WriteShaderRecord(Dst, RecordSize, StateObject, HitGroupName);
	}

	return Dst;

}