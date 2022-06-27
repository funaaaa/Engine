#include "SceneMgr.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

SceneMgr::SceneMgr()
{

	/*===== �R���X�g���N�^ =====*/

}

void SceneMgr::Init()
{

	/*===== ���������� =====*/

	// �V�[���𐶐��B
	scenes.emplace_back(std::make_shared<TitleScene>());
	scenes.emplace_back(std::make_shared<GameScene>());
	scenes.emplace_back(std::make_shared<ResultScene>());

	// �V�[���ԍ���ݒ�B
	nowScene = BaseScene::SCENE_ID::TITLE;
	nextScene = BaseScene::SCENE_ID::TITLE;

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

		// �V�[�����������B
		scenes[nowScene]->Init();

		// �f�o�b�O�p�Ŏ��V�[���̏���ۑ����Ă����B
		nextScene = scenes[nowScene]->GetNextScene();

	}

}

void SceneMgr::Draw()
{

	/*===== �`�揈�� =====*/

	scenes[nowScene]->Draw();

}
