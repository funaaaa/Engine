#pragma once
#include "BaseScene.h"
#include "Singleton.h"
#include <vector>
#include <memory>

// �V�[���Ǘ��N���X
class SceneMgr : public Singleton<SceneMgr> {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<BaseScene>> scenes_;

public:

	BaseScene::SCENE_ID nowScene_;	// ���݂̃V�[���ԍ�
	BaseScene::SCENE_ID nextScene_;	// ���J�ڂ���Ƃ��̃V�[���ԍ�


public:

	/*===== �����o�֐� =====*/

	SceneMgr();
	void Init();
	void Update();
	void Draw();

};