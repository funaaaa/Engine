#include "TitleScene.h"
#include "Input.h"

TitleScene::TitleScene()
{

	/*===== �R���X�g���N�^ =====*/

	isTransition = false;
	nextScene = SCENE_ID::GAME;

}

void TitleScene::Init()
{

	/*===== ���������� =====*/

	isTransition = false;
	nextScene = SCENE_ID::GAME;

}

void TitleScene::Update()
{

	/*===== �X�V���� =====*/

	if (Input::Ins()->isKeyTrigger(DIK_RETURN)) {

		isTransition = true;

	}

}

void TitleScene::Draw()
{
}
