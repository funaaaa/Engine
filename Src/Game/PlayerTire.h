#pragma once
#include "Vec.h"

class PlayerTire{

private:

	/*===== �����o�ϐ� =====*/

	int tireInsIndex_;	// �^�C���̃C���X�^���X�̃C���f�b�N�X
	Vec3 pos_;			// ���W
	Vec3 prevPos_;		// �O�t���[���̍��W
	Vec3 rot_;			// ��]��
	bool isBehindTire_;	// ��ւ��ǂ���

	
public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	PlayerTire(const int& TireInsIndex, const bool& IsBehindTire);

	// �X�V����
	void Update();

	// ��]����
	void Rot(const bool& IsDrift, const float& Rate);

};