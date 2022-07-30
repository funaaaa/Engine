#include "ShellItem.h"
#include "ShellObjectMgr.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"

void ShellItem::Generate(const int& CharaInsIndex)
{

	/*===== 生成処理 =====*/

	itemID_ = BaseItem::ItemID::SHELL;
	charaInsIndex_ = CharaInsIndex;

}

void ShellItem::Update()
{

	/*===== 更新処理 =====*/

	// 更新処理は何も行わない。

}

void ShellItem::Use(const float& CharaRotY)
{

	/*===== 使用処理 =====*/

	// 甲羅アイテム生成情報。
	Vec3 shellPos = PolygonInstanceRegister::Ins()->GetPos(charaInsIndex_);
	Vec3 shellVec = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(charaInsIndex_));

	// 甲羅アイテムを生成する。
	ShellObjectMgr::Ins()->AddObject(shellPos, shellVec, CharaRotY);




	/*
	
	
	後ろに甲羅を置く機能を実装する際は、AddObjectを甲羅を投げる処理に変え、甲羅を持つ処理を追加する。その際はプレイヤーのInsIndexを渡して追尾させる。
	
	
	*/




}
