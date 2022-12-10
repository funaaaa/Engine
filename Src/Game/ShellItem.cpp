#include "ShellItem.h"
#include "ShellObjectMgr.h"
#include "ShellObject.h"
#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "FHelper.h"

ShellItem::ShellItem()
{
	itemID_ = ItemID::NONE;
}

void ShellItem::Generate(std::weak_ptr<PolygonMeshInstance> CharaInstance)
{

	/*===== 生成処理 =====*/

	itemID_ = BaseItem::ItemID::SHELL;
	charaInstance = CharaInstance;

}

void ShellItem::Update()
{

	/*===== 更新処理 =====*/

	// 更新処理は何も行わない。

}

int ShellItem::Use(float CharaRotY, int ParamID)
{

	/*===== 使用処理 =====*/

	// 甲羅アイテム生成情報。
	Vec3 shellPos = charaInstance.lock()->GetPos();
	Vec3 shellVec;

	switch (static_cast<ShellItem::PARAM_ID>(ParamID))
	{
	case ShellItem::PARAM_ID::BEHIND:

		// 甲羅アイテムを生成する。
		behindShellIndex_ = ShellObjectMgr::Ins()->AddObject(shellPos, Vec3(0, 0, 0), CharaRotY, static_cast<int>(ShellObject::SHELL_ID::BEHIND), charaInstance);

		break;
	case ShellItem::PARAM_ID::FORWARD_THROW:

		// 正面方向にベクトルを生成
		shellVec = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), charaInstance.lock()->GetRotate());

		// 甲羅を飛ばす。
		ShellObjectMgr::Ins()->ChangeStatus(behindShellIndex_, shellVec, static_cast<int>(ShellObject::SHELL_ID::FORWARD_THROW));

		break;
	case ShellItem::PARAM_ID::BEHIND_THROW:

		// 後ろ方向にベクトルを生成
		shellVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), charaInstance.lock()->GetRotate());

		// 甲羅を飛ばす。
		ShellObjectMgr::Ins()->ChangeStatus(behindShellIndex_, shellVec, static_cast<int>(ShellObject::SHELL_ID::BEHIND_THROW));

		break;
	default:
		break;
	}

	return behindShellIndex_;

}
