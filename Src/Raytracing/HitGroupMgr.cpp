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

	/*===== ヒットグループ設定 =====*/

	// ヒットグループ名のコンテナを設定。
	hitGroupNames.emplace_back(L"DEF_HIT_GROUP");

	// ヒットグループを生成。
	HitGroupInitData initData;

	// 第一要素
	initData.CH = { L"mainCHS",true };
	initData.AH = { L"",false };
	initData.IS = { L"",false };
	initData.SRVcount = 3;
	initData.CBVcount = 0;
	initData.UAVcount = 0;
	hitGroup.push_back(std::make_shared<HitGroup>());	// ここでsize()をそのまま渡しているのは、0番目にはグローバルルートシグネチャが入るのでインクリメントする必要があるから。
	hitGroup[DEF_HIT_GROUP]->Generate(initData, hitGroup.size(), hitGroupNames[DEF_HIT_GROUP]);


}