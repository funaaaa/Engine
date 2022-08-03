#include "FirstAIWaypointMgr.h"
#include "FirstAIWayPoint.h"

FirstAIWayPointMgr::FirstAIWayPointMgr()
{

	/*===== �R���X�g���N�^ =====*/

}

void FirstAIWayPointMgr::Setting()
{

	/*===== �ݒ菈�� =====*/

	waypoint.emplace_back(std::make_shared<FirstAIWayPoint>());

}

void FirstAIWayPointMgr::Reset()
{

	/*===== �S�ėL���� =====*/

	for (auto& index : waypoint) {

		index->Validation();

	}

}

void FirstAIWayPointMgr::SetPositionData(std::vector<Vec3> PositionData)
{

	/*===== ���W�f�[�^���Z�b�g���� =====*/

	const int POSITIONDATA_SIZE = static_cast<int>(PositionData.size());
	const int WAYPOINT_SIZE = static_cast<int>(waypoint.size());

	for (int index = 0; index < WAYPOINT_SIZE; ++index) {

		// �͈͊O�Q�Ƃ�j�~�B
		if (POSITIONDATA_SIZE <= index) break;

		waypoint[index]->SetPos(PositionData[index]);

	}

}
