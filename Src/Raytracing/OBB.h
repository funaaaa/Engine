#pragma once
#include "Vec.h"
#include <array>

class OBB
{

public:

	/*===== �����o�ϐ� =====*/

	Vec3 pos;					// �ʒu
	std::array<Vec3, 3> dir;	// �����x�N�g��
	Vec3 length;				// �e�������̒���
	Vec3 defLength;				// �f�t�H���g�̊e�������̒���

	// �f�o�b�O�p
	int blasIndex;
	int insIndex;


public:

	/*===== �����o�֐� =====*/

	// OBB�𐶐�
	void Setting(const int& BlasIndex, const int& InsIndex);

	// InstanceID���w�肵�Ċe��s���ݒ�B
	void SetMat(const int& InsIndex);

	// OBB�Ƃ̓����蔻��
	bool CheckHitOBB(OBB TargetOBB);

	inline const std::array<Vec3, 3>& GetDir() { return dir; };		// �w�莲�ԍ��̕����x�N�g�����擾
	inline const Vec3 GetLength() { return length; };				// �w�莲�����̒������擾
	inline const Vec3& GetPos() { return pos; };					// �ʒu���擾

private:

	// �������ɓ��e���ꂽ���������瓊�e���������Z�o
	float LenSegOnSeparateAxis(Vec3 Sep, Vec3 E1, Vec3 E2, Vec3 E3 = 0);

};