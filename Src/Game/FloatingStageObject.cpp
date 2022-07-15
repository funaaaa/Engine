#include "FloatingStageObject.h"
#include "PolygonInstanceRegister.h"
#include "OBB.h"

void FloatingStageObject::Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID)
{

	/*===== セッティング処理 =====*/

	BasicInit(ObjectID, CollisionID, InstanceID);

	// 移動量を初期化。
	timerOffset_ = InstanceID;	// InstanceIDをタイマーのオフセットに使用する。これによって連続で生成したFloatingStageObjectは波のように動く。
	floatingMove_ = Vec3();

}

void FloatingStageObject::Destroy()
{

	/*===== インスタンス破棄 =====*/

	PolygonInstanceRegister::Ins()->DestroyInstance(insIndex_);
	isActive_ = false;

}

void FloatingStageObject::Update(const int& Timer)
{

	/*===== 更新処理 =====*/

	// 前フレームの移動をかき消す。
	AddTrans(-floatingMove_);

	// 新たに移動量を求める。 Timerがint型なので10.0fで割って少数にする。
	floatingMove_.y_ = sinf(static_cast<float>(Timer) / 10.0f + timerOffset_);

	// 移動させる。
	AddTrans(floatingMove_);

}
