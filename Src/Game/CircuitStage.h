#pragma once
#include "BaseStage.h"
#include <vector>

// �T�[�L�b�g�X�e�[�W
class CircuitStage : public BaseStage {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<int> floatingObjIndex_;


public:

	/*===== �����o�֐� =====*/

	void Setting()override;
	void Destroy()override;
	void Update()override;

};