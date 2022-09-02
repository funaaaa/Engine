#pragma once
#include <vector>
#include <memory>
#include "BaseStageObject.h"
#include "Singleton.h"
#include "BaseStage.h"
#include "FHelper.h"

class StageObjectMgr {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<BaseStageObject>> objects_;



public:

	/*===== メンバ変数 =====*/

	// 追加する。
	int AddObject(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID);

	// 更新処理
	void Update(const int& Timer);

	// 当たり判定
	BaseStage::ColliderOutput Collider(BaseStage::ColliderInput Input);

	// ギミック集を追加。
	std::vector<std::shared_ptr<BaseStageObject>> GetGimmickData() { return objects_; }

	// 移動関係
	void AddTrans(const int& Index, const Vec3& Trans);
	void ChangeTrans(const int& Index, const Vec3& Trans);

	// サイズ関係
	void AddScale(const int& Index, const Vec3& Scale);
	void ChangeScale(const int& Index, const Vec3& Scale);

	// 回転関係
	void AddRotate(const int& Index, const Vec3& Rotate);
	void ChangeRotate(const int& Index, const Vec3& Rotate);

	// Instanseを非表示、表示関数
	void NonDisplay(const int& Index);
	void Display(const int& Index);

	// ゲッタ
	int GetBlasIndex(const int& Index) { return objects_[Index]->GetBLASIndex(); }
	int GetInstanceIndex(const int& Index) { return objects_[Index]->GetInstanceIndex(); }


private:

	// ステージ、草との当たり判定
	BaseStage::ColliderOutput StageMeshCollider(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output, BaseStageObject::OBJECT_ID ObjectID);

	// 装飾オブジェクトとの当たり判定
	BaseStage::ColliderOutput OrnamentMeshCollider(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output);

	// 4方向に当たり判定
	BaseStage::ColliderOutput Decision4Way(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output, BaseStageObject::OBJECT_ID ObjectID);

	// 斜め床の回転
	void RotObliqueFloor(BaseStage::ColliderInput Input, const Vec3& HitNormal, BaseStage::ColliderOutput& Output);

};