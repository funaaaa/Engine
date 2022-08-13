#pragma once
#include "Singleton.h"

class GameSceneMode : public Singleton<GameSceneMode> {

	/*===== ƒƒ“ƒo•Ï” =====*/

public:

	enum class MODE_ID {

		DEF,
		AI,
		WRITE_GHOST,

	};

	MODE_ID id_;

};