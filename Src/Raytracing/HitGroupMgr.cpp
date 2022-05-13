#include "HitGroupMgr.h"
#include "HitGroup.h"
#include <assert.h>

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

int HitGroupMgr::GetHitGroupID(const LPCWSTR& HitGroupName)
{

	/*===== �q�b�g�O���[�v������q�b�g�O���[�vID���擾����B =====*/

	int hitGroupID = -1;

	for (int index = 0; index < MAX_HIT_GROUP; ++index) {

		// �������O��������B
		if (HitGroupName != hitGroupNames[index]) continue;

		// ID��ۑ��B
		hitGroupID = index;

	}

	// -1�������疼�O�̑��݂��Ȃ��q�b�g�O���[�v���Z�b�g���悤�Ƃ��Ă���̂ŃA�T�[�g���o���B
	if (hitGroupID == -1) {

		// ���݂��Ȃ��q�b�g�O���[�v���Z�b�g���悤�Ƃ��Ă���B
		assert(0);

	}

	return hitGroupID;
}

int HitGroupMgr::GetHitGroupSRVCount(const int& HitGroupID)
{

	/*===== �q�b�g�O���[�v��SRV�̐����擾 =====*/

	return hitGroup[HitGroupID]->GetSRVCount();

}

void HitGroupMgr::Setting()
{

	/*===== �q�b�g�O���[�v�ݒ� =====*/

	// �q�b�g�O���[�v���̃R���e�i��ݒ�B
	hitGroupNames.emplace_back(L"DEF_HIT_GROUP");
	hitGroupNames.emplace_back(L"AO_HIT_GROUP");

	// �q�b�g�O���[�v�𐶐��B
	HitGroupInitData initData;

	// ���v�f
	initData.CH = { L"mainCHS",true };
	initData.AH = { L"mainAnyHit",true };
	initData.IS = { L"",false };
	initData.SRVcount = 4;
	initData.CBVcount = 0;
	initData.UAVcount = 0;
	hitGroup.push_back(std::make_shared<HitGroup>());	// ������size()�����̂܂ܓn���Ă���̂́A0�Ԗڂɂ̓O���[�o�����[�g�V�O�l�`��������̂ŃC���N�������g����K�v�����邩��B
	hitGroup[DEF_HIT_GROUP]->Generate(initData, 1, hitGroupNames[DEF_HIT_GROUP]);

	// ���v�f
	initData.CH = { L"mainCHS",true };
	initData.AH = { L"mainAnyHit",true };
	initData.IS = { L"",false };
	initData.SRVcount = 4;
	initData.CBVcount = 0;
	initData.UAVcount = 0;
	hitGroup.push_back(std::make_shared<HitGroup>());	// ������size()�����̂܂ܓn���Ă���̂́A0�Ԗڂɂ̓O���[�o�����[�g�V�O�l�`��������̂ŃC���N�������g����K�v�����邩��B
	hitGroup[AO_HIT_GROUP]->Generate(initData, 1, hitGroupNames[AO_HIT_GROUP]);


}