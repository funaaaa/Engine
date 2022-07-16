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

	/*===== ヒットグループ名からヒットグループIDを取得する。 =====*/

	int hitGroupID = -1;

	for (int index = 0; index < MAX_HIT_GROUP; ++index) {

		// 同じ名前だったら。
		if (HitGroupName != hitGroupNames[index]) continue;

		// IDを保存。
		hitGroupID = index;

	}

	// -1だったら名前の存在しないヒットグループをセットしようとしているのでアサートを出す。
	if (hitGroupID == -1) {

		// 存在しないヒットグループをセットしようとしている。
		assert(0);

	}

	return hitGroupID;
}

int HitGroupMgr::GetHitGroupSRVCount(const int& HitGroupID)
{

	/*===== ヒットグループのSRVの数を取得 =====*/

	return hitGroup_[HitGroupID]->GetSRVCount();

}

int HitGroupMgr::GetHitGroupUAVCount(const int& HitGroupID)
{

	/*===== ヒットグループのSRVの数を取得 =====*/

	return hitGroup_[HitGroupID]->GetUAVCount();

}

void HitGroupMgr::Setting()
{

	/*===== ヒットグループ設定 =====*/

	// ヒットグループ名のコンテナを設定。
	hitGroupNames.emplace_back(L"DENOISE_AO_HIT_GROUP");

	// ヒットグループを生成。
	HitGroupInitData initData;

	// 第一要素
	initData.CH_ = { L"mainCHS",true };
	initData.AH_ = { L"mainAnyHit",true };
	initData.IS_ = { L"",false };
	initData.SRVcount_ = 5;
	initData.UAVcount_ = 0;
	initData.CBVcount_ = 0;
	hitGroup_.push_back(std::make_shared<HitGroup>());
	hitGroup_[DEF]->Generate(initData, 1, hitGroupNames[DEF]);

}