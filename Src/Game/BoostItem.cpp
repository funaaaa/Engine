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

	// �X�V�����͉����s��Ȃ��B

}

int BoostItem::Use(float CharaRotY, int ParamID)
{

	/*===== �g�p���� =====*/

	// ���̂��A�C�e�����̂ł͎g�p���ꂽ�Ƃ��͉������Ȃ��B
	CharaRotY;
	ParamID;

	return 0;

}
