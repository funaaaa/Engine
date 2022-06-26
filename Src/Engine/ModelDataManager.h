#pragma once
#include <string>
#include <map>
#include <vector>
#pragma warning(push)
#pragma warning(disable:4023)
#pragma warning(disable:4099)
#include "Fbxsdk.h"
#pragma warning(pop)
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

	// マテリアル
	struct Material {
		Vec3 ambient = { 0.3f,0.3f,0.3f };	//アンビエント影響度
		float pad1;
		Vec3 diffuse = { 0.3f,0.3f,0.3f };	//ディフューズ影響度
		float pad2;
		Vec3 specular = { 0.3f,0.3f,0.3f };//スペキュラー影響度
		float alpha;									//アルファ
	};

	// データを渡す構造体
	struct ObjectData {

		std::vector<Vertex> vertex;
		std::vector<UINT> index;
		Material material;

	};

	// modelのデータ構造体
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
	void LoadObjMaterial(const std::string& MaterialFileName, ModelData& ModelData);

	//法線の平均を求める関数
	void CalculateSmoothedVertexNormals(std::map<unsigned short, std::vector<unsigned short>>& SmoothData, ObjectData& ObjectBuffer, ModelData& ModelData);

	int GetModelCount() { return static_cast<int>(modelData.size()); }
	const ModelData& GetModelData(const int& Index) { return modelData[Index]; }

};