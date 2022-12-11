#pragma once
#include <memory>
#include <vector>
#include "Vec.h"

class FirstAIWayPoint;

// �ŏ��ɍ����AI�̃E�F�C�|�C���g��ێ����A�L�����N�^�[���ǂ��瑤�Ƀn���h����؂�΂悢���𔻒肷��N���X�B
class FirstAIWayPointMgr {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<std::shared_ptr<FirstAIWayPoint>> waypoint_;	// �E�F�C�|�C���g�̔z��

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

	// �E�F�C�|�C���g��z�u����B
	void SetPositionData(std::vector<Vec3> PositionData);

	// ���̃E�F�C�|�C���g�Ɉړ�����ۂɂǂ��瑤�Ƀn���h����؂�Ηǂ����𔻒f����֐��B
	float LeftRightCheck(const Vec3& Pos, const Vec3& ForwardVec);

};