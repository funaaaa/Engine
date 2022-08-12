#pragma once
#include "BaseOperationObject.h"

class PlayerOperationObject : public BaseOperationObject {

private:

	/*===== �����o�ϐ� =====*/

	int controllerIndex_;	// ���삷��R���g���[���[�̃C���f�b�N�X


public:

	/*===== �����o�֐� =====*/

	PlayerOperationObject(const int& ControllerIndex);

	BaseOperationObject::Operation Input(const BaseOperationObject::OperationInputData& InputData)override;

	void OnGoal()override;


};