#include "SceneMgr.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

SceneMgr::SceneMgr()
{

	/*===== コンストラクタ =====*/

}

void SceneMgr::Init()
{

	/*===== 初期化処理 =====*/

	// シーンを生成。
	scenes.emplace_back(std::make_shared<TitleScene>());
	scenes.emplace_back(std::make_shared<GameScene>());
	scenes.emplace_back(std::make_shared<ResultScene>());

	// シーン番号を設定。
	nowScene = BaseScene::SCENE_ID::TITLE;
	nextScene = BaseScene::SCENE_ID::TITLE;

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

		// シーンを初期化。
		scenes[nowScene]->Init();

		// デバッグ用で次シーンの情報を保存しておく。
		nextScene = scenes[nowScene]->GetNextScene();

	}

}

void SceneMgr::Draw()
{

	/*===== 描画処理 =====*/

	scenes[nowScene]->Draw();

}
