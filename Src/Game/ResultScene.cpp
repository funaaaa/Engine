#include "ResultScene.h"
#include "Input.h"
#include "FHelper.h"
#include "Pipeline.h"

ResultScene::ResultScene()
{

	/*===== コンストラクタ =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::TITLE;

	result_.GenerateForTexture(FHelper::WindowCenterPos(), FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Result/result.dds");

}

void ResultScene::Init()
{

	/*===== 初期化処理 =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::TITLE;

}

void ResultScene::Update()
{

	/*===== 更新処理 =====*/

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		isTransition_ = true;

	}

	ImGui::Text("Press A or Enter to return to the title!");

}

void ResultScene::Draw()
{

	result_.Draw();

}
