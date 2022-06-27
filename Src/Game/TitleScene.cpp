#include "TitleScene.h"
#include "Input.h"

TitleScene::TitleScene()
{

	/*===== コンストラクタ =====*/

	isTransition = false;
	nextScene = SCENE_ID::GAME;

}

void TitleScene::Init()
{

	/*===== 初期化処理 =====*/

	isTransition = false;
	nextScene = SCENE_ID::GAME;

}

void TitleScene::Update()
{

	/*===== 更新処理 =====*/

	if (Input::Ins()->isKeyTrigger(DIK_RETURN)) {

		isTransition = true;

	}

}

void TitleScene::Draw()
{
}
