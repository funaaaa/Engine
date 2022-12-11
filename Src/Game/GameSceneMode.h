#pragma once
#include "Singleton.h"

// �ǂ̃Q�[�����[�h��I�����邩�̃N���X�B��Ƀf�o�b�O�Ŏg�p����B�ʏ펞��AI��I������B
class GameSceneMode : public Singleton<GameSceneMode> {

	/*===== �����o�ϐ� =====*/

public:

	// �Q�[�����[�h
	enum class MODE {

		DEF,			// ��l
		AI,				// AI
		WRITE_GHOST,	// �S�[�X�g�������݃��[�h
		GHOST,			// �S�[�X�g

	};

	MODE mode_;	// �ǂ̃��[�h��
	int level_;	// �e���[�h�̃��x��

	GameSceneMode() {
		mode_ = MODE::AI;
		level_ = 0;
	}

};