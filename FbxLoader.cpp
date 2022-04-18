#include "FbxLoader.h"
#include "DirectXBase.h"
#include "ModelDataManager.h"
#include "Struct.h"
#include <assert.h>

void FbxLoader::Init()
{

	/*===== 初期化処理 =====*/

	// 二重で初期化していないかをチェック。
	assert(fbxMgr == nullptr);

	// Fbxマネージャーを生成。
	fbxMgr = std::make_unique<FbxManager>((FbxManager::Create()));

	// Fbxマネージャーの入出力設定
	std::unique_ptr<FbxIOSettings> ios = std::make_unique<FbxIOSettings>(FbxIOSettings::Create(fbxMgr.get(), IOSROOT));
	fbxMgr->SetIOSettings(ios.get());

	// Fbxインポータの生成
	fbxImporter = std::make_unique<FbxImporter>(FbxImporter::Create(fbxMgr.get(), ""));

}

void FbxLoader::LoadModelFromFile(const string& directryPath, const string& modelName)
{

	/*===== ロード関数 =====*/

	// このモデルが既にロードされていたら。
	const int MODEL_COUNT = ModelDataManager::GetModelCount();
	for (int index = 0; index < MODEL_COUNT; ++index) {

		// 名前が同じファイルがあったら。
		ModelData buff = ModelDataManager::GetModelData(index);
		if (buff.modelName == directryPath + modelName) {

			// そのデータを返す。

		}

	}

	// シーンを生成。
	std::shared_ptr<FbxScene> fbxScene = std::make_unique<FbxScene>(FbxScene::Create(fbxMgr.get(), "fbxScene"));

	// ファイルからロードしたFBXの情報をシーンにインポート。
	fbxImporter->Import(fbxScene.get());

	// モデルを生成。
	FbxModel modelData;

	// FBXノードの数を取得。
	int nodeCount = fbxScene->GetNodeCount();

	// 予め必要分のメモリを確保することで、アドレスがずれるのを予防。
	modelData.nodes.reserve(nodeCount);

	// ルートノードから順に解析してモデルに流し込む。
	ParseNodeRecursive(modelData, fbxScene->GetRootNode());

}

void FbxLoader::ParseNodeRecursive(FbxModel& Model, FbxNode* InputFbxNode, Node* parent)
{

	/*===== 再帰的にノードを解析 =====*/

	// nullチェック。
	if (InputFbxNode == nullptr) return;

	// モデルにノードを追加。
	Model.nodes.emplace_back();
	Node& node = Model.nodes.back();

	// ノード名を取得。
	node.name = InputFbxNode->GetName();

	// FBXノードのローカル移動情報
	FbxDouble3 rotation = InputFbxNode->LclRotation.Get();
	FbxDouble3 scaling = InputFbxNode->LclScaling.Get();
	FbxDouble3 translation = InputFbxNode->LclTranslation.Get();

	// 形式変更して代入。
	node.rotation = { (float)rotation[0], (float)rotation[1], (float)rotation[2], 0.0f };
	node.scaling = { (float)scaling[0], (float)scaling[1], (float)scaling[2], 0.0f };
	node.translation = { (float)translation[0], (float)translation[1], (float)translation[2], 1.0f };

	// 回転角をDegree(度)からラジアンに変換
	node.rotation.m128_f32[0] = XMConvertToRadians(node.rotation.m128_f32[0]);
	node.rotation.m128_f32[1] = XMConvertToRadians(node.rotation.m128_f32[1]);
	node.rotation.m128_f32[2] = XMConvertToRadians(node.rotation.m128_f32[2]);

	// スケール、回転、平行移動行列の計算
	XMMATRIX matScaling, matRotation, matTranslation;
	matScaling = XMMatrixScalingFromVector(node.scaling);
	matRotation = XMMatrixScalingFromVector(node.rotation);
	matTranslation = XMMatrixScalingFromVector(node.translation);

	// ローカル変形行列の計算
	node.transform = XMMatrixIdentity();
	node.transform *= matScaling;
	node.transform *= matRotation;
	node.transform *= matTranslation;

	// グローバル変形行列の計算
	node.globalTransform = node.transform;
	if (parent) {

		node.parent = parent;
		// 親の変形を乗算。
		node.globalTransform *= parent->globalTransform;

	}

	// FBXノードのメッシュ情報を解析
	std::unique_ptr<FbxNodeAttribute> fbxNodeAttribute = std::make_unique<FbxNodeAttribute>(InputFbxNode->GetNodeAttribute());

	if (fbxNodeAttribute) {

		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {

			Model.meshNode = &node;
			parseMesh(Model, InputFbxNode);

		}

	}

	// 子ノードに対して再帰呼び出し。
	const int CHILD_COUNT = InputFbxNode->GetChildCount();
	for (int index = 0; index < CHILD_COUNT; ++index) {
		ParseNodeRecursive(Model, InputFbxNode->GetChild(index), &node);
	}

}

void FbxLoader::parseMesh(FbxModel& Model, FbxNode* InputFbxNode)
{

	/*===== メッシュの読み取り =====*/

	// ノードのメッシュを取得。
	std::unique_ptr<FbxMesh> fbxMesh = std::make_unique<FbxMesh>(InputFbxNode->GetMesh());

	// 頂点情報読み取り。
	ParseMeshVertices(Model, fbxMesh.get());
	// 面を構成するデータの読み取り。
	ParseMeshFaces(Model, fbxMesh.get());
	// マテリアルの読み取り。
	ParseMeshMaterial(Model, InputFbxNode);

}

void FbxLoader::ParseMeshVertices(FbxModel& Model, FbxMesh* InputFbxMesh)
{

	/*===== 頂点情報の読み取り =====*/

	auto& vertices = Model.vertices;

	// 頂点座標データの数。
	const int controlPointsCount = InputFbxMesh->GetControlPointsCount();

	// 必要数だけ頂点データ配列を確保。
	FbxModel::VertexPosNormalUv vert{};
	Model.vertices.resize(controlPointsCount, vert);

	// FBXメッシュの頂点座標配列を取得。
	FbxVector4* pCoord = InputFbxMesh->GetControlPoints();

	// FBXメッシュの全頂点座標をモデル内の配列にコピーする。
	for(int index = 0; index < controlPointsCount; ++index){

		FbxModel::VertexPosNormalUv& vertex = vertices[index];

		// 座標のコピー
		vertex.pos.x = (float)pCoord[index][0];
		vertex.pos.y = (float)pCoord[index][1];
		vertex.pos.z = (float)pCoord[index][2];

	}

}

void FbxLoader::ParseMeshFaces(FbxModel& Model, FbxMesh* InputFbxMesh)
{

	/*===== 面情報の読み取り =====*/

}

void FbxLoader::ParseMeshMaterial(FbxModel& Model, FbxNode* InputFbxNode)
{
}

void FbxLoader::ParseMeshTexture(FbxModel& Model, const std::string& filePath)
{
}
