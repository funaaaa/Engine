#pragma once
#include "BaseScene.h"
#include "Sprite.h"

// ���U���g�V�[��
class ResultScene : public BaseScene {

private:

	/*===== �����o�ϐ� =====*/

	Sprite result;

public:

	/*===== �����o�֐� =====*/

	ResultScene();
	void Init()override;
	void Update()override;
	void Draw()override;

};