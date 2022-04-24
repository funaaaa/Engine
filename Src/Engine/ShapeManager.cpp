#include "ShapeManager.h"

//vector<ShapeData> ShapeManager::shapeData = {};
//
//void ShapeManager::LoadShape(int shapeID, Sprite& proSprite)
//{
//	//図形が生成済みかどうかを確認する
//	bool isLoad = false;
//	int dataNum = 0;
//	for (int i = 0; i < shapeData.size(); ++i) {
//		if (shapeData.at(i).dataID == shapeID) {
//			isLoad = true;
//			dataNum = i;
//		}
//	}
//
//	//未生成だったら図形を生成して値をコピーする
//	if (isLoad == false) {
//		//生成する図形によって処理を分ける
//		switch (shapeID) {
//		case ShapeDataCone:
//			CreateCone(proSprite);
//			break;
//		case ShapeDataCylinder:
//			CreateCylinder(proSprite);
//			break;
//		case ShapeDataSphere:
//			CreateSphere(proSprite);
//			break;
//		}
//	}
//	//生成済みだったら情報をコピーする
//	else {
//		for (int i = 0; i < shapeData.at(dataNum).index.size(); ++i) {
//			proSprite.index.push_back(shapeData.at(dataNum).index.at(i));
//		}
//		for (int i = 0; i < shapeData.at(dataNum).vertex.size(); ++i) {
//			proSprite.vertex.push_back(shapeData.at(dataNum).vertex.at(i));
//		}
//	}
//}
//
//void ShapeManager::CreateCone(Sprite& proSprite)
//{
//	//shapedataを生成
//	shapeData.push_back({});
//	shapeData.at(shapeData.size() - 1).dataID = ShapeDataCone;
//	//頂点バッファの生成
//	int vertexCount = 30;			//頂点数
//	//底面の中心以外の座標を計算
//	for (int i = 0; i < vertexCount - 2; ++i) {
//		Vertex pos;
//		pos.pos = XMFLOAT3(1 * sinf(XM_2PI / (vertexCount - 2) * i),
//			1 * (cosf(XM_2PI / (vertexCount - 2) * i)),
//			0);
//		pos.uv = XMFLOAT2(0, 0);
//		//shapedetaへ保存
//		shapeData.at(shapeData.size() - 1).vertex.push_back(pos);
//		//prospriteへのコピーも行う
//		proSprite.vertex.push_back(pos);
//	}
//	Vertex circleCenterPos{};
//	circleCenterPos.pos = XMFLOAT3(0, 0, 0);
//	circleCenterPos.uv = XMFLOAT2(0, 0);
//	//shapedetaへ保存
//	shapeData.at(shapeData.size() - 1).vertex.push_back(circleCenterPos);
//	//prospriteへのコピーも行う
//	proSprite.vertex.push_back(circleCenterPos);
//	Vertex topHeightPos{};
//	topHeightPos.pos = XMFLOAT3(0, 0, -1);
//	topHeightPos.uv = XMFLOAT2(0, 0);
//	//shapedetaへ保存
//	shapeData.at(shapeData.size() - 1).vertex.push_back(topHeightPos);
//	//prospriteへのコピーも行う
//	proSprite.vertex.push_back(topHeightPos);
//
//	//底面のインデックスの設定
//	int i = 0;
//	for (; i < vertexCount - 3; ++i) {
//		//shapedetaへ保存
//		shapeData.at(shapeData.size() - 1).index.push_back(i);
//		shapeData.at(shapeData.size() - 1).index.push_back(vertexCount - 2);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + 1);
//		//prospriteへのコピーも行う
//		proSprite.index.push_back(i);
//		proSprite.index.push_back(vertexCount - 2);
//		proSprite.index.push_back(i + 1);
//	}
//	//shapedetaへ保存
//	shapeData.at(shapeData.size() - 1).index.push_back(i);
//	shapeData.at(shapeData.size() - 1).index.push_back(vertexCount - 2);
//	shapeData.at(shapeData.size() - 1).index.push_back(0);
//	//prospriteへのコピーも行う
//	proSprite.index.push_back(i);
//	proSprite.index.push_back(vertexCount - 2);
//	proSprite.index.push_back(0);
//	//側面のインデックスの設定
//	i = 0;
//	for (; i < vertexCount - 3; ++i) {
//		//shapedetaへ保存
//		shapeData.at(shapeData.size() - 1).index.push_back(i);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + 1);
//		shapeData.at(shapeData.size() - 1).index.push_back(vertexCount - 1);
//		//prospriteへのコピーも行う
//		proSprite.index.push_back(i);
//		proSprite.index.push_back(i + 1);
//		proSprite.index.push_back(vertexCount - 1);
//	}
//	//shapedetaへ保存
//	shapeData.at(shapeData.size() - 1).index.push_back(i);
//	shapeData.at(shapeData.size() - 1).index.push_back(0);
//	shapeData.at(shapeData.size() - 1).index.push_back(vertexCount - 1);
//	//prospriteへのコピーも行う
//	proSprite.index.push_back(i);
//	proSprite.index.push_back(0);
//	proSprite.index.push_back(vertexCount - 1);
//
//	//法線ベクトルの計算
//	for (int i = 0; i < proSprite.index.size() / 3; ++i) {
//		//三角形ひとつひとつ計算していく
//		//三角形のインデックスを取り出して、一時的な変数に入れる
//		unsigned short index0 = proSprite.index.at(i * 3 + 0);
//		unsigned short index1 = proSprite.index.at(i * 3 + 1);
//		unsigned short index2 = proSprite.index.at(i * 3 + 2);
//		//三角形を構成する頂点座標をベクトルに代入
//		XMVECTOR p0 = XMLoadFloat3(&proSprite.vertex.at(index0).pos);
//		XMVECTOR p1 = XMLoadFloat3(&proSprite.vertex.at(index1).pos);
//		XMVECTOR p2 = XMLoadFloat3(&proSprite.vertex.at(index2).pos);
//		//p0->p1ベクトル,p0->p2ベクトルを計算
//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
//		//外戚は両方から垂直なベクトル
//		XMVECTOR normal = XMVector3Cross(v1, v2);
//		//正規化
//		normal = XMVector3Normalize(normal);
//		//求めた法線を頂点データに代入
//		//shapedetaへ保存
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index0).normal, normal);
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index1).normal, normal);
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index2).normal, normal);
//		//prospriteへのコピーも行う
//		XMStoreFloat3(&proSprite.vertex.at(index0).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index1).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index2).normal, normal);
//	}
//}
//
//void ShapeManager::CreateCylinder(Sprite& proSprite)
//{
//	//shapedataを生成
//	shapeData.push_back({});
//	shapeData.at(shapeData.size() - 1).dataID = ShapeDataCylinder;
//	//頂点バッファの生成
//	int vertexCount = 30;			//頂点数
//	//底面の座標を計算
//	for (int i = 0; i < vertexCount / 2 - 1; ++i) {
//		Vertex pos;
//		pos.pos = XMFLOAT3(1 * sinf(XM_2PI / (vertexCount / 2 - 1) * i),
//			1 * (cosf(XM_2PI / (vertexCount / 2 - 1) * i)),
//			0);
//		pos.uv = XMFLOAT2(0, 0);
//		//shapedetaへ保存
//		shapeData.at(shapeData.size() - 1).vertex.push_back(pos);
//		//proSpriteへのデータコピーも行う
//		proSprite.vertex.push_back(pos);
//	}
//	Vertex circleCenterPos{};
//	circleCenterPos.pos = XMFLOAT3(0, 0, 0);
//	circleCenterPos.uv = XMFLOAT2(0, 0);
//	//shapedetaへ保存
//	shapeData.at(shapeData.size() - 1).vertex.push_back(circleCenterPos);
//	//proSpriteへのデータコピーも行う
//	proSprite.vertex.push_back(circleCenterPos);
//	//天面の座標を計算
//	for (int i = 0; i < vertexCount / 2 - 1; ++i) {
//		Vertex pos;
//		pos.pos = XMFLOAT3(1 * sinf(XM_2PI / (vertexCount / 2 - 1) * i),
//			1 * (cosf(XM_2PI / (vertexCount / 2 - 1) * i)),
//			-1);
//		pos.uv = XMFLOAT2(0, 0);
//		//shapedetaへ保存
//		shapeData.at(shapeData.size() - 1).vertex.push_back(pos);
//		//proSpriteへのデータコピーも行う
//		proSprite.vertex.push_back(pos);
//	}
//	Vertex topHeightPos{};
//	topHeightPos.pos = XMFLOAT3(0, 0, -1);
//	topHeightPos.uv = XMFLOAT2(0, 0);
//	//shapedetaへ保存
//	shapeData.at(shapeData.size() - 1).vertex.push_back(topHeightPos);
//	//proSpriteへのデータコピーも行う
//	proSprite.vertex.push_back(topHeightPos);
//
//	//底面のインデックスの設定
//	int i = 0;
//	for (; i < vertexCount / 2 - 2; ++i) {
//		//shapedetaへ保存
//		shapeData.at(shapeData.size() - 1).index.push_back(i);
//		shapeData.at(shapeData.size() - 1).index.push_back(vertexCount / 2 - 1);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + 1);
//		//proSpriteへのデータコピーも行う
//		proSprite.index.push_back(i);
//		proSprite.index.push_back(vertexCount / 2 - 1);
//		proSprite.index.push_back(i + 1);
//	}
//	//shapedetaへ保存
//	shapeData.at(shapeData.size() - 1).index.push_back(i);
//	shapeData.at(shapeData.size() - 1).index.push_back(vertexCount / 2 - 1);
//	shapeData.at(shapeData.size() - 1).index.push_back(0);
//	//proSpriteへのデータコピーも行う
//	proSprite.index.push_back(i);
//	proSprite.index.push_back(vertexCount / 2 - 1);
//	proSprite.index.push_back(0);
//	//天面のインデックスの設定
//	i = 0;
//	for (; i < vertexCount / 2 - 2; ++i) {
//		//shapedetaへ保存
//		shapeData.at(shapeData.size() - 1).index.push_back(i + vertexCount / 2);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + 1 + vertexCount / 2);
//		shapeData.at(shapeData.size() - 1).index.push_back(vertexCount - 1);
//		//proSpriteへのデータコピーも行う
//		proSprite.index.push_back(i + vertexCount / 2);
//		proSprite.index.push_back(i + 1 + vertexCount / 2);
//		proSprite.index.push_back(vertexCount - 1);
//	}
//	//shapedetaへ保存
//	shapeData.at(shapeData.size() - 1).index.push_back(i + vertexCount / 2);
//	shapeData.at(shapeData.size() - 1).index.push_back(vertexCount / 2);
//	shapeData.at(shapeData.size() - 1).index.push_back(vertexCount - 1);
//	//proSpriteへのデータコピーも行う
//	proSprite.index.push_back(i + vertexCount / 2);
//	proSprite.index.push_back(vertexCount / 2);
//	proSprite.index.push_back(vertexCount - 1);
//	//側面のインデックスの設定
//	i = 0;
//	for (; i < vertexCount / 2 - 2; ++i) {
//		//shapedetaへ保存
//		shapeData.at(shapeData.size() - 1).index.push_back(i);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + 1);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + vertexCount / 2);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + 1);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + 1 + vertexCount / 2);
//		shapeData.at(shapeData.size() - 1).index.push_back(i + vertexCount / 2);
//		//proSpriteへのデータコピーも行う
//		proSprite.index.push_back(i);
//		proSprite.index.push_back(i + 1);
//		proSprite.index.push_back(i + vertexCount / 2);
//		proSprite.index.push_back(i + 1);
//		proSprite.index.push_back(i + 1 + vertexCount / 2);
//		proSprite.index.push_back(i + vertexCount / 2);
//	}
//	//shapedetaへ保存
//	shapeData.at(shapeData.size() - 1).index.push_back(i);
//	shapeData.at(shapeData.size() - 1).index.push_back(0);
//	shapeData.at(shapeData.size() - 1).index.push_back(i + vertexCount / 2);
//	shapeData.at(shapeData.size() - 1).index.push_back(0);
//	shapeData.at(shapeData.size() - 1).index.push_back(vertexCount / 2);
//	shapeData.at(shapeData.size() - 1).index.push_back(i + vertexCount / 2);
//	//proSpriteへのデータコピーも行う
//	proSprite.index.push_back(i);
//	proSprite.index.push_back(0);
//	proSprite.index.push_back(i + vertexCount / 2);
//	proSprite.index.push_back(0);
//	proSprite.index.push_back(vertexCount / 2);
//	proSprite.index.push_back(i + vertexCount / 2);
//
//	//法線ベクトルの計算
//	for (int i = 0; i < proSprite.index.size() / 3; ++i) {
//		//三角形ひとつひとつ計算していく
//		//三角形のインデックスを取り出して、一時的な変数に入れる
//		unsigned short index0 = proSprite.index.at(i * 3 + 0);
//		unsigned short index1 = proSprite.index.at(i * 3 + 1);
//		unsigned short index2 = proSprite.index.at(i * 3 + 2);
//		//三角形を構成する頂点座標をベクトルに代入
//		XMVECTOR p0 = XMLoadFloat3(&proSprite.vertex.at(index0).pos);
//		XMVECTOR p1 = XMLoadFloat3(&proSprite.vertex.at(index1).pos);
//		XMVECTOR p2 = XMLoadFloat3(&proSprite.vertex.at(index2).pos);
//		//p0->p1ベクトル,p0->p2ベクトルを計算
//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
//		//外戚は両方から垂直なベクトル
//		XMVECTOR normal = XMVector3Cross(v1, v2);
//		//正規化
//		normal = XMVector3Normalize(normal);
//		//求めた法線を頂点データに代入
//		//shapedetaへ保存
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index0).normal, normal);
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index1).normal, normal);
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index2).normal, normal);
//		//proSpriteへのデータコピーも行う
//		XMStoreFloat3(&proSprite.vertex.at(index0).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index1).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index2).normal, normal);
//	}
//}
//
//void ShapeManager::CreateSphere(Sprite& proSprite)
//{
//	//shapedataを生成
//	shapeData.push_back({});
//	shapeData.at(shapeData.size() - 1).dataID = ShapeDataSphere;
//	//頂点バッファの生成
//	int vertexCount = 20;			//頂点数
//	Vertex pos;
//	//底面の座標
//	pos.pos = XMFLOAT3(0, 0, -1);
//	pos.uv = XMFLOAT2(0, 0);
//	//shapeDataに保存
//	shapeData.at(shapeData.size() - 1).vertex.push_back(pos);
//	//proSpriteにもデータをコピー
//	proSprite.vertex.push_back(pos);
//	//側面の座標の計算
//	for (int i = 1; i < vertexCount; ++i) {
//		for (int j = 0; j < vertexCount; ++j) {
//			float circleRadius = (1 * sinf(XM_PI / vertexCount * i));
//			pos.pos = XMFLOAT3(circleRadius * sinf(XM_2PI / vertexCount * j),
//				circleRadius * (cosf(XM_2PI / vertexCount * j)),
//				-1 * (cosf((XM_PI / vertexCount * i)))
//			);
//			pos.uv = XMFLOAT2(0, 0);
//			//shapeDataに保存
//			shapeData.at(shapeData.size() - 1).vertex.push_back(pos);
//			//proSpriteにもデータをコピー
//			proSprite.vertex.push_back(pos);
//		}
//	}
//	//天面の座標
//	pos.pos = XMFLOAT3(0, 0, 1);
//	pos.uv = XMFLOAT2(0, 0);
//	//shapeDataに保存
//	shapeData.at(shapeData.size() - 1).vertex.push_back(pos);
//	//proSpriteにもデータをコピー
//	proSprite.vertex.push_back(pos);
//
//	//底面のインデックスの設定
//	int buff = 1;
//	for (int i = 0; i < vertexCount - 1; ++i) {
//		//shapeDataに保存
//		shapeData.at(shapeData.size() - 1).index.push_back(buff);
//		shapeData.at(shapeData.size() - 1).index.push_back(buff + 1);
//		shapeData.at(shapeData.size() - 1).index.push_back(0);
//		//proSpriteにもデータをコピー
//		proSprite.index.push_back(buff);
//		proSprite.index.push_back(buff + 1);
//		proSprite.index.push_back(0);
//		buff++;
//	}
//	//shapeDataに保存
//	shapeData.at(shapeData.size() - 1).index.push_back(buff);
//	shapeData.at(shapeData.size() - 1).index.push_back(1);
//	shapeData.at(shapeData.size() - 1).index.push_back(0);
//	//proSpriteにもデータをコピー
//	proSprite.index.push_back(buff);
//	proSprite.index.push_back(1);
//	proSprite.index.push_back(0);
//	buff++;
//	//側面のインデックスの設定
//	for (int i = 0; i < vertexCount - 2; ++i) {
//		for (int j = 0; j < vertexCount - 1; ++j) {
//			//shapeDataに保存
//			shapeData.at(shapeData.size() - 1).index.push_back(buff);
//			shapeData.at(shapeData.size() - 1).index.push_back(buff + 1);
//			shapeData.at(shapeData.size() - 1).index.push_back(buff - (vertexCount - 1));
//			shapeData.at(shapeData.size() - 1).index.push_back(buff);
//			shapeData.at(shapeData.size() - 1).index.push_back(buff - (vertexCount - 1));
//			shapeData.at(shapeData.size() - 1).index.push_back(buff - vertexCount);
//			//proSpriteにもデータをコピー
//			proSprite.index.push_back(buff);
//			proSprite.index.push_back(buff + 1);
//			proSprite.index.push_back(buff - (vertexCount - 1));
//			proSprite.index.push_back(buff);
//			proSprite.index.push_back(buff - (vertexCount - 1));
//			proSprite.index.push_back(buff - vertexCount);
//			buff++;
//		}
//		//shapeDataに保存
//		shapeData.at(shapeData.size() - 1).index.push_back(buff);
//		shapeData.at(shapeData.size() - 1).index.push_back(buff - (vertexCount - 1));
//		shapeData.at(shapeData.size() - 1).index.push_back(buff - (vertexCount - 1) - vertexCount);
//		shapeData.at(shapeData.size() - 1).index.push_back(buff);
//		shapeData.at(shapeData.size() - 1).index.push_back(buff - (vertexCount - 1) - vertexCount);
//		shapeData.at(shapeData.size() - 1).index.push_back(buff - vertexCount);
//		//proSpriteにもデータをコピー
//		proSprite.index.push_back(buff);
//		proSprite.index.push_back(buff - (vertexCount - 1));
//		proSprite.index.push_back(buff - (vertexCount - 1) - vertexCount);
//		proSprite.index.push_back(buff);
//		proSprite.index.push_back(buff - (vertexCount - 1) - vertexCount);
//		proSprite.index.push_back(buff - vertexCount);
//		buff++;
//	}
//	//天面のインデックスの設定
//	for (int i = vertexCount - 1; i > 0; --i) {
//		//shapeDataに保存
//		shapeData.at(shapeData.size() - 1).index.push_back(proSprite.vertex.size() - 1 - i);
//		shapeData.at(shapeData.size() - 1).index.push_back(proSprite.vertex.size() - 1 - i - 1);
//		shapeData.at(shapeData.size() - 1).index.push_back(proSprite.vertex.size() - 1);
//		//proSpriteにもデータをコピー
//		proSprite.index.push_back(proSprite.vertex.size() - 1 - i);
//		proSprite.index.push_back(proSprite.vertex.size() - 1 - i - 1);
//		proSprite.index.push_back(proSprite.vertex.size() - 1);
//	}
//	//shapeDataに保存
//	shapeData.at(shapeData.size() - 1).index.push_back(proSprite.vertex.size() - 1 - 1);
//	shapeData.at(shapeData.size() - 1).index.push_back(proSprite.vertex.size() - 1);
//	shapeData.at(shapeData.size() - 1).index.push_back(proSprite.vertex.size() - 1 - vertexCount);
//	//proSpriteにもデータをコピー
//	proSprite.index.push_back(proSprite.vertex.size() - 1 - 1);
//	proSprite.index.push_back(proSprite.vertex.size() - 1);
//	proSprite.index.push_back(proSprite.vertex.size() - 1 - vertexCount);
//
//	//法線ベクトルの計算
//	for (int i = 0; i < proSprite.index.size() / 3; ++i) {
//		//三角形ひとつひとつ計算していく
//		//三角形のインデックスを取り出して、一時的な変数に入れる
//		unsigned short index0 = proSprite.index.at(i * 3 + 0);
//		unsigned short index1 = proSprite.index.at(i * 3 + 1);
//		unsigned short index2 = proSprite.index.at(i * 3 + 2);
//		//三角形を構成する頂点座標をベクトルに代入
//		XMVECTOR p0 = XMLoadFloat3(&proSprite.vertex.at(index0).pos);
//		XMVECTOR p1 = XMLoadFloat3(&proSprite.vertex.at(index1).pos);
//		XMVECTOR p2 = XMLoadFloat3(&proSprite.vertex.at(index2).pos);
//		//p0->p1ベクトル,p0->p2ベクトルを計算
//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
//		//外戚は両方から垂直なベクトル
//		XMVECTOR normal = XMVector3Cross(v1, v2);
//		//正規化
//		normal = XMVector3Normalize(normal);
//		//求めた法線を頂点データに代入
//		//shapeDataに保存
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index0).normal, normal);
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index1).normal, normal);
//		XMStoreFloat3(&shapeData.at(shapeData.size() - 1).vertex.at(index2).normal, normal);
//		//proSpriteにもデータをコピー
//		XMStoreFloat3(&proSprite.vertex.at(index0).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index1).normal, normal);
//		XMStoreFloat3(&proSprite.vertex.at(index2).normal, normal);
//	}
//}
