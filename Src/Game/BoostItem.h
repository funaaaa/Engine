#pragma once
#include "BaseItem.h"

class BoostItem : public BaseItem {

private:

	/*===== �����o�ϐ� =====*/


public:

	/*===== �����o�֐� =====*/

	void Generate(const int& CharaInsIndex)override;
	void Update()override;
	void Use(const float& CharaRotY = 0)override;

};