#include "SceneMgr.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "SceneTransition.h"

SceneMgr::SceneMgr()
{

	/*===== �R���X�g���N�^ =====*/

	SceneTransition::Ins()->Setting();

}

void SceneMgr::Init()
{

	/*===== ���������� =====*/

	// �V�[���𐶐��B
	scenes_.emplace_back(std::make_shared<TitleScene>());
	scenes_.emplace_back(std::make_shared<GameScene>());

	// �V�[���ԍ���ݒ�B
	nowScene_ = BaseScene::SCENE_ID::TITLE;
	nextScene_ = BaseScene::SCENE_ID::GAME;

	// �V�[�����������B
	scenes_[nowScene_]->Init();

}

void SceneMgr::Update()
{

	/*===== �X�V���� =====*/

	// ���ݍX�V���̃V�[���őJ�ڃt���O����������B
	if (scenes_[nowScene_]->GetIsTransition()) {

		// ���V�[���Ɉړ��B
		nowScene_ = scenes_[nowScene_]->GetNextScene();

		// �V�[�����������B
		scenes_[nowScene_]->Init();

		// �f�o�b�O�p�Ŏ��V�[���̏���ۑ����Ă����B
		nextScene_ = scenes_[nowScene_]->GetNextScene();

	}

	// �V�[���̍X�V���s���B
	scenes_[nowScene_]->Update();

	// �V�[���J�ڂ̍X�V����
	SceneTransition::Ins()->Update();

}

void SceneMgr::Draw()
{

	/*===== �`�揈�� =====*/

	scenes_[nowScene_]->Draw();

}
