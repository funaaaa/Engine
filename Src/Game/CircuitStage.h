#pragma once
#include "BaseStage.h"
#include <vector>

// �T�[�L�b�g�X�e�[�W
class CircuitStage : public BaseStage {

private:

	/*===== �����o�ϐ� =====*/

	int timer_;	// �e�I�u�W�F�N�g�Ŏg�p����^�C�}�[�B�Z�b�e�B���O����Ă���C���N�������g��������B


public:

	/*===== �����o�֐� =====*/

	void Setting()override;
	void Destroy()override;
	void Update()override;

};