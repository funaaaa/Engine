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

}
