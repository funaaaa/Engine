#include "SceneMgr.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "SceneTransition.h"

SceneMgr::SceneMgr()
{

	/*===== コンストラクタ =====*/

	SceneTransition::Ins()->Setting();

}

void SceneMgr::Init()
{

	/*===== 初期化処理 =====*/

	// シーンを生成。
	scenes_.emplace_back(std::make_shared<TitleScene>());
	scenes_.emplace_back(std::make_shared<GameScene>());

	// シーン番号を設定。
	nowScene_ = BaseScene::SCENE_ID::TITLE;
	nextScene_ = BaseScene::SCENE_ID::GAME;

	// シーンを初期化。
	scenes_[nowScene_]->Init();

}

void SceneMgr::Update()
{

	/*===== 更新処理 =====*/

	// 現在更新中のシーンで遷移フラグが立ったら。
	if (scenes_[nowScene_]->GetIsTransition()) {

		// 次シーンに移動。
		nowScene_ = scenes_[nowScene_]->GetNextScene();

		// シーンを初期化。
		scenes_[nowScene_]->Init();

		// デバッグ用で次シーンの情報を保存しておく。
		nextScene_ = scenes_[nowScene_]->GetNextScene();

	}

	// シーンの更新を行う。
	scenes_[nowScene_]->Update();

	// シーン遷移の更新処理
	SceneTransition::Ins()->Update();

}

void SceneMgr::Draw()
{

	/*===== 描画処理 =====*/

	scenes_[nowScene_]->Draw();

}
