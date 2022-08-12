#pragma once
#include "BaseOperationObject.h"

class PlayerOperationObject : public BaseOperationObject {

private:

	/*===== メンバ変数 =====*/

	int controllerIndex_;	// 操作するコントローラーのインデックス


public:

	/*===== メンバ関数 =====*/

	PlayerOperationObject(const int& ControllerIndex);

	BaseOperationObject::Operation Input(const BaseOperationObject::OperationInputData& InputData)override;

	void OnGoal()override;


};