#pragma once
#include "Vec.h"
#include <memory>

class PolygonMeshInstance;

class PlayerTire{

private:

	/*===== �����o�ϐ� =====*/

	std::weak_ptr<PolygonMeshInstance> tireInstance;	// �^�C���̃C���X�^���X�̃C���f�b�N�X
	Vec3 pos_;			// ���W
	Vec3 prevPos_;		// �O�t���[���̍��W
	Vec3 rot_;			// ��]��
	bool isBehindTire_;	// ��ւ��ǂ���

	
public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	PlayerTire(std::weak_ptr<PolygonMeshInstance> TireInstance, bool IsBehindTire);

	// �X�V����
	void Update();

	// ��]����
	void Rot(bool IsDrift, float Rate);

};