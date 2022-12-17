#pragma once
#include "BaseOperationObject.h"
#include <memory>

class FirstAIWayPointMgr;

// 最初に作ったAIの操作オブジェクト
class FirstAIOperationObject : public BaseOperationObject {

private:

	/*===== メンバ変数 =====*/

	std::shared_ptr<FirstAIWayPointMgr> waypointMgr_;
	int level_;
	int charaPersonality_;

	// レベルが上った際のオフセット移動量
	const float LEVEL_OFFSET_SPEED = 0.3f;


public:

	/*===== メンバ関数 =====*/

	FirstAIOperationObject(int WayPointOffset, int Level, int CharaPersonality);

	BaseOperationObject::Operation Input(const BaseOperationObject::OperationInputData& InputData)override;

	void OnGoal()override;


};