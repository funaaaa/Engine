#include "ItemBoxObject.h"
#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"

void ItemBoxObject::Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*===== セッティング処理 =====*/

	BasicInit(ObjectID, CollisionID, Instance);

}

void ItemBoxObject::Destroy()
{

	/*===== インスタンス破棄 =====*/

	PolygonInstanceRegister::Ins()->DestroyInstance(instance_);
	isActive_ = false;

}

void ItemBoxObject::Update(const int& Timer)
{

	/*===== 更新処理 =====*/

	Timer;
	if (!isActive_) {

		NonDisplay();

		++activationTimer_;
		if (timerToActivation_ < activationTimer_) {

			isActive_ = true;

			Display();

		}

	}

	// 回転させる。
	instance_.lock()->AddRotate(Vec3(0.01f, 0.02f, 0.0f));


}

void ItemBoxObject::Disable(const int& TimerToActivation)
{

	/*===== 無効化して再度有効化されるまでの時間をセット =====*/

	isActive_ = false;

	timerToActivation_ = TimerToActivation;
	activationTimer_ = 0;

}
