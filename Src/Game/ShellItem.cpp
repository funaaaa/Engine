#include "ShellItem.h"
#include "ShellObjectMgr.h"
#include "ShellObject.h"
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

void ShellItem::Use(const float& CharaRotY, const int ParamID)
{

	/*===== 使用処理 =====*/

	// 甲羅アイテム生成情報。
	Vec3 shellPos = PolygonInstanceRegister::Ins()->GetPos(charaInsIndex_);
	Vec3 shellVec;

	switch (static_cast<ShellItem::PARAM_ID>(ParamID))
	{
	case ShellItem::PARAM_ID::BEHIND:

		// 甲羅アイテムを生成する。
		behindShellIndex_ = ShellObjectMgr::Ins()->AddObject(shellPos, Vec3(0, 0, 0), CharaRotY, static_cast<int>(ShellObject::SHELL_ID::BEHIND), charaInsIndex_);

		break;
	case ShellItem::PARAM_ID::FORWARD_THROW:

		// 正面方向にベクトルを生成
		shellVec = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(charaInsIndex_));

		// 甲羅アイテムを生成する。
		ShellObjectMgr::Ins()->AddObject(shellPos, shellVec, CharaRotY, static_cast<int>(ShellObject::SHELL_ID::FORWARD_THROW), charaInsIndex_);

		// 保持していた甲羅を破棄。
		ShellObjectMgr::Ins()->DestroyObject(behindShellIndex_);

		break;
	case ShellItem::PARAM_ID::BEHIND_THROW:

		// 後ろ方向にベクトルを生成
		shellVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PolygonInstanceRegister::Ins()->GetRotate(charaInsIndex_));

		// 甲羅アイテムを生成する。
		ShellObjectMgr::Ins()->AddObject(shellPos, shellVec, CharaRotY, static_cast<int>(ShellObject::SHELL_ID::BEHIND_THROW), charaInsIndex_);

		// 保持していた甲羅を破棄。
		ShellObjectMgr::Ins()->DestroyObject(behindShellIndex_);

		break;
	default:
		break;
	}

}
