#include "ModelDataManager.h"
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

void ModelDataManager::LoadObj(std::string DirectryPath, std::string FileName, ObjectData& ObjectBuffer, bool IsSmoothing)
{
	// objファイルがロード済みかどうか
	bool isLoad = false;
	// ロード済みだった場合、何番目の要素に保存されているのかを取得する変数
	int dataNumber = 0;

	const int MODELDATA_SIZE = static_cast<int>(modelData.size());
	for (int index = 0; index < MODELDATA_SIZE; ++index) {
		if (modelData[index].modelName == DirectryPath + FileName && modelData[index].isSmoothing == IsSmoothing) {
			isLoad = true;
			dataNumber = index;
			break;
		}
	}

	// objファイルが未ロードだったらロードする
	if (!isLoad) {
		// 同じ頂点を指し示している頂点インデックスを保存しておくためのマップ
		std::map<unsigned short, std::vector<unsigned short>> smoothData;

		modelData.push_back({});
		ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).modelName = DirectryPath + FileName;
		ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).isSmoothing = IsSmoothing;

		// 変数を初期化。
		ObjectBuffer.vertexMax = Vec3();
		ObjectBuffer.vertexMin = Vec3();

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
		std::vector<Vec3> position;			// 座標を保存しておく
		std::vector<Vec2> uv;				// uvを保存しておく
		std::vector<Vec3> normal;			// 法線ベクトルを保存しておく
		std::vector<unsigned short> index;
		while (getline(file, line)) {
			// 1行分の文字列をストリームに変換して解析しやすくする
			std::istringstream lineStream(line);
			// 半角スペース区切りで行の先頭文字を取得
			std::string key;
			std::getline(lineStream, key, ' ');

			// 先頭文字がvなら頂点座標
			if (key == "v") {
				// X,Y,Z座標読み込み
				Vec3 pos{};
				lineStream >> pos.x;
				lineStream >> pos.y;
				lineStream >> pos.z;
				// 座標を一旦保存
				position.push_back(pos);
				// 頂点の最大最小要素を保存。
				SaveVertexMinMaxInfo(ObjectBuffer, pos);
				modelData[(static_cast<int>(modelData.size()) - 1)].vertexMin = ObjectBuffer.vertexMin;
				modelData[(static_cast<int>(modelData.size()) - 1)].vertexMax = ObjectBuffer.vertexMax;
			}
			// 先頭文字がvtならテクスチャ
			if (key == "vt") {
				// U,V成分読み込み
				Vec2 texcoord{};
				lineStream >> texcoord.x;
				lineStream >> texcoord.y;
				// V方向反転
				texcoord.y = 1.0f - texcoord.y;
				// テクスチャ座標データに追加
				uv.push_back(texcoord);
			}
			// 先頭文字がvnなら法線ベクトル
			if (key == "vn") {
				// X,Y,Z成分読み込み
				Vec3 norm{};
				lineStream >> norm.x;
				lineStream >> norm.y;
				lineStream >> norm.z;
				// 法線ベクトルデータに追加
				normal.push_back(norm);
			}
			// 先頭文字がfならポリゴン(三角形)
			if (key == "f") {
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
					vert.pos = position[indexPosition - 1];
					vert.normal = normal[indexNormal - 1];
					vert.uv = uv[indexTexcoord - 1];
					// モデルデータに追加
					modelData[(static_cast<int>(modelData.size()) - 1)].vertex.push_back(vert);
					modelData[(static_cast<int>(modelData.size()) - 1)].index.push_back(static_cast<unsigned short>(modelData[(static_cast<int>(modelData.size())) - 1].index.size()));
					// proSpriteにも追加
					ObjectBuffer.vertex.push_back(vert);
					ObjectBuffer.index.push_back(static_cast<int>(ObjectBuffer.index.size()));
					// isSmoothingがtrueなら頂点情報を追加する
					if (IsSmoothing == true) {
						smoothData[indexPosition].push_back(static_cast<unsigned short>(ObjectBuffer.vertex.size()) - 1);
					}
				}
			}
			// 先頭文字列がmtllibならマテリアル
			if (key == "mtllib") {
				// マテリアルのファイル名読み込み
				std::string materialFileName;
				lineStream >> materialFileName;
				// マテリアルの読み込み
				LoadObjMaterial(DirectryPath + materialFileName, modelData.at(modelData.size() - 1));
				ObjectBuffer.material = modelData[modelData.size() - 1].material;

			}
		}
		// ファイルを閉じる
		file.close();

		// isSmoothingがtrueだったら法線情報をなめらかにする
		if (IsSmoothing) {
			CalculateSmoothedVertexNormals(smoothData, ObjectBuffer, modelData.at(modelData.size() - 1));
		}

		return;
	}

	// objのデータをObjectDataに入れる
	for (auto& index : modelData[dataNumber].index) {
		ObjectBuffer.index.push_back(index);
	}
	for (auto& index : modelData[dataNumber].vertex) {
		ObjectBuffer.vertex.push_back(index);
	}
	ObjectBuffer.material = modelData[dataNumber].material;
	ObjectBuffer.vertexMin = modelData[dataNumber].vertexMin;
	ObjectBuffer.vertexMax = modelData[dataNumber].vertexMax;

}

void ModelDataManager::LoadObjMaterial(const std::string& MaterialFileName, ModelData& ModelData)
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
		// 先頭文字列がKaならアンビエント色
		if (key == "Ka") {
			lineStream >> ModelData.material.ambient.x;
			lineStream >> ModelData.material.ambient.y;
			lineStream >> ModelData.material.ambient.z;
		}
		// 先頭文字列がKdならディフューズ色
		if (key == "Kd") {
			lineStream >> ModelData.material.diffuse.x;
			lineStream >> ModelData.material.diffuse.y;
			lineStream >> ModelData.material.diffuse.z;
		}
		// 先頭文字がKsならスペキュラー色
		if (key == "Ks") {
			lineStream >> ModelData.material.specular.x;
			lineStream >> ModelData.material.specular.y;
			lineStream >> ModelData.material.specular.z;
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
		Vec3 normal = {};
		for (unsigned short index : v) {
			normal += ObjectData.vertex[index].normal;
		}
		normal = normal / (float)v.size();
		normal.Normalize();

		for (unsigned short index : v) {
			ObjectData.vertex[index].normal = normal;
			ModelData.vertex[index].normal = normal;
		}
	}
}

void ModelDataManager::SaveVertexMinMaxInfo(ObjectData& ObjectBuffer, const Vec3& Pos)
{

	/*===== 頂点の最大最小の情報を保存 =====*/

	// 最大を保存。

	// Posの各成分が保存されている値よりも大きかったら。
	if (ObjectBuffer.vertexMax.x < Pos.x) {
		ObjectBuffer.vertexMax.x = Pos.x;
	}
	if (ObjectBuffer.vertexMax.y < Pos.y) {
		ObjectBuffer.vertexMax.y = Pos.y;
	}
	if (ObjectBuffer.vertexMax.z < Pos.z) {
		ObjectBuffer.vertexMax.z = Pos.z;
	}

	// Posの各成分が保存されている値よりも小さかったら。
	if (Pos.x < ObjectBuffer.vertexMin.x) {
		ObjectBuffer.vertexMin.x = Pos.x;
	}
	if (Pos.y < ObjectBuffer.vertexMin.y) {
		ObjectBuffer.vertexMin.y = Pos.y;
	}
	if (Pos.z < ObjectBuffer.vertexMin.z) {
		ObjectBuffer.vertexMin.z = Pos.z;
	}

}
