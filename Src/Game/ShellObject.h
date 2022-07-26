#pragma once
#include "Vec.h"
#include <memory>

class OBB;

class ShellObject {

private:

	/*===== �����o�ϐ� =====*/

	int blasIndex_;			// BLAS�̃C���f�b�N�X
	int insIndex_;			// INSTANCE�̃C���f�b�N�X
	Vec3 pos_;				// ���W
	Vec3 forwardVec_;		// �ړ������x�N�g��
	float speed_;			// �ړ����x
	bool isActive_;			// �L�����t���O

	std::shared_ptr<OBB> obb_;	// �����蔻��pOBB

	const float SPEED = 30.0f;


public:

	/*===== �����o�֐� =====*/

	// ��������
	void Generate(const Vec3& Pos, const Vec3& ForwardVec);

	// �X�V����
	void Update();

	bool GetIsActive() { return isActive_; }

};