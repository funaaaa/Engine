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

	if (Input::Ins()->isKeyTrigger(DIK_RETURN)) {

		isTransition = true;

	}

}

void ResultScene::Draw()
{
}