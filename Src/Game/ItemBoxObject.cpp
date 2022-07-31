#include "ItemBoxObject.h"
#include "PolygonInstanceRegister.h"

void ItemBoxObject::Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID)
{

	/*===== セッティング処理 =====*/

	BasicInit(ObjectID, CollisionID, InstanceID);

}

void ItemBoxObject::Destroy()
{

	/*===== インスタンス破棄 =====*/

	PolygonInstanceRegister::Ins()->DestroyInstance(insIndex_);
	isActive_ = false;

}

void ItemBoxObject::Update(const int& Timer)
{

	/*===== 更新処理 =====*/

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

	/*===== 無効化して再度有効化されるまでの時間をセット =====*/

	isActive_ = false;

	timerToActivation_ = TimerToActivation;
	activationTimer_ = 0;

}
