#include "BasicStageObject.h"
#include "PolygonInstanceRegister.h"
#include "OBB.h"

void BasicStageObject::Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID)
{

	/*===== セッティング処理 =====*/

	BasicInit(ObjectID, CollisionID, InstanceID);

}

void BasicStageObject::Destroy()
{

	/*===== インスタンス破棄 =====*/

	PolygonInstanceRegister::Ins()->DestroyInstance(insIndex_);
	isActive_ = false;

}

void BasicStageObject::Update(const int& Timer)
{

	/*===== 更新処理 =====*/

}
