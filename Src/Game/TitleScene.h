#pragma once
#include "BaseScene.h"

// タイトルシーン
class TitleScene : public BaseScene {

private:

	/*===== メンバ変数 =====*/


public:

	/*===== メンバ関数 =====*/

	TitleScene();
	void Init()override;
	void Update()override;
	void Draw()override;

};