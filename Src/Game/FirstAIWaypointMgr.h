#pragma once
#include <memory>
#include <vector>
#include "Vec.h"

class FirstAIWayPoint;

class FirstAIWayPointMgr {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<FirstAIWayPoint>> waypoint_;

public:

	// �E�F�C�|�C���g�̔z�u�̃I�t�Z�b�g
	enum class WAYPOINT_OFFSET {
		CENTER,	// ���S
	};

private:

	WAYPOINT_OFFSET waypointOffset_;


public:

	/*===== �����o�֐� =====*/

	FirstAIWayPointMgr();
	void Setting(WAYPOINT_OFFSET Offset);
	void Reset();

	void SetPositionData(std::vector<Vec3> PositionData);

	float LeftRightCheck(const Vec3& Pos, const Vec3& ForwardVec);

};