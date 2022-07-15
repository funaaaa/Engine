#pragma once
#include "BaseStage.h"
#include <vector>

// サーキットステージ
class CircuitStage : public BaseStage {

private:

	/*===== メンバ変数 =====*/

	std::vector<int> floatingObjIndex_;


public:

	/*===== メンバ関数 =====*/

	void Setting()override;
	void Destroy()override;
	void Update()override;

};