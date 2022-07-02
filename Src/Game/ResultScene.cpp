#include "ResultScene.h"
#include "Input.h"

ResultScene::ResultScene()
{

	/*===== コンストラクタ =====*/

	isTransition = false;
	nextScene = SCENE_ID::GAME;

}

void ResultScene::Init()
{

	/*===== 初期化処理 =====*/

	isTransition = false;
	nextScene = SCENE_ID::GAME;

}

void ResultScene::Update()
{

	/*===== 更新処理 =====*/

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		isTransition = true;

	}

}

void ResultScene::Draw()
{
}
