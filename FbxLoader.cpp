#include "FbxLoader.h"
#include "DirectXBase.h"
#include "Struct.h"
#include <assert.h>
#include "TextureManager.h"

void FbxLoader::Init()
{

	/*===== 初期化処理 =====*/

	// 二重で初期化していないかをチェック。
	assert(fbxMgr == nullptr);

	// Fbxマネージャーを生成。
	fbxMgr = FbxManager::Create();

	// Fbxマネージャーの入出力設定
	FbxIOSettings* ios = FbxIOSettings::Create(fbxMgr, IOSROOT);
	fbxMgr->SetIOSettings(ios);

	// Fbxインポータの生成
	fbxImporter = FbxImporter::Create(fbxMgr, "");

}

FbxModel FbxLoader::LoadModelFromFile(const string& directryPath, const string& modelName)
{

	/*===== ロード関数 =====*/

	// このモデルが既にロードされていたら。
	const int MODEL_COUNT = fbxModelData.size();
	for (int index = 0; index < MODEL_COUNT; ++index) {

		// 名前が同じファイルがあったら。
		if (fbxModelData[index].modelName == directryPath + modelName) {

			// そのデータを返す。
			return fbxModelData[index];

		}

	}

	// ディレクトリーパスを保存。
	this->directryPath = directryPath;

	// ファイル名を指定してFBXファイルを読み込む。
	if (!fbxImporter->Initialize((directryPath + modelName).c_str(), -1, fbxMgr->GetIOSettings())) {
		assert(0);
	}

	// シーンを生成。
	FbxScene* fbxScene = FbxScene::Create(fbxMgr, "fbxScene");

	// ファイルからロードしたFBXの情報をシーンにインポート。
	fbxImporter->Import(fbxScene);

	// モデルを生成。
	FbxModel modelData;

	// FBXノードの数を取得。
	int nodeCount = fbxScene->GetNodeCount();

	// 予め必要分のメモリを確保することで、アドレスがずれるのを予防。
	modelData.nodes.reserve(nodeCount);

	// ルートノードから順に解析してモデルに流し込む。
	ParseNodeRecursive(modelData, fbxScene->GetRootNode());

	// FBXシーン開放。
	fbxScene->Destroy();

	return modelData;

}

Object3DDeliveryData FbxLoader::ConvertObject3DDeliveryData(const FbxModel& modelData)
{
	Object3DDeliveryData returnData;

	// 頂点データを変換。
	const int VERTEX_COUNT = modelData.vertices.size();
	returnData.vertex.resize(VERTEX_COUNT);
	for (int index = 0; index < VERTEX_COUNT; ++index) {
		returnData.vertex[index].pos = modelData.vertices[index].pos;
		returnData.vertex[index].normal = modelData.vertices[index].normal;
		returnData.vertex[index].uv = modelData.vertices[index].uv;
	}

	// 頂点インデックスデータを変換。
	const int INDEX_COUNT = modelData.indices.size();
	returnData.index.resize(INDEX_COUNT);
	for (int index = 0; index < INDEX_COUNT; ++index) {
		returnData.index[index] = modelData.indices[index];
	}

	// ライティング周りの定数バッファデータを変換。
	returnData.constBufferDataB1.alpha = 1.0f;
	returnData.constBufferDataB1.ambient = modelData.ambient;
	returnData.constBufferDataB1.diffuse = modelData.diffuse;
	returnData.constBufferDataB1.specular = {};

	return returnData;
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
	FbxNodeAttribute* fbxNodeAttribute = InputFbxNode->GetNodeAttribute();

	if (fbxNodeAttribute) {

		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {

			Model.meshNode = &node;
			ParseMesh(Model, InputFbxNode);

		}

	}

	// 子ノードに対して再帰呼び出し。
	const int CHILD_COUNT = InputFbxNode->GetChildCount();
	for (int index = 0; index < CHILD_COUNT; ++index) {
		ParseNodeRecursive(Model, InputFbxNode->GetChild(index), &node);
	}

}

