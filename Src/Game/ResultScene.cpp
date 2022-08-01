#include "ResultScene.h"
#include "Input.h"
#include "FHelper.h"
#include "Pipline.h"

ResultScene::ResultScene()
{

	/*===== コンストラクタ =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::GAME;

	result_.GenerateForTexture(FHelper::WindowCenterPos(), FHelper::WindowHalfSize(), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Result/result.png");

}

void ResultScene::Init()
{

	/*===== 初期化処理 =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::GAME;

}

void ResultScene::Update()
{

	/*===== 更新処理 =====*/

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		isTransition_ = true;

	}

}

void ResultScene::Draw()
{

	result_.Draw();

}
