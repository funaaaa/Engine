#include "ModelDataManager.h"
#include <array>
#pragma warning(push)
#pragma warning(disable:4099)
#pragma warning(disable:4023)
#include "Fbxsdk.h"
#pragma warning(pop)
#include <sstream>
#include <fstream>
#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)
#include "GLTF.h"

void ModelDataManager::LoadObj(std::string DirectryPath, std::string FileName, ObjectData& ObjectBuffer, bool IsSmoothing)
{
	// objファイルがロード済みかどうか
	bool isLoad = false;
	// ロード済みだった場合、何番目の要素に保存されているのかを取得する変数
	int dataNumber = 0;

	const int MODELDATA_SIZE = static_cast<int>(modelData_.size());
	for (int index = 0; index < MODELDATA_SIZE; ++index) {
		if (modelData_[index].modelName_ == DirectryPath + FileName && modelData_[index].isSmoothing_ == IsSmoothing) {
			isLoad = true;
			dataNumber = index;
			break;
		}
	}

	// objファイルが未ロードだったらロードする
	if (!isLoad) {
		// 同じ頂点を指し示している頂点インデックスを保存しておくためのマップ
		std::map<unsigned short, std::vector<unsigned short>> smoothData;

		modelData_.push_back({});
		ModelDataManager::modelData_.at(ModelDataManager::modelData_.size() - 1).modelName_ = DirectryPath + FileName;
		ModelDataManager::modelData_.at(ModelDataManager::modelData_.size() - 1).isSmoothing_ = IsSmoothing;

		// 変数を初期化。
		ObjectBuffer.vertexMax_ = Vec3();
		ObjectBuffer.vertexMin_ = Vec3();

		// フィルストリーム
		std::ifstream file;
		// .objファイルを開く
		file.open(DirectryPath + FileName);
		// ファイルオープン失敗をチェック
		if (file.fail()) {
			assert(0);
		}
		// 一行ずつ読み込む
		std::string line;
		std::vector<Vec3> position_;			// 座標を保存しておく
		std::vector<Vec2> uv_;				// uvを保存しておく
		std::vector<Vec3> normal_;			// 法線ベクトルを保存しておく
		std::vector<unsigned short> index_;
		bool isFace = false;	// faceが存在したかどうかのフラグ
		while (getline(file, line)) {
			// 1行分の文字列をストリームに変換して解析しやすくする
			std::istringstream lineStream(line);
			// 半角スペース区切りで行の先頭文字を取得
			std::string key;
			std::getline(lineStream, key, ' ');

			// 先頭文字がvなら頂点座標
			if (key == "v") {
				// X,Y,Z座標読み込み
				Vec3 pos_{};
				lineStream >> pos_.x_;
				lineStream >> pos_.y_;
				lineStream >> pos_.z_;
				// 座標を一旦保存
				position_.push_back(pos_);
				// 頂点の最大最小要素を保存。
				SaveVertexMinMaxInfo(ObjectBuffer, pos_);
				modelData_[(static_cast<int>(modelData_.size()) - 1)].vertexMin_ = ObjectBuffer.vertexMin_;
				modelData_[(static_cast<int>(modelData_.size()) - 1)].vertexMax_ = ObjectBuffer.vertexMax_;
			}
			// 先頭文字がvtならテクスチャ
			if (key == "vt") {
				// U,V成分読み込み
				Vec2 texcoord{};
				lineStream >> texcoord.x_;
				lineStream >> texcoord.y_;
				// V方向反転
				texcoord.y_ = 1.0f - texcoord.y_;

				// テクスチャ座標データに追加
				uv_.push_back(texcoord);
			}
			// 先頭文字がvnなら法線ベクトル
			if (key == "vn") {
				// X,Y,Z成分読み込み
				Vec3 norm{};
				lineStream >> norm.x_;
				lineStream >> norm.y_;
				lineStream >> norm.z_;
				// 法線ベクトルデータに追加
				normal_.push_back(norm);
			}
			// 先頭文字がfならポリゴン(三角形)
			if (key == "f") {
				isFace = true;
				// 半角スペース区切りで行の続きを読み込む
				std::string indexString;
				unsigned short indexPosition;		// 受け皿
				unsigned short indexNormal;			// 受け皿
				unsigned short indexTexcoord;		// 受け皿
				while (std::getline(lineStream, indexString, ' ')) {
					// 頂点インデックス一個分の文字列をストリームに変換して解析しやすくする
					std::istringstream indexStream(indexString);
					indexStream >> indexPosition;
					indexStream.seekg(1, std::ios_base::cur);	// スラッシュを飛ばす
					indexStream >> indexTexcoord;
					indexStream.seekg(1, std::ios_base::cur);	// スラッシュを飛ばす
					indexStream >> indexNormal;
					// 頂点データの追加
					Vertex vert{};
					vert.pos_ = position_[indexPosition - 1];
					vert.normal_ = normal_[indexNormal - 1];
					vert.uv_ = uv_[indexTexcoord - 1];
					// モデルデータに追加
					modelData_[(static_cast<int>(modelData_.size()) - 1)].vertex_.push_back(vert);
					modelData_[(static_cast<int>(modelData_.size()) - 1)].index_.push_back(static_cast<unsigned short>(modelData_[(static_cast<int>(modelData_.size())) - 1].index_.size()));
					// proSpriteにも追加
					ObjectBuffer.vertex_.push_back(vert);
					ObjectBuffer.index_.push_back(static_cast<int>(ObjectBuffer.index_.size()));
					// isSmoothingがtrueなら頂点情報を追加する
					if (IsSmoothing == true) {
						smoothData[indexPosition].push_back(static_cast<unsigned short>(ObjectBuffer.vertex_.size()) - 1);
					}
				}
			}
			// 先頭文字列がmtllibならマテリアル
			if (key == "mtllib") {
				// マテリアルのファイル名読み込み
				std::string materialFileName;
				lineStream >> materialFileName;
				// マテリアルの読み込み
				LoadObjMaterial(DirectryPath, DirectryPath + materialFileName, modelData_.at(modelData_.size() - 1), ObjectBuffer);
				ObjectBuffer.material_ = modelData_[modelData_.size() - 1].material_;
				ObjectBuffer.material_.textureHandle_ = ObjectBuffer.textureHandle_;
				modelData_[modelData_.size() - 1].material_.textureHandle_ = ObjectBuffer.textureHandle_;

			}
		}
		// ファイルを閉じる
		file.close();

		// isSmoothingがtrueだったら法線情報をなめらかにする
		if (IsSmoothing) {
			CalculateSmoothedVertexNormals(smoothData, ObjectBuffer, modelData_.at(modelData_.size() - 1));
		}

		// faceが存在しなかったら頂点データを保存する。
		if (!isFace) {
			for (auto& index : position_) {
				ObjectBuffer.vertex_.emplace_back();
				ObjectBuffer.vertex_.back().pos_ = index;
			}
			for (auto& index : position_) {
				modelData_[(static_cast<int>(modelData_.size()) - 1)].vertex_.emplace_back();
				modelData_[(static_cast<int>(modelData_.size()) - 1)].vertex_.back().pos_ = index;
			}
		}

		return;
	}

	// objのデータをObjectDataに入れる
	for (auto& index_ : modelData_[dataNumber].index_) {
		ObjectBuffer.index_.push_back(index_);
	}
	for (auto& index_ : modelData_[dataNumber].vertex_) {
		ObjectBuffer.vertex_.push_back(index_);
	}
	ObjectBuffer.textureHandle_ = modelData_[dataNumber].material_.textureHandle_;
	ObjectBuffer.material_ = modelData_[dataNumber].material_;
	ObjectBuffer.vertexMin_ = modelData_[dataNumber].vertexMin_;
	ObjectBuffer.vertexMax_ = modelData_[dataNumber].vertexMax_;

}

