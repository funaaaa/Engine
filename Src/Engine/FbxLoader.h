#pragma once
#include "Singleton.h"

#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)

#include <vector>
#include <string>
#include <array>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include "Vec.h"

#pragma warning(push)
#pragma warning(disable:4023)
#pragma warning(disable:4099)
#include "fbxsdk.h"
#pragma warning(pop)

// ボーン番号とスキンウェイトのペア
struct WeightSet {

	UINT index;
	float weight;

};

// ボーン構造体
struct Bone {
	// 名前
	std::string name;
	// 初期姿勢座標の逆行列
	DirectX::XMMATRIX invInitialPose;
	// クラスター(FBX側のボーン情報)
	FbxCluster* fbxCluster;
	// コンストラクタ
	Bone(const std::string& Name) : name(Name) {};
};

// ノード構造体
struct Node {
	// 名前
	std::string name;
	// パディング
	Vec2 pad;
	// ローカル変換行列
	DirectX::XMMATRIX transform;
	// グローバル変換行列
	DirectX::XMMATRIX globalTransform;
	// ローカルスケール
	DirectX::XMVECTOR scaling = { 1.0f,1.0f,1.0f,0.0f };
	// ローカル回転角
	DirectX::XMVECTOR rotation = { 0.0f,0.0f,0.0f,0.0f };
	// ローカル移動
	DirectX::XMVECTOR translation = { 0.0f,0.0f,0.0f,1.0f };
	// 親ノード
	Node* parent;
};

// FBXモデルクラス
class FbxModel {

public:

	// フレンドクラス
	friend class FbxLoader;

	// ボーンの最大数
	static const int MAX_BONE_INDICES = 4;

	// 頂点データ構造体
	struct VertexPosNormalUvSkin
	{
		Vec3 pos_;		// 座標
		Vec3 normal;	// 法線
		Vec2 uv;		// UV
		std::array<UINT, MAX_BONE_INDICES> boneIndex;	// ボーンの番号
		std::array<float, MAX_BONE_INDICES> boneWeight;	// ボーンの重み
	};

	// モデル名
	std::string modelName;
	// メッシュを持つノード
	Node* meshNode;
	// 頂点データ配列
	std::vector<VertexPosNormalUvSkin> vertices;
	// 頂点インデックス配列
	std::vector<unsigned short> indices;
	// ボーン配列
	std::vector<Bone> bones;

	FbxTime frameTime;		// 1フレームの時間
	FbxTime startTime;		// 開始時間
	FbxTime endTime;		// 終了時間
	FbxTime currentTime;	// 現在の時間
	bool isPlay = false;
	bool isInit = false;
	bool hasAnimation = false;	// アニメーションを持っているか

public:

	// アンビエント係数
	Vec3 ambient = { 1.0f,1.0f,1.0f };
	// ディフーズ係数
	Vec3 diffuse = { 1.0f,1.0f,1.0f };
	// テクスチャID
	int textureID;

private:

	// ノード配列
	std::vector<Node> nodes;


public:

	FbxModel() {
		frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);
	}

	void InitAnimation();
	void PlayAnimation();
	void StopAnimation();

};

// FBX読み込みクラス
class FbxLoader : public Singleton<FbxLoader> {

private:

	/*===== メンバ変数 =====*/

	FbxManager* fbxMgr;
	FbxImporter* fbxImporter;
	FbxScene* fbxScene;

	std::string directryPath;	// ディレクトリーパスを一次保存しておくための変数。

	std::vector<FbxModel> fbxModelData;	// 生成されたモデルデータ


public:

	// ボーンの最大数
	static const int MAX_BONES = 16;
	// スキニング用構造体(コンピュートシェーダーに送る用)
	struct SkinData {
		std::array<DirectX::XMMATRIX, MAX_BONES> bones;
	};
	// スキニングアニメーションの行列計算に使用するコンピュートシェーダーの入力用構造体
	struct SkinComputeInput {
		FbxModel::VertexPosNormalUvSkin vertex;
		FbxLoader::SkinData skinData;
	};

	struct Vertex {
		Vec3 pos_;
		Vec3 normal;
		Vec2 uv;
	};
	void GetFbxData(const int& Index, std::vector<Vertex>& OutputVertex, std::vector<UINT>& OutputVertexIndex);


private:

	using string = std::string;


public:

	/*===== メンバ関数 =====*/

	// 初期化処理
	void Init();

	// ロード関数
	int LoadModelFromFile(const string& DirectryPath, const string& ModelName);

	// FBXの行列をXMMATRIXに変換
	void ConvertMatrixFromFBX(DirectX::XMMATRIX& Dst, const FbxAMatrix& Src);

	// スキニング行列を取得。
	SkinData GetSkinMat(const int& Index);

	// スキニングアニメーション用コンピュートシェーダーの入力構造体を取得。
	void GetSkinComputeInput(const int& Index, std::vector<SkinComputeInput>& Input);

	// FBXシーンのゲッタ
	FbxScene* GetFbxScene() { return fbxScene; }

	// モデル情報のゲッタ
	FbxModel& GetFbxModel(const int& Index) { return fbxModelData[Index]; }


private:

	// 再帰的にノード構成を解析。
	void ParseNodeRecursive(FbxModel& Model, FbxNode* InputFbxNode, Node* Parent = nullptr);

	// メッシュの読み取り
	void ParseMesh(FbxModel& Model, FbxNode* InputFbxNode);

	// 頂点座標読み取り
	void ParseMeshVertices(FbxModel& Model, FbxMesh* InputFbxMesh);
	// 面情報読み取り
	void ParseMeshFaces(FbxModel& Model, FbxMesh* InputFbxMesh);
	// マテリアル情報読み取り
	void ParseMeshMaterial(FbxModel& Model, FbxNode* InputFbxNode);
	// テクスチャ情報読み取り
	//void ParseMeshTexture(FbxModel& Model, const std::string& FilePath);
	// スキニング情報読み取り
	void ParseSkin(FbxModel& Model, FbxMesh* InputFbxMesh);

	// ディレクトリを含んだファイルパスからファイル名を抽出する。
	std::string ExtractFileName(const std::string& path);

};