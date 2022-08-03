#pragma once
#include "BaseOperationObject.h"

class FirstAIOperationObject : public BaseOperationObject {

private:

	/*===== メンバ変数 =====*/

	int controllerIndex_;	// 操作するコントローラーのインデックス


public:

	/*===== メンバ関数 =====*/

	FirstAIOperationObject();

	BaseOperationObject::Operation Input()override;


};