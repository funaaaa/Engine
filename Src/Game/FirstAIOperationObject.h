#pragma once
#include "BaseOperationObject.h"

class FirstAIOperationObject : public BaseOperationObject {

private:

	/*===== �����o�ϐ� =====*/

	int controllerIndex_;	// ���삷��R���g���[���[�̃C���f�b�N�X


public:

	/*===== �����o�֐� =====*/

	FirstAIOperationObject();

	BaseOperationObject::Operation Input()override;


};