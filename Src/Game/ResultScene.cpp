#include "ResultScene.h"
#include "Input.h"

ResultScene::ResultScene()
{

	/*===== �R���X�g���N�^ =====*/

	isTransition = false;
	nextScene = SCENE_ID::GAME;

}

void ResultScene::Init()
{

	/*===== ���������� =====*/

	isTransition = false;
	nextScene = SCENE_ID::GAME;

}

void ResultScene::Update()
{

	/*===== �X�V���� =====*/

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		isTransition = true;

	}

}

void ResultScene::Draw()
{
}
