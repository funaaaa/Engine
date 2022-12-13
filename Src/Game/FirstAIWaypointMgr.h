#pragma once
#include <memory>
#include <vector>
#include "Vec.h"

class FirstAIWayPoint;

// 最初に作ったAIのウェイポイントを保持し、キャラクターがどちら側にハンドルを切ればよいかを判定するクラス。
class FirstAIWayPointMgr {

private:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<FirstAIWayPoint>> waypoint_;	// ウェイポイントの配列

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

	// ウェイポイントを配置する。
	void SetPositionData(std::vector<Vec3> PositionData);

	// 次のウェイポイントに移動する際にどちら側にハンドルを切れば良いかを判断する関数。
	float LeftRightCheck(const Vec3& Pos, const Vec3& ForwardVec);

};