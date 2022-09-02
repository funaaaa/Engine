#pragma once
#include "BaseStage.h"
#include "Vec.h"
#include <vector>

// �����^�X�e�[�W
class MugenStage : public BaseStage {

private:

	/*===== �����o�ϐ� =====*/

	int timer_;	// �e�I�u�W�F�N�g�Ŏg�p����^�C�}�[�B�Z�b�e�B���O����Ă���C���N�������g��������B
	int goalInsIndex;	// �S�[���̃C���X�^���X�̃C���f�b�N�X�B

	std::vector<Vec3> pointLightPos;

public:

	/*===== �����o�֐� =====*/

	void Setting(const int& TireMaskIndex)override;
	void Destroy()override;
	void Update(RayConstBufferData& ConstBufferData)override;
	BaseStage::ColliderOutput Collider(BaseStage::ColliderInput Input)override;

	// �S�[���̕\���A��\��
	void DisplayGoal()override;
	void NonDisplayGoal()override;

};