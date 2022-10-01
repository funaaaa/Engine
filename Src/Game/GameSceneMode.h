#pragma once
#include "Singleton.h"

class GameSceneMode : public Singleton<GameSceneMode> {

	/*===== ƒƒ“ƒo•Ï” =====*/

public:

	enum class MODE {

		DEF,
		AI,
		WRITE_GHOST,
		GHOST,

	};

	MODE mode_;
	int level_;

	GameSceneMode() {
		mode_ = MODE::GHOST;
		level_;
	}

};