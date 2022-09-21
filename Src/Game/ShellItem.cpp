#include "ShellItem.h"
#include "ShellObjectMgr.h"
#include "ShellObject.h"
#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "FHelper.h"

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

void ShellItem::Use(const float& CharaRotY, const int ParamID)
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

		// �b���𓮂����B
		shellPos += shellVec * 90.0f;

		// �b���A�C�e���𐶐�����B
		ShellObjectMgr::Ins()->AddObject(shellPos, shellVec, CharaRotY, static_cast<int>(ShellObject::SHELL_ID::FORWARD_THROW), charaInstance);

		// �ێ����Ă����b����j���B
		ShellObjectMgr::Ins()->DestroyObject(behindShellIndex_);

		break;
	case ShellItem::PARAM_ID::BEHIND_THROW:

		// �������Ƀx�N�g���𐶐�
		shellVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), charaInstance.lock()->GetRotate());

		// �b���A�C�e���𐶐�����B
		ShellObjectMgr::Ins()->AddObject(shellPos, shellVec, CharaRotY, static_cast<int>(ShellObject::SHELL_ID::BEHIND_THROW), charaInstance);

		// �ێ����Ă����b����j���B
		ShellObjectMgr::Ins()->DestroyObject(behindShellIndex_);

		break;
	default:
		break;
	}

}
