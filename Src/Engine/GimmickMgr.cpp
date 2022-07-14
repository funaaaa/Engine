#include "GimmickMgr.h"
#include "BoostGimmick.h"
#include "BLASRegister.h"

int GimmickMgr::AddGimmick(const BaseGimmick::ID& GimmickID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID)
{

	/*===== �M�~�b�N��ǉ� =====*/

	// ID���u�[�X�g��������
	if (GimmickID == BaseGimmick::ID::BOOST) {
		gimmicks_.emplace_back(std::make_shared<BoostGimmick>());
	}

	// Blas�����[�h
	int blasIndex_ = BLASRegister::Ins()->GenerateObj(DirectryPath, ModelName, HitGroupName, TexturePath);
	// instance�𐶐��B
	int instanceIndex = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex_, ShaderID);

	gimmicks_.back()->Setting(GimmickID, instanceIndex);

	return static_cast<int>(gimmicks_.size()) - 1;

}

void GimmickMgr::AddTrans(const int& GimmickIndex, const Vec3& Trans)
{

	/*===== �ړ������Z =====*/

	gimmicks_[GimmickIndex]->AddTrans(Trans);

}

void GimmickMgr::ChangeTrans(const int& GimmickIndex, const Vec3& Trans)
{

	/*===== �ړ����� =====*/

	gimmicks_[GimmickIndex]->ChangeTrans(Trans);

}

void GimmickMgr::AddScale(const int& GimmickIndex, const Vec3& Scale)
{

	/*===== �X�P�[�������Z =====*/

	gimmicks_[GimmickIndex]->AddScale(Scale);

}

void GimmickMgr::ChangeScale(const int& GimmickIndex, const Vec3& Scale)
{

	/*===== �X�P�[������ =====*/

	gimmicks_[GimmickIndex]->ChangeScale(Scale);

}

void GimmickMgr::AddRotate(const int& GimmickIndex, const Vec3& Rotate)
{

	/*===== ��]�����Z =====*/

	gimmicks_[GimmickIndex]->AddRotate(Rotate);

}

void GimmickMgr::ChangeRotate(const int& GimmickIndex, const Vec3& Rotate)
{

	/*===== ��]���� =====*/

	gimmicks_[GimmickIndex]->ChangeRotate(Rotate);

}
