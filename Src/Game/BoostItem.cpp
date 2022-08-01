#include "BoostItem.h"

void BoostItem::Generate(const int& CharaInsIndex)
{

	/*===== 生成処理 =====*/

	itemID_ = BaseItem::ItemID::BOOST;
	charaInsIndex_ = CharaInsIndex;

}

void BoostItem::Update()
{

	/*===== 更新処理 =====*/

}

void BoostItem::Use(const float& CharaRotY, const int ParamID)
{

	/*===== 使用処理 =====*/

	// きのこアイテム自体では使用されたときは何もしない。
	CharaRotY;
	ParamID;

}
