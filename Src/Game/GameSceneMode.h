#pragma once
#include "Singleton.h"

// どのゲームモードを選択するかのクラス。主にデバッグで使用する。通常時はAIを選択する。
class GameSceneMode : public Singleton<GameSceneMode> {

	/*===== メンバ変数 =====*/

public:

	// ゲームモード
	enum class MODE {

		DEF,			// 一人
		AI,				// AI
		WRITE_GHOST,	// ゴースト書き込みモード
		GHOST,			// ゴースト

	};

	MODE mode_;	// どのモードか
	int level_;	// 各モードのレベル

	GameSceneMode() {
		mode_ = MODE::AI;
		level_ = 0;
	}

};