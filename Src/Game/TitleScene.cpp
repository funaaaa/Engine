#include "TitleScene.h"
#include "Input.h"
#include "Pipline.h"
#include "DirectXBase.h"
#include "FHelper.h"
#include "GameSceneMode.h"

TitleScene::TitleScene()
{

	/*===== コンストラクタ =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::GAME;

	title_.GenerateForTexture(FHelper::WindowCenterPos(), FHelper::WindowHalfSize(), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/title.png");

}

void TitleScene::Init()
{

	/*===== 初期化処理 =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::GAME;

}

void TitleScene::Update()
{

	/*===== 更新処理 =====*/

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		isTransition_ = true;

	}

	int mode = GameSceneMode::Ins()->ghostLevel_;
	ImGui::RadioButton("Level : 1", &mode, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Level : 2", &mode, 1);
	ImGui::SameLine();
	ImGui::RadioButton("Level : 3", &mode, 2);
	ImGui::SameLine();
	ImGui::RadioButton("Level : 4", &mode, 3);
	GameSceneMode::Ins()->ghostLevel_ = mode;

}

void TitleScene::Draw()
{

	title_.Draw();

}
