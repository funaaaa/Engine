#include "ResultScene.h"
#include "Input.h"
#include "FHelper.h"
#include "Pipline.h"

ResultScene::ResultScene()
{

	/*===== �R���X�g���N�^ =====*/

	isTransition = false;
	nextScene_ = SCENE_ID::GAME;

	result_.GenerateForTexture(FHelper::WindowCenterPos(), FHelper::WindowHalfSize(), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Result/result.png");

}

void ResultScene::Init()
{

	/*===== ���������� =====*/

	isTransition = false;
	nextScene_ = SCENE_ID::GAME;

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

	result_.Draw();

}
