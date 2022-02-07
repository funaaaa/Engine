#pragma once
#include "BaseObject3D.h"

class LightCamera;

class Object3D : public BaseObject3D {

public:

	// 生成処理
	void Generate(XMFLOAT3 centerPos, int projectionID, int piplineID, string directoryPath, string modelFileName, LPCWSTR textureFileName, bool isSmoothing);

	// 描画処理
	void Draw();

	// パイプラインを更新する処理
	void UpdatePiplineID(int piplineID);

	// インデックスサイズを取得する
	inline int GetIndexSize() { return index.size(); }

	// 頂点コンテナを取得する
	inline vector<Vertex> GetVertex() { return vertex; }

	// 頂点インデックスコンテナを取得する
	inline vector<unsigned short> GetIndex() { return index; }

};