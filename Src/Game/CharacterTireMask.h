#pragma once
#include "Vec.h"
#include "PlayerModel.h"
#include <memory>

class CharacterGameFinish;
class CharacterDrift;
class BaseStage;
struct CharacterFlags;

// �L�����N�^�[�̃^�C�����Ɋւ��鏈�����܂Ƃ߂��N���X
class CharacterTireMask {

public:

	/*===== �����o�ϐ� =====*/

	// UV���
	struct TireUVSet {
		Vec2 uv_;
		Vec2 prevuv_;
		TireUVSet() : uv_(Vec2()), prevuv_(Vec2()) {}
	};

	// �^�C�����������ݗp
	struct TireMaskUV {
		TireUVSet forwardLeftUV_;
		TireUVSet forwardRightUV_;
		TireUVSet behindLeftUV_;
		TireUVSet behindRightUV_;
		TireMaskUV() : forwardLeftUV_(TireUVSet()), forwardRightUV_(TireUVSet()), behindLeftUV_(TireUVSet()), behindRightUV_(TireUVSet()) {};
	};

	// �^�C�����X�V�������K�v�ȓ��͏��
	struct TireMaskInData {
		std::weak_ptr<CharacterGameFinish> gameFinish_;
		std::weak_ptr<CharacterFlags> flags_;
		std::weak_ptr<CharacterDrift> drift_;
		std::weak_ptr<BaseStage> stageData_;
		PlayerModel playerModel_;
		Vec3 bottomVec_;
	};


private:

	TireMaskUV tireMaskUV_;				// �^�C�������o���ۂɎg�p����B


public:

	/*===== �����o�֐� =====*/

	CharacterTireMask();
	void Init();
	bool CheckTireMask(const TireMaskInData& InData, TireMaskUV& TireMaskUVData);

};