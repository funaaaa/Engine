#pragma once
#include "BaseOperationObject.h"
#include <memory>

class FirstAIWayPointMgr;

class FirstAIOperationObject : public BaseOperationObject {

private:

	/*===== ƒƒ“ƒo•Ï” =====*/

	std::shared_ptr<FirstAIWayPointMgr> waypointMgr_;
	int level_;


public:

	/*===== ƒƒ“ƒoŠÖ” =====*/

	FirstAIOperationObject(const int& WayPointOffset, const int& Level);

	BaseOperationObject::Operation Input(const BaseOperationObject::OperationInputData& InputData)override;

	void OnGoal()override;


};