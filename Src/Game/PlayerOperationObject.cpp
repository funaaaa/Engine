#include "PlayerOperationObject.h"
#include "Input.h"

PlayerOperationObject::PlayerOperationObject(const int& ControllerIndex)
{

	/*===== �R���X�g���N�^ =====*/

	controllerIndex_ = ControllerIndex;

}

BaseOperationObject::Operation PlayerOperationObject::Input()
{

	/*===== ���͏��� =====*/

	BaseOperationObject::Operation operation;

	// �O�i�ɂ��Ă̓��͏���
	operation.accelerationRate_ = Input::Ins()->PadTrigger(XINPUT_TRIGGER_RIGHT);
	if (Input::Ins()->IsKey(DIK_W)) {
		operation.accelerationRate_ = 1.0f;
	}

	// �A�C�e�������Ɍ����̓��͏���
	if (Input::Ins()->IsKey(DIK_O)) {
		operation.isShotBehind_ = true;
	}

	// ���E�̃n���h������ɂ��Ă̓��͏���
	operation.handleDriveRate_ = Input::Ins()->PadStick(XINPUT_THUMB_LEFTSIDE);
	if (Input::Ins()->IsKey(DIK_D)) {
		operation.handleDriveRate_ = 1.0f;
	}
	if (Input::Ins()->IsKey(DIK_A)) {
		operation.handleDriveRate_ = -1.0f;
	}

	// �h���t�g�ɂ��Ă̓��͏������s���B
	operation.isDrift_ = 0.9f < Input::Ins()->PadTrigger(XINPUT_TRIGGER_LEFT);
	if (Input::Ins()->IsKey(DIK_LSHIFT)) {
		operation.isDrift_ = true;
	}


	return operation;

}