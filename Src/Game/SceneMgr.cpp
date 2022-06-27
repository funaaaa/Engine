#include "SceneMgr.h"

SceneMgr::SceneMgr()
{

	/*===== �R���X�g���N�^ =====*/

}

void SceneMgr::Init()
{

	/*===== ���������� =====*/

	// �V�[���𐶐��B
	scenes.emplace_back(std::make_shared<GameScene>());

	// �V�[���ԍ���ݒ�B
	nowScene = BaseScene::SCENE_ID::GAME;
	nextScene = BaseScene::SCENE_ID::GAME;

}

void SceneMgr::Update()
{

	/*===== �X�V���� =====*/

	// �V�[���̍X�V���s���B
	scenes[nowScene]->Update();

	// ���ݍX�V���̃V�[���őJ�ڃt���O����������B
	if (scenes[nowScene]->GetIsTransition()) {

		// ���V�[���Ɉړ��B
		nowScene = scenes[nowScene]->GetNextScene();

		// �f�o�b�O�p�Ŏ��V�[���̏���ۑ����Ă����B
		nextScene = scenes[nowScene]->GetNextScene();

	}

}

void SceneMgr::Draw()
{

	/*===== �`�揈�� =====*/

	scenes[nowScene]->Draw();

}
