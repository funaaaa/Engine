#pragma once
#include "BaseStage.h"
#include "Vec.h"
#include <vector>
#include <string>

// �����^�X�e�[�W
class MugenStage : public BaseStage {

private:

	/*===== �����o�ϐ� =====*/

	int timer_;	// �e�I�u�W�F�N�g�Ŏg�p����^�C�}�[�B�Z�b�e�B���O����Ă���C���N�������g��������B
	int goalInsIndex;	// �S�[���̃C���X�^���X�̃C���f�b�N�X�B
	int tunnelIndex_;

	std::vector<Vec3> pointLightPos;

public:

	enum class STATUS {

		DEF,
		REFLECTION,

	};

private:

	STATUS status_;

public:

	/*===== �����o�֐� =====*/

	void Setting(int TireMaskIndex)override;
	void Destroy()override;
	void Update()override;
	BaseStage::ColliderOutput Collider(BaseStage::ColliderInput Input)override;


private:

	enum class OBJ_DATA_ID {
		ID,
		POSITION,
		SCALE,
		ROTATE,
		X,
		Y,
		Z,
	};
	void LoadBuilding();
	void GetBuildingData(std::string FilePath);

};