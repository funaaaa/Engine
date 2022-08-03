#pragma once
#include <memory>
#include <vector>
#include "Vec.h"

class FirstAIWayPoint;

class FirstAIWayPointMgr {

public:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<FirstAIWayPoint>> waypoint;


public:

	/*===== �����o�֐� =====*/

	FirstAIWayPointMgr();
	void Setting();
	void Reset();

	void SetPositionData(std::vector<Vec3> PositionData);


	// �����Ɏ��̃E�F�C�|�C���g�܂ł̍��E������s�������������B

};