#include "FString.h"
void ModelDataManager::LoadGLTF(std::wstring Path, ObjectData& ObjectBuffer) {

	/*===== GLTFをロード =====*/

	// gltfファイルがロード済みかどうか
	bool isLoad = false;
	// ロード済みだった場合、何番目の要素に保存されているのかを取得する変数
	int dataNumber = 0;

	const int MODELDATA_SIZE = static_cast<int>(modelData_.size());
	for (int index = 0; index < MODELDATA_SIZE; ++index) {
		if (modelData_[index].modelName_ == FString::WStringToString(Path)) {
			isLoad = true;
			dataNumber = index;
			break;
		}
	}

	// gltfファイルが未ロードだったらロードする
	if (!isLoad) {

		modelData_.push_back({});
		ModelDataManager::modelData_.at(ModelDataManager::modelData_.size() - 1).modelName_ = FString::WStringToString(Path);

		gltf_.emplace_back(GLTF());
		gltf_.back().LoadModel(Path);

		// モデルのデータを取得。
		GLTF::VertexAttributeVisitor vertexInfo = gltf_.back().GetVertexData();
		filePath_.emplace_back(gltf_.back().GetFileName());
		for (auto& index : vertexInfo.positionBuffer) {

			Vertex buff;
			buff.pos_ = index;
			buff.normal_ = vertexInfo.normalBuffer[static_cast<int>(&index - &vertexInfo.positionBuffer[0])];
			buff.uv_ = vertexInfo.texcoordBuffer[static_cast<int>(&index - &vertexInfo.positionBuffer[0])];

			// 頂点の最大と最小を保存しておく。
			SaveVertexMinMaxInfo(ObjectBuffer, buff.pos_);
			SaveVertexMinMaxInfo(ObjectBuffer, buff.pos_);

			ObjectBuffer.vertex_.emplace_back(buff);
			modelData_.back().vertex_.emplace_back(buff);

		}
		for (auto& index : vertexInfo.indexBuffer) {

			ObjectBuffer.index_.emplace_back(index);
			modelData_.back().index_.emplace_back(index);

		}
		ObjectBuffer.textureHandle_ = gltf_.back().GetTextureIndex();

		// マテリアルを保存。
		ObjectBuffer.material_.baseColor_ = gltf_.back().GetMaterial().baseColor_;
		ObjectBuffer.material_.metalness_ = gltf_.back().GetMaterial().metalness_;
		ObjectBuffer.material_.roughness_ = gltf_.back().GetMaterial().roughness_;
		ObjectBuffer.material_.specular_ = gltf_.back().GetMaterial().specular_;

		modelData_.back().material_.baseColor_ = gltf_.back().GetMaterial().baseColor_;
		modelData_.back().material_.metalness_ = gltf_.back().GetMaterial().metalness_;
		modelData_.back().material_.roughness_ = gltf_.back().GetMaterial().roughness_;
		modelData_.back().material_.specular_ = gltf_.back().GetMaterial().specular_;
		modelData_.back().material_.textureHandle_ = ObjectBuffer.textureHandle_;


		return;

	}

	// gltfのデータをObjectDataに入れる
	for (auto& index_ : modelData_[dataNumber].index_) {
		ObjectBuffer.index_.push_back(index_);
	}
	for (auto& index_ : modelData_[dataNumber].vertex_) {
		ObjectBuffer.vertex_.push_back(index_);
	}
	ObjectBuffer.textureHandle_ = modelData_[dataNumber].material_.textureHandle_;
	ObjectBuffer.material_ = modelData_[dataNumber].material_;
	ObjectBuffer.vertexMin_ = modelData_[dataNumber].vertexMin_;
	ObjectBuffer.vertexMax_ = modelData_[dataNumber].vertexMax_;

}

