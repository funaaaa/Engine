#pragma once
#include "BaseOperationObject.h"
#include <memory>

class FirstAIWayPointMgr;

class FirstAIOperationObject : public BaseOperationObject {

private:

	/*===== �����o�ϐ� =====*/

	std::shared_ptr<FirstAIWayPointMgr> waypointMgr_;
	int level_;


public:

	/*===== �����o�֐� =====*/

	FirstAIOperationObject(const int& WayPointOffset, const int& Level);

	BaseOperationObject::Operation Input(const BaseOperationObject::OperationInputData& InputData)override;

	void OnGoal()override;


};