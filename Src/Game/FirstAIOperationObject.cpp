#include "FirstAIOperationObject.h"
#include "FirstAIWaypointMgr.h"
#include "FirstAIWayPoint.h"

FirstAIOperationObject::FirstAIOperationObject(const int& WayPointOffset)
{

	/*===== コンストラクタ =====*/

	waypointMgr_ = std::make_shared<FirstAIWayPointMgr>();
	waypointMgr_->Setting(static_cast<FirstAIWayPointMgr::WAYPOINT_OFFSET>(WayPointOffset));

}

BaseOperationObject::Operation FirstAIOperationObject::Input(const BaseOperationObject::OperationInputData& InputData)
{

	/*===== キャラ操作 =====*/

	BaseOperationObject::Operation operation;

	operation.accelerationRate_ = 1.0f;

	operation.handleDriveRate_ = waypointMgr_->LeftRightCheck(InputData.pos_, InputData.forwradVec_);

	if (0.3f < fabs(operation.handleDriveRate_)) {

		operation.isDrift_ = true;

	}

	return operation;

}

void FirstAIOperationObject::OnGoal()
{

	/*===== ゴール時の処理 =====*/

	waypointMgr_->Reset();

}