#include "TextureManager.h"
void ModelDataManager::LoadObjMaterial(std::string DirectoryPath, const std::string& MaterialFileName, ModelData& ModelData, ObjectData& ObjectBuffer)
{

	// ファイルストリーム
	std::ifstream file;
	// マテリアルファイルを開く
	file.open(MaterialFileName);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}
	// 一行ずつ読み込む
	std::string line;
	while (getline(file, line)) {
		// 位置行分の文字列をストリームに変換
		std::istringstream lineStream(line);
		// 半角スペース区切りで行の先頭文字列を取得
		std::string key;
		std::getline(lineStream, key, ' ');
		// 先頭のタブ文字は無視する
		if (key[0] == '\t') {
			key.erase(key.begin());	// 先頭の文字を削除
		}
		// 先頭文字列がnewmtlならマテリアル名
		if (key == "newmtl") {
			// マテリアル名の読み込み
			// lineStream >> ModelData.material.name;
		}
		//// 先頭文字列がKaならアンビエント色
		//if (key == "Ka") {
		//	lineStream >> ModelData.material_.ambient_.x_;
		//	lineStream >> ModelData.material_.ambient_.y_;
		//	lineStream >> ModelData.material_.ambient_.z_;
		//}
		//// 先頭文字列がKdならディフューズ色
		if (key == "Kd") {
			lineStream >> ModelData.material_.roughness_;
			//	lineStream >> ModelData.material_.diffuse_.x_;
			//	lineStream >> ModelData.material_.diffuse_.y_;
			//	lineStream >> ModelData.material_.diffuse_.z_;
		}
		// 先頭文字がKsならスペキュラー色
		if (key == "Ks") {
			lineStream >> ModelData.material_.metalness_;
			//lineStream >> ModelData.material_.specular.y_;
			//lineStream >> ModelData.material_.specular.z_;
		}
		// 先頭文字がmap_Kdならテクスチャ
		if (key == "map_Kd") {

			std::array<wchar_t, 128> wFilePath;

			std::string fullPath;
			lineStream >> fullPath;
			fullPath = DirectoryPath + fullPath;

			MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wFilePath.data(), static_cast<int>(wFilePath.size()));

			// テクスチャをロード。
			ObjectBuffer.textureHandle_ = TextureManager::Ins()->LoadTexture(wFilePath);

		}
	}
}

