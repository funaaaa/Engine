#pragma once
#include "BaseItem.h"

class ShellItem : public BaseItem {

private:

	/*===== �����o�ϐ� =====*/


public:

	/*===== �����o�֐� =====*/

	void Generate(const int& CharaInsIndex)override;
	void Update()override;
	void Use()override;

};