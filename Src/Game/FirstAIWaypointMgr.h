#pragma once
#include <memory>
#include <vector>
#include "Vec.h"

class FirstAIWayPoint;

class FirstAIWayPointMgr {

public:

	/*===== メンバ変数 =====*/

	std::vector<std::shared_ptr<FirstAIWayPoint>> waypoint;


public:

	/*===== メンバ関数 =====*/

	FirstAIWayPointMgr();
	void Setting();
	void Reset();

	void SetPositionData(std::vector<Vec3> PositionData);


	// ここに次のウェイポイントまでの左右判定を行う処理を書く。

};