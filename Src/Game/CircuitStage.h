#pragma once
#include "BaseStage.h"
#include <vector>

// サーキットステージ
class CircuitStage : public BaseStage {

private:

	/*===== メンバ変数 =====*/

	int timer_;	// 各オブジェクトで使用するタイマー。セッティングされてからインクリメントし続ける。


public:

	/*===== メンバ関数 =====*/

	void Setting()override;
	void Destroy()override;
	void Update()override;

};