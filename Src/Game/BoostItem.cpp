#include "BoostItem.h"

BoostItem::BoostItem()
{
	itemID_ = ItemID::NONE;
}

void BoostItem::Generate(std::weak_ptr<PolygonMeshInstance> CharaInstance)
{

	/*===== �������� =====*/

	itemID_ = BaseItem::ItemID::BOOST;
	charaInstance = CharaInstance;

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
