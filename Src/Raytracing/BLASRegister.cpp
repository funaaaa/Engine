#include "BLASRegister.h"
#include "BLAS.h"

void BLASRegister::Setting()
{

	/*===== BLASを準備 =====*/

	for (auto& index : blas_) {

		index = std::make_shared<BLAS>();

	}

}

int BLASRegister::GenerateObj(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath, const bool& IsSmoothing, const bool& IsOpaque, const bool& IsNewGenerate)
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

		return blasIndex_;

	}
	else {

		for (auto& index : blas_) {

			if (index->GetIsGenerate()) continue;

			index->GenerateBLASObj(DirectryPath, ModelName, HitGroupName, TexturePath, IsSmoothing, IsOpaque);

			return static_cast<int>(&index - &blas_[0]);

		}

		// 要素数をオーバーしました！
		assert(0);

		return 0;

	}

}

int BLASRegister::GenerateFbx(const std::string& DirectryPath, const std::string& ModelName, const std::wstring& HitGroupName, std::vector<LPCWSTR> TexturePath)
{

	/*===== BLASを生成 =====*/

	for (auto& index : blas_) {

		if (index->GetIsGenerate()) continue;

		index->GenerateBLASFbx(DirectryPath, ModelName, HitGroupName, TexturePath);

		return static_cast<int>(&index - &blas_[0]);

	}

	// 要素数をオーバーしました！
	assert(0);

	return 0;

}

int BLASRegister::GenerateData(ModelDataManager::ObjectData Data, const std::wstring& HitGroupName, std::vector<int> TextureHandle, const bool& IsOpaque)
{

	/*===== BLASを生成 =====*/

	for (auto& index : blas_) {

		if (index->GetIsGenerate()) continue;

		index->GenerateBLASData(Data, HitGroupName, TextureHandle, IsOpaque);

		return static_cast<int>(&index - &blas_[0]);

	}

	// 要素数をオーバーしました！
	assert(0);

	return 0;

}

void BLASRegister::Update(const int& Index)
{

	/*===== 指定のBLASを更新 =====*/

	blas_[Index]->Update();

}

void BLASRegister::ChangeTex(const int& BlasIndex, const int& TexIndex, const int& TexHandle)
{

	/*===== テクスチャを変更 =====*/

	// 範囲外じゃないかをチェック。
	if (BlasIndex < 0 || blas_.size() <= BlasIndex) assert(0);

	blas_[BlasIndex]->ChangeTex(TexIndex, TexHandle);

}

void BLASRegister::ComputeSkin(const int& Index)
{

	/*===== 指定のBLASのスキニングアニメーションを計算 =====*/

	blas_[Index]->ComputeSkin();

}

void BLASRegister::InitAnimation(const int& Index)
{

	/*===== 指定のBLASのスキニングアニメーションを初期化 =====*/

	blas_[Index]->InitAnimation();

}

void BLASRegister::PlayAnimation(const int& Index)
{

	/*===== 指定のBLASのスキニングアニメーションを開始 =====*/

	blas_[Index]->PlayAnimation();

}

void BLASRegister::StopAnimation(const int& Index)
{

	/*===== 指定のBLASのスキニングアニメーションを停止 =====*/

	blas_[Index]->StopAnimation();

}

uint8_t* BLASRegister::WriteShaderRecord(uint8_t* Dst, const int& Index, UINT RecordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& StateObject, LPCWSTR HitGroupName)
{

	/*===== シェーダーレコードを書き込む =====*/

	return blas_[Index]->WriteShaderRecord(Dst, RecordSize, StateObject, HitGroupName);

}

Microsoft::WRL::ComPtr<ID3D12Resource>& BLASRegister::GetBLASBuffer(const int& Index)
{
	return blas_[Index]->GetBLASBuffer();
}

void BLASRegister::MulVec3Vertex(const int& Index, Vec3 Vec)
{

	/*===== 全ての頂点情報に行列情報をかける =====*/

	blas_[Index]->MulVec3Vertex(Vec);

}

const Vec3& BLASRegister::GetVertexMin(const int& Index)
{

	/*===== 頂点の最小の値を取得 =====*/

	return blas_[Index]->GetVertexMin();

}

const Vec3& BLASRegister::GetVertexMax(const int& Index)
{

	/*===== 頂点の最大の値を取得 =====*/

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

	/*===== 指定の要素を削除 =====*/

	// 範囲内かチェックする。
	if (Index < 0 || BLAS_COUNT <= Index) assert(0);

	blas_[Index]->Init();

}
