#pragma once
#include "BaseStageObject.h"

// ふわふわ動く飾り用オブジェクトクラス
class FloatingStageObject : public BaseStageObject{

private:

	/*===== メンバ変数 =====*/

	Vec3 floatingMove_;		// ふわふわ動いた量
	float timerOffset_;		// タイマーのオフセット ふわふわを波のように動かすために使用する。
	const Vec3 FLOATING_MOVE = 10.0f;	// ふわふわ動く量


public:

	/*===== メンバ関数 =====*/

	void Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID)override;
	void Destroy()override;
	void Update(const int& Timer)override;

};