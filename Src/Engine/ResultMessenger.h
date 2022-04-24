#pragma once

#include "Singleton.h"
#include "SoundManager.h"

class ResultMessenger : public Singleton<ResultMessenger> {

public:

	/*-- �����o�ϐ� --*/

	float secondTimer;		// �X�^�[�g���Ă���̕b��
	float miriSecondTimer;	// �~���b
	int textureIDZeroOffset;
	SoundData* BGM;


public:

	/*-- �����o�֐� --*/

	// �����𒲂ׂ�B
	int CountDigits(int number);

	// �w��̌����̐������擾�B
	int GetDigits(int value, int m, int n);


};