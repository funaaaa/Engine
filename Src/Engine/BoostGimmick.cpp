#include "BoostGimmick.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"

void BoostGimmick::Setting(const ID& GimmickID, const UINT& InstanceID, const UINT& ShaderID)
{

	/*===== �Z�b�e�B���O���� =====*/

	// �I�u�W�F�N�g�����[�h
	insIndex = InstanceID;
	blasIndex = PolygonInstanceRegister::Ins()->GetBLASIndex(InstanceID);

	// �e��ϐ����������B
	gimmickID = GimmickID;
	isActive = false;

}

void BoostGimmick::Update()
{

	/*===== �X�V���� =====*/

}
