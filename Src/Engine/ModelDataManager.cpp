#include "ModelDataManager.h"
#include "Fbxsdk.h"
#include <sstream>
#include <fstream>
#include <memory>

vector<ModelData> ModelDataManager::modelData{};

void ModelDataManager::LoadFbx(const char* fbxFileName, Object3DDeliveryData& objectBuffer)
{
	//fbxファイルがロード済みかどうか
	bool isLoad = false;
	//ロード済みだった場合、何番目の要素に保存されているのかを取得する変数
	int dataNumber = 0;

	for (int i = 0; i < ModelDataManager::modelData.size(); ++i) {
		if (modelData.at(i).modelName == fbxFileName) {
			isLoad = true;
			dataNumber = i;
			break;
		}
	}

	//fbxファイルが未ロードだったらロードする
	if (isLoad == false) {
		ModelDataManager::modelData.push_back({});
		ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).modelName = fbxFileName;

		//FbxManager作成			FbxImportやFbxSceneを作成するために必要な管理クラス
		FbxManager* fbx_manager = FbxManager::Create();

		//FbxImporter作成		FbxファイルをImportするためのパーサークラス。このクラスでファイルの持つデータをメッシュやカメラなどの情報に分解してくれる。
		FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager, "ImportTest");

		//FbxScene作成			分解されたFbxのデータを保存するためのクラス
		FbxScene* fbx_scene = FbxScene::Create(fbx_manager, "SceneTest");

		//ファイルを初期化する		ファイルをロード
		if (fbx_importer->Initialize(fbxFileName) == false)
		{
			fbx_importer->Destroy();
			fbx_scene->Destroy();
			fbx_manager->Destroy();

			//初期化失敗
			return;
		}

		//インポート				Importに入っている元データを分解したデータをsceneに入れる。分解処理をするので非常に処理が重い。
		if (fbx_importer->Import(fbx_scene) == false)
		{
			fbx_importer->Destroy();
			fbx_scene->Destroy();
			fbx_manager->Destroy();

			//インポート失敗
			return;
		}

		//ルートノードを取得		データを調べる時、ルートノードを取得すれば全てのデータにたどり着くことができる。
		FbxNode* root_node = fbx_scene->GetRootNode();

		FbxGeometryConverter converter(fbx_manager);
		//ポリゴンを三角形にする		この処理は重い。
		converter.Triangulate(fbx_scene, true);

		//全Mesh分割
		bool i = converter.SplitMeshesPerMaterial(fbx_scene, true);

		//メッシュNodeを探す		頂点と法線のデータが含まれているeMeshを探す
		vector<pair<string, FbxMesh*>> meshList;
		CollectMeshNode(fbx_scene->GetRootNode(), meshList);

		//頂点インデックスの設定		ポリゴンの数だけ連番として保存する
		int polygonCount = 0;	//まえまでのポリゴン数を保存する
		for (int i = 0; i < meshList.size(); ++i) {
			int* indices = meshList.at(i).second->GetPolygonVertices();
			int vertexCount = meshList.at(i).second->GetPolygonCount();
			for (int k = 0; k < vertexCount; k++)
			{
				//2 => 1 => 0にしてるのは左手系対策
				ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).index.push_back(k * 3 + 2);
				ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).index.push_back(k * 3 + 1);
				ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).index.push_back(k * 3);

				//ついでにproSpriteにも値を入れる
				objectBuffer.index.push_back(k * 3 + 2);
				objectBuffer.index.push_back(k * 3 + 1);
				objectBuffer.index.push_back(k * 3);
			}
			polygonCount += vertexCount;
		}

		//頂点の設定
		for (int i = 0; i < meshList.size(); ++i) {
			//頂点バッファを取得
			FbxVector4* vertices = meshList.at(i).second->GetControlPoints();
			//頂点座標の数の取得
			int polygon_vertex_count = meshList.at(i).second->GetPolygonVertexCount();
			//頂点インデックスバッファの取得
			int* indices = meshList.at(i).second->GetPolygonVertices();
			//uvsetの名前保存用
			FbxStringList uvset_names;
			//UVSetの名前リストを取得
			meshList.at(i).second->GetUVSetNames(uvset_names);
			FbxArray<FbxVector2> uv_buffer;
			//UVSetの名前からUVSetを取得する
			//今回はシングルなので最初の名前を使う
			meshList.at(i).second->GetPolygonVertexUVs(uvset_names.GetStringAt(0), uv_buffer);
			//法線リストの取得
			FbxArray<FbxVector4> normals;
			meshList.at(i).second->GetPolygonVertexNormals(normals);

			for (int j = 0; j < polygon_vertex_count; j++)
			{
				int index = indices[j];
				Vertex vertex{};
				//頂点の設定
				XMFLOAT3 pos{};
				pos.x = (float)-vertices[index][0];
				pos.y = (float)vertices[index][1];
				pos.z = (float)vertices[index][2];
				XMVECTOR a = XMVector3Transform(XMLoadFloat3(&pos), XMMatrixRotationX(XM_PIDIV2));
				XMStoreFloat3(&vertex.pos, a);
				//vertex.pos = pos;
				//法線の設定
				XMFLOAT3 normal{};
				normal.x = -normals[j][0];
				normal.y = normals[j][1];
				normal.z = normals[j][2];
				vertex.normal = normal;
				//uvの設定
				XMFLOAT2 uv{};
				uv.x = uv_buffer[j][0];
				uv.y = -uv_buffer[j][1];
				vertex.uv = uv;
				ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).vertex.push_back(vertex);
				//ついでにproSpriteにも値を入れる
				objectBuffer.vertex.push_back(vertex);
			}
		}
		return;
	}

	//fbxのデータをproSpriteに入れる
	for (int i = 0; i < modelData.at(dataNumber).index.size(); ++i) {
		objectBuffer.index.push_back(modelData.at(dataNumber).index.at(i));
	}
	for (int i = 0; i < modelData.at(dataNumber).vertex.size(); ++i) {
		objectBuffer.vertex.push_back(modelData.at(dataNumber).vertex.at(i));
	}
}

void ModelDataManager::LoadObj(string directoryPath, string fileName, Object3DDeliveryData& objectBuffer, bool isSmoothing)
{
	//objファイルがロード済みかどうか
	bool isLoad = false;
	//ロード済みだった場合、何番目の要素に保存されているのかを取得する変数
	int dataNumber = 0;

	for (int i = 0; i < ModelDataManager::modelData.size(); ++i) {
		if (modelData.at(i).modelName == directoryPath + fileName && modelData.at(i).isSmoothing == isSmoothing) {
			isLoad = true;
			dataNumber = i;
			break;
		}
	}

	//objファイルが未ロードだったらロードする
	if (isLoad == false) {
		//同じ頂点を指し示している頂点インデックスを保存しておくためのマップ
		map<unsigned short, vector<unsigned short>> smoothData;

		modelData.push_back({});
		ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).modelName = directoryPath + fileName;
		ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).isSmoothing = isSmoothing;

		//フィルストリーム
		ifstream file;
		//.objファイルを開く
		file.open(directoryPath + fileName);
		//ファイルオープン失敗をチェック
		if (file.fail()) {
			assert(0);
		}
		//一行ずつ読み込む
		string line;
		vector<XMFLOAT3> position;	//座標を保存しておく
		vector<XMFLOAT2> uv;		//uvを保存しておく
		vector<XMFLOAT3> normal;	//法線ベクトルを保存しておく
		vector<unsigned short> index;
		while (getline(file, line)) {
			//1行分の文字列をストリームに変換して解析しやすくする
			istringstream lineStream(line);
			//半角スペース区切りで行の先頭文字を取得
			string key;
			getline(lineStream, key, ' ');

			//先頭文字がvなら頂点座標
			if (key == "v") {
				//X,Y,Z座標読み込み
				XMFLOAT3 pos{};
				lineStream >> pos.x;
				lineStream >> pos.y;
				lineStream >> pos.z;
				//座標を一旦保存
				position.push_back(pos);
			}
			//先頭文字がvtならテクスチャ
			if (key == "vt") {
				//U,V成分読み込み
				XMFLOAT2 texcoord{};
				lineStream >> texcoord.x;
				lineStream >> texcoord.y;
				//V方向反転
				texcoord.y = 1.0f - texcoord.y;
				//テクスチャ座標データに追加
				uv.push_back(texcoord);
			}
			//先頭文字がvnなら法線ベクトル
			if (key == "vn") {
				//X,Y,Z成分読み込み
				XMFLOAT3 norm{};
				lineStream >> norm.x;
				lineStream >> norm.y;
				lineStream >> norm.z;
				//法線ベクトルデータに追加
				normal.push_back(norm);
			}
			//先頭文字がfならポリゴン(三角形)
			if (key == "f") {
				//半角スペース区切りで行の続きを読み込む
				string indexString;
				unsigned short indexPosition;		//受け皿
				unsigned short indexNormal;			//受け皿
				unsigned short indexTexcoord;		//受け皿
				while (getline(lineStream, indexString, ' ')) {
					//頂点インデックス一個分の文字列をストリームに変換して解析しやすくする
					istringstream indexStream(indexString);
					indexStream >> indexPosition;
					indexStream.seekg(1, ios_base::cur);	//スラッシュを飛ばす
					indexStream >> indexTexcoord;
					indexStream.seekg(1, ios_base::cur);	//スラッシュを飛ばす
					indexStream >> indexNormal;
					//頂点データの追加
					Vertex vert{};
					vert.pos = position[indexPosition - 1];
					vert.normal = normal[indexNormal - 1];
					vert.uv = uv[indexTexcoord - 1];
					//モデルデータに追加
					modelData.at(modelData.size() - 1).vertex.push_back(vert);
					modelData.at(modelData.size() - 1).index.push_back(modelData.at(modelData.size() - 1).index.size());
					//proSpriteにも追加
					objectBuffer.vertex.push_back(vert);
					objectBuffer.index.push_back(objectBuffer.index.size());
					//isSmoothingがtrueなら頂点情報を追加する
					if (isSmoothing == true) {
						smoothData[indexPosition].push_back(objectBuffer.vertex.size() - 1);
					}
				}
			}
			//先頭文字列がmtllibならマテリアル
			if (key == "mtllib") {
				//マテリアルのファイル名読み込み
				string materialFileName;
				lineStream >> materialFileName;
				//マテリアルの読み込み
				LoadObjMaterial(directoryPath + materialFileName, modelData.at(modelData.size() - 1), objectBuffer);
			}
		}
		//ファイルを閉じる
		file.close();

		//isSmoothingがtrueだったら法線情報をなめらかにする
		if (isSmoothing == true) {
			CalculateSmoothedVertexNormals(smoothData, objectBuffer, modelData.at(modelData.size() - 1));
		}

		return;
	}

	//objのデータをproSpriteに入れる
	for (int i = 0; i < modelData.at(dataNumber).index.size(); ++i) {
		objectBuffer.index.push_back(modelData.at(dataNumber).index.at(i));
	}
	for (int i = 0; i < modelData.at(dataNumber).vertex.size(); ++i) {
		objectBuffer.vertex.push_back(modelData.at(dataNumber).vertex.at(i));
	}
	objectBuffer.constBufferDataB1.alpha = 1;
	objectBuffer.constBufferDataB1.ambient = modelData.at(dataNumber).material.ambient;
	objectBuffer.constBufferDataB1.diffuse = modelData.at(dataNumber).material.diffuse;
	objectBuffer.constBufferDataB1.specular = modelData.at(dataNumber).material.specular;
}

//void ModelDataManager::LoadObj(string directoryPath, string fileName, Object3DShadow& objectBuffer, bool isSmoothing)
//{
//	//objファイルがロード済みかどうか
//	bool isLoad = false;
//	//ロード済みだった場合、何番目の要素に保存されているのかを取得する変数
//	int dataNumber = 0;
//
//	for (int i = 0; i < ModelDataManager::modelData.size(); ++i) {
//		if (modelData.at(i).modelName == directoryPath + fileName && modelData.at(i).isSmoothing == isSmoothing) {
//			isLoad = true;
//			dataNumber = i;
//			break;
//		}
//	}
//
//	//objファイルが未ロードだったらロードする
//	if (isLoad == false) {
//		//同じ頂点を指し示している頂点インデックスを保存しておくためのマップ
//		map<unsigned short, vector<unsigned short>> smoothData;
//
//		modelData.push_back({});
//		ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).modelName = directoryPath + fileName;
//		ModelDataManager::modelData.at(ModelDataManager::modelData.size() - 1).isSmoothing = isSmoothing;
//
//		//フィルストリーム
//		ifstream file;
//		//.objファイルを開く
//		file.open(directoryPath + fileName);
//		//ファイルオープン失敗をチェック
//		if (file.fail()) {
//			assert(0);
//		}
//		//一行ずつ読み込む
//		string line;
//		vector<XMFLOAT3> position;	//座標を保存しておく
//		vector<XMFLOAT2> uv;		//uvを保存しておく
//		vector<XMFLOAT3> normal;	//法線ベクトルを保存しておく
//		vector<unsigned short> index;
//		while (getline(file, line)) {
//			//1行分の文字列をストリームに変換して解析しやすくする
//			istringstream lineStream(line);
//			//半角スペース区切りで行の先頭文字を取得
//			string key;
//			getline(lineStream, key, ' ');
//
//			//先頭文字がvなら頂点座標
//			if (key == "v") {
//				//X,Y,Z座標読み込み
//				XMFLOAT3 pos{};
//				lineStream >> pos.x;
//				lineStream >> pos.y;
//				lineStream >> pos.z;
//				//座標を一旦保存
//				position.push_back(pos);
//			}
//			//先頭文字がvtならテクスチャ
//			if (key == "vt") {
//				//U,V成分読み込み
//				XMFLOAT2 texcoord{};
//				lineStream >> texcoord.x;
//				lineStream >> texcoord.y;
//				//V方向反転
//				texcoord.y = 1.0f - texcoord.y;
//				//テクスチャ座標データに追加
//				uv.push_back(texcoord);
//			}
//			//先頭文字がvnなら法線ベクトル
//			if (key == "vn") {
//				//X,Y,Z成分読み込み
//				XMFLOAT3 norm{};
//				lineStream >> norm.x;
//				lineStream >> norm.y;
//				lineStream >> norm.z;
//				//法線ベクトルデータに追加
//				normal.push_back(norm);
//			}
//			//先頭文字がfならポリゴン(三角形)
//			if (key == "f") {
//				//半角スペース区切りで行の続きを読み込む
//				string indexString;
//				unsigned short indexPosition;		//受け皿
//				unsigned short indexNormal;			//受け皿
//				unsigned short indexTexcoord;		//受け皿
//				while (getline(lineStream, indexString, ' ')) {
//					//頂点インデックス一個分の文字列をストリームに変換して解析しやすくする
//					istringstream indexStream(indexString);
//					indexStream >> indexPosition;
//					indexStream.seekg(1, ios_base::cur);	//スラッシュを飛ばす
//					indexStream >> indexTexcoord;
//					indexStream.seekg(1, ios_base::cur);	//スラッシュを飛ばす
//					indexStream >> indexNormal;
//					//頂点データの追加
//					Vertex vert{};
//					vert.pos = position[indexPosition - 1];
//					vert.normal = normal[indexNormal - 1];
//					vert.uv = uv[indexTexcoord - 1];
//					//モデルデータに追加
//					modelData.at(modelData.size() - 1).vertex.push_back(vert);
//					modelData.at(modelData.size() - 1).index.push_back(modelData.at(modelData.size() - 1).index.size());
//					//proSpriteにも追加
//					objectBuffer.vertex.push_back(vert);
//					objectBuffer.index.push_back(objectBuffer.index.size());
//					//isSmoothingがtrueなら頂点情報を追加する
//					if (isSmoothing == true) {
//						smoothData[indexPosition].push_back(objectBuffer.vertex.size() - 1);
//					}
//				}
//			}
//			//先頭文字列がmtllibならマテリアル
//			if (key == "mtllib") {
//				//マテリアルのファイル名読み込み
//				string materialFileName;
//				lineStream >> materialFileName;
//				//マテリアルの読み込み
//				LoadObjMaterial(directoryPath + materialFileName, modelData.at(modelData.size() - 1), objectBuffer);
//			}
//		}
//		//ファイルを閉じる
//		file.close();
//
//		//isSmoothingがtrueだったら法線情報をなめらかにする
//		if (isSmoothing == true) {
//			CalculateSmoothedVertexNormals(smoothData, objectBuffer, modelData.at(modelData.size() - 1));
//		}
//
//		return;
//	}
//
//	//objのデータをproSpriteに入れる
//	for (int i = 0; i < modelData.at(dataNumber).index.size(); ++i) {
//		objectBuffer.index.push_back(modelData.at(dataNumber).index.at(i));
//	}
//	for (int i = 0; i < modelData.at(dataNumber).vertex.size(); ++i) {
//		objectBuffer.vertex.push_back(modelData.at(dataNumber).vertex.at(i));
//	}
//	objectBuffer.constBuffDataB1.alpha = 1;
//	objectBuffer.constBuffDataB1.ambient = modelData.at(dataNumber).material.ambient;
//	objectBuffer.constBuffDataB1.diffuse = modelData.at(dataNumber).material.diffuse;
//	objectBuffer.constBuffDataB1.specular = modelData.at(dataNumber).material.specular;
//}

void ModelDataManager::CollectMeshNode(FbxNode* node, vector<pair<string, FbxMesh*>>& list)
{
	for (int i = 0; i < node->GetNodeAttributeCount(); i++)
	{
		FbxNodeAttribute* attribute = node->GetNodeAttributeByIndex(i);
		int aaa = attribute->GetAttributeType();

		// Attributeがメッシュなら追加
		if (attribute->GetAttributeType() == FbxNodeAttribute::EType::eMesh)
		{
			pair<string, FbxMesh*> pre;
			pre.first = node->GetName();
			pre.second = (FbxMesh*)attribute;
			int polygon_vertex_count = pre.second->GetPolygonVertexCount();
			list.push_back(pre);
		}
	}

	for (int i = 0; i < node->GetChildCount(); i++)
	{
		CollectMeshNode(node->GetChild(i), list);
	}
}

void ModelDataManager::LoadObjMaterial(const string& materialFileName, ModelData& modelData, Object3DDeliveryData& objectBuffer)
{
	//マテリアルを一旦初期化する
	objectBuffer.constBufferDataB1.alpha = 1;
	objectBuffer.constBufferDataB1.ambient = modelData.material.ambient;
	objectBuffer.constBufferDataB1.diffuse = modelData.material.diffuse;
	objectBuffer.constBufferDataB1.specular = modelData.material.specular;

	//ファイルストリーム
	ifstream file;
	//マテリアルファイルを開く
	file.open(materialFileName);
	//ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}
	//一行ずつ読み込む
	string line;
	while (getline(file, line)) {
		//位置行分の文字列をストリームに変換
		istringstream lineStream(line);
		//半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(lineStream, key, ' ');
		//先頭のタブ文字は無視する
		if (key[0] == '\t') {
			key.erase(key.begin());	//先頭の文字を削除
		}
		//先頭文字列がnewmtlならマテリアル名
		if (key == "newmtl") {
			//マテリアル名の読み込み
			lineStream >> modelData.material.name;
		}
		//先頭文字列がKaならアンビエント色
		if (key == "Ka") {
			lineStream >> modelData.material.ambient.x;
			lineStream >> modelData.material.ambient.y;
			lineStream >> modelData.material.ambient.z;
			objectBuffer.constBufferDataB1.ambient.x = modelData.material.ambient.x;
			objectBuffer.constBufferDataB1.ambient.y = modelData.material.ambient.y;
			objectBuffer.constBufferDataB1.ambient.z = modelData.material.ambient.z;
		}
		//先頭文字列がKdならディフューズ色
		if (key == "Kd") {
			lineStream >> modelData.material.diffuse.x;
			lineStream >> modelData.material.diffuse.y;
			lineStream >> modelData.material.diffuse.z;
			objectBuffer.constBufferDataB1.diffuse.x = modelData.material.diffuse.x;
			objectBuffer.constBufferDataB1.diffuse.y = modelData.material.diffuse.y;
			objectBuffer.constBufferDataB1.diffuse.z = modelData.material.diffuse.z;
		}
		//先頭文字がKsならスペキュラー色
		if (key == "Ks") {
			lineStream >> modelData.material.specular.x;
			lineStream >> modelData.material.specular.y;
			lineStream >> modelData.material.specular.z;
			objectBuffer.constBufferDataB1.specular.x = modelData.material.specular.x;
			objectBuffer.constBufferDataB1.specular.y = modelData.material.specular.y;
			objectBuffer.constBufferDataB1.specular.z = modelData.material.specular.z;
		}
	}
}

void ModelDataManager::CalculateSmoothedVertexNormals(map<unsigned short, vector<unsigned short>>& smoothData, Object3DDeliveryData& objectBuffer, ModelData& modelData)
{
	auto itr = smoothData.begin();
	for (; itr != smoothData.end(); ++itr) {
		// 各面用の共通頂点コレクション
		std::vector<unsigned short>& v = itr->second;
		// 全頂点の法線を平均する
		XMVECTOR normal = {};
		for (unsigned short index : v) {
			normal += XMVectorSet(objectBuffer.vertex[index].normal.x, objectBuffer.vertex[index].normal.y, objectBuffer.vertex[index].normal.z, 0);
		}
		normal = XMVector3Normalize(normal / (float)v.size());

		for (unsigned short index : v) {
			objectBuffer.vertex[index].normal = { normal.m128_f32[0], normal.m128_f32[1], normal.m128_f32[2] };
			modelData.vertex[index].normal = { normal.m128_f32[0], normal.m128_f32[1], normal.m128_f32[2] };
		}
	}
}
