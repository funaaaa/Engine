#include "BasicStageObject.h"
#include "PolygonInstanceRegister.h"
#include "OBB.h"

void BasicStageObject::Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID)
{

	/*===== �Z�b�e�B���O���� =====*/

	BasicInit(ObjectID, CollisionID, InstanceID);

}

void BasicStageObject::Destroy()
{

	/*===== �C���X�^���X�j�� =====*/

	PolygonInstanceRegister::Ins()->DestroyInstance(insIndex_);
	isActive_ = false;

}

void BasicStageObject::Update(const int& Timer)
{

	/*===== �X�V���� =====*/

}
