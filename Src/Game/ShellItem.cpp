#include "ShellItem.h"
#include "ShellObjectMgr.h"
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

void ShellItem::Use(const float& CharaRotY)
{

	/*===== �g�p���� =====*/

	// �b���A�C�e���������B
	Vec3 shellPos = PolygonInstanceRegister::Ins()->GetPos(charaInsIndex_);
	Vec3 shellVec = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(charaInsIndex_));

	// �b���A�C�e���𐶐�����B
	ShellObjectMgr::Ins()->AddObject(shellPos, shellVec, CharaRotY);




	/*
	
	
	���ɍb����u���@�\����������ۂ́AAddObject���b���𓊂��鏈���ɕς��A�b������������ǉ�����B���̍ۂ̓v���C���[��InsIndex��n���Ēǔ�������B
	
	
	*/




}
