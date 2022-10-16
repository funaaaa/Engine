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
	scenes_.emplace_back(std::make_shared<TitleScene>());
	scenes_.emplace_back(std::make_shared<GameScene>());
	scenes_.emplace_back(std::make_shared<ResultScene>());

	// �V�[���ԍ���ݒ�B
	nowScene_ = BaseScene::SCENE_ID::TITLE;
	nextScene_ = BaseScene::SCENE_ID::TITLE;

	// �V�[�����������B
	scenes_[nowScene_]->Init();

}

void SceneMgr::Update()
{

	/*===== �X�V���� =====*/

	// �V�[���̍X�V���s���B
	scenes_[nowScene_]->Update();

	// ���ݍX�V���̃V�[���őJ�ڃt���O����������B
	if (scenes_[nowScene_]->GetIsTransition()) {

		// ���V�[���Ɉړ��B
		nowScene_ = scenes_[nowScene_]->GetNextScene();

		// �V�[�����������B
		scenes_[nowScene_]->Init();

		// �f�o�b�O�p�Ŏ��V�[���̏���ۑ����Ă����B
		nextScene_ = scenes_[nowScene_]->GetNextScene();

	}

}

void SceneMgr::Draw()
{

	/*===== �`�揈�� =====*/

	scenes_[nowScene_]->Draw();

}
