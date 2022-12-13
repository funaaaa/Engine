#pragma once
#include "Vec.h"

// �ŏ���AI����������ۂɎg�p���铹�p�E�F�C�|�C���g
class FirstAIWayPoint {

public:

	/*===== �����o�ϐ� =====*/

	Vec3 pos_;			// �z�u������W
	int insIndex_;		// �f�o�b�O�ŃE�F�C�|�C���g��`�悷��ۂ̕`��f�[�^
	int blasIndex_;		// �f�o�b�O�ŃE�F�C�|�C���g��`�悷��ۂ̃��f���f�[�^
	bool isActive_;		// ���̃E�F�C�|�C���g���L��������Ă��邩�B

public:

	static const int RADIUS = 500;	// �E�F�C�|�C���g�ƎԂ̓����蔻����s���ۂ̃T�C�Y


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