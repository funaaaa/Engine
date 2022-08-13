#pragma once
#include "Singleton.h"

class GameSceneMode : public Singleton<GameSceneMode> {

	/*===== �����o�ϐ� =====*/

public:

	enum class MODE_ID {

		DEF,
		AI,
		WRITE_GHOST,

	};

	MODE_ID id_;

	GameSceneMode() {
		id_ = MODE_ID::WRITE_GHOST;
	}

};