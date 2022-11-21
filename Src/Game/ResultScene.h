#pragma once
#include "BaseScene.h"
#include "Sprite.h"

// リザルトシーン
class ResultScene : public BaseScene {

private:

	/*===== メンバ変数 =====*/

	Sprite result_;
	bool isFinishTransition_;
	bool isStartTransition_;

public:

	/*===== メンバ関数 =====*/

	ResultScene();
	void Init()override;
	void Update()override;
	void Draw()override;

};