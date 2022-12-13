#pragma once
#include "Vec.h"
#include "Sprite.h"
#include "Singleton.h"

// �V�[���J�ڂ��s���N���X�B ���݂͍����e�N�X�`�����A���t�@�������ďo���Ă���B
class SceneTransition : public Singleton<SceneTransition> {

private:

	/*===== �����o�ϐ� =====*/

	Sprite titleLogo_;
	float alpha_;
	bool isAppear_;
	float easingTimer_;
	const float ADD_EASING_TIMER = 0.05f;


public:

	// �Z�b�e�B���O
	void Setting();

	// �J�n
	void Appear();

	// �I��
	void Exit();

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// �I�����Ă��邩�t���O
	bool GetIsFinish() { return 1.0f <= easingTimer_; }


};