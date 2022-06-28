#pragma once
#include "BaseScene.h"
#include "Sprite.h"

// タイトルシーン
class TitleScene : public BaseScene {

private:

	/*===== メンバ変数 =====*/

	Sprite title;

public:

	/*===== メンバ関数 =====*/

	TitleScene();
	void Init()override;
	void Update()override;
	void Draw()override;

};