#include "ItemBoxObject.h"
#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"

void ItemBoxObject::Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*===== �Z�b�e�B���O���� =====*/

	BasicInit(ObjectID, CollisionID, Instance);

}

void ItemBoxObject::Destroy()
{

	/*===== �C���X�^���X�j�� =====*/

	PolygonInstanceRegister::Ins()->DestroyInstance(instance_);
	isActive_ = false;

}

void ItemBoxObject::Update(const int& Timer)
{

	/*===== �X�V���� =====*/

	Timer;
	if (!isActive_) {

		NonDisplay();

		++activationTimer_;
		if (timerToActivation_ < activationTimer_) {

			isActive_ = true;

			Display();

		}

	}

	// ��]������B
	instance_.lock()->AddRotate(Vec3(0.01f, 0.02f, 0.0f));


}

void ItemBoxObject::Disable(const int& TimerToActivation)
{

	/*===== ���������čēx�L���������܂ł̎��Ԃ��Z�b�g =====*/

	isActive_ = false;

	timerToActivation_ = TimerToActivation;
	activationTimer_ = 0;

}
