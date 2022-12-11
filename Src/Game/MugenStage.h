#pragma once
#include "BaseStage.h"
#include "Vec.h"
#include <vector>
#include <string>

// �����^�X�e�[�W ��{�͂��̃X�e�[�W���g�p����B
class MugenStage : public BaseStage {

private:

	/*===== �����o�ϐ� =====*/

	int timer_;	// �e�I�u�W�F�N�g�Ŏg�p����^�C�}�[�B�Z�b�e�B���O����Ă���C���N�������g��������B
	int goalInsIndex;	// �S�[���̃C���X�^���X�̃C���f�b�N�X�B
	int tunnelIndex_;

public:

	/*===== �����o�֐� =====*/

	void Setting(int TireMaskIndex)override;
	void Destroy()override;
	void Update()override;
	BaseStage::ColliderOutput Collider(BaseStage::ColliderInput Input)override;


private:

	// �r�������[�h����ۂɎg�p����p�����[�^�[
	enum class OBJ_DATA_ID {
		ID,			// �I�u�W�F�N�g��ID
		POSITION,	// �I�u�W�F�N�g�̍��W�p�����[�^�[
		SCALE,		// �I�u�W�F�N�g�̃X�P�[���p�����[�^�[
		ROTATE,		// �I�u�W�F�N�g�̉�]�p�����[�^�[
		X,			// �e�p�����[�^�[��X����
		Y,			// �e�p�����[�^�[��Y����
		Z,			// �e�p�����[�^�[��Z����
	};
	void LoadBuilding();
	void GetBuildingData(std::string FilePath);

	// �����M�~�b�N���Z�b�g�B�������txt����ǂݎ���悤�ɂ���\��ł��B
	void SetBoostGimmick();

};