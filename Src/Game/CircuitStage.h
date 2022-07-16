#pragma once
#include "BaseStage.h"
#include <vector>

// �T�[�L�b�g�X�e�[�W
class CircuitStage : public BaseStage {

private:

	/*===== �����o�ϐ� =====*/

	int timer_;	// �e�I�u�W�F�N�g�Ŏg�p����^�C�}�[�B�Z�b�e�B���O����Ă���C���N�������g��������B
	int goalInsIndex;	// �S�[���̃C���X�^���X�̃C���f�b�N�X�B


public:

	/*===== �����o�֐� =====*/

	void Setting()override;
	void Destroy()override;
	void Update()override;
	BaseStage::ColliderOutput Collider(BaseStage::ColliderInput Input)override;
	
	// �S�[���̕\���A��\��
	void DisplayGoal()override;
	void NonDisplayGoal()override;

};