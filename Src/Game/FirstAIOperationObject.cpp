#include "FirstAIOperationObject.h"
#include "FirstAIWaypointMgr.h"
#include "FirstAIWayPoint.h"
#include "FHelper.h"

FirstAIOperationObject::FirstAIOperationObject(int WayPointOffset, int Level, int CharaPersonality)
{

	/*===== �R���X�g���N�^ =====*/

	waypointMgr_ = std::make_shared<FirstAIWayPointMgr>();
	waypointMgr_->Setting(static_cast<FirstAIWayPointMgr::WAYPOINT_OFFSET>(WayPointOffset));

	level_ = Level;
	charaPersonality_ = CharaPersonality;

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

	// ���ɂ���đ��x��ς���B
	operation.accelerationRate_ += charaPersonality_ * 0.3f;

	operation.handleDriveRate_ = waypointMgr_->LeftRightCheck(InputData.pos_, InputData.forwradVec_);

	// ���ɂ���ăW�����v�A�N�V�������s�����ǂ����̃t���O��؂�ւ���B
	bool isJumpAction = InputData.isHitJumpBoostGimmick_;
	if (isJumpAction) {
		if (charaPersonality_ == 0) {
			isJumpAction = FHelper::GetRand(0, 3) == 0;
		}
		else if (charaPersonality_ == 1) {
			isJumpAction = FHelper::GetRand(0, 2) == 0;
		}
		else {
			isJumpAction = true;
		}
	}
	operation.isDriftTrigger_ = isJumpAction;

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
