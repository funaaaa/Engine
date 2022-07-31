#include "ShellItem.h"
#include "ShellObjectMgr.h"
#include "ShellObject.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"

void ShellItem::Generate(const int& CharaInsIndex)
{

	/*===== �������� =====*/

	itemID_ = BaseItem::ItemID::SHELL;
	charaInsIndex_ = CharaInsIndex;

}

void ShellItem::Update()
{

	/*===== �X�V���� =====*/

	// �X�V�����͉����s��Ȃ��B

}

void ShellItem::Use(const float& CharaRotY, const int ParamID)
{

	/*===== �g�p���� =====*/

	// �b���A�C�e���������B
	Vec3 shellPos = PolygonInstanceRegister::Ins()->GetPos(charaInsIndex_);
	Vec3 shellVec;

	switch (static_cast<ShellItem::PARAM_ID>(ParamID))
	{
	case ShellItem::PARAM_ID::BEHIND:

		// �b���A�C�e���𐶐�����B
		behindShellIndex_ = ShellObjectMgr::Ins()->AddObject(shellPos, Vec3(0, 0, 0), CharaRotY, static_cast<int>(ShellObject::SHELL_ID::BEHIND), charaInsIndex_);

		break;
	case ShellItem::PARAM_ID::FORWARD_THROW:

		// ���ʕ����Ƀx�N�g���𐶐�
		shellVec = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(charaInsIndex_));

		// �b���A�C�e���𐶐�����B
		ShellObjectMgr::Ins()->AddObject(shellPos, shellVec, CharaRotY, static_cast<int>(ShellObject::SHELL_ID::FORWARD_THROW), charaInsIndex_);

		// �ێ����Ă����b����j���B
		ShellObjectMgr::Ins()->DestroyObject(behindShellIndex_);

		break;
	case ShellItem::PARAM_ID::BEHIND_THROW:

		// �������Ƀx�N�g���𐶐�
		shellVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PolygonInstanceRegister::Ins()->GetRotate(charaInsIndex_));

		// �b���A�C�e���𐶐�����B
		ShellObjectMgr::Ins()->AddObject(shellPos, shellVec, CharaRotY, static_cast<int>(ShellObject::SHELL_ID::BEHIND_THROW), charaInsIndex_);

		// �ێ����Ă����b����j���B
		ShellObjectMgr::Ins()->DestroyObject(behindShellIndex_);

		break;
	default:
		break;
	}

}
