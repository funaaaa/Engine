#pragma once
#include "BaseStageObject.h"

// 基本的なStageObjectクラス
class BasicStageObject : public BaseStageObject{

private:

	/*===== メンバ変数 =====*/


public:

	/*===== メンバ関数 =====*/

	void Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, std::weak_ptr<PolygonMeshInstance> Instance)override;
	void Destroy()override;
	void Update(int Timer)override;
	void Disable(int TimerToActivation)override;

};