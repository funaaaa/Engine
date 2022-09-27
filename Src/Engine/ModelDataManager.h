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

		Vec3 pos_;
		Vec3 normal_;
		Vec2 uv_;

	};

	// マテリアル
	struct Material {
		Vec3 baseColor_;
		float metalness_;
		float specular_;
		float roughness_;
		Vec2 pad_;
		Material() :baseColor_(Vec3(1,1,1)), metalness_(0.0f), specular_(0.5f), roughness_(0.0f) {};
	};

	// データを渡す構造体
	struct ObjectData {

		std::vector<Vertex> vertex_;
		std::vector<UINT> index_;
		Material material_;
		Vec3 vertexMax_;	// 頂点における各軸の最大の位置
		Vec3 vertexMin_;	// 頂点における各軸の最小の位置
		int textureHandle_;

	};

	// modelのデータ構造体
	struct ModelData {
		std::string modelName_;				//モデルファイル名
		Material material_;					//マテリアル
		std::vector<unsigned short> index_;	//頂点インデックス
		std::vector<Vertex> vertex_;			//頂点
		Vec3 vertexMax_;	// 頂点における各軸の最大の位置
		Vec3 vertexMin_;	// 頂点における各軸の最小の位置
		bool isSmoothing_;					//法線をスムーズにするかどうか
	};

private:


	std::vector<ModelData> modelData_;			//モデルデータを保存しておく配列

public:

	// objファイルをロードして値をコピーする。ロード済みのファイルだったらロードせずにコピーだけ行う。
	void LoadObj(std::string DirectoryPath, std::string FileName, ObjectData& ObjectBuffer, bool IsSmoothing);
	void LoadGLTF(std::wstring Path, ObjectData& ObjectBuffer);

	// objファイルの読み込み時にマテリアルをロードするための関数
	void LoadObjMaterial(std::string DirectoryPath, const std::string& MaterialFileName, ModelData& ModelData, ObjectData& ObjectBuffer);

	// 法線の平均を求める関数
	void CalculateSmoothedVertexNormals(std::map<unsigned short, std::vector<unsigned short>>& SmoothData, ObjectData& ObjectBuffer, ModelData& ModelData);

	int GetModelCount() { return static_cast<int>(modelData_.size()); }
	const ModelData& GetModelData(const int& Index) { return modelData_[Index]; }

	// 最大最小の頂点を保存。
	void SaveVertexMinMaxInfo(ObjectData& ObjectBuffer, const Vec3& Pos);

};