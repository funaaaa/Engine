#include "FirstAIWaypointMgr.h"
#include "FirstAIWayPoint.h"
#include <ConvertGimmickInfoFromBlender.h>

FirstAIWayPointMgr::FirstAIWayPointMgr()
{

	/*===== �R���X�g���N�^ =====*/

}

void FirstAIWayPointMgr::Setting(WAYPOINT_OFFSET Offset)
{

	/*===== �ݒ菈�� =====*/

	switch (Offset)
	{
	case FirstAIWayPointMgr::WAYPOINT_OFFSET::CENTER:

	{

		// �E�F�C�|�C���g��ǂݍ��ށB
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

		headIndex = static_cast<int>(&index - &waypoint_[0]);

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