void ModelDataManager::CalculateSmoothedVertexNormals(std::map<unsigned short, std::vector<unsigned short>>& SmoothData, ObjectData& ObjectData, ModelData& ModelData)
{
	auto itr = SmoothData.begin();
	for (; itr != SmoothData.end(); ++itr) {
		// 各面用の共通頂点コレクション
		std::vector<unsigned short>& v = itr->second;
		// 全頂点の法線を平均する
		Vec3 normal_ = {};
		for (unsigned short index_ : v) {
			normal_ += ObjectData.vertex_[index_].normal_;
		}
		normal_ = normal_ / (float)v.size();
		normal_.Normalize();

		for (unsigned short index_ : v) {
			ObjectData.vertex_[index_].normal_ = normal_;
			ModelData.vertex_[index_].normal_ = normal_;
		}
	}
}

void ModelDataManager::SaveVertexMinMaxInfo(ObjectData& ObjectBuffer, const Vec3& Pos)
{

	/*===== 頂点の最大最小の情報を保存 =====*/

	// 最大を保存。

	// Posの各成分が保存されている値よりも大きかったら。
	if (ObjectBuffer.vertexMax_.x_ < Pos.x_) {
		ObjectBuffer.vertexMax_.x_ = Pos.x_;
	}
	if (ObjectBuffer.vertexMax_.y_ < Pos.y_) {
		ObjectBuffer.vertexMax_.y_ = Pos.y_;
	}
	if (ObjectBuffer.vertexMax_.z_ < Pos.z_) {
		ObjectBuffer.vertexMax_.z_ = Pos.z_;
	}

	// Posの各成分が保存されている値よりも小さかったら。
	if (Pos.x_ < ObjectBuffer.vertexMin_.x_) {
		ObjectBuffer.vertexMin_.x_ = Pos.x_;
	}
	if (Pos.y_ < ObjectBuffer.vertexMin_.y_) {
		ObjectBuffer.vertexMin_.y_ = Pos.y_;
	}
	if (Pos.z_ < ObjectBuffer.vertexMin_.z_) {
		ObjectBuffer.vertexMin_.z_ = Pos.z_;
	}

}
