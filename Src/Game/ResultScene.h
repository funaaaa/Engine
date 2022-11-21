#pragma once
#include "BaseScene.h"
#include "Sprite.h"

// ���U���g�V�[��
class ResultScene : public BaseScene {

private:

	/*===== �����o�ϐ� =====*/

	Sprite result_;
	bool isFinishTransition_;
	bool isStartTransition_;

public:

	/*===== �����o�֐� =====*/

	ResultScene();
	void Init()override;
	void Update()override;
	void Draw()override;

};