#include "TitleScene.h"
#include "Input.h"
#include "Pipline.h"
#include "DirectXBase.h"

TitleScene::TitleScene()
{

	/*===== �R���X�g���N�^ =====*/

	isTransition = false;
	nextScene = SCENE_ID::GAME;

	title.GenerateForTexture(Vec3(500,300,0.1f), DirectX::XMFLOAT2(128,64), Pipline::PROJECTIONID::UI, Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/title.png");

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

	if (Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		isTransition = true;

	}

}

void TitleScene::Draw()
{

	title.Draw();

}
