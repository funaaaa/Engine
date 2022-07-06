#include "BoostGimmick.h"
#include "BLASRegister.h"
#include "HitGroupMgr.h"
#include "PorygonInstanceRegister.h"

void BoostGimmick::Setting(const ID& GimmickID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID)
{

	/*===== �Z�b�e�B���O���� =====*/

	// �I�u�W�F�N�g�����[�h
	blasIndex = BLASRegister::Ins()->GenerateObj(DirectryPath, ModelName, HitGroupName, TexturePath);
	insIndex = PorygonInstanceRegister::Ins()->CreateInstance(blasIndex, ShaderID);

	// �e��ϐ����������B
	gimmickID = GimmickID;
	isActive = false;

}

void BoostGimmick::Update()
{

	/*===== �X�V���� =====*/

}
