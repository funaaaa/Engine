#pragma once
#include "Struct.h"
#include <string>
#include <map>
#include "Fbxsdk.h"

class ModelDataManager {
private:
	static vector<ModelData> modelData;			//モデルデータを保存しておく配列

public:
	//fbxファイルをロードして値をコピーする。ロード済みのファイルだったらロードせずにコピーだけ行う。
	static void LoadFbx(const char* fileName, Object3DDeliveryData& objectBuffer);

	//objファイルをロードして値をコピーする。ロード済みのファイルだったらロードせずにコピーだけ行う。
	static void LoadObj(string directoryPath, string fileName, Object3DDeliveryData& objectBuffer, bool isSmoothing);

	//fbxファイルを読み込む際、すべてのノードにアクセスするための関数
	static void CollectMeshNode(FbxNode* node, vector<pair<string, FbxMesh*>>& list);

	//objファイルの読み込み時にマテリアルをロードするための関数
	static void LoadObjMaterial(const string& materialFileName, ModelData& modelData, Object3DDeliveryData& spriteData);

	//法線の平均を求める関数
	static void CalculateSmoothedVertexNormals(map<unsigned short, vector<unsigned short>>& smoothData, Object3DDeliveryData& objectBuffer, ModelData& modelData);

	static int GetModelCount() { return modelData.size(); }
	static const ModelData& GetModelData(const int& index) { return modelData[index]; }

};