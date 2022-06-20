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



	/*-- �h���t�g�Ɋւ���ϐ� --*/

	float boostSpeed;					// �u�[�X�g����Ă���Ƃ��̈ړ����x
	int driftBoostTimer;				// �h���t�g�Ńu�[�X�g����܂ł̃^�C�}�[
	bool isDrift;						// �h���t�g��Ԃ��ǂ����B

	const float HANDLE_DRIFT = 0.1f;	// �h���t�g���̃n���h�����O�̊p�x
	const float MAX_BOOST_SPEED = 20.0f;// �u�[�X�g�̈ړ��ʂ̍ő�l
	const float SUB_BOOST_SPEED = 0.2f;	// �u�[�X�g�̈ړ��ʂ̌��c��
	const int DRIFT_BOOST_TIMER = 30;	// �h���t�g�Ńu�[�X�g����܂ł̃^�C�}�[


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
	float GetNowSpeedPer();

private:

	// ���͏���
	void Input();

	// �ړ�����
	void Move();

};