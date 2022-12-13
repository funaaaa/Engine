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

	/*===== �������� =====*/

	itemID_ = BaseItem::ItemID::SHELL;
	charaInstance = CharaInstance;

}

void ShellItem::Update()
{

	/*===== �X�V���� =====*/

	// �X�V�����͉����s��Ȃ��B

}

int ShellItem::Use(float CharaRotY, int ParamID)
{

	/*===== �g�p���� =====*/

	// �b���A�C�e���������B
	Vec3 shellPos = charaInstance.lock()->GetPos();
	Vec3 shellVec;

	switch (static_cast<ShellItem::PARAM_ID>(ParamID))
	{
	case ShellItem::PARAM_ID::BEHIND:

		// �b���A�C�e���𐶐�����B
		behindShellIndex_ = ShellObjectMgr::Ins()->AddObject(shellPos, Vec3(0, 0, 0), CharaRotY, static_cast<int>(ShellObject::SHELL_ID::BEHIND), charaInstance);

		break;
	case ShellItem::PARAM_ID::FORWARD_THROW:

		// ���ʕ����Ƀx�N�g���𐶐�
		shellVec = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), charaInstance.lock()->GetRotate());

		// �b�����΂��B
		ShellObjectMgr::Ins()->ChangeStatus(behindShellIndex_, shellVec, static_cast<int>(ShellObject::SHELL_ID::FORWARD_THROW));

		break;
	case ShellItem::PARAM_ID::BEHIND_THROW:

		// �������Ƀx�N�g���𐶐�
		shellVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), charaInstance.lock()->GetRotate());

		// �b�����΂��B
		ShellObjectMgr::Ins()->ChangeStatus(behindShellIndex_, shellVec, static_cast<int>(ShellObject::SHELL_ID::BEHIND_THROW));

		break;
	default:
		break;
	}

	return behindShellIndex_;

}
