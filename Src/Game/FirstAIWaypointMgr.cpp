#include "FirstAIWaypointMgr.h"
#include "FirstAIWayPoint.h"

FirstAIWayPointMgr::FirstAIWayPointMgr()
{

	/*===== �R���X�g���N�^ =====*/

}

void FirstAIWayPointMgr::Setting(WAYPOINT_OFFSET Offset)
{

	/*===== �ݒ菈�� =====*/

	switch (Offset)
	{
	case FirstAIWayPointMgr::WAYPOINT_OFFSET::LEFT_LEARNING:

		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());
		waypoint_.emplace_back(std::make_shared<FirstAIWayPoint>());

		SetPositionData({ Vec3(0.0f,0.0f,-2054.0f), Vec3(-933.0f, 0.0f, -2885.5f), Vec3(-2963.0f, 0.0f, -2591.0f),
			Vec3(-3292.0f, 0.0f, -573.5f), Vec3(-3703.0f, 0.0f, -192.5f), Vec3(-5380.0f, 0.0f, -191.0f), Vec3(-9335.5f, 2844.0f, -1.5f), Vec3(-9335.5f, 3407.0f, 764.5f),
			Vec3(-9336.0f, 3070.5f, 2569.5f), Vec3(-9332.0f, 1766.5f, 2891.0f), Vec3(-8280.0f, 672.0f, 2838.5f), Vec3(-2251.0f, 0.0f, 2872.0f), Vec3(-1733.0f, 0.0f, 3191.5f),
			Vec3(-1774.0f, 0.0f, 4425.0f), Vec3(-2206.5f, 0.0f, 4821.5f), Vec3(-2206.5f, 0.0f, 4821.5f), Vec3(-2894.5f, 0.0f, 4891.5f),
			Vec3(-3294.0f, 0.0f, 5313.5f), Vec3(-3269.0f, 0.0f, 5964.5f), Vec3(-2676.5f, 0.0f, 6433.5f), Vec3(-736.0f, 0.0f, 6355.0f),
			Vec3(99.0f, 0.0f, 5409.5f), Vec3(99.0f, 0.0f, 4019.5f), Vec3(-82.5f, 0.0f, 2999.5f), Vec3(94.5f, 0.0f, 1405.5f) });

		break;
	default:
		break;
	}

}

void FirstAIWayPointMgr::Reset()
{

	/*===== �S�ėL���� =====*/

	for (auto& index : waypoint_) {

		index->Validation();

	}

}

void FirstAIWayPointMgr::SetPositionData(std::vector<Vec3> PositionData)
{

	/*===== ���W�f�[�^���Z�b�g���� =====*/

	const int POSITIONDATA_SIZE = static_cast<int>(PositionData.size());
	const int WAYPOINT_SIZE = static_cast<int>(waypoint_.size());

	for (int index = 0; index < WAYPOINT_SIZE; ++index) {

		// �͈͊O�Q�Ƃ�j�~�B
		if (POSITIONDATA_SIZE <= index) break;

		waypoint_[index]->SetPos(PositionData[index]);

	}

}

float FirstAIWayPointMgr::LeftRightCheck(const Vec3& Pos, const Vec3& ForwardVec)
{

	/*===== �i�s�������E�������������`�F�b�N���� =====*/

	// �擪��Index�����߂�B
	int headIndex = 0;
	for (auto& index : waypoint_) {

		if (!index->GetIsActive()) continue;

		headIndex = &index - &waypoint_[0];

		break;

	}

	// ���łɓ����蔻����s���B
	if (fabs(Vec3(waypoint_[headIndex]->GetPos() - Pos).Length()) < FirstAIWayPoint::RADIUS) {

		// �E�F�C�|�C���g�𖳌����B
		waypoint_[headIndex]->Nullification();

		++headIndex;
		if (static_cast<int>(waypoint_.size()) <= headIndex) {

			headIndex = 0;

		}

	}

	// WayPoint�̍��W���擾�B
	Vec3 waypointPos = waypoint_[headIndex]->GetPos();

	// WayPoint�܂ł̃x�N�g�������߂�B
	Vec3 waypointVec = Vec3(waypointPos - Pos).GetNormal();

	// �L�����̐��ʃx�N�g��
	Vec3 forwardVec = ForwardVec;

	Vec3 handleRate = forwardVec.Cross(waypointVec);

	if (0.1f < fabs(handleRate.y_)) {

		return handleRate.y_;

	}
	else {

		return handleRate.x_;

	}



}
