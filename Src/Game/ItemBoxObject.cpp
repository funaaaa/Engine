#include "ItemBoxObject.h"
#include "PolygonInstanceRegister.h"

void ItemBoxObject::Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID)
{

	/*===== �Z�b�e�B���O���� =====*/

	BasicInit(ObjectID, CollisionID, InstanceID);

}

void ItemBoxObject::Destroy()
{

	/*===== �C���X�^���X�j�� =====*/

	PolygonInstanceRegister::Ins()->DestroyInstance(insIndex_);
	isActive_ = false;

}

void ItemBoxObject::Update(const int& Timer)
{

	/*===== �X�V���� =====*/

	if (!isActive_) {

		NonDisplay();

		++activationTimer_;
		if (timerToActivation_ < activationTimer_) {

			isActive_ = true;

			Display();

		}

	}


}

void ItemBoxObject::Disable(const int& TimerToActivation)
{

	/*===== ���������čēx�L���������܂ł̎��Ԃ��Z�b�g =====*/

	isActive_ = false;

	timerToActivation_ = TimerToActivation;
	activationTimer_ = 0;

}
