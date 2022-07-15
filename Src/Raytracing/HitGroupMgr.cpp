#include "HitGroupMgr.h"
#include "HitGroup.h"
#include <assert.h>

const LPCWSTR& HitGroupMgr::GetCH(const int& Index)
{
	return hitGroup_[Index]->GetCH();
}

const LPCWSTR& HitGroupMgr::GetAH(const int& Index)
{
	return hitGroup_[Index]->GetAH();
}

const LPCWSTR& HitGroupMgr::GetIS(const int& Index)
{
	return hitGroup_[Index]->GetIS();
}
const bool& HitGroupMgr::GetCHFlag(const int& Index)
{
	return hitGroup_[Index]->GetCHFlag();
}
const bool& HitGroupMgr::GetAHFlag(const int& Index)
{
	return hitGroup_[Index]->GetAHFlag();
}

const bool& HitGroupMgr::GetISFlag(const int& Index)
{
	return hitGroup_[Index]->GetISFlag();
}

const int& HitGroupMgr::GetSRVCount(const int& Index)
{
	return hitGroup_[Index]->GetSRVCount();
}

const int& HitGroupMgr::GetCBVCount(const int& Index)
{
	return hitGroup_[Index]->GetCBVCount();
}

const int& HitGroupMgr::GetUAVCount(const int& Index)
{
	return hitGroup_[Index]->GetUAVCount();
}

const std::shared_ptr<RayRootsignature> HitGroupMgr::GetLocalRootSig(const int& Index)
{
	return hitGroup_[Index]->GetLoacalRootSig();
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

	return hitGroup_[HitGroupID]->GetSRVCount();

}

int HitGroupMgr::GetHitGroupUAVCount(const int& HitGroupID)
{

	/*===== �q�b�g�O���[�v��SRV�̐����擾 =====*/

	return hitGroup_[HitGroupID]->GetUAVCount();

}

void HitGroupMgr::Setting()
{

	/*===== �q�b�g�O���[�v�ݒ� =====*/

	// �q�b�g�O���[�v���̃R���e�i��ݒ�B
	hitGroupNames.emplace_back(L"DENOISE_AO_HIT_GROUP");

	// �q�b�g�O���[�v�𐶐��B
	HitGroupInitData initData;

	// ���v�f
	initData.CH_ = { L"mainCHS",true };
	initData.AH_ = { L"mainAnyHit",true };
	initData.IS_ = { L"",false };
	initData.SRVcount_ = 5;
	initData.UAVcount_ = 0;
	initData.CBVcount_ = 0;
	hitGroup_.push_back(std::make_shared<HitGroup>());
	hitGroup_[DEF]->Generate(initData, 1, hitGroupNames[DEF]);

}