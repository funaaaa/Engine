#pragma once
#include "BaseStageObject.h"

// アイテムボックスクラス
class ItemBoxObject : public BaseStageObject{

private:

	/*===== メンバ変数 =====*/

	int timerToActivation_;	// 有効化までのタイマー
	int activationTimer_;	// 有効化までのタイマー計測用



public:

	/*===== メンバ関数 =====*/

	void Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID)override;
	void Destroy()override;
	void Update(const int& Timer)override;
	void Disable(const int& TimerToActivation)override;

};