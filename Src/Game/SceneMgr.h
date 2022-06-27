#pragma once
#include "GameScene.h"
#include "Singleton.h"
#include <vector>
#include <memory>

// �V�[���Ǘ��N���X
class SceneMgr : public Singleton<SceneMgr> {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<BaseScene>> scenes;

public:

	BaseScene::SCENE_ID nowScene;	// ���݂̃V�[���ԍ�
	BaseScene::SCENE_ID nextScene;	// ���J�ڂ���Ƃ��̃V�[���ԍ�


public:

	/*===== �����o�֐� =====*/

	SceneMgr();
	void Init();
	void Update();
	void Draw();

};