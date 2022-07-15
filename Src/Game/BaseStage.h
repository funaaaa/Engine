#pragma once
#include <memory>
#include "Vec.h"

class StageObjectMgr;

class BaseStage {

protected:

	/*====== �����o�ϐ� =====*/

	std::shared_ptr<StageObjectMgr> stageObjectMgr_;	// �X�e�[�W�I�u�W�F�N�g


public:

	/*===== �����o�֐� =====*/

	virtual void Setting() = 0;
	virtual void Destroy() = 0;
	virtual void Update() = 0;

};