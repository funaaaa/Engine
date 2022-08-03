#include "FirstAIWaypointMgr.h"
#include "FirstAIWayPoint.h"

FirstAIWayPointMgr::FirstAIWayPointMgr()
{

	/*===== コンストラクタ =====*/

}

void FirstAIWayPointMgr::Setting()
{

	/*===== 設定処理 =====*/

	waypoint.emplace_back(std::make_shared<FirstAIWayPoint>());

}

void FirstAIWayPointMgr::Reset()
{

	/*===== 全て有効化 =====*/

	for (auto& index : waypoint) {

		index->Validation();

	}

}

void FirstAIWayPointMgr::SetPositionData(std::vector<Vec3> PositionData)
{

	/*===== 座標データをセットする =====*/

	const int POSITIONDATA_SIZE = static_cast<int>(PositionData.size());
	const int WAYPOINT_SIZE = static_cast<int>(waypoint.size());

	for (int index = 0; index < WAYPOINT_SIZE; ++index) {

		// 範囲外参照を阻止。
		if (POSITIONDATA_SIZE <= index) break;

		waypoint[index]->SetPos(PositionData[index]);

	}

}
