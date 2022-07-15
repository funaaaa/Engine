#include "StageObjectMgr.h"
#include "BasicStageObject.h"
#include "FloatingStageObject.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"

int StageObjectMgr::AddObject(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID)
{

	/*===== ギミックを追加 =====*/

	// ふわふわ動く装飾オブジェクトだったら。
	if (ObjectID == BaseStageObject::OBJECT_ID::FLOATING_ORNAMENT) {

		objects_.emplace_back(std::make_shared<FloatingStageObject>());

	}
	// それ以外の通常のオブジェクトだったら。
	else {

		objects_.emplace_back(std::make_shared<BasicStageObject>());

	}

	// Blasをロード
	int blasIndex = BLASRegister::Ins()->GenerateObj(DirectryPath, ModelName, HitGroupName, TexturePath);
	// instanceを生成。
	int instanceIndex = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex, ShaderID);

	// オブジェクトを設定。
	objects_.back()->Setting(ObjectID, CollisionID, instanceIndex);

	return static_cast<int>(objects_.size()) - 1;

}

void StageObjectMgr::Update(const int& Timer)
{

	/*===== 更新処理 =====*/

	for (auto& index : objects_) {

		index->Update(Timer);

	}

}

void StageObjectMgr::AddTrans(const int& Index, const Vec3& Trans)
{

	/*===== 移動を加算 =====*/

	objects_[Index]->AddTrans(Trans);

}

void StageObjectMgr::ChangeTrans(const int& Index, const Vec3& Trans)
{

	/*===== 移動を代入 =====*/

	objects_[Index]->ChangeTrans(Trans);

}

void StageObjectMgr::AddScale(const int& Index, const Vec3& Scale)
{

	/*===== スケールを加算 =====*/

	objects_[Index]->AddScale(Scale);

}

void StageObjectMgr::ChangeScale(const int& Index, const Vec3& Scale)
{

	/*===== スケールを代入 =====*/

	objects_[Index]->ChangeScale(Scale);

}

void StageObjectMgr::AddRotate(const int& Index, const Vec3& Rotate)
{

	/*===== 回転を加算 =====*/

	objects_[Index]->AddRotate(Rotate);

}

void StageObjectMgr::ChangeRotate(const int& Index, const Vec3& Rotate)
{

	/*===== 回転を代入 =====*/

	objects_[Index]->ChangeRotate(Rotate);

}
