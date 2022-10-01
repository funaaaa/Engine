#include "FirstAIWaypointMgr.h"
#include "FirstAIWayPoint.h"
#include <ConvertGimmickInfoFromBlender.h>

FirstAIWayPointMgr::FirstAIWayPointMgr()
{

	/*===== コンストラクタ =====*/

}

void FirstAIWayPointMgr::Setting(WAYPOINT_OFFSET Offset)
{

	/*===== 設定処理 =====*/

	switch (Offset)
	{
	case FirstAIWayPointMgr::WAYPOINT_OFFSET::CENTER:

	{

		// ウェイポイントを読み込む。
		ConvertGimmickInfoFromBlender wayPointData;
		wayPointData.Convert("Resource/Game/Stage/MugenStage/WayPoint/", "WayPoint.obj", DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), DirectX::XMMatrixScaling(120, 120, 120));

		int wayPointCount = wayPointData.GetVertexSize();
		std::vector<Vec3> wayPointPos;
		for (int index = 0; index < wayPointCount; ++index) {

			waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
			wayPointPos.emplace_back(wayPointData.GetVertex(index));

		}

		SetPositionData(wayPointPos);

	}

	break;
	default:
		break;
	}

}

void FirstAIWayPointMgr::Reset()
{

	/*===== 全て有効化 =====*/

	for (auto& index : waypoint_) {

		index->Validation();

	}

}

void FirstAIWayPointMgr::SetPositionData(std::vector<Vec3> PositionData)
{

	/*===== 座標データをセットする =====*/

	const int POSITIONDATA_SIZE = static_cast<int>(PositionData.size());
	const int WAYPOINT_SIZE = static_cast<int>(waypoint_.size());

	for (int index = 0; index < WAYPOINT_SIZE; ++index) {

		// 範囲外参照を阻止。
		if (POSITIONDATA_SIZE <= index) break;

		waypoint_[index]->SetPos(PositionData[index]);

	}

}

float FirstAIWayPointMgr::LeftRightCheck(const Vec3& Pos, const Vec3& ForwardVec)
{

	/*===== 進行方向が右側か左側かをチェックする =====*/

	// 先頭のIndexを求める。
	int headIndex = 0;
	for (auto& index : waypoint_) {

		if (!index->GetIsActive()) continue;

		headIndex = static_cast<int>(&index - &waypoint_[0]);

		break;

	}

	// ついでに当たり判定も行う。
	if (fabs(Vec3(waypoint_[headIndex]->GetPos() - Pos).Length()) < FirstAIWayPoint::RADIUS) {

		// ウェイポイントを無効化。
		waypoint_[headIndex]->Nullification();

		++headIndex;
		if (static_cast<int>(waypoint_.size()) <= headIndex) {

			headIndex = 0;

		}

	}

	// WayPointの座標を取得。
	Vec3 waypointPos = waypoint_[headIndex]->GetPos();

	// WayPointまでのベクトルを求める。
	Vec3 waypointVec = Vec3(waypointPos - Pos).GetNormal();

	// キャラの正面ベクトル
	Vec3 forwardVec = ForwardVec;

	Vec3 handleRate = forwardVec.Cross(waypointVec);

	if (0.1f < fabs(handleRate.y_)) {

		return handleRate.y_;

	}
	else {

		return handleRate.x_;

	}



}
