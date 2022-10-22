#pragma once
#include <memory>
#include <vector>
#include "Vec.h"

class FirstAIWayPoint;

class FirstAIWayPointMgr {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<FirstAIWayPoint>> waypoint_;

public:

	// ウェイポイントの配置のオフセット
	enum class WAYPOINT_OFFSET {
		CENTER,	// 中心
	};

private:

	WAYPOINT_OFFSET waypointOffset_;


public:

	/*===== メンバ関数 =====*/

	FirstAIWayPointMgr();
	void Setting(WAYPOINT_OFFSET Offset);
	void Reset();

	void SetPositionData(std::vector<Vec3> PositionData);

	float LeftRightCheck(const Vec3& Pos, const Vec3& ForwardVec);

};