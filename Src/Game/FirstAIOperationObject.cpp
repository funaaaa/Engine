#include "FirstAIOperationObject.h"
#include "FirstAIWaypointMgr.h"
#include "FirstAIWayPoint.h"

FirstAIOperationObject::FirstAIOperationObject(const int& WayPointOffset, const int& Level)
{

	/*===== �R���X�g���N�^ =====*/

	waypointMgr_ = std::make_shared<FirstAIWayPointMgr>();
	waypointMgr_->Setting(static_cast<FirstAIWayPointMgr::WAYPOINT_OFFSET>(WayPointOffset));

	level_ = Level;

}

BaseOperationObject::Operation FirstAIOperationObject::Input(const BaseOperationObject::OperationInputData& InputData)
{

	/*===== �L�������� =====*/

	BaseOperationObject::Operation operation;

	// �ړ����x�����x���ɂ���ĕύX�B
	if (level_ == 0) {

		operation.accelerationRate_ = 0.7f;

	}
	else if (level_ == 1) {

		operation.accelerationRate_ = 1.0f;

	}
	else if (level_ == 2) {

		operation.accelerationRate_ = 1.5f;

	}

	operation.handleDriveRate_ = waypointMgr_->LeftRightCheck(InputData.pos_, InputData.forwradVec_);

	operation.isJumpActionTrigger_ = InputData.isHitJumpBoostGimmick_;

	operation.isUseItemTrigger_ = InputData.hasItemID_ == BaseItem::ItemID::BOOST;

	if (0.3f < fabs(operation.handleDriveRate_)) {

		operation.isDrift_ = true;

	}

	return operation;

}

void FirstAIOperationObject::OnGoal()
{

	/*===== �S�[�����̏��� =====*/

	waypointMgr_->Reset();

}
