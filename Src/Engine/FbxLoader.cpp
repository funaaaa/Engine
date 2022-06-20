#include "FbxLoader.h"
#include "DirectXBase.h"
#include <assert.h>
#include "TextureManager.h"

void FbxLoader::GetFbxData(const int& Index, std::vector<Vertex>& OutputVertex, std::vector<UINT> OutputVertexIndex)
{

	// 指定されたインデックスが範囲外だったら何も返さない。
	if (fbxModelData.size() - 1 < Index || Index < 0) return;

	// 指定されたインデックスを取得。
	FbxModel modelData = fbxModelData[Index];

	// 頂点データを変換。
	const int VERTEX_COUNT = static_cast<int>(modelData.vertices.size());
	OutputVertex.resize(VERTEX_COUNT);
	for (int index = 0; index < VERTEX_COUNT; ++index) {
		OutputVertex[index].pos = modelData.vertices[index].pos;
		OutputVertex[index].normal = modelData.vertices[index].normal;
		OutputVertex[index].uv = modelData.vertices[index].uv;
	}

	// 頂点インデックスデータを変換。
	const int INDEX_COUNT = static_cast<int>(modelData.indices.size());
	OutputVertexIndex.resize(INDEX_COUNT);
	for (int index = 0; index < INDEX_COUNT; ++index) {
		OutputVertexIndex[index] = modelData.indices[index];
	}

}

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

	// シーンを生成。
	fbxScene = FbxScene::Create(fbxMgr, "fbxScene");

}

int FbxLoader::LoadModelFromFile(const string& DirectryPath, const string& ModelName)
{

	/*===== ロード関数 =====*/

	// このモデルが既にロードされていたら。
	const int MODEL_COUNT = static_cast<int>(fbxModelData.size());
	for (int index = 0; index < MODEL_COUNT; ++index) {

		// 名前が同じファイルがあったら。
		if (fbxModelData[index].modelName == DirectryPath + ModelName) {

			// そのデータを返す。
			return index;

		}

	}

	// ディレクトリーパスを保存。
	this->directryPath = DirectryPath;

	// ファイル名を指定してFBXファイルを読み込む。
	if (!fbxImporter->Initialize((DirectryPath + ModelName).c_str(), -1, fbxMgr->GetIOSettings())) {
		assert(0);
	}

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

	// データを保存。
	fbxModelData.emplace_back(modelData);

	return static_cast<int>(fbxModelData.size()) - 1;

}

void FbxLoader::ConvertMatrixFromFBX(DirectX::XMMATRIX& Dst, const FbxAMatrix& Src)
{

	/*===== FBXの行列をXMMATRIXに変換 =====*/

	// 行
	for (int row = 0; row < 4; ++row) {

		// 列
		for (int str = 0; str < 4; ++str) {

			Dst.r[row].m128_f32[str] = (float)Src.Get(row, str);

		}

	}

}

FbxLoader::SkinData FbxLoader::GetSkinMat(const int& Index)
{

	/*===== スキニング行列を取得 =====*/

	// 指定されたインデックスが範囲外だったら何も返さない。
	if (static_cast<int>(fbxModelData.size()) - 1 < Index || Index < 0) return {};

	// ボーン配列
	std::vector<Bone>& bones = fbxModelData[Index].bones;

	SkinData returnData = {};

	const int BONE_SIZE = static_cast<int>(bones.size());
	for (int index = 0; index < BONE_SIZE; ++index) {

		// 今の姿勢行列
		DirectX::XMMATRIX matCurrentPose;

		// 今の姿勢行列を取得。
		FbxAMatrix fbxCurrentPose = bones[index].fbxCluster->GetLink()->EvaluateGlobalTransform(fbxModelData[Index].currentTime);

		// XMMATRIXに変換。
		ConvertMatrixFromFBX(matCurrentPose, fbxCurrentPose);

		// 合成してスキニング行列にする。
		returnData.bones[index] = bones[index].invInitialPose * matCurrentPose;

	}

	return returnData;

}

void FbxLoader::GetSkinComputeInput(const int& Index, std::vector<SkinComputeInput>& Input)
{

	/*===== スキニングアニメーション用コンピュートシェーダーの入力構造体を取得 =====*/

	// 指定されたインデックスが範囲外だったら何も返さない。
	if (static_cast<int>(fbxModelData.size()) - 1 < Index || Index < 0) return;

	FbxModel indexModel = fbxModelData[Index];
	SkinData skinData = GetSkinMat(Index);

	const int VERTEX_COUNT = static_cast<int>(indexModel.vertices.size());
	for (int index = 0; index < VERTEX_COUNT; ++index) {

		Input[index].vertex = indexModel.vertices[index];
		Input[index].skinData = skinData;

	}

}

void FbxLoader::ParseNodeRecursive(FbxModel& Model, FbxNode* InputFbxNode, Node* Parent)
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
	node.rotation.m128_f32[0] = DirectX::XMConvertToRadians(node.rotation.m128_f32[0]);
	node.rotation.m128_f32[1] = DirectX::XMConvertToRadians(node.rotation.m128_f32[1]);
	node.rotation.m128_f32[2] = DirectX::XMConvertToRadians(node.rotation.m128_f32[2]);

	// スケール、回転、平行移動行列の計算
	DirectX::XMMATRIX matScaling, matRotation, matTranslation;
	matScaling = DirectX::XMMatrixScalingFromVector(node.scaling);
	matRotation = DirectX::XMMatrixScalingFromVector(node.rotation);
	matTranslation = DirectX::XMMatrixScalingFromVector(node.translation);

	// ローカル変形行列の計算
	node.transform = DirectX::XMMatrixIdentity();
	node.transform *= matScaling;
	node.transform *= matRotation;
	node.transform *= matTranslation;

	// グローバル変形行列の計算
	node.globalTransform = node.transform;
	if (Parent) {

		node.parent = Parent;
		// 親の変形を乗算。
		node.globalTransform *= Parent->globalTransform;

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
	// スキニング情報の読み取り。
	ParseSkin(Model, fbxMesh);

}

void FbxLoader::ParseMeshVertices(FbxModel& Model, FbxMesh* InputFbxMesh)
{

	/*===== 頂点情報の読み取り =====*/

	auto& vertices = Model.vertices;

	// 頂点座標データの数。
	const int controlPointsCount = InputFbxMesh->GetControlPointsCount();

	// 必要数だけ頂点データ配列を確保。
	FbxModel::VertexPosNormalUvSkin vert{};
	Model.vertices.resize(controlPointsCount, vert);

	// FBXメッシュの頂点座標配列を取得。
	FbxVector4* pCoord = InputFbxMesh->GetControlPoints();

	// FBXメッシュの全頂点座標をモデル内の配列にコピーする。
	for (int index = 0; index < controlPointsCount; ++index) {

		FbxModel::VertexPosNormalUvSkin& vertex = vertices[index];

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
			FbxModel::VertexPosNormalUvSkin& vertex = vertices[fbxIndex];
			FbxVector4 normal;
			if (InputFbxMesh->GetPolygonVertexNormal(index, porygonIndex, normal)) {
				vertex.normal.x = (float)normal[0];
				vertex.normal.y = (float)normal[1];
				vertex.normal.z = (float)normal[2];
			}

			// テクスチャUV読み込み
			if (0 < textureUVCount) {
				FbxVector2 uvs;
				bool lUnmappedUV;
				// 0版決め打ちで読み込み
				if (InputFbxMesh->GetPolygonVertexUV(index, porygonIndex, uvNames[0], uvs, lUnmappedUV)) {
					vertex.uv.x = (float)uvs[0];
					vertex.uv.y = (float)uvs[1];
				}
			}

			// インデックス配列に頂点インデックスを追加。

			// 3頂点まで
			if (porygonIndex < 3) {

				// 1点追加し、他の2頂点と三角形を構成する。
				indices.emplace_back(static_cast<unsigned short>(fbxIndex));

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
					Model.textureID = TextureManager::Ins()->LoadTexture(wFilePath);

					textureLoaded = true;

				}

			}

		}

		// テクスチャがない場合は白テクスチャを貼る。
		if (!textureLoaded) {

			Model.textureID = TextureManager::Ins()->CreateTexture({ 1.0f,1.0f,1.0f,1.0f });

		}

	}

}

//void FbxLoader::ParseMeshTexture(FbxModel& Model, const std::string& filePath)
//{
//}

void FbxLoader::ParseSkin(FbxModel& Model, FbxMesh* InputFbxMesh)
{

	/*===== スキニング情報読み取り =====*/

	FbxSkin* fbxSkin = static_cast<FbxSkin*>(InputFbxMesh->GetDeformer(0, FbxDeformer::eSkin));

	// スキニング情報がなければ終了。
	if (fbxSkin == nullptr) {
		return;
	}

	// アニメーションを持っているフラグを立てる。
	Model.hasAnimation = true;

	// ボーン配列の参照。
	std::vector<Bone>& bones = Model.bones;

	// ボーンの数。
	const int CLUSTER_COUNT = fbxSkin->GetClusterCount();
	bones.reserve(CLUSTER_COUNT);

	// 全てのボーンについて。
	for (int index = 0; index < CLUSTER_COUNT; ++index) {

		// FBXボーン情報。
		FbxCluster* fbxCluster = fbxSkin->GetCluster(index);

		// ボーン自体のノードの名前を取得。
		const char* boneName = fbxCluster->GetLink()->GetName();

		// 新しくボーンを追加し、追加したボーンの参照を得る。
		bones.emplace_back(Bone(boneName));
		Bone& bone = bones.back();

		// 自作ボーンとFBXのボーンを紐づける。
		bone.fbxCluster = fbxCluster;

		// FBXから初期姿勢行列を取得する。
		FbxAMatrix fbxMat;
		fbxCluster->GetTransformLinkMatrix(fbxMat);

		// XMMatrix型に変換する。
		DirectX::XMMATRIX initialPose;
		ConvertMatrixFromFBX(initialPose, fbxMat);

		// 初期姿勢行列の逆行列を得る。
		bone.invInitialPose = DirectX::XMMatrixInverse(nullptr, initialPose);

	}

	// 二次元配列(ジャグ配列)
	// list   : 頂点が影響を受けるボーンの全リスト
	// vector : それを全頂点分
	std::vector<std::list<WeightSet>> weightLists(Model.vertices.size());

	// 全てのボーンについて。
	for (int index = 0; index < CLUSTER_COUNT; ++index) {

		// FBXボーン情報。
		FbxCluster* fbxCluster = fbxSkin->GetCluster(index);

		// このボーンに影響を受ける頂点の数。
		int CONTROL_POINT_INDICES_COUNT = fbxCluster->GetControlPointIndicesCount();

		// このボーンに影響を受ける頂点の数。
		int* controlPointIndices = fbxCluster->GetControlPointIndices();
		double* controlPointWeights = fbxCluster->GetControlPointWeights();

		// 影響を受ける全頂点について。
		for (int vertexIndex = 0; vertexIndex < CONTROL_POINT_INDICES_COUNT; ++vertexIndex) {

			// 頂点番号
			int vertIndex = controlPointIndices[vertexIndex];
			// スキンウェイト
			float weight = (float)controlPointWeights[vertexIndex];
			// その頂点の影響を受けるボーンリストに、ボーンとウェイトのペアを追加。
			weightLists[vertIndex].emplace_back(WeightSet({ (UINT)index, (weight) }));

		}

	}

	// 頂点配列書き換え用の参照
	auto& vertices = Model.vertices;

	// 各頂点について処理
	const int VERTICES_SIZE = static_cast<int>(vertices.size());
	for (int index = 0; index < VERTICES_SIZE; ++index) {

		// 頂点のウェイトから最も大きい4つを選択。
		auto& weightList = weightLists[index];

		// 大小比較用のラムダ式をセットして降順にソート。
		weightList.sort([](auto const& lhs, auto const& rhs) {
			// ひだりの要素のほうが大きければtrue そうでなければfalseを返す。
			return rhs.weight < lhs.weight;
			});

		int weightArrayIndex = 0;

		// 降順ソート済みのウェイトリストから。
		for (auto& weightSet : weightList) {

			// 頂点データに書き込み。
			vertices[index].boneIndex[weightArrayIndex] = weightSet.index;
			vertices[index].boneWeight[weightArrayIndex] = weightSet.weight;

			// 4つに達したら終了。
			if (FbxModel::MAX_BONE_INDICES <= ++weightArrayIndex) {

				float weight = 0.0f;

				// 2番目以降のウェイトを合計。
				for (int boneIndex = 1; boneIndex < FbxModel::MAX_BONE_INDICES; ++boneIndex) {

					weight += vertices[index].boneWeight[boneIndex];

				}

				// 合計で1.0f(100%)になる用に調整。
				vertices[index].boneWeight[0] = 1.0f - weight;
				break;

			}

		}

	}

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

void FbxModel::InitAnimation()
{

	/*===== FBXアニメーションの再生 =====*/

	FbxScene* fbxScene = FbxLoader::Ins()->GetFbxScene();

	// 0番アニメーションの再生。
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);

	// アニメーションの名前取得。
	const char* animstackName = animstack->GetName();

	// アニメーションの時間情報。
	FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animstackName);

	// 開始時間取得。
	startTime = takeInfo->mLocalTimeSpan.GetStart();

	// 終了時間取得。
	endTime = takeInfo->mLocalTimeSpan.GetStop();

	// 開始時間に合わせる。
	currentTime = startTime;

	isInit = true;

}

void FbxModel::PlayAnimation()
{

	/*===== FBXアニメーションの再生 =====*/

	// 初期化されていたら初期化処理を呼ばない。
	if (!isInit) {

		FbxScene* fbxScene = FbxLoader::Ins()->GetFbxScene();

		// 0番アニメーションの再生。
		FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);

		// アニメーションの名前取得。
		const char* animstackName = animstack->GetName();

		// アニメーションの時間情報。
		FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animstackName);

		// 開始時間取得。
		startTime = takeInfo->mLocalTimeSpan.GetStart();

		// 終了時間取得。
		endTime = takeInfo->mLocalTimeSpan.GetStop();

		// 開始時間に合わせる。
		currentTime = startTime;

		isInit = true;

	}

	// 再生中状態にする。
	isPlay = true;

}

void FbxModel::StopAnimation()
{

	/*===== FBXアニメーションの停止 =====*/

	isPlay = false;

}
