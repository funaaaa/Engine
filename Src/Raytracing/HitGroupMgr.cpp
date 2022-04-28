#include "HitGroupMgr.h"
#include "HitGroup.h"

void HitGroupMgr::Setting()
{

	/*===== ヒットグループ設定 =====*/

	// ヒットグループ名のコンテナを設定。
	hitGroupNames.emplace_back(L"DEF_HIT_GROUP");

	// ヒットグループを生成。

	// 第一要素
	HitGroupInitData initData;
	initData.CH = L"mainCHS";
	//hitGroup.push_back(std::make_shared<HitGroup>());

}