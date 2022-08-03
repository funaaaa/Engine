#include "PlayerOperationObject.h"
#include "Input.h"

PlayerOperationObject::PlayerOperationObject(const int& ControllerIndex)
{

	/*===== コンストラクタ =====*/

	controllerIndex_ = ControllerIndex;

}

BaseOperationObject::Operation PlayerOperationObject::Input()
{

	/*===== 入力処理 =====*/

	BaseOperationObject::Operation operation;

	// 前進についての入力処理
	operation.accelerationRate_ = Input::Ins()->PadTrigger(XINPUT_TRIGGER_RIGHT);
	if (Input::Ins()->IsKey(DIK_W)) {
		operation.accelerationRate_ = 1.0f;
	}

	// アイテムを後ろに撃つかの入力処理
	if (Input::Ins()->IsKey(DIK_O)) {
		operation.isShotBehind_ = true;
	}

	// 左右のハンドル操作についての入力処理
	operation.handleDriveRate_ = Input::Ins()->PadStick(XINPUT_THUMB_LEFTSIDE);
	if (Input::Ins()->IsKey(DIK_D)) {
		operation.handleDriveRate_ = 1.0f;
	}
	if (Input::Ins()->IsKey(DIK_A)) {
		operation.handleDriveRate_ = -1.0f;
	}

	// ドリフトについての入力処理を行う。
	operation.isDrift_ = 0.9f < Input::Ins()->PadTrigger(XINPUT_TRIGGER_LEFT);
	if (Input::Ins()->IsKey(DIK_LSHIFT)) {
		operation.isDrift_ = true;
	}


	return operation;

}