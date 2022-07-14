#include "BoostGimmick.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"
#include "ModelDataManager.h"
#include "HitGroupMgr.h"
#include "TextureManager.h"

void BoostGimmick::Setting(const ID& GimmickID, const UINT& InstanceID)
{

	/*===== セッティング処理 =====*/

	// オブジェクトをロード
	insIndex_ = InstanceID;
	blasIndex_ = PolygonInstanceRegister::Ins()->GetBLASIndex(InstanceID);

	// 各種変数を初期化。
	gimmickID_ = GimmickID;
	isActive_ = true;

	// OBBをセット。
	collider_.Setting(blasIndex_, insIndex_);

}

void BoostGimmick::Update()
{

	/*===== 更新処理 =====*/

}

ModelDataManager::ObjectData BoostGimmick::GenerateVertex(OBB Obb)
{

	/*===== obbとInstance情報から頂点情報を生成 =====*/

	ModelDataManager::ObjectData objectData;

	// 頂点を生成。 前面右上
	ModelDataManager::Vertex vertex;
	vertex.pos_ = Vec3(Obb.length.x, Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(1, 0);
	objectData.vertex.emplace_back(vertex);
	// 前面右下
	vertex.pos_ = Vec3(Obb.length.x, -Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(1, 1);
	objectData.vertex.emplace_back(vertex);
	// 前面左下
	vertex.pos_ = Vec3(-Obb.length.x, -Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(0, 1);
	objectData.vertex.emplace_back(vertex);
	// 前面左上
	vertex.pos_ = Vec3(-Obb.length.x, Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(0, 0);
	objectData.vertex.emplace_back(vertex);

	// 背面右上
	vertex.pos_ = Vec3(Obb.length.x, Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(1, 0);
	objectData.vertex.emplace_back(vertex);
	// 背面右下
	vertex.pos_ = Vec3(Obb.length.x, -Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(1, 1);
	objectData.vertex.emplace_back(vertex);
	// 背面左下
	vertex.pos_ = Vec3(-Obb.length.x, -Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(0, 1);
	objectData.vertex.emplace_back(vertex);
	// 背面左上
	vertex.pos_ = Vec3(-Obb.length.x, Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(0, 0);
	objectData.vertex.emplace_back(vertex);

	// 右面右上
	vertex.pos_ = Vec3(Obb.length.x, Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(1, 0);
	objectData.vertex.emplace_back(vertex);
	// 右面右下
	vertex.pos_ = Vec3(Obb.length.x, -Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(1, 1);
	objectData.vertex.emplace_back(vertex);
	// 右面左下
	vertex.pos_ = Vec3(Obb.length.x, -Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(0, 1);
	objectData.vertex.emplace_back(vertex);
	// 右面左上
	vertex.pos_ = Vec3(Obb.length.x, Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(0, 0);
	objectData.vertex.emplace_back(vertex);

	// 左面右上
	vertex.pos_ = Vec3(-Obb.length.x, Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(1, 0);
	objectData.vertex.emplace_back(vertex);
	// 左面右下
	vertex.pos_ = Vec3(-Obb.length.x, -Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(1, 1);
	objectData.vertex.emplace_back(vertex);
	// 左面左下
	vertex.pos_ = Vec3(-Obb.length.x, -Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(0, 1);
	objectData.vertex.emplace_back(vertex);
	// 左面左上
	vertex.pos_ = Vec3(-Obb.length.x, Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(0, 0);
	objectData.vertex.emplace_back(vertex);

	// 上面右上
	vertex.pos_ = Vec3(Obb.length.x, Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(1, 0);
	objectData.vertex.emplace_back(vertex);
	// 上面右下
	vertex.pos_ = Vec3(Obb.length.x, Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(1, 1);
	objectData.vertex.emplace_back(vertex);
	// 上面左下
	vertex.pos_ = Vec3(-Obb.length.x, Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(0, 1);
	objectData.vertex.emplace_back(vertex);
	// 上面左上
	vertex.pos_ = Vec3(-Obb.length.x, Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(0, 0);
	objectData.vertex.emplace_back(vertex);

	// 下面右上
	vertex.pos_ = Vec3(Obb.length.x, -Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(1, 0);
	objectData.vertex.emplace_back(vertex);
	// 下面右下
	vertex.pos_ = Vec3(Obb.length.x, -Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(1, 1);
	objectData.vertex.emplace_back(vertex);
	// 下面左下
	vertex.pos_ = Vec3(-Obb.length.x, -Obb.length.y, Obb.length.z);
	vertex.uv = Vec2(0, 1);
	objectData.vertex.emplace_back(vertex);
	// 下面左上
	vertex.pos_ = Vec3(-Obb.length.x, -Obb.length.y, -Obb.length.z);
	vertex.uv = Vec2(0, 0);
	objectData.vertex.emplace_back(vertex);

	// インデックスを設定。
	objectData.index = {
		// 前面
		0,1,3,
		1,2,3,
		// 背面
		4,5,7,
		5,6,7,
		// 右面
		8,9,11,
		9,10,11,
		// 左面
		12,13,15,
		13,14,15,
		// 上面
		16,17,19,
		17,18,19,
		// 下面
		20,21,23,
		21,22,23,
	};

	return objectData;
}
