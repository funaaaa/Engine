#pragma once


// シーン基底クラス
class BaseScene {

public:

	enum SCENE_ID {

		TITLE,			// タイトルシーン
		GAME,			// ゲームシーン
		RESULT,			// リザルトシーン
		SCENE_COUNT,	// シーン数

	};

protected:

	SCENE_ID nextScene_;	// 次のシーン
	bool isTransition;	// 遷移するかどうかのフラグ

public:

	BaseScene() {
		nextScene_ = SCENE_ID::GAME;
		isTransition = false;
	}
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	const bool& GetIsTransition() { return isTransition; }
	const SCENE_ID& GetNextScene() { return nextScene_; }

};