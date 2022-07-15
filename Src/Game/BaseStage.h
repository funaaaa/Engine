#pragma once
#include <memory>
#include "Vec.h"

class StageObjectMgr;

class BaseStage {

protected:

	/*====== メンバ変数 =====*/

	std::shared_ptr<StageObjectMgr> stageObjectMgr_;	// ステージオブジェクト


public:

	/*===== メンバ関数 =====*/

	virtual void Setting() = 0;
	virtual void Destroy() = 0;
	virtual void Update() = 0;

};