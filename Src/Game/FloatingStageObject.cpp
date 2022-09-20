#include "FloatingStageObject.h"
#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "OBB.h"

void FloatingStageObject::Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*===== セッティング処理 =====*/

	BasicInit(ObjectID, CollisionID, Instance);

	// 移動量を初期化。
	timerOffset_ = static_cast<float>(Instance.lock()->GetInstanceIndex());	// InstanceIDをタイマーのオフセットに使用する。これによって連続で生成したFloatingStageObjectは波のように動く。
	floatingMove_ = Vec3();

}

void FloatingStageObject::Destroy()
{

	/*===== インスタンス破棄 =====*/

	PolygonInstanceRegister::Ins()->DestroyInstance(instance_);
	isActive_ = false;

}

void FloatingStageObject::Update(const int& Timer)
{

	/*===== 更新処理 =====*/

	// 前フレームの移動をかき消す。
	AddTrans(-floatingMove_);

	// 新たに移動量を求める。 Timerがint型なので20.0fで割って少数にする。
	floatingMove_.y_ = sinf(static_cast<float>(Timer) / 20.0f + timerOffset_) * FLOATING_MOVE;

	// 移動させる。
	AddTrans(floatingMove_);

	// 回転させる。
	AddRotate(Vec3(0.01f, 0.01f, 0));

}

void FloatingStageObject::Disable(const int& TimerToActivation)
{

	/*===== 無効化して再設定までのタイマーをセット =====*/

	TimerToActivation;

}
