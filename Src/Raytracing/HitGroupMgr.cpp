#include "HitGroupMgr.h"
#include "HitGroup.h"

const LPCWSTR& HitGroupMgr::GetCH(const int& Index)
{
	return hitGroup[Index]->GetCH();
}

const LPCWSTR& HitGroupMgr::GetAH(const int& Index)
{
	return hitGroup[Index]->GetAH();
}

const LPCWSTR& HitGroupMgr::GetIS(const int& Index)
{
	return hitGroup[Index]->GetIS();
}
const bool& HitGroupMgr::GetCHFlag(const int& Index)
{
	return hitGroup[Index]->GetCHFlag();
}
const bool& HitGroupMgr::GetAHFlag(const int& Index)
{
	return hitGroup[Index]->GetAHFlag();
}

const bool& HitGroupMgr::GetISFlag(const int& Index)
{
	return hitGroup[Index]->GetISFlag();
}

const int& HitGroupMgr::GetSRVCount(const int& Index)
{
	return hitGroup[Index]->GetSRVCount();
}

const int& HitGroupMgr::GetCBVCount(const int& Index)
{
	return hitGroup[Index]->GetCBVCount();
}

const int& HitGroupMgr::GetUAVCount(const int& Index)
{
	return hitGroup[Index]->GetUAVCount();
}

const std::shared_ptr<RayRootsignature> HitGroupMgr::GetLocalRootSig(const int& Index)
{
	return hitGroup[Index]->GetLoacalRootSig();
}

void HitGroupMgr::Setting()
{

	/*===== �q�b�g�O���[�v�ݒ� =====*/

	// �q�b�g�O���[�v���̃R���e�i��ݒ�B
	hitGroupNames.emplace_back(L"DEF_HIT_GROUP");

	// �q�b�g�O���[�v�𐶐��B
	HitGroupInitData initData;

	// ���v�f
	initData.CH = { L"mainCHS",true };
	initData.AH = { L"",false };
	initData.IS = { L"",false };
	initData.SRVcount = 3;
	initData.CBVcount = 0;
	initData.UAVcount = 0;
	hitGroup.push_back(std::make_shared<HitGroup>());	// ������size()�����̂܂ܓn���Ă���̂́A0�Ԗڂɂ̓O���[�o�����[�g�V�O�l�`��������̂ŃC���N�������g����K�v�����邩��B
	hitGroup[DEF_HIT_GROUP]->Generate(initData, hitGroup.size(), hitGroupNames[DEF_HIT_GROUP]);


}