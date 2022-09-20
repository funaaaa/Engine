#include "BasicStageObject.h"
#include "PolygonInstanceRegister.h"
#include "OBB.h"

void BasicStageObject::Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*===== �Z�b�e�B���O���� =====*/

	BasicInit(ObjectID, CollisionID, Instance);

}

void BasicStageObject::Destroy()
{

	/*===== �C���X�^���X�j�� =====*/

	PolygonInstanceRegister::Ins()->DestroyInstance(instance_);
	isActive_ = false;

}

void BasicStageObject::Update(const int& Timer)
{

	/*===== �X�V���� =====*/

	Timer;

}

void BasicStageObject::Disable(const int& TimerToActivation)
{

	/*===== ���������čĐݒ�܂ł̎��Ԃ��Z�b�g =====*/

	TimerToActivation;

}
