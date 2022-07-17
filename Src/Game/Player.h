#pragma once
#include "Vec.h"
#include "ConstBuffers.h"
#include <vector>
#include "PlayerModel.h"
#include <memory>

class BaseStage;
class OBB;
class PlayerTire;

class Player {

private:

	/*===== �����o�ϐ� =====*/
public:

	Vec3 pos_;				// �Ԃ̍��W
	Vec3 prevPos_;			// �Ԃ̑O�t���[���̍��W
	Vec3 forwardVec_;		// �i�s�����x�N�g��
	Vec3 bottomVec;			// �������x�N�g��
	Vec3 upVec_;			// ������x�N�g��
	Vec3 size_;				// �T�C�Y
	float speed_;			// �ړ����x
	float gravity_;			// �d��
	float rotY_;				// �n���h������ɂ���ĕς��Y���̉�]��
	int returnDefPosTimer_;	// �f�t�H���g�̈ʒu�ɖ߂�܂ł̎��� �ޗ��ɗ��������p
	const int RETURN_DEFPOS_TIMER = 600;
	bool isGround_;			// �n��ɂ��邩 t=�n�� f=��
	bool isGrass_;			// ���̏�ɂ��邩 t=���̏� f=���̏ザ��Ȃ�

	std::shared_ptr<OBB> obb_;	// �����蔻��pOBB

	std::vector<std::shared_ptr<PlayerTire>> tires_;

	bool IsTurningIndicatorRed_;// �E�C���J�[�̐F���Ԃ��ǂ����B
	int turningIndicatorTimer_;	// �E�C���J�[���`�J�`�J����^�C�}�[
	const int TURNING_INDICATOR_TIMER = 30;

	const float MAX_SPEED = 16.0f;		// �ړ����x�̍ő�l
	const float MAX_SPEED_ON_GRASS = 8.0f;// ���̏�ɂ���Ƃ��̍ő呬�x
	const float ADD_SPEED = 2.0f;		// �ړ����x�̉��Z��
	const float HANDLE_NORMAL = 0.03f;	// �ʏ펞�̃n���h�����O�̊p�x
	const float MAX_GRAV = 7.0f;		// �d�͂̍ő��
	const float ADD_GRAV = 0.05f;		// �d�͂̉��Z��
	const Vec3 PLAYER_DEF_POS = Vec3(0, 30, -30);


	/*-- ���f���̃f�[�^�Ɋւ���ϐ� --*/

	PlayerModel playerModel_;


	/*-- �h���t�g�Ɋւ���ϐ� --*/

	float boostSpeed_;					// �u�[�X�g����Ă���Ƃ��̈ړ����x
	int driftBoostTimer_;				// �h���t�g�Ńu�[�X�g����܂ł̃^�C�}�[
	bool isDrift_;						// �h���t�g��Ԃ��ǂ����B

	const float HANDLE_DRIFT = 0.06f;	// �h���t�g���̃n���h�����O�̊p�x
	const float MAX_BOOST_SPEED = 20.0f;// �u�[�X�g�̈ړ��ʂ̍ő�l
	const float SUB_BOOST_SPEED = 0.2f;	// �u�[�X�g�̈ړ��ʂ̌��c��
	const int DRIFT_BOOST_TIMER = 30;	// �h���t�g�Ńu�[�X�g����܂ł̃^�C�}�[


public:

	/*===== �����o�֐� =====*/

	// ����������
	Player();
	void Init();

	// �X�V����
	void Update(std::weak_ptr<BaseStage> StageData, RayConstBufferData& ConstBufferData, bool& IsPassedMiddlePoint, int& RapCount);

	// �`�揈��
	void Draw();

	const Vec3& GetPos() { return pos_; }
	const Vec3& GetForwardVec() { return forwardVec_; }
	Vec3 GetUpVec() { return upVec_; };
	float GetNowSpeedPer();

private:

	// ���͏���
	void Input(RayConstBufferData& ConstBufferData);

	// �ړ�����
	void Move();

	// �����蔻��
	void CheckHit(std::weak_ptr<BaseStage> StageData, bool& IsPassedMiddlePoint, int& RapCount);

	// �΂ߏ��̉�]
	void RotObliqueFloor(const Vec3& HitNormal);

};