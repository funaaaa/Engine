#include "BoostItem.h"

void BoostItem::Generate(const int& CharaInsIndex)
{

	/*===== �������� =====*/

	itemID_ = BaseItem::ItemID::BOOST;
	charaInsIndex_ = CharaInsIndex;

}

void BoostItem::Update()
{

	/*===== �X�V���� =====*/

}

void BoostItem::Use(const float& CharaRotY, const int ParamID)
{

	/*===== �g�p���� =====*/

	// ���̂��A�C�e�����̂ł͎g�p���ꂽ�Ƃ��͉������Ȃ��B
	CharaRotY;
	ParamID;

}
