#pragma once
#include "BaseScene.h"
#include "Sprite.h"

// �^�C�g���V�[��
class TitleScene : public BaseScene {

private:

	/*===== �����o�ϐ� =====*/

	Sprite title_;

public:

	/*===== �����o�֐� =====*/

	TitleScene();
	void Init()override;
	void Update()override;
	void Draw()override;

};