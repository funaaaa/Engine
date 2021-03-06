#include "MultiMeshLoadOBJ.h"
#include "BLAS.h"
#include "Vec.h"
#include "BLASRegister.h"
#include "TextureManager.h"
#include "PolygonInstanceRegister.h"
#include <DirectXMath.h>
#include <fstream>
#include <sstream>
#include <cassert>
#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)
#include "ModelDataManager.h"

std::vector<int> MultiMeshLoadOBJ::RayMultiMeshLoadOBJ(const string& DirectryPath, const string& FilePath, const LPCWSTR& HitGroupName)
{

	/*===== レイトレ用OBJ複数メッシュ読み込み関数 =====*/

	std::vector<int> InstanceID;

	// フィルストリーム
	std::ifstream file;

	// OBJファイルを開く
	file.open(DirectryPath + FilePath);

	// ファイルオープン失敗をチェック。
	if (file.fail()) {
		assert(0);
	}

	// BLASを生成する際に値として渡すデータ。
	ModelDataManager::ObjectData blasData;

	// 一行ずつ読み込む。
	string line;

	// データを一次保存しておくためのコンテナ達。
	std::vector<Vec3> position_;
	std::vector<DirectX::XMFLOAT2> uv_;
	std::vector<Vec3> normal_;
	std::vector<int> textureHandle_;

	// 使用するマテリアル名とマテリアルファイル名の両方を取得したらマテリアルファイルを読み込むようにするために変数。
	bool isLoadMaterialFile = false;
	bool isLoadMaterialName = false;
	bool isLoadTexture = false;

	// マテリアルの情報。
	string materialName;
	string materialFile;

	// 最初の"o"を無効化するためのフラグ。
	bool isFirst = true;

	// 不透明フラグを初期化。
	isOpaque_ = true;

	// 床のみにGIを適応させるためのフラグ。
	isFloor_ = false;

	while (std::getline(file, line)) {

		// 1行分の文字列をストリームに変換して解析しやすくする。
		std::istringstream lineStream(line);

		// 半角スペース区切りで行の先頭文字を取得。
		string key;
		std::getline(lineStream, key, ' ');

		// 先頭文字がvなら頂点座標。
		if (key == "v") {

			// X,Y,Z座標読み込み。
			Vec3 pos_{};
			lineStream >> pos_.x_;
			lineStream >> pos_.y_;
			lineStream >> pos_.z_;

			// 座標を一旦保存。
			position_.emplace_back(pos_);

		}
		// 先頭文字がvtならテクスチャ。
		if (key == "vt") {

			// UV成分読み込み。
			Vec2 texcoord{};
			lineStream >> texcoord.x_;
			lineStream >> texcoord.y_;

			// V方向反転。
			texcoord.y_ = 1.0f - texcoord.y_;

			// テクスチャ座標データに追加。
			uv_.emplace_back(texcoord.ConvertXMFLOAT2());

		}
		// 先頭文字がvnなら法線ベクトル。
		if (key == "vn") {

			// X,Y,Z成分読み込み。
			Vec3 norm{};
			lineStream >> norm.x_;
			lineStream >> norm.y_;
			lineStream >> norm.z_;

			// 法線ベクトルデータに追加。
			normal_.emplace_back(norm);

		}
		// 先頭文字がfならポリゴン(三角形)。
		if (key == "f") {

			// 半角スペース区切りで行の続きを読み込む。
			string indexString;

			while (getline(lineStream, indexString, ' ')) {

				// 頂点インデックス一個分の文字列をストリームに変換して解析しやすくする。
				std::istringstream indexStream(indexString);

				unsigned int indexPosition = 0;		// 受け皿
				unsigned int indexNormal = 0;		// 受け皿
				unsigned int indexTexcoord = 0;		// 受け皿

				indexStream >> indexPosition;
				indexStream.seekg(1, std::ios_base::cur);	// スラッシュを飛ばす。
				indexStream >> indexTexcoord;
				indexStream.seekg(1, std::ios_base::cur);	// スラッシュを飛ばす。
				indexStream >> indexNormal;

				// 頂点データの追加。
				ModelDataManager::Vertex vert{};
				vert.pos_ = position_[indexPosition - 1].ConvertXMFLOAT3();
				vert.normal_ = normal_[indexNormal - 1].ConvertXMFLOAT3();
				vert.uv_ = uv_[indexTexcoord - 1];

				// BLASのデータに追加。
				blasData.vertex_.emplace_back(vert);
				blasData.index_.emplace_back(static_cast<UINT>(blasData.index_.size()));

			}
		}

		// 先頭文字列がmtllibならマテリアルファイル。
		if (key == "mtllib") {

			// マテリアルのファイル名読み込み。
			lineStream >> materialFile;

			isLoadMaterialFile = true;

		}

		// 先頭文字列がusemtlならマテリアル。
		if (key == "usemtl") {

			// マテリアル名読み込み。
			lineStream >> materialName;

			isLoadMaterialName = true;

		}

		// 先頭文字列がoだったらBLASを生成する。
		if (key == "o") {

			// 最初の一回は無視する。
			if (isFirst) {

				isFirst = false;

			}
			else {

				// BLASを生成する。
				int blasIDBuff = BLASRegister::Ins()->GenerateData(blasData, HitGroupName, textureHandle_, isOpaque_);
				std::pair<std::vector<int>, int> buff = { textureHandle_,blasIDBuff };
				blasID_.emplace_back(buff);

				// 保存されているBLASIDでインスタンスを生成する。
				int idBuff = PolygonInstanceRegister::Ins()->CreateInstance(blasIDBuff, isFloor_ ? 10 : 0);
				InstanceID.emplace_back(idBuff);

				// その他データを初期化する。
				blasData.index_.clear();
				blasData.vertex_.clear();
				isLoadTexture = false;
				isLoadMaterialName = false;
				textureHandle_.clear();
				textureHandle_.shrink_to_fit();

				isFloor_ = false;

				isOpaque_ = true;

			}

		}

		// マテリアルファイル名とマテリアル名のどちらとも取得できたら、テクスチャをロードする。
		if ((!isLoadTexture) && isLoadMaterialFile && isLoadMaterialName) {

			// テクスチャを読み込む。
			LoadMaterial(DirectryPath, materialFile, materialName, textureHandle_);

			// テクスチャをロード済みにする。
			isLoadTexture = true;

		}

	}

	// 一番最後のBLASを生成。
	int blasIDBuff = BLASRegister::Ins()->GenerateData(blasData, HitGroupName, textureHandle_, isOpaque_);
	std::pair<std::vector<int>, int> buff = { textureHandle_,blasIDBuff };
	blasID_.emplace_back(buff);

	// 保存されているBLASIDでインスタンスを生成する。
	int idBuff = PolygonInstanceRegister::Ins()->CreateInstance(blasIDBuff, 0);
	InstanceID.emplace_back(idBuff);

	// ファイルを閉じる。
	file.close();

	return InstanceID;

}

void MultiMeshLoadOBJ::LoadMaterial(const string& DirectryPath, const string& MaterialFileName, const string& MaterialName, std::vector<int>& TextureHandle)
{

	// ファイルストリーム。
	std::ifstream file;

	// マテリアルファイルを開く。
	file.open(DirectryPath + MaterialFileName);

	// ファイルオープン失敗をチェック。
	if (file.fail()) {

		assert(0);

	}

	// 一行ずつ読み込む。
	string line;

	// 使用するマテリアルのところに達したか。
	bool isLoadTexture = false;

	// 目的のマテリアルにたどり着いた際に、もう一度"newmtl"にたどり着いたら処理を終えるためのフラグ。
	bool isEnd = false;

	while (getline(file, line)) {

		// 位置行分の文字列をストリームに変換。
		std::istringstream lineStream(line);

		// 半角スペース区切りで行の先頭文字列を取得。
		string key;
		std::getline(lineStream, key, ' ');

		// 先頭のタブ文字は無視する。
		if (key[0] == '\t') {

			key.erase(key.begin());	//先頭の文字を削除

		}

		// 先頭文字列がnewmtlならマテリアル名。
		if (key == "newmtl") {

			// 既にマテリアル名を読み込み済みだったら処理を終える。
			if (isEnd) {

				break;

			}

			// マテリアル名の読み込み。
			string materialNameBuff;
			lineStream >> materialNameBuff;

			// 現在のマテリアル名と引数のマテリアル名が同じだったら。
			if (MaterialName == materialNameBuff) {

				isLoadTexture = true;

				// 次にマテリアル名を読み込んだら処理を終えるようにする。
				isEnd = true;

			}

		}

		// 使用するテクスチャのところに達していたら。
		if (isLoadTexture) {

			// 先頭文字列がmap_Kdならテクスチャ名。
			if (key == "map_Kd") {

				// テクスチャ名を保存。
				string textureNameBuff;
				lineStream >> textureNameBuff;

				// テクスチャ名を変換。
				std::wstring buff = StringToWString(DirectryPath + textureNameBuff);

				// 既に生成済みかをチェックする。
				const int TEXPATH_COUNT = static_cast<int>(texturePath_.size());
				bool isLoad = false;
				for (int index = 0; index < TEXPATH_COUNT; ++index) {

					if (buff == texturePath_[index]) {

						isLoad = true;

						// 草テクスチャだったら不透明フラグを折る。
						if (textureNameBuff == "sponzaTextures/vase_plant.png") {

							isOpaque_ = false;

						}

						// 床テクスチャだったらGIを折る。
						if (textureNameBuff == "sponzaTextures/sponza_floor_a_diff.png") {

							isFloor_ = true;

						}

						// テクスチャを読み込む。
						TextureHandle.emplace_back(TextureManager::Ins()->LoadTexture(texturePath_[index].c_str()));


					}

				}

				// ロードしていなかったら。
				if (!isLoad) {

					texturePath_.emplace_back();
					texturePath_[texturePath_.size() - 1] = buff;

					// テクスチャを読み込む。
					TextureHandle.emplace_back(TextureManager::Ins()->LoadTexture(texturePath_[texturePath_.size() - 1].c_str()));

					// 草テクスチャだったら不透明フラグを折る。
					if (textureNameBuff == "sponzaTextures/vase_plant.png") {

						isOpaque_ = false;

					}
					// 床テクスチャだったらGIを折る。
					if (textureNameBuff == "sponzaTextures/sponza_floor_a_diff.png") {

						isFloor_ = true;

					}

				}

			}

			// 先頭文字列がmap_Dispなら法線マップ。
			//if (key == "map_Disp") {

			//	// 法線テクスチャ名を保存。
			//	string textureNameBuff;
			//	lineStream >> textureNameBuff;

			//	// 法線テクスチャ名を変換。
			//	wstring buff = StringToWString(DirectryPath + textureNameBuff);

			//	// 既に生成済みかをチェックする。
			//	const int TEXPATH_COUNT = texturePath_.size();
			//	bool isLoad = false;
			//	for (int index = 0; index < TEXPATH_COUNT; ++index) {

			//		if (buff == texturePath_[index]) {

			//			isLoad = true;

			//			// テクスチャを読み込む。
			//			TextureHandle.emplace_back(TextureManager::Ins()->LoadTextureInDescriptorHeapMgr(texturePath_[index].c_str()));


			//		}

			//	}

			//	// ロードしていなかったら。
			//	if (!isLoad) {

			//		texturePath_.emplace_back();
			//		texturePath_[texturePath_.size() - 1] = buff;

			//		// テクスチャを読み込む。
			//		TextureHandle.emplace_back(TextureManager::Ins()->LoadTextureInDescriptorHeapMgr(texturePath_[texturePath_.size() - 1].c_str()));

			//	}

			//}

		}

	}

}

std::wstring MultiMeshLoadOBJ::StringToWString(std::string oString)
{
	auto num1 = MultiByteToWideChar(
		CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		oString.c_str(), -1, nullptr, 0);

	std::wstring wstr;
	wstr.resize(num1);

	MultiByteToWideChar(
		CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		oString.c_str(), -1, &wstr[0], num1);

	// 変換結果を返す
	return(wstr);
}
