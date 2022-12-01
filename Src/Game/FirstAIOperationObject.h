#pragma once
#include "BaseOperationObject.h"
#include <memory>

class FirstAIWayPointMgr;

class FirstAIOperationObject : public BaseOperationObject {

private:

	/*===== �����o�ϐ� =====*/

	std::shared_ptr<FirstAIWayPointMgr> waypointMgr_;
	int level_;
	int charaPersonality_;


public:

	/*===== �����o�֐� =====*/

	FirstAIOperationObject(int WayPointOffset, int Level, int CharaPersonality);

	BaseOperationObject::Operation Input(const BaseOperationObject::OperationInputData& InputData)override;

	void OnGoal()override;


};