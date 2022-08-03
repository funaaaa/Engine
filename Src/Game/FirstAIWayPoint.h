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

	const float RADIUS = 60.0f;


public:

	/*===== �����o�֐� =====*/

	FirstAIWayPoint();

	void Validation() { isActive_ = true; }
	void Nullification() { isActive_ = false; }

	// Imgui�Z�b�g�p �z�u����������폜�\��
	void SetPos(Vec3 Pos) { pos_ = Pos; }
	Vec3 GetPos() { return pos_; }

};