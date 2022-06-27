#include "SceneMgr.h"

SceneMgr::SceneMgr()
{

	/*===== コンストラクタ =====*/

}

void SceneMgr::Init()
{

	/*===== 初期化処理 =====*/

	// シーンを生成。
	scenes.emplace_back(std::make_shared<GameScene>());

	// シーン番号を設定。
	nowScene = BaseScene::SCENE_ID::GAME;
	nextScene = BaseScene::SCENE_ID::GAME;

}

void SceneMgr::Update()
{

	/*===== 更新処理 =====*/

	// シーンの更新を行う。
	scenes[nowScene]->Update();

	// 現在更新中のシーンで遷移フラグが立ったら。
	if (scenes[nowScene]->GetIsTransition()) {

		// 次シーンに移動。
		nowScene = scenes[nowScene]->GetNextScene();

		// デバッグ用で次シーンの情報を保存しておく。
		nextScene = scenes[nowScene]->GetNextScene();

	}

}

void SceneMgr::Draw()
{

	/*===== 描画処理 =====*/

	scenes[nowScene]->Draw();

}
