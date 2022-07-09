#pragma once
#include "Vec.h"
#include "ConstBuffers.h"
#include <vector>
#include "OBB.h"


class Player {


public:

	// �X�e�[�W�̃��f���f�[�^
	struct StageData {

		int stageBlasIndex;
		int stageInsIndex;
		int stageGrassBlasIndex;
		int stageGrassInsIndex;
		int goalBlasIndex;
		int goalInsIndex;
		int middlePointBlasIndex;
		int middlePointInsIndex;
		std::vector<int> stageOrnamentBlasIndex;
		std::vector<int> stageOrnamentInsIndex;

	};

private:

	/*===== �����o�ϐ� =====*/
public:

	Vec3 pos;				// �Ԃ̍��W
	Vec3 prevPos;			// �Ԃ̑O�t���[���̍��W
	Vec3 forwardVec;		// �i�s�����x�N�g��
	Vec3 bottomVec;			// �������x�N�g��
	Vec3 upVec;				// ������x�N�g��
	Vec3 size;				// �T�C�Y
	float speed;			// �ړ����x
	float gravity;			// �d��
	float rotY;				// �n���h������ɂ���ĕς��Y���̉�]��
	int returnDefPosTimer;	// �f�t�H���g�̈ʒu�ɖ߂�܂ł̎��� �ޗ��ɗ��������p
	const int RETURN_DEFPOS_TIMER = 600;
	bool isGround;			// �n��ɂ��邩 t=�n�� f=��
	bool isGrass;			// ���̏�ɂ��邩 t=���̏� f=���̏ザ��Ȃ�

	OBB obb;	// �����蔻��pOBB
	OBB testOBB;
	bool test;

	const float MAX_SPEED = 16.0f;		// �ړ����x�̍ő�l
	const float MAX_SPEED_ON_GRASS = 8.0f;// ���̏�ɂ���Ƃ��̍ő呬�x
	const float ADD_SPEED = 2.0f;		// �ړ����x�̉��Z��
	const float HANDLE_NORMAL = 0.03f;	// �ʏ펞�̃n���h�����O�̊p�x
	const float MAX_GRAV = 7.0f;		// �d�͂̍ő��
	const float ADD_GRAV = 0.05f;		// �d�͂̉��Z��
	const Vec3 PLAYER_DEF_POS = Vec3(0, 30, -30);


	/*-- ���f���̃f�[�^�Ɋւ���ϐ� --*/
	int carBlasIndex;		// �Ԃ̃��f����BLAS�̃C���f�b�N�X
	int carInstanceIndex;	// �Ԃ̃��f���̃C���X�^���X�̃C���f�b�N�X
	StageData stageModelData;	// �X�e�[�W�̃��f���f�[�^


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
	Player(const StageData& StageObjectData);
	void Init();

	// �X�V����
	void Update(RayConstBufferData& ConstBufferData, bool& IsPassedMiddlePoint, int& RapCount);

	// �`�揈��
	void Draw();

	const Vec3& GetPos() { return pos; }
	const Vec3& GetForwardVec() { return forwardVec; }
	Vec3 GetUpVec() { return upVec; };
	float GetNowSpeedPer();

private:

	// ���͏���
	void Input(RayConstBufferData& ConstBufferData);

	// �ړ�����
	void Move();

	// �����蔻��
	void CheckHit(bool& IsPassedMiddlePoint, int& RapCount);

	// �΂ߏ��̉�]
	void RotObliqueFloor(const Vec3& HitNormal);

};