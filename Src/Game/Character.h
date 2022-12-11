#pragma once
#include "Vec.h"
#include "ConstBuffers.h"
#include "PlayerModel.h"
#include "CharacterTireMask.h"
#include "FHelper.h"
#include <vector>
#include <memory>

class BaseStage;
class OBB;
class PlayerTire;
class BaseItem;
class BaseOperationObject;
class PolygonMeshInstance;
class BaseStage;
class CharacterInclineBody;
class CharacterGameFinish;
class CharacterDrift;
class CharacterRocket;
class CharacterTireMask;
struct CharacterFlags;

// �L�����N�^�[�N���X�B������Ǘ����鑀��I�u�W�F�N�g�����ւ��邱�Ƃɂ����AI�ƃv���C���[��؂�ւ���B
class Character {

private:

	/*===== �����o�ϐ� =====*/
public:

	Vec3 pos_;								// �Ԃ̍��W
	Vec3 prevPos_;							// �Ԃ̑O�t���[���̍��W
	Vec3 forwardVec_;						// �i�s�����x�N�g��
	Vec3 cameraForwardVec_;					// �b���ɂ��������u�Ԃ̐��ʃx�N�g��
	Vec3 bottomVec;							// �������x�N�g��
	Vec3 upVec_;							// ������x�N�g��
	Vec3 size_;								// �T�C�Y
	float boostSpeed_;						// �u�[�X�g����Ă���Ƃ��̈ړ����x
	int boostTimer_;						// �u�[�X�g����t���[����
	const int BOOST_GIMMICK_BOOST_TIMER = 20;
	float speed_;							// �ړ����x
	float gravity_;							// �d��
	float rotY_;							// �n���h������ɂ���ĕς��Y���̉�]��
	const float DEF_ROTY = -0.367411435f;
	float shellHitRot_;						// �b���ɓ��������Ƃ��̉�]�B
	DirectX::XMMATRIX defBodyMatRot_;		// ���̃t���[���̃f�t�H���g�̉�]�s��
	int returnDefPosTimer_;					// �f�t�H���g�̈ʒu�ɖ߂�܂ł̎��� �ޗ��ɗ��������p
	int charaIndex_;
	int canNotMoveTimer_;					// ����s�\�̃^�C�}�[
	int shellIndex_;
	int timerToSkipShellCollider_;			// �b���̓����蔻��𖳌�������^�C�}�[(�b�����˂�����ɒ����Ɏ����ɓ�����Ȃ��悤�ɂ��邽��)
	std::shared_ptr<CharacterFlags> flags_;	// �L�����N�^�[�Ŏg�p����t���O���܂Ƃ߂��\����
	const int TIMER_TO_SKIP_SHELL_COLLIDER = 60;
	const int CAN_NOT_MOVE_TIMER_SHELL_HIT = 60;
	const int RETURN_DEFPOS_TIMER = 600;

	// �����蔻��pOBB
	std::shared_ptr<OBB> obb_;

	// �ʂȎԂƂ̓����蔻��p�̃q�b�g�{�b�N�X
	std::weak_ptr<BLAS> hitBoxBlas_;
	std::weak_ptr<PolygonMeshInstance> hitBox_;

	std::vector<std::shared_ptr<PlayerTire>> tires_;


	// ����I�u�W�F�N�g
	std::shared_ptr<BaseOperationObject> operationObject_;

	// �A�C�e���N���X
	std::shared_ptr<BaseItem> item_;

	const float MAX_SPEED = 16.0f;			// �ړ����x�̍ő�l
	const float MAX_SPEED_ON_GRASS = 12.0f;	// ���̏�ɂ���Ƃ��̍ő呬�x
	const float ADD_SPEED = 2.0f;			// �ړ����x�̉��Z��
	const float HANDLE_DRIFT = 0.05f;		// �h���t�g���̃n���h�����O�̊p�x
	const float MAX_BOOST_SPEED = 15.0f;	// �u�[�X�g�̈ړ��ʂ̍ő�l
	const float SUB_BOOST_SPEED = 0.2f;		// �u�[�X�g�̈ړ��ʂ̌��c��
	const float HANDLE_NORMAL = 0.03f;		// �ʏ펞�̃n���h�����O�̊p�x
	const float MAX_GRAV = 8.0f;			// �d�͂̍ő��
	const float ADD_GRAV = 0.4f;			// �d�͂̉��Z��
	Vec3 defPos_ = Vec3(0, 30, -30);
	const Vec3 PLAYER_DEF_POS = Vec3(-141.943f, 30.0f, 100.0f);
	const std::array<Vec3, 3> GHOST_DEF_POS = { Vec3(-59, 30, -106), Vec3(99, 30, -34), Vec3(23.775f, 30.0f, 166.0f) };


	/*-- �h���t�g�A�������̎ԑ̂̉�]�Ɋւ���ϐ� --*/
	std::shared_ptr<CharacterInclineBody> inclineBody_;	// �L�����N�^�[����]������ۂɎg�p���鏈�����܂Ƃ߂��N���X


	/*-- �^�C�����Ɋւ��鏈�� --*/
	std::shared_ptr<CharacterTireMask> tireMask_;		// �^�C�����Ɋւ��鏈�����܂Ƃ߂��N���X


	/*-- ���f���̃f�[�^�Ɋւ���ϐ� --*/
	PlayerModel playerModel_;


	/*-- �h���t�g�Ɋւ���ϐ� --*/
	std::shared_ptr<CharacterDrift> drift_;				// �h���t�g�Ɋւ��鏈�����܂Ƃ߂��N���X


	/*-- �W�����v�̃u�[�X�g�Ɋւ���ϐ� --*/
	float jumpBoostSpeed_;
	const float JUMP_BOOST_SPEED = 10.0f;
	const float SUB_JUMP_BOOST_SPEED = 0.2f;


	/*-- �G���W���p�ϐ� --*/
	float engineWaveTimer_;		// �J�n�O�ɃT�C���g�̓������w����悤�̃^�C�}�[	
	float engineWaveAmount_;


	/*-- �J�n�O�p�ϐ� --*/
	const float BEFORE_START_WAVE_LENGTH_RUN = 0.05f;
	const float BEFORE_START_WAVE_LENGTH_DEF = 0.3f;
	const float BEFORE_START_WAVE_LENGTH_ACCELL = 1.0f;


	/*-- �S�[�����o�p�ϐ� --*/
	std::shared_ptr<CharacterGameFinish> gameFinish_;	// �L�����N�^�[�̃Q�[���I�����ɍs���������܂Ƃ߂��N���X


	/*-- �S�[���p --*/
	int rapCount_;
	bool isPassedMiddlePoint_;


	/*-- ���P�b�g�A�C�e���֌W --*/
	std::shared_ptr<CharacterRocket> rocket_;


public:

	enum class CHARA_ID {

		P1,	// �v���C���[1
		P1_WGHOST,	// �v���C���[1�S�[�X�g�������ݗL��
		AI1,	// AI1
		GHOST,	// �S�[�X�g

	};


public:

	CHARA_ID charaID_;


public:

	/*===== �����o�֐� =====*/

	// ����������
	Character(CHARA_ID CharaID, int CharaIndex, int Level, int CharaPersonality = 0);
	void Init();

	// �X�V����
	void Update(std::weak_ptr<BaseStage> StageData, std::vector<std::shared_ptr<Character>> CharaData, bool IsBeforeStart, bool IsGameFinish);

	// �`�揈��
	void Draw();

	// �^�C�������o
	bool CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, CharacterTireMask::TireMaskUV& TireMaskUVData);


	const Vec3& GetPos() { return pos_; }
	const Vec3& GetPrevPos() { return prevPos_; }
	Vec3 GetCameraForwardVec();
	const Vec3& GetForwardVec() { return forwardVec_; }
	Vec3 GetUpVec() { return upVec_; };
	float GetNowSpeedPer();
	bool GetIsGetItem();	// �A�C�e�����擾�����u��
	bool GetIsItem();	// �A�C�e���������Ă��邩�B
	bool GetUseItem();	// �A�C�e�����g�����u�ԁB
	bool GetIdConcentrationLine();
	int GetRapCount() { return rapCount_; }
	bool GetIsPassedMiddlePoint() { return isPassedMiddlePoint_; }

	// ���f�����폜�B
	void DeleteInstance();

	// �f�o�b�O�p
	bool IsP1() { return charaID_ == CHARA_ID::P1; }

private:

	// ���͏���
	void Input(bool IsBeforeStart);

	// �ړ�����
	void Move(bool IsBeforeStart);

	// �h���t�g�Ɋւ���X�V����
	void UpdateDrift();

	// �����蔻��
	void CheckHit(std::weak_ptr<BaseStage> StageData, std::vector<std::shared_ptr<Character>> CharaData);

	// �ԑ̌X���̏���
	void InclineCarBody();

	// �G���W���̏㉺
	void EngineSineWave();

	// �Q�[���I�����̍X�V����
	void UpdateGameFinish();

	// �h���t�g�p�[�e�B�N���̍X�V����
	void UpdateDriftParticle(bool IsGameFinish, bool IsBeforeStart);

	// �ԂƎԂ̉����߂�����
	void CheckHitOtherCar(std::vector<std::shared_ptr<Character>> CharaData);

	// �ԂƂ̓����蔻��
	void CheckHitCar(const FHelper::RayToModelCollisionData& CollisionData, std::weak_ptr<Character> IndexCharacter, float CheckHitSize);

	// ���P�b�g�Ɋւ���X�V����
	void UpdateRocket();

};