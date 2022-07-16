#pragma once
#include "BaseScene.h"
#include "Singleton.h"
#include <vector>
#include <memory>

// シーン管理クラス
class SceneMgr : public Singleton<SceneMgr> {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<BaseScene>> scenes_;

public:

	BaseScene::SCENE_ID nowScene_;	// 現在のシーン番号
	BaseScene::SCENE_ID nextScene_;	// 次遷移するときのシーン番号


public:

	/*===== メンバ関数 =====*/

	SceneMgr();
	void Init();
	void Update();
	void Draw();

};