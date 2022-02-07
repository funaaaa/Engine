#pragma once

#include "Singleton.h"
#include "SoundManager.h"

class ResultMessenger : public Singleton<ResultMessenger> {

public:

	/*-- メンバ変数 --*/

	float secondTimer;		// スタートしてからの秒数
	float miriSecondTimer;	// ミリ秒
	int textureIDZeroOffset;
	SoundData* BGM;


public:

	/*-- メンバ関数 --*/

	// 桁数を調べる。
	int CountDigits(int number);

	// 指定の桁数の数字を取得。
	int GetDigits(int value, int m, int n);


};