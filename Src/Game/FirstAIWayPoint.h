#pragma once
#include "Vec.h"

// �ŏ���AI����������ۂɎg�p���铹�p�E�F�C�|�C���g
class FirstAIWayPoint {

public:

	/*===== �����o�ϐ� =====*/

	Vec3 pos_;
	int insIndex_;
	int blasIndex_;
	bool isActive_;

public:

	static const int RADIUS = 300;


public:

	/*===== �����o�֐� =====*/

	FirstAIWayPoint();

	void Validation() { isActive_ = true; }
	void Nullification() { isActive_ = false; }

	bool GetIsActive() { return isActive_; }

	// Imgui�Z�b�g�p �z�u����������폜�\��
	void SetPos(Vec3 Pos);
	Vec3 GetPos() { return pos_; }

};