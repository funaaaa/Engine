#pragma once
#include "Vec.h"
#include <array>
#include <memory>

class PolygonMeshInstance;

class OBB
{

public:

	/*===== �����o�ϐ� =====*/

	Vec3 pos_;					// �ʒu
	std::array<Vec3, 3> dir_;	// �����x�N�g��
	Vec3 length_;				// �e�������̒���
	Vec3 defLength_;				// �f�t�H���g�̊e�������̒���

	// �f�o�b�O�p
	int blasIndex_;
	int insIndex_;


public:

	/*===== �����o�֐� =====*/

	// OBB�𐶐�
	void Setting(const int& BlasIndex, std::weak_ptr<PolygonMeshInstance> Instance);

	// Instance���w�肵�Ċe��s���ݒ�B
	void SetMat(std::weak_ptr<PolygonMeshInstance> Instance);

	// OBB�Ƃ̓����蔻��
	bool CheckHitOBB(OBB TargetOBB);
	bool CheckHitOBB(std::weak_ptr<OBB> TargetOBB);

	inline const std::array<Vec3, 3>& GetDir() { return dir_; };		// �w�莲�ԍ��̕����x�N�g�����擾
	inline const Vec3 GetLength() { return length_; };				// �w�莲�����̒������擾
	inline const Vec3& GetPos() { return pos_; };					// �ʒu���擾

private:

	// �������ɓ��e���ꂽ���������瓊�e���������Z�o
	float LenSegOnSeparateAxis(Vec3 Sep, Vec3 E1, Vec3 E2, Vec3 E3 = 0);

};