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

	return hitGroup[HitGroupID]->GetSRVCount();

}

void HitGroupMgr::Setting()
{

	/*===== ヒットグループ設定 =====*/

	// ヒットグループ名のコンテナを設定。
	hitGroupNames.emplace_back(L"DEF_HIT_GROUP");
	hitGroupNames.emplace_back(L"AO_HIT_GROUP");

	// ヒットグループを生成。
	HitGroupInitData initData;

	// 第一要素
	initData.CH = { L"mainCHS",true };
	initData.AH = { L"mainAnyHit",true };
	initData.IS = { L"",false };
	initData.SRVcount = 4;
	initData.CBVcount = 0;
	initData.UAVcount = 0;
	hitGroup.push_back(std::make_shared<HitGroup>());	// ここでsize()をそのまま渡しているのは、0番目にはグローバルルートシグネチャが入るのでインクリメントする必要があるから。
	hitGroup[DEF_HIT_GROUP]->Generate(initData, 1, hitGroupNames[DEF_HIT_GROUP]);

	// 第二要素
	initData.CH = { L"mainCHS",true };
	initData.AH = { L"mainAnyHit",true };
	initData.IS = { L"",false };
	initData.SRVcount = 4;
	initData.CBVcount = 0;
	initData.UAVcount = 0;
	hitGroup.push_back(std::make_shared<HitGroup>());	// ここでsize()をそのまま渡しているのは、0番目にはグローバルルートシグネチャが入るのでインクリメントする必要があるから。
	hitGroup[AO_HIT_GROUP]->Generate(initData, 1, hitGroupNames[AO_HIT_GROUP]);


}