#pragma once
#include "Vec.h"

class Player {

private:

	/*===== �����o�ϐ� =====*/

	int carBlasIndex;		// �Ԃ̃��f����BLAS�̃C���f�b�N�X
	int carInstanceIndex;	// �Ԃ̃��f���̃C���X�^���X�̃C���f�b�N�X
	int stageBlasIndex;		// �X�e�[�W��BLAS�̃C���f�b�N�X
	int stageInstanceIndex;	// �X�e�[�W�̃C���X�^���X�̃C���f�b�N�X
	Vec3 pos;				// �Ԃ̍��W
	Vec3 forwardVec;		// �i�s�����x�N�g��
	Vec3 bottomVec;			// �������x�N�g��
	Vec3 upVec;				// ������x�N�g��
	Vec3 size;				// �T�C�Y
	float speed;			// �ړ����x
	float gravity;			// �d��
	float rotY;				// �n���h������ɂ���ĕς��Y���̉�]��
	bool isGround;			// �n��ɂ��邩 t=�n�� f=��

	const float MAX_SPEED = 10.0f;		// �ړ����x�̍ő�l
	const float ADD_SPEED = 2.0f;		// �ړ����x�̉��Z��
	const float HANDLE_NORMAL = 0.03f;	// �ʏ펞�̃n���h�����O�̊p�x
	const float MAX_GRAV = 5.0f;		// �d�͂̍ő��
	const float ADD_GRAV = 0.05f;		// �d�͂̉��Z��



	/*-- �h���t�g�Ɋւ���ϐ� --*/

	float boostSpeed;					// �u�[�X�g����Ă���Ƃ��̈ړ����x
	int driftBoostTimer;				// �h���t�g�Ńu�[�X�g����܂ł̃^�C�}�[
	bool isDrift;						// �h���t�g��Ԃ��ǂ����B

	const float HANDLE_DRIFT = 0.06f;	// �h���t�g���̃n���h�����O�̊p�x
	const float MAX_BOOST_SPEED = 20.0f;// �u�[�X�g�̈ړ��ʂ̍ő�l
	const float SUB_BOOST_SPEED = 0.2f;	// �u�[�X�g�̈ړ��ʂ̌��c��
	const int DRIFT_BOOST_TIMER = 30;	// �h���t�g�Ńu�[�X�g����܂ł̃^�C�}�[


public:

	/*===== �����o�֐� =====*/

	// ����������
	void Init(const int& StageBlasIndex, const int& StageInstanceIndex);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	const Vec3& GetPos() { return pos; }
	const Vec3& GetForwardVec() { return forwardVec; }
	Vec3 GetUpVec() { return upVec; };
	float GetNowSpeedPer();

private:

	// ���͏���
	void Input();

	// �ړ�����
	void Move();

	// �����蔻��
	void CheckHit();

	// �΂ߏ��̉�]
	void RotObliqueFloor(const Vec3& HitNormal);

};