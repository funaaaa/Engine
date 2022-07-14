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
	ModelDataManager::Vertex vertex_;
	vertex_.pos_ = Vec3(Obb.length_.x_, Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 0);
	objectData.vertex_.emplace_back(vertex_);
	// 前面右下
	vertex_.pos_ = Vec3(Obb.length_.x_, -Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 1);
	objectData.vertex_.emplace_back(vertex_);
	// 前面左下
	vertex_.pos_ = Vec3(-Obb.length_.x_, -Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 1);
	objectData.vertex_.emplace_back(vertex_);
	// 前面左上
	vertex_.pos_ = Vec3(-Obb.length_.x_, Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 0);
	objectData.vertex_.emplace_back(vertex_);

	// 背面右上
	vertex_.pos_ = Vec3(Obb.length_.x_, Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 0);
	objectData.vertex_.emplace_back(vertex_);
	// 背面右下
	vertex_.pos_ = Vec3(Obb.length_.x_, -Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 1);
	objectData.vertex_.emplace_back(vertex_);
	// 背面左下
	vertex_.pos_ = Vec3(-Obb.length_.x_, -Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 1);
	objectData.vertex_.emplace_back(vertex_);
	// 背面左上
	vertex_.pos_ = Vec3(-Obb.length_.x_, Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 0);
	objectData.vertex_.emplace_back(vertex_);

	// 右面右上
	vertex_.pos_ = Vec3(Obb.length_.x_, Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 0);
	objectData.vertex_.emplace_back(vertex_);
	// 右面右下
	vertex_.pos_ = Vec3(Obb.length_.x_, -Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 1);
	objectData.vertex_.emplace_back(vertex_);
	// 右面左下
	vertex_.pos_ = Vec3(Obb.length_.x_, -Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 1);
	objectData.vertex_.emplace_back(vertex_);
	// 右面左上
	vertex_.pos_ = Vec3(Obb.length_.x_, Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 0);
	objectData.vertex_.emplace_back(vertex_);

	// 左面右上
	vertex_.pos_ = Vec3(-Obb.length_.x_, Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 0);
	objectData.vertex_.emplace_back(vertex_);
	// 左面右下
	vertex_.pos_ = Vec3(-Obb.length_.x_, -Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 1);
	objectData.vertex_.emplace_back(vertex_);
	// 左面左下
	vertex_.pos_ = Vec3(-Obb.length_.x_, -Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 1);
	objectData.vertex_.emplace_back(vertex_);
	// 左面左上
	vertex_.pos_ = Vec3(-Obb.length_.x_, Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 0);
	objectData.vertex_.emplace_back(vertex_);

	// 上面右上
	vertex_.pos_ = Vec3(Obb.length_.x_, Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 0);
	objectData.vertex_.emplace_back(vertex_);
	// 上面右下
	vertex_.pos_ = Vec3(Obb.length_.x_, Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 1);
	objectData.vertex_.emplace_back(vertex_);
	// 上面左下
	vertex_.pos_ = Vec3(-Obb.length_.x_, Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 1);
	objectData.vertex_.emplace_back(vertex_);
	// 上面左上
	vertex_.pos_ = Vec3(-Obb.length_.x_, Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 0);
	objectData.vertex_.emplace_back(vertex_);

	// 下面右上
	vertex_.pos_ = Vec3(Obb.length_.x_, -Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 0);
	objectData.vertex_.emplace_back(vertex_);
	// 下面右下
	vertex_.pos_ = Vec3(Obb.length_.x_, -Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(1, 1);
	objectData.vertex_.emplace_back(vertex_);
	// 下面左下
	vertex_.pos_ = Vec3(-Obb.length_.x_, -Obb.length_.y_, Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 1);
	objectData.vertex_.emplace_back(vertex_);
	// 下面左上
	vertex_.pos_ = Vec3(-Obb.length_.x_, -Obb.length_.y_, -Obb.length_.z_);
	vertex_.uv_ = Vec2(0, 0);
	objectData.vertex_.emplace_back(vertex_);

	// インデックスを設定。
	objectData.index_ = {
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
