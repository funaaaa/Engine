#pragma once
#include "BaseItem.h"

class ShellItem : public BaseItem {

private:

	/*===== メンバ変数 =====*/


public:

	/*===== メンバ関数 =====*/

	void Generate(const int& CharaInsIndex)override;
	void Update()override;
	void Use(const float& CharaRotY = 0)override;

};