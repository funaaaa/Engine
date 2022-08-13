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

	int mode = static_cast<int>(GameSceneMode::Ins()->id_);
	ImGui::RadioButton("DEF", &mode, static_cast<int>(GameSceneMode::MODE_ID::DEF));
	ImGui::SameLine();
	ImGui::RadioButton("AI", &mode, static_cast<int>(GameSceneMode::MODE_ID::AI));
	ImGui::SameLine();
	ImGui::RadioButton("WRITE_GHOST", &mode, static_cast<int>(GameSceneMode::MODE_ID::WRITE_GHOST));
	ImGui::SameLine();
	ImGui::RadioButton("GHOST", &mode, static_cast<int>(GameSceneMode::MODE_ID::GHOST));
	GameSceneMode::Ins()->id_ = static_cast<GameSceneMode::MODE_ID>(mode);

}

void TitleScene::Draw()
{

	title_.Draw();

}
