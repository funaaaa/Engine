#pragma once
#include "BaseOperationObject.h"
#include <memory>

class FirstAIWayPointMgr;

// �ŏ��ɍ����AI�̑���I�u�W�F�N�g
class FirstAIOperationObject : public BaseOperationObject {

private:

	/*===== �����o�ϐ� =====*/

	std::shared_ptr<FirstAIWayPointMgr> waypointMgr_;
	int level_;
	int charaPersonality_;

	// ���x����������ۂ̃I�t�Z�b�g�ړ���
	const float LEVEL_OFFSET_SPEED = 0.3f;


public:

	/*===== �����o�֐� =====*/

	FirstAIOperationObject(int WayPointOffset, int Level, int CharaPersonality);

	BaseOperationObject::Operation Input(const BaseOperationObject::OperationInputData& InputData)override;

	void OnGoal()override;


};