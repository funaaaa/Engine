#pragma once


// �V�[�����N���X
class BaseScene {

public:

	enum SCENE_ID {

		GAME,			// �Q�[���V�[��
		SCENE_COUNT,	// �V�[����

	};

protected:

	SCENE_ID nextScene;	// ���̃V�[��
	bool isTransition;	// �J�ڂ��邩�ǂ����̃t���O

public:

	BaseScene() {
		nextScene = SCENE_ID::GAME;
		isTransition = false;
	}
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	const bool& GetIsTransition() { return isTransition; }
	const SCENE_ID& GetNextScene() { return nextScene; }

};