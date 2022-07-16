#pragma once
#include "BaseStageObject.h"

// 基本的なStageObjectクラス
class BasicStageObject : public BaseStageObject{

private:

	/*===== メンバ変数 =====*/


public:

	/*===== メンバ関数 =====*/

	void Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID)override;
	void Destroy()override;
	void Update(const int& Timer)override;

};