#pragma once


// �V�[�����N���X
class BaseScene {

public:

	enum SCENE_ID {

		TITLE,			// �^�C�g���V�[��
		GAME,			// �Q�[���V�[��
		SCENE_COUNT,	// �V�[����

	};

protected:

	SCENE_ID nextScene_;	// ���̃V�[��
	bool isTransition_;	// �J�ڂ��邩�ǂ����̃t���O

public:

	BaseScene() {
		nextScene_ = SCENE_ID::GAME;
		isTransition_ = false;
	}
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	bool GetIsTransition() { return isTransition_; }
	const SCENE_ID& GetNextScene() { return nextScene_; }
	void ImGuiDebug(bool& IsMoveOnly1F);

};