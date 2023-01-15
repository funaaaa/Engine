#pragma once


// シーン基底クラス
class BaseScene {

public:

	enum SCENE_ID {

		TITLE,			// タイトルシーン
		GAME,			// ゲームシーン
		SCENE_COUNT,	// シーン数

	};

protected:

	SCENE_ID nextScene_;	// 次のシーン
	bool isTransition_;	// 遷移するかどうかのフラグ

public:

	BaseScene() {
		nextScene_ = SCENE_ID::GAME;
		isTransition_ = false;
	}
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	bool GetIsTransition() { return isTransition_; }
	const SCENE_ID& GetNextScene() { return nextScene_; }
	void ImGuiDebug(bool& IsMoveOnly1F);

};