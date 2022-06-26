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

void ModelDataManager::LoadObj(std::string DirectryPath, std::string FileName, ObjectData& ObjectData, bool IsSmoothing)
{
	//objファイルがロード済みかどうか
	bool isLoad = false;
	//ロード済みだった場合、何番目の要素に保存されているのかを取得する変数
	int dataNumber = 0;

	for (int i = 0; i < ModelDataManager::modelData.size(); ++i) {
		if (modelData.at(i).modelName == DirectryPath + FileName && modelData.at(i).isSmoothing == IsSmoothing) {
			isLoad = true;
			dataNumber = i;
			break;
		}
	}

	//objファイルが未ロードだったらロードする
	if (!isLoad) {
		//同じ頂点を指し示している頂点インデックスを保存しておくためのマップ
		std::map<unsigned short, std::vector<unsigned short>> smoothData;

		modelData.push_back({});
		ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).modelName = DirectryPath + FileName;
		ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).isSmoothing = IsSmoothing;

		//フィルストリーム
		std::ifstream file;
		//.objファイルを開く
		file.open(DirectryPath + FileName);
		//ファイルオープン失敗をチェック
		if (file.fail()) {
			assert(0);
		}
		//一行ずつ読み込む
		std::string line;
		std::vector<Vec3> position;	//座標を保存しておく
		std::vector<DirectX::XMFLOAT2> uv;		//uvを保存しておく
		std::vector<Vec3> normal;	//法線ベクトルを保存しておく
		std::vector<unsigned short> index;
		while (getline(file, line)) {
			//1行分の文字列をストリームに変換して解析しやすくする
			std::istringstream lineStream(line);
			//半角スペース区切りで行の先頭文字を取得
			std::string key;
			std::getline(lineStream, key, ' ');

			//先頭文字がvなら頂点座標
			if (key == "v") {
				//X,Y,Z座標読み込み
				Vec3 pos{};
				lineStream >> pos.x;
				lineStream >> pos.y;
				lineStream >> pos.z;
				//座標を一旦保存
				position.push_back(pos);
			}
			//先頭文字がvtならテクスチャ
			if (key == "vt") {
				//U,V成分読み込み
				DirectX::XMFLOAT2 texcoord{};
				lineStream >> texcoord.x;
				lineStream >> texcoord.y;
				//V方向反転
				texcoord.y = 1.0f - texcoord.y;
				//テクスチャ座標データに追加
				uv.push_back(texcoord);
			}
			//先頭文字がvnなら法線ベクトル
			if (key == "vn") {
				//X,Y,Z成分読み込み
				Vec3 norm{};
				lineStream >> norm.x;
				lineStream >> norm.y;
				lineStream >> norm.z;
				//法線ベクトルデータに追加
				normal.push_back(norm);
			}
			//先頭文字がfならポリゴン(三角形)
			if (key == "f") {
				//半角スペース区切りで行の続きを読み込む
				std::string indexString;
				unsigned short indexPosition;		//受け皿
				unsigned short indexNormal;			//受け皿
				unsigned short indexTexcoord;		//受け皿
				while (std::getline(lineStream, indexString, ' ')) {
					//頂点インデックス一個分の文字列をストリームに変換して解析しやすくする
					std::istringstream indexStream(indexString);
					indexStream >> indexPosition;
					indexStream.seekg(1, std::ios_base::cur);	//スラッシュを飛ばす
					indexStream >> indexTexcoord;
					indexStream.seekg(1, std::ios_base::cur);	//スラッシュを飛ばす
					indexStream >> indexNormal;
					//頂点データの追加
					Vertex vert{};
					vert.pos = position[indexPosition - 1];
					vert.normal = normal[indexNormal - 1];
					vert.uv = uv[indexTexcoord - 1];
					//モデルデータに追加
					modelData[(static_cast<int>(modelData.size()) - 1)].vertex.push_back(vert);
					modelData[(static_cast<int>(modelData.size()) - 1)].index.push_back(static_cast<unsigned short>(modelData[(static_cast<int>(modelData.size())) - 1].index.size()));
					//proSpriteにも追加
					ObjectData.vertex.push_back(vert);
					ObjectData.index.push_back(static_cast<int>(ObjectData.index.size()));
					//isSmoothingがtrueなら頂点情報を追加する
					if (IsSmoothing == true) {
						smoothData[indexPosition].push_back(static_cast<unsigned short>(ObjectData.vertex.size()) - 1);
					}
				}
			}
			//先頭文字列がmtllibならマテリアル
			if (key == "mtllib") {
				//マテリアルのファイル名読み込み
				std::string materialFileName;
				lineStream >> materialFileName;
				//マテリアルの読み込み
				LoadObjMaterial(DirectryPath + materialFileName, modelData.at(modelData.size() - 1));
				ObjectData.material = modelData[modelData.size() - 1].material;

			}
		}
		//ファイルを閉じる
		file.close();

		//isSmoothingがtrueだったら法線情報をなめらかにする
		if (IsSmoothing == true) {
			CalculateSmoothedVertexNormals(smoothData, ObjectData, modelData.at(modelData.size() - 1));
		}

		return;
	}

	//objのデータをproSpriteに入れる
	for (int index = 0; index < modelData[dataNumber].index.size(); ++index) {
		ObjectData.index.push_back(modelData[dataNumber].index[index]);
	}
	for (int index = 0; index < modelData[dataNumber].vertex.size(); ++index) {
		ObjectData.vertex.push_back(modelData[dataNumber].vertex[index]);
	}
	ObjectData.material = modelData[dataNumber].material;

}

void ModelDataManager::LoadObjMaterial(const std::string& MaterialFileName, ModelData& ModelData)
{

	//ファイルストリーム
	std::ifstream file;
	//マテリアルファイルを開く
	file.open(MaterialFileName);
	//ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}
	//一行ずつ読み込む
	std::string line;
	while (getline(file, line)) {
		//位置行分の文字列をストリームに変換
		std::istringstream lineStream(line);
		//半角スペース区切りで行の先頭文字列を取得
		std::string key;
		std::getline(lineStream, key, ' ');
		//先頭のタブ文字は無視する
		if (key[0] == '\t') {
			key.erase(key.begin());	//先頭の文字を削除
		}
		//先頭文字列がnewmtlならマテリアル名
		if (key == "newmtl") {
			//マテリアル名の読み込み
			//lineStream >> ModelData.material.name;
		}
		//先頭文字列がKaならアンビエント色
		if (key == "Ka") {
			lineStream >> ModelData.material.ambient.x;
			lineStream >> ModelData.material.ambient.y;
			lineStream >> ModelData.material.ambient.z;
		}
		//先頭文字列がKdならディフューズ色
		if (key == "Kd") {
			lineStream >> ModelData.material.diffuse.x;
			lineStream >> ModelData.material.diffuse.y;
			lineStream >> ModelData.material.diffuse.z;
		}
		//先頭文字がKsならスペキュラー色
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
