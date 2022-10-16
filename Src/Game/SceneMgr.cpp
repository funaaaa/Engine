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
	scenes_.emplace_back(std::make_shared<TitleScene>());
	scenes_.emplace_back(std::make_shared<GameScene>());
	scenes_.emplace_back(std::make_shared<ResultScene>());

	// シーン番号を設定。
	nowScene_ = BaseScene::SCENE_ID::TITLE;
	nextScene_ = BaseScene::SCENE_ID::TITLE;

	// シーンを初期化。
	scenes_[nowScene_]->Init();

}

void SceneMgr::Update()
{

	/*===== 更新処理 =====*/

	// シーンの更新を行う。
	scenes_[nowScene_]->Update();

	// 現在更新中のシーンで遷移フラグが立ったら。
	if (scenes_[nowScene_]->GetIsTransition()) {

		// 次シーンに移動。
		nowScene_ = scenes_[nowScene_]->GetNextScene();

		// シーンを初期化。
		scenes_[nowScene_]->Init();

		// デバッグ用で次シーンの情報を保存しておく。
		nextScene_ = scenes_[nowScene_]->GetNextScene();

	}

}

void SceneMgr::Draw()
{

	/*===== 描画処理 =====*/

	scenes_[nowScene_]->Draw();

}
