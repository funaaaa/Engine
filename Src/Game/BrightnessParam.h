#pragma once
#include "Singleton.h"

// �g���l���ł̋P�x�����p�̃t���O �g�[���}�b�v�����������ۂɂ͏�����B
class BrightnessParam : public Singleton<BrightnessParam> {

public:

	/*===== �����o�ϐ� =====*/

	bool isBright_;

};