#pragma once
#include "Singleton.h"
#include <memory>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include "fbxsdk.h"
#include "Struct.h"

// ノード構造体
struct Node {
	// 名前
	std::string name;
	// ローカルスケール
	DirectX::XMVECTOR scaling = { 1.0f,1.0f,1.0f,0.0f };
	// ローカル回転角
	DirectX::XMVECTOR rotation = { 0.0f,0.0f,0.0f,0.0f };
	// ローカル移動
	DirectX::XMVECTOR translation = { 0.0f,0.0f,0.0f,1.0f };
	// ローカル変換行列
	DirectX::XMMATRIX transform;
	// グローバル変換行列
	DirectX::XMMATRIX globalTransform;
	// 親ノード
	Node* parent;
};

// FBXモデルクラス
class FbxModel {

public:

	// フレンドクラス
	friend class FbxLoader;

	// 頂点データ構造体
	struct VertexPosNormalUv
	{
		DirectX::XMFLOAT3 pos;		// 座標
		DirectX::XMFLOAT3 normal;	// 法線
		DirectX::XMFLOAT2 uv;		// UV
	};

	// モデル名
	std::string modelName;
	// メッシュを持つノード
	Node* meshNode;
	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices;
	// 頂点インデックス配列
	std::vector<unsigned short> indices;

public:

	// アンビエント係数
	DirectX::XMFLOAT3 ambient = { 1.0f,1.0f,1.0f };
	// ディフーズ係数
	DirectX::XMFLOAT3 diffuse = { 1.0f,1.0f,1.0f };
	// テクスチャID
	int textureID;

private:

	// ノード配列
	std::vector<Node> nodes;

};

// FBX読み込みクラス
class FbxLoader : public Singleton<FbxLoader> {

private:

	/*===== メンバ変数 =====*/

	FbxManager* fbxMgr;
	FbxImporter* fbxImporter;

	std::string directryPath;	// ディレクトリーパスを一次保存しておくための変数。

	std::vector<FbxModel> fbxModelData;	// 生成されたモデルデータ


private:

	using string = std::string;


public:

	/*===== メンバ関数 =====*/

	// 初期化処理
	void Init();

	// ロード関数
	FbxModel LoadModelFromFile(const string& directryPath, const string& modelName);

	// FbxModelをObject3DDeliveryDataに変換
	Object3DDeliveryData ConvertObject3DDeliveryData(const FbxModel& modelData);


private:

	// 再帰的にノード構成を解析。
	void ParseNodeRecursive(FbxModel& Model, FbxNode* InputFbxNode, Node* parent = nullptr);

	// メッシュの読み取り
	void ParseMesh(FbxModel& Model, FbxNode* InputFbxNode);

	// 頂点座標読み取り
	void ParseMeshVertices(FbxModel& Model, FbxMesh* InputFbxMesh);
	// 面情報読み取り
	void ParseMeshFaces(FbxModel& Model, FbxMesh* InputFbxMesh);
	// マテリアル情報読み取り
	void ParseMeshMaterial(FbxModel& Model, FbxNode* InputFbxNode);
	// テクスチャ情報読み取り
	void ParseMeshTexture(FbxModel& Model, const std::string& filePath);

	// ディレクトリを含んだファイルパスからファイル名を抽出する。
	std::string ExtractFileName(const std::string& path);

};