void FbxLoader::ParseMesh(FbxModel& Model, FbxNode* InputFbxNode)
{

	/*===== メッシュの読み取り =====*/

	// ノードのメッシュを取得。
	FbxMesh* fbxMesh = InputFbxNode->GetMesh();

	// 頂点情報読み取り。
	ParseMeshVertices(Model, fbxMesh);
	// 面を構成するデータの読み取り。
	ParseMeshFaces(Model, fbxMesh);
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
	for (int index = 0; index < controlPointsCount; ++index) {

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

	auto& vertices = Model.vertices;
	auto& indices = Model.indices;

	// 1ファイルに複数メッシュのモデルは非対応
	assert(indices.size() == 0);

	// 面の数
	const int polygonCount = InputFbxMesh->GetPolygonCount();

	// UVデータの数
	const int textureUVCount = InputFbxMesh->GetTextureUVCount();

	// UV各リスト
	FbxStringList uvNames;
	InputFbxMesh->GetUVSetNames(uvNames);

	// 面ごとの情報の読み取り
	for (int index = 0; index < polygonCount; ++index) {

		// 面を構成する頂点の数を取得(3なら三角形ポリゴン)
		const int polygonSize = InputFbxMesh->GetPolygonSize(index);
		assert(polygonSize <= 4);

		// 1頂点ずつ処理
		for (int porygonIndex = 0; porygonIndex < polygonSize; ++porygonIndex) {

			// FBX頂点配列のインデックス
			int fbxIndex = InputFbxMesh->GetPolygonVertex(index, porygonIndex);
			assert(fbxIndex >= 0);

			// 法線情報読み込み
			FbxModel::VertexPosNormalUv& vertex = vertices[fbxIndex];
			FbxVector4 normal;
			if(InputFbxMesh->GetPolygonVertexNormal(index, porygonIndex, normal)){
				vertex.normal.x = (float)normal[0];
				vertex.normal.y = (float)normal[1];
				vertex.normal.z = (float)normal[2];
			}

			// テクスチャUV読み込み
			if(0 < textureUVCount){
				FbxVector2 uvs;
				bool lUnmappedUV;
				// 0版決め打ちで読み込み
				if(InputFbxMesh->GetPolygonVertexUV(index,porygonIndex,uvNames[0],uvs,lUnmappedUV)){
					vertex.uv.x = (float)uvs[0];
					vertex.uv.y = (float)uvs[1];
				}
			}

			// インデックス配列に頂点インデックスを追加。

			// 3頂点まで
			if (porygonIndex < 3) {

				// 1点追加し、他の2頂点と三角形を構成する。
				indices.emplace_back(fbxIndex);

			}
			// 4頂点目
			else {

				// 3点追加し、四角形の0,1,2,3の内、2,3,0で三角形を構成する。
				int index2 = indices[indices.size() - 1];
				int index3 = fbxIndex;
				int index0 = indices[indices.size() - 3];
				indices.emplace_back(index2);
				indices.emplace_back(index3);
				indices.emplace_back(index0);

			}

		}

	}

}

void FbxLoader::ParseMeshMaterial(FbxModel& Model, FbxNode* InputFbxNode)
{

	/*===== マテリアル情報読み取り =====*/

	const int materialCount = InputFbxNode->GetMaterialCount();
	if (0 < materialCount) {

		// 先頭のマテリアルを取得。
		FbxSurfaceMaterial* material = InputFbxNode->GetMaterial(0);

		// テクスチャを読み込んだかどうかを表すフラグ。
		bool textureLoaded = false;

		if (material) {

			// FbxSurfaceLambertクラスかどうかを調べる。
			if (material->GetClassId().Is(FbxSurfaceLambert::ClassId)) {

				FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(material);

				// 環境光係数
				FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
				Model.ambient.x = (float)ambient.Get()[0];
				Model.ambient.y = (float)ambient.Get()[1];
				Model.ambient.z = (float)ambient.Get()[2];

				// 拡散反射光係数
				FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
				Model.diffuse.x = (float)diffuse.Get()[0];
				Model.diffuse.y = (float)diffuse.Get()[1];
				Model.diffuse.z = (float)diffuse.Get()[2];

			}

			// ディフーズテクスチャを張り出す。
			const FbxProperty diffuseProperty = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (diffuseProperty.IsValid()) {

				const FbxFileTexture* texture = diffuseProperty.GetSrcObject<FbxFileTexture>();
				if (texture) {

					const char* filePath = texture->GetFileName();
					// ファイルパスからファイル名抽出。
					string path_str(filePath);
					string name = ExtractFileName(path_str);

					// パスを変換。
					wchar_t wFilePath[128];
					string fullPath = directryPath + name;
					MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wFilePath, _countof(wFilePath));

					// テクスチャ読み込み。
					Model.textureID = TextureManager::Instance()->LoadTexture(wFilePath);

					textureLoaded = true;

				}

			}

		}

		// テクスチャがない場合は白テクスチャを貼る。
		if (!textureLoaded) {

			Model.textureID = TextureManager::Instance()->CreateTexture({ 1.0f,1.0f,1.0f,1.0f });

		}

	}

}

void FbxLoader::ParseMeshTexture(FbxModel& Model, const std::string& filePath)
{
}

std::string FbxLoader::ExtractFileName(const std::string& path)
{

	/*===== ディレクトリを含んだファイルパスからファイル名を抽出 =====*/

	size_t pos1;

	// 区切り文字 '\\' が出てくる一番最後の部分を検索。
	pos1 = path.rfind('\\');
	if (pos1 != string::npos) {
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	// 区切り文字'/'が出てくる一番最後の部分を参照
	pos1 = path.rfind('/');
	if (pos1 != string::npos) {
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	return path;
}