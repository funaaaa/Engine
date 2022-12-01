#pragma once
#include "BaseOperationObject.h"
#include <memory>

class FirstAIWayPointMgr;

class FirstAIOperationObject : public BaseOperationObject {

private:

	/*===== ƒƒ“ƒo•Ï” =====*/

	std::shared_ptr<FirstAIWayPointMgr> waypointMgr_;
	int level_;
	int charaPersonality_;


public:

	/*===== ƒƒ“ƒoŠÖ” =====*/

	FirstAIOperationObject(int WayPointOffset, int Level, int CharaPersonality);

	BaseOperationObject::Operation Input(const BaseOperationObject::OperationInputData& InputData)override;

	void OnGoal()override;


};