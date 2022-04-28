#include "HitGroupMgr.h"
#include "HitGroup.h"

void HitGroupMgr::Setting()
{

	/*===== ヒットグループ設定 =====*/

	// ヒットグループ名のコンテナを設定。
	hitGroupNames.emplace_back(L"DEF_HIT_GROUP");

	// ヒットグループを生成。
	HitGroupInitData initData;

	// 第一要素
	initData.CH = L"mainCHS";
	initData.AH = L"mainAnyHit";
	initData.SRVcount = 3;
	initData.CBVcount = 0;
	initData.UAVcount = 0;
	hitGroup.push_back(std::make_shared<HitGroup>());	// ここでsize()をそのまま渡しているのは、0番目にはグローバルルートシグネチャが入るのでインクリメントする必要があるから。
	hitGroup[DEF_HIT_GROUP]->Generate(initData, hitGroup.size(), hitGroupNames[DEF_HIT_GROUP]);


}