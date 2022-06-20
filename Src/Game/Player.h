#pragma once
#include "Vec.h"

class Player {

private:

	/*===== �����o�ϐ� =====*/

	int carInstanceIndex;	// �Ԃ̃��f���̃C���X�^���X�̃C���f�b�N�X
	Vec3 pos;				// �Ԃ̍��W
	Vec3 forwardVec;		// �i�s�����x�N�g��
	float speed;			// �ړ����x

	const float MAX_SPEED = 10.0f;		// �ړ����x�̍ő�l
	const float ADD_SPEED = 2.0f;		// �ړ����x�̉��Z��
	const float HANDLE_NORMAL = 0.05f;	// �ʏ펞�̃n���h�����O�̊p�x


public:

	/*===== �����o�֐� =====*/

	// ����������
	void Init();

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	const Vec3& GetPos() { return pos; }
	const Vec3& GetForwardVec() { return forwardVec; }
	Vec3 GetUpVec();
	float GetNowSpeedPer() { return speed / MAX_SPEED; }

private:

	// ���͏���
	void Input();

	// �ړ�����
	void Move();

};