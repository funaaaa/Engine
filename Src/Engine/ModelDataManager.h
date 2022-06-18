#pragma once
#include <string>
#include <map>
#include <vector>
#include "Fbxsdk.h"
#include "Vec.h"
#include <wtypes.h>
#include "Singleton.h"

class ModelDataManager : public Singleton<ModelDataManager> {

public:

	// 頂点データ
	struct Vertex {

		Vec3 pos;
		Vec3 normal;
		DirectX::XMFLOAT2 uv;

	};

	// データを渡す構造体
	struct ObjectData {

		std::vector<Vertex> vertex;
		std::vector<UINT> index;

	};

	//マテリアル
	struct Material {
		std::string name;								//マテリアル名
		std::string textureName;						//テクスチャの名前
		DirectX::XMFLOAT3 ambient = { 0.3f,0.3f,0.3f };	//アンビエント影響度
		DirectX::XMFLOAT3 diffuse = { 0.3f,0.3f,0.3f };	//ディフューズ影響度
		DirectX::XMFLOAT3 specular = { 0.3f,0.3f,0.3f };//スペキュラー影響度
		float alpha;									//アルファ
	};

	//modelのデータ構造体
	struct ModelData {
		std::string modelName;				//モデルファイル名
		Material material;					//マテリアル
		std::vector<unsigned short> index;	//頂点インデックス
		std::vector<Vertex> vertex;			//頂点
		bool isSmoothing;					//法線をスムーズにするかどうか
	};

private:


	std::vector<ModelData> modelData;			//モデルデータを保存しておく配列

public:

	//objファイルをロードして値をコピーする。ロード済みのファイルだったらロードせずにコピーだけ行う。
	void LoadObj(std::string DirectoryPath, std::string FileName, ObjectData& ObjectBuffer, bool IsSmoothing);

	//objファイルの読み込み時にマテリアルをロードするための関数
	void LoadObjMaterial(const std::string& MaterialFileName, ModelData& ModelData, ObjectData& ObjectData);

	//法線の平均を求める関数
	void CalculateSmoothedVertexNormals(std::map<unsigned short, std::vector<unsigned short>>& SmoothData, ObjectData& ObjectBuffer, ModelData& ModelData);

	int GetModelCount() { return modelData.size(); }
	const ModelData& GetModelData(const int& Index) { return modelData[Index]; }

};