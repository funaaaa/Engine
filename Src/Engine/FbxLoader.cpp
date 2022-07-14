#include "FbxLoader.h"
#include "DirectXBase.h"
#include <assert.h>
#include "TextureManager.h"

void FbxLoader::GetFbxData(const int& Index, std::vector<Vertex>& OutputVertex, std::vector<UINT>& OutputVertexIndex)
{

	// 指定されたインデックスが範囲外だったら何も返さない。
	if (fbxModelData_.size() - 1 < Index || Index < 0) return;

	// 指定されたインデックスを取得。
	FbxModel modelData_ = fbxModelData_[Index];

	// 頂点データを変換。
	const int VERTEX_COUNT = static_cast<int>(modelData_.vertices_.size());
	OutputVertex.resize(VERTEX_COUNT);
	for (int index_ = 0; index_ < VERTEX_COUNT; ++index_) {
		OutputVertex[index_].pos_ = modelData_.vertices_[index_].pos_;
		OutputVertex[index_].normal_ = modelData_.vertices_[index_].normal_;
		OutputVertex[index_].uv_ = modelData_.vertices_[index_].uv_;
	}

	// 頂点インデックスデータを変換。
	const int INDEX_COUNT = static_cast<int>(modelData_.indices_.size());
	OutputVertexIndex.resize(INDEX_COUNT);
	for (int index_ = 0; index_ < INDEX_COUNT; ++index_) {
		OutputVertexIndex[index_] = modelData_.indices_[index_];
	}

}

void FbxLoader::Init()
{

	/*===== 初期化処理 =====*/

	// 二重で初期化していないかをチェック。
	assert(fbxMgr_ == nullptr);

	// Fbxマネージャーを生成。
	fbxMgr_ = FbxManager::Create();

	// Fbxマネージャーの入出力設定
	FbxIOSettings* ios = FbxIOSettings::Create(fbxMgr_, IOSROOT);
	fbxMgr_->SetIOSettings(ios);

	// Fbxインポータの生成
	fbxImporter_ = FbxImporter::Create(fbxMgr_, "");

	// シーンを生成。
	fbxScene_ = FbxScene::Create(fbxMgr_, "fbxScene");

}

int FbxLoader::LoadModelFromFile(const string& DirectryPath, const string& ModelName)
{

	/*===== ロード関数 =====*/

	// このモデルが既にロードされていたら。
	const int MODEL_COUNT = static_cast<int>(fbxModelData_.size());
	for (int index_ = 0; index_ < MODEL_COUNT; ++index_) {

		// 名前が同じファイルがあったら。
		if (fbxModelData_[index_].modelName_ == DirectryPath + ModelName) {

			// そのデータを返す。
			return index_;

		}

	}

	// ディレクトリーパスを保存。
	this->directryPath_ = DirectryPath;

	// ファイル名を指定してFBXファイルを読み込む。
	if (!fbxImporter_->Initialize((DirectryPath + ModelName).c_str(), -1, fbxMgr_->GetIOSettings())) {
		assert(0);
	}

	// ファイルからロードしたFBXの情報をシーンにインポート。
	fbxImporter_->Import(fbxScene_);

	// モデルを生成。
	FbxModel modelData_;

	// FBXノードの数を取得。
	int nodeCount = fbxScene_->GetNodeCount();

	// 予め必要分のメモリを確保することで、アドレスがずれるのを予防。
	modelData_.nodes_.reserve(nodeCount);

	// ルートノードから順に解析してモデルに流し込む。
	ParseNodeRecursive(modelData_, fbxScene_->GetRootNode());

	// データを保存。
	fbxModelData_.emplace_back(modelData_);

	return static_cast<int>(fbxModelData_.size()) - 1;

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
	if (static_cast<int>(fbxModelData_.size()) - 1 < Index || Index < 0) return {};

	// ボーン配列
	std::vector<Bone>& bones_ = fbxModelData_[Index].bones_;

	SkinData returnData = {};

	const int BONE_SIZE = static_cast<int>(bones_.size());
	for (int index_ = 0; index_ < BONE_SIZE; ++index_) {

		if (16 <= index_)break;

		// 今の姿勢行列
		DirectX::XMMATRIX matCurrentPose;

		// 今の姿勢行列を取得。
		FbxAMatrix fbxCurrentPose = bones_[index_].fbxCluster_->GetLink()->EvaluateGlobalTransform(fbxModelData_[Index].currentTime_);

		// XMMATRIXに変換。
		ConvertMatrixFromFBX(matCurrentPose, fbxCurrentPose);

		// 合成してスキニング行列にする。
		returnData.bones_[index_] = bones_[index_].invInitialPose_ * matCurrentPose;

	}

	return returnData;

}

void FbxLoader::GetSkinComputeInput(const int& Index, std::vector<SkinComputeInput>& Input)
{

	/*===== スキニングアニメーション用コンピュートシェーダーの入力構造体を取得 =====*/

	// 指定されたインデックスが範囲外だったら何も返さない。
	if (static_cast<int>(fbxModelData_.size()) - 1 < Index || Index < 0) return;

	FbxModel indexModel = fbxModelData_[Index];
	SkinData skinData_ = GetSkinMat(Index);

	const int VERTEX_COUNT = static_cast<int>(indexModel.vertices_.size());
	for (int index_ = 0; index_ < VERTEX_COUNT; ++index_) {

		Input[index_].vertex_ = indexModel.vertices_[index_];
		Input[index_].skinData_ = skinData_;

	}

}

void FbxLoader::ParseNodeRecursive(FbxModel& Model, FbxNode* InputFbxNode, Node* Parent)
{

	/*===== 再帰的にノードを解析 =====*/

	// nullチェック。
	if (InputFbxNode == nullptr) return;

	// モデルにノードを追加。
	Model.nodes_.emplace_back();
	Node& node = Model.nodes_.back();

	// ノード名を取得。
	node.name_ = InputFbxNode->GetName();

	// FBXノードのローカル移動情報
	FbxDouble3 rotation_ = InputFbxNode->LclRotation.Get();
	FbxDouble3 scaling_ = InputFbxNode->LclScaling.Get();
	FbxDouble3 translation_ = InputFbxNode->LclTranslation.Get();

	// 形式変更して代入。
	node.rotation_ = { (float)rotation_[0], (float)rotation_[1], (float)rotation_[2], 0.0f };
	node.scaling_ = { (float)scaling_[0], (float)scaling_[1], (float)scaling_[2], 0.0f };
	node.translation_ = { (float)translation_[0], (float)translation_[1], (float)translation_[2], 1.0f };

	// 回転角をDegree(度)からラジアンに変換
	node.rotation_.m128_f32[0] = DirectX::XMConvertToRadians(node.rotation_.m128_f32[0]);
	node.rotation_.m128_f32[1] = DirectX::XMConvertToRadians(node.rotation_.m128_f32[1]);
	node.rotation_.m128_f32[2] = DirectX::XMConvertToRadians(node.rotation_.m128_f32[2]);

	// スケール、回転、平行移動行列の計算
	DirectX::XMMATRIX matScaling, matRotation, matTranslation;
	matScaling = DirectX::XMMatrixScalingFromVector(node.scaling_);
	matRotation = DirectX::XMMatrixScalingFromVector(node.rotation_);
	matTranslation = DirectX::XMMatrixScalingFromVector(node.translation_);

	// ローカル変形行列の計算
	node.transform_ = DirectX::XMMatrixIdentity();
	node.transform_ *= matScaling;
	node.transform_ *= matRotation;
	node.transform_ *= matTranslation;

	// グローバル変形行列の計算
	node.globalTransform_ = node.transform_;
	if (Parent) {

		node.parent_ = Parent;
		// 親の変形を乗算。
		node.globalTransform_ *= Parent->globalTransform_;

	}

	// FBXノードのメッシュ情報を解析
	FbxNodeAttribute* fbxNodeAttribute = InputFbxNode->GetNodeAttribute();

	if (fbxNodeAttribute) {

		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {

			Model.meshNode_ = &node;
			ParseMesh(Model, InputFbxNode);

		}

	}

	// 子ノードに対して再帰呼び出し。
	const int CHILD_COUNT = InputFbxNode->GetChildCount();
	for (int index_ = 0; index_ < CHILD_COUNT; ++index_) {
		ParseNodeRecursive(Model, InputFbxNode->GetChild(index_), &node);
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

	auto& vertices_ = Model.vertices_;

	// 頂点座標データの数。
	const int controlPointsCount = InputFbxMesh->GetControlPointsCount();

	// 必要数だけ頂点データ配列を確保。
	FbxModel::VertexPosNormalUvSkin vert{};
	Model.vertices_.resize(controlPointsCount, vert);

	// FBXメッシュの頂点座標配列を取得。
	FbxVector4* pCoord = InputFbxMesh->GetControlPoints();

	// FBXメッシュの全頂点座標をモデル内の配列にコピーする。
	for (int index_ = 0; index_ < controlPointsCount; ++index_) {

		FbxModel::VertexPosNormalUvSkin& vertex_ = vertices_[index_];

		// 座標のコピー
		vertex_.pos_.x_ = (float)pCoord[index_][0];
		vertex_.pos_.y_ = (float)pCoord[index_][1];
		vertex_.pos_.z_ = (float)pCoord[index_][2];

	}

}

void FbxLoader::ParseMeshFaces(FbxModel& Model, FbxMesh* InputFbxMesh)
{

	/*===== 面情報の読み取り =====*/

	auto& vertices_ = Model.vertices_;
	auto& indices_ = Model.indices_;

	// 1ファイルに複数メッシュのモデルは非対応
	assert(indices_.size() == 0);

	// 面の数
	const int polygonCount = InputFbxMesh->GetPolygonCount();

	// UVデータの数
	const int textureUVCount = InputFbxMesh->GetTextureUVCount();

	// UV各リスト
	FbxStringList uvNames;
	InputFbxMesh->GetUVSetNames(uvNames);

	// 面ごとの情報の読み取り
	for (int index_ = 0; index_ < polygonCount; ++index_) {

		// 面を構成する頂点の数を取得(3なら三角形ポリゴン)
		const int polygonSize = InputFbxMesh->GetPolygonSize(index_);
		assert(polygonSize <= 4);

		// 1頂点ずつ処理
		for (int porygonIndex = 0; porygonIndex < polygonSize; ++porygonIndex) {

			// FBX頂点配列のインデックス
			int fbxIndex = InputFbxMesh->GetPolygonVertex(index_, porygonIndex);
			assert(0 <= fbxIndex);

			// 法線情報読み込み
			FbxModel::VertexPosNormalUvSkin& vertex_ = vertices_[fbxIndex];
			FbxVector4 normal_;
			if (InputFbxMesh->GetPolygonVertexNormal(index_, porygonIndex, normal_)) {
				vertex_.normal_.x_ = static_cast<float>(normal_[0]);
				vertex_.normal_.y_ = static_cast<float>(normal_[1]);
				vertex_.normal_.z_ = static_cast<float>(normal_[2]);
			}

			// テクスチャUV読み込み
			if (0 < textureUVCount) {
				FbxVector2 uvs;
				bool lUnmappedUV;
				// 0版決め打ちで読み込み
				if (InputFbxMesh->GetPolygonVertexUV(index_, porygonIndex, uvNames[0], uvs, lUnmappedUV)) {
					vertex_.uv_.x_ = static_cast<float>(uvs[0]);
					vertex_.uv_.y_ = static_cast<float>(uvs[1]);
				}
			}

			// インデックス配列に頂点インデックスを追加。

			// 3頂点まで
			if (porygonIndex < 3) {

				// 1点追加し、他の2頂点と三角形を構成する。
				indices_.emplace_back(static_cast<unsigned short>(fbxIndex));

			}
			// 4頂点目
			else {

				// 3点追加し、四角形の0,1,2,3の内、2,3,0で三角形を構成する。
				int index2 = indices_[indices_.size() - 1];
				int index3 = fbxIndex;
				int index0 = indices_[indices_.size() - 3];
				indices_.emplace_back(static_cast<unsigned short>(index2));
				indices_.emplace_back(static_cast<unsigned short>(index3));
				indices_.emplace_back(static_cast<unsigned short>(index0));

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
		FbxSurfaceMaterial* material_ = InputFbxNode->GetMaterial(0);

		// テクスチャを読み込んだかどうかを表すフラグ。
		bool textureLoaded = false;

		if (material_) {

			// FbxSurfaceLambertクラスかどうかを調べる。
			if (material_->GetClassId().Is(FbxSurfaceLambert::ClassId)) {

				FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(material_);

				// 環境光係数
				FbxPropertyT<FbxDouble3> ambient_ = lambert->Ambient;
				Model.ambient_.x_ = static_cast<float>(ambient_.Get()[0]);
				Model.ambient_.y_ = static_cast<float>(ambient_.Get()[1]);
				Model.ambient_.z_ = static_cast<float>(ambient_.Get()[2]);

				// 拡散反射光係数
				FbxPropertyT<FbxDouble3> diffuse_ = lambert->Diffuse;
				Model.diffuse_.x_ = static_cast<float>(diffuse_.Get()[0]);
				Model.diffuse_.y_ = static_cast<float>(diffuse_.Get()[1]);
				Model.diffuse_.z_ = static_cast<float>(diffuse_.Get()[2]);

			}

			// ディフーズテクスチャを張り出す。
			const FbxProperty diffuseProperty = material_->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (diffuseProperty.IsValid()) {

				const FbxFileTexture* texture_ = diffuseProperty.GetSrcObject<FbxFileTexture>();
				if (texture_) {

					const char* filePath = texture_->GetFileName();
					// ファイルパスからファイル名抽出。
					string path_str(filePath);
					string name_ = ExtractFileName(path_str);

					// パスを変換。
					std::array<wchar_t, 128> wFilePath;
					string fullPath = directryPath_ + name_;
					MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wFilePath.data(), static_cast<int>(wFilePath.size()));

					// テクスチャ読み込み。
					Model.textureID_ = TextureManager::Ins()->LoadTexture(wFilePath.data());

					textureLoaded = true;

				}

			}

		}

		// テクスチャがない場合は白テクスチャを貼る。
		if (!textureLoaded) {

			Model.textureID_ = TextureManager::Ins()->CreateTexture({ 1.0f,1.0f,1.0f,1.0f });

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
	Model.hasAnimation_ = true;

	// ボーン配列の参照。
	std::vector<Bone>& bones_ = Model.bones_;

	// ボーンの数。
	const int CLUSTER_COUNT = fbxSkin->GetClusterCount();
	bones_.reserve(CLUSTER_COUNT);

	// 全てのボーンについて。
	for (int index_ = 0; index_ < CLUSTER_COUNT; ++index_) {

		// FBXボーン情報。
		FbxCluster* fbxCluster_ = fbxSkin->GetCluster(index_);

		// ボーン自体のノードの名前を取得。
		const char* boneName = fbxCluster_->GetLink()->GetName();

		// 新しくボーンを追加し、追加したボーンの参照を得る。
		bones_.emplace_back(Bone(boneName));
		Bone& bone = bones_.back();

		// 自作ボーンとFBXのボーンを紐づける。
		bone.fbxCluster_ = fbxCluster_;

		// FBXから初期姿勢行列を取得する。
		FbxAMatrix fbxMat;
		fbxCluster_->GetTransformLinkMatrix(fbxMat);

		// XMMatrix型に変換する。
		DirectX::XMMATRIX initialPose;
		ConvertMatrixFromFBX(initialPose, fbxMat);

		// 初期姿勢行列の逆行列を得る。
		bone.invInitialPose_ = DirectX::XMMatrixInverse(nullptr, initialPose);

	}

	// 二次元配列(ジャグ配列)
	// list   : 頂点が影響を受けるボーンの全リスト
	// vector : それを全頂点分
	std::vector<std::list<WeightSet>> weightLists(Model.vertices_.size());

	// 全てのボーンについて。
	for (int index_ = 0; index_ < CLUSTER_COUNT; ++index_) {

		// FBXボーン情報。
		FbxCluster* fbxCluster_ = fbxSkin->GetCluster(index_);

		// このボーンに影響を受ける頂点の数。
		int CONTROL_POINT_INDICES_COUNT = fbxCluster_->GetControlPointIndicesCount();

		// このボーンに影響を受ける頂点の数。
		int* controlPointIndices = fbxCluster_->GetControlPointIndices();
		double* controlPointWeights = fbxCluster_->GetControlPointWeights();

		// 影響を受ける全頂点について。
		for (int vertexIndex = 0; vertexIndex < CONTROL_POINT_INDICES_COUNT; ++vertexIndex) {

			// 頂点番号
			int vertIndex = controlPointIndices[vertexIndex];
			// スキンウェイト
			float weight_ = (float)controlPointWeights[vertexIndex];
			// その頂点の影響を受けるボーンリストに、ボーンとウェイトのペアを追加。
			weightLists[vertIndex].emplace_back(WeightSet({ static_cast<UINT>(index_), (weight_) }));

		}

	}

	// 頂点配列書き換え用の参照
	auto& vertices_ = Model.vertices_;

	// 各頂点について処理
	const int VERTICES_SIZE = static_cast<int>(vertices_.size());
	for (int index_ = 0; index_ < VERTICES_SIZE; ++index_) {

		// 頂点のウェイトから最も大きい4つを選択。
		auto& weightList = weightLists[index_];

		// 大小比較用のラムダ式をセットして降順にソート。
		weightList.sort([](auto const& lhs, auto const& rhs) {
			// ひだりの要素のほうが大きければtrue そうでなければfalseを返す。
			return rhs.weight_ < lhs.weight_;
			});

		int weightArrayIndex = 0;

		// 降順ソート済みのウェイトリストから。
		for (auto& weightSet : weightList) {

			// 頂点データに書き込み。
			vertices_[index_].boneIndex_[weightArrayIndex] = weightSet.index_;
			vertices_[index_].boneWeight_[weightArrayIndex] = weightSet.weight_;

			// 4つに達したら終了。
			if (FbxModel::MAX_BONE_INDICES <= ++weightArrayIndex) {

				float weight_ = 0.0f;

				// 2番目以降のウェイトを合計。
				for (int boneIndex_ = 1; boneIndex_ < FbxModel::MAX_BONE_INDICES; ++boneIndex_) {

					weight_ += vertices_[index_].boneWeight_[boneIndex_];

				}

				// 合計で1.0f(100%)になる用に調整。
				vertices_[index_].boneWeight_[0] = 1.0f - weight_;
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

	FbxScene* fbxScene_ = FbxLoader::Ins()->GetFbxScene();

	// 0番アニメーションの再生。
	FbxAnimStack* animstack = fbxScene_->GetSrcObject<FbxAnimStack>(0);

	// アニメーションの名前取得。
	const char* animstackName = animstack->GetName();

	// アニメーションの時間情報。
	FbxTakeInfo* takeInfo = fbxScene_->GetTakeInfo(animstackName);

	// 開始時間取得。
	startTime_ = takeInfo->mLocalTimeSpan.GetStart();

	// 終了時間取得。
	endTime_ = takeInfo->mLocalTimeSpan.GetStop();

	// 開始時間に合わせる。
	currentTime_ = startTime_;

	isInit_ = true;

}

void FbxModel::PlayAnimation()
{

	/*===== FBXアニメーションの再生 =====*/

	// 初期化されていたら初期化処理を呼ばない。
	if (!isInit_) {

		FbxScene* fbxScene_ = FbxLoader::Ins()->GetFbxScene();

		// 0番アニメーションの再生。
		FbxAnimStack* animstack = fbxScene_->GetSrcObject<FbxAnimStack>(0);

		// アニメーションの名前取得。
		const char* animstackName = animstack->GetName();

		// アニメーションの時間情報。
		FbxTakeInfo* takeInfo = fbxScene_->GetTakeInfo(animstackName);

		// 開始時間取得。
		startTime_ = takeInfo->mLocalTimeSpan.GetStart();

		// 終了時間取得。
		endTime_ = takeInfo->mLocalTimeSpan.GetStop();

		// 開始時間に合わせる。
		currentTime_ = startTime_;

		isInit_ = true;

	}

	// 再生中状態にする。
	isPlay_ = true;

}

void FbxModel::StopAnimation()
{

	/*===== FBXアニメーションの停止 =====*/

	isPlay_ = false;

}
