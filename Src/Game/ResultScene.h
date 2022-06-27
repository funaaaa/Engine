#pragma once
#include "BaseScene.h"

// リザルトシーン
class ResultScene : public BaseScene {

private:

	/*===== メンバ変数 =====*/


public:

	/*===== メンバ関数 =====*/

	ResultScene();
	void Init()override;
	void Update()override;
	void Draw()override;

};