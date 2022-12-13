#pragma once
#include "Singleton.h"

// トンネルでの輝度調整用のフラグ トーンマップを実装した際には消える。
class BrightnessParam : public Singleton<BrightnessParam> {

public:

	/*===== メンバ変数 =====*/

	bool isBright_;

};