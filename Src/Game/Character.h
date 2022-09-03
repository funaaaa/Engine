#pragma once
#include "Vec.h"
#include "ConstBuffers.h"
#include <vector>
#include "PlayerModel.h"
#include <memory>

class BaseStage;
class OBB;
class PlayerTire;
class BaseItem;
class BaseOperationObject;
class BaseStage;

class Character {

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
	float rotY_;			// �n���h������ɂ���ĕς��Y���̉�]��
	const float DEF_ROTY = -0.367411435f;
	float shellHitRot_;		// �b���ɓ��������Ƃ��̉�]�B
	DirectX::XMMATRIX defBodyMatRot_;	// ���̃t���[���̃f�t�H���g�̉�]�s��
	int returnDefPosTimer_;	// �f�t�H���g�̈ʒu�ɖ߂�܂ł̎��� �ޗ��ɗ��������p
	int charaIndex_;
	int canNotMoveTimer_;	// ����s�\�̃^�C�}�[
	const int CAN_NOT_MOVE_TIMER_SHELL_HIT = 60;
	const int RETURN_DEFPOS_TIMER = 600;
	bool isShotBehind_;		// ��둤�ɍb���𓊂��邩�̃t���O
	bool onGround_;			// �n��ɂ��邩 t=�n�� f=��
	bool onGroundPrev_;		// �O�t���[����onGround_
	bool onGrass_;			// ���̏�ɂ��邩 t=���̏� f=���̏ザ��Ȃ�
	bool isConcentrationLine_;	// �W�������o�����t���O�B
	bool isUseItem_;	// �A�C�e�����g�����u�ԁB
	bool isJumpActionTrigger_;	// �W�����v�A�N�V�����̃g���K�[
	bool isJumpAction_;			// �W�����v�A�N�V�������s��ꂽ���B

	std::shared_ptr<OBB> obb_;	// �����蔻��pOBB

	std::vector<std::shared_ptr<PlayerTire>> tires_;

	bool isGetItem_;	// �A�C�e�����擾�����t���[���̔���

	// ����I�u�W�F�N�g
	std::shared_ptr<BaseOperationObject> operationObject_;

	// �A�C�e���N���X
	std::shared_ptr<BaseItem> item_;

	bool IsTurningIndicatorRed_;// �E�C���J�[�̐F���Ԃ��ǂ����B
	int turningIndicatorTimer_;	// �E�C���J�[���`�J�`�J����^�C�}�[
	const int TURNING_INDICATOR_TIMER = 30;

	const float MAX_SPEED = 16.0f;		// �ړ����x�̍ő�l
	const float MAX_SPEED_ON_GRASS = 12.0f;// ���̏�ɂ���Ƃ��̍ő呬�x
	const float ADD_SPEED = 2.0f;		// �ړ����x�̉��Z��
	const float HANDLE_NORMAL = 0.03f;	// �ʏ펞�̃n���h�����O�̊p�x
	const float MAX_GRAV = 8.0f;		// �d�͂̍ő��
	const float ADD_GRAV = 0.4f;		// �d�͂̉��Z��
	const Vec3 PLAYER_DEF_POS = Vec3(0, 30, -30);


	/*-- �h���t�g�A�������̎ԑ̂̉�]�Ɋւ���ϐ� --*/

	DirectX::XMVECTOR handleRotQ_;		// �n���h���̉�]��
	DirectX::XMVECTOR nowHandleRotQ_;	// �n���h���̉�]��
	DirectX::XMVECTOR boostRotQ_;		// �������̐��ʕ����ւ̉�]�̍s��
	DirectX::XMVECTOR nowBoostRotQ_;	// �������̐��ʕ����ւ̉�]�̍s��
	float handleAmount_;				// �n���h����
	const float  MAX_DRIFT_ROT = 1.6f;
	float baseDriftRot_;
	float nowDriftRot_;
	float baseBoostRot_;
	float nowBoostRot_;
	float tireLollingAmount_;
	int forwardTireLollingTimer_;
	const int FORWARD_TIMER_LOLLING_TIMER = 20;
	int driftRotTimer_;
	const int MAX_DRIFT_ROT_TIMER = 10;
	bool isRotRightSide_ = false;


	/*-- ���f���̃f�[�^�Ɋւ���ϐ� --*/

	PlayerModel playerModel_;


	/*-- �h���t�g�Ɋւ���ϐ� --*/

	Vec3 driftJumpVec_;
	float driftJumpSpeed_;
	const float DRIFT_JUMP_SPEED = 6.0f;
	const float SUB_DRIFT_JUMP_SPEED = 0.6f;
	float boostSpeed_;					// �u�[�X�g����Ă���Ƃ��̈ړ����x
	int driftTimer_;
	int boostTimer_;					// �u�[�X�g����t���[����
	bool isDriftRight_;					// �h���t�g���E�����ǂ����B
	bool isInputLTPrev_;				// �O�t���[����LT�������ꂽ���ǂ���
	bool isInputLT_;					// LT�������ꂽ���ǂ����B
	bool isDriftJump_;					// �h���t�g�O�̃W�����v�����ǂ����B
	bool isDriftJumpPrev_;				// �h���t�g�O�̃W�����v�����ǂ����̑OF�t���O�B
	bool isDrift_;						// �h���t�g��Ԃ��ǂ����B
	bool isTireMask_;

	const std::array<int, 3> DRIFT_TIMER = { 30,90,160 };
	const std::array<int, 3> DRIFT_BOOST = { 10,20,30 };


	/*-- �W�����v�̃u�[�X�g�Ɋւ���ϐ� --*/

	float jumpBoostSpeed_;
	const float JUMP_BOOST_SPEED = 10.0f;
	const float SUB_JUMP_BOOST_SPEED = 0.2f;


	/*-- �G���W���p�ϐ� --*/

	float engineWaveTimer_;	// �J�n�O�ɃT�C���g�̓������w����悤�̃^�C�}�[	
	float engineWaveAmount_;


	/*-- �J�n�O�p�ϐ� --*/

	bool isAccel_;
	bool isBeforeStartPrev_;
	int beforeStartSmokeTimer_;
	const int BEFORE_START_SMOKE_TIMER = 5;
	const float BEFORE_START_WAVE_LENGTH_RUN = 0.05f;
	const float BEFORE_START_WAVE_LENGTH_DEF = 0.3f;
	const float BEFORE_START_WAVE_LENGTH_ACCELL = 1.0f;


	/*-- �S�[�����o�p�ϐ� --*/

	DirectX::XMMATRIX gameFinishTriggerMatRot_;
	Vec3 gameFinishTruggerForardVec_;// �Q�[���I�����̐��ʃx�N�g��
	float gameFinishTriggerRotY_;	// �Q�[�����I������u�Ԃ̉�]��
	float gameFinishEasingTimer_;	// �Q�[���I�������o�Ɏg�p����C�[�W���O�^�C�}�[
	float gameFinishRotStopAmount_;	// ���S�ɒ�~�����ۂ̉�]�ʁB
	float gameFinishRotStopReturnAmount_;	// ���S�ɒꂵ���ۂɌ��̈ʒu�ɖ߂����߂̉�]�ʁB
	const float GAME_FINISH_EASING_TIMER = 0.01f;
	const float GAME_FINISH_STOP_ROT = 2.0f;
	const float GAME_FINISH_STOP_ROT_LIMIT = 0.3f;
	bool isGameFinish_;				// �Q�[�����I������t���O
	bool isPrevGameFinish_;			// �O�t���[���̃Q�[�����I������t���O
	bool isGameFinishDriftLeft_;	// �Q�[���I�����o�łǂ���Ƀh���t�g���邩�B t:�� f:�E


public:

	struct TireUVSet {
		Vec2 uv_;
		Vec2 prevuv_;
	};

	// �^�C�����������ݗp
	struct TireMaskUV {
		TireUVSet forwardLeftUV_;
		TireUVSet forwardRightUV_;
		TireUVSet behindLeftUV_;
		TireUVSet behindRightUV_;
	};

private:

	TireMaskUV tireMaskUV_;				// �^�C�������o���ۂɎd�l

	const float HANDLE_DRIFT = 0.05f;	// �h���t�g���̃n���h�����O�̊p�x
	const float MAX_BOOST_SPEED = 15.0f;// �u�[�X�g�̈ړ��ʂ̍ő�l
	const float SUB_BOOST_SPEED = 0.2f;	// �u�[�X�g�̈ړ��ʂ̌��c��


public:

	enum class CHARA_ID {

		P1,	// �v���C���[1
		P1_WGHOST,	// �v���C���[1�S�[�X�g�������ݗL��
		AI1,	// AI1
		GHOST,	// �S�[�X�g

	};


private:

	CHARA_ID charaID_;


public:

	/*===== �����o�֐� =====*/

	// ����������
	Character(CHARA_ID CharaID, const int& CharaIndex, const int& Param);
	void Init();

	// �X�V����
	void Update(std::weak_ptr<BaseStage> StageData, RayConstBufferData& ConstBufferData, bool& IsPassedMiddlePoint, int& RapCount, const bool& IsBeforeStart, const bool& IsGameFinish);

	// �`�揈��
	void Draw();

	// �^�C�������o
	bool CheckTireMask(std::weak_ptr<BaseStage> BaseStageData, TireMaskUV& TireMaskUVData);


	const Vec3& GetPos() { return pos_; }
	const Vec3& GetPrevPos() { return prevPos_; }
	Vec3 GetCameraForwardVec();
	const Vec3& GetForwardVec() { return forwardVec_; }
	Vec3 GetUpVec() { return upVec_; };
	float GetNowSpeedPer();
	bool GetIsGetItem() { return isGetItem_; }	// �A�C�e�����擾�����u��
	bool GetIsItem();	// �A�C�e���������Ă��邩�B
	bool GetUseItem() { return isUseItem_; }	// �A�C�e�����g�����u�ԁB
	bool GetIdConcentrationLine() { return isConcentrationLine_; }

	// ���f�����폜�B
	void DeleteInstance();

	// �f�o�b�O�p
	bool IsP1() { return charaID_ == CHARA_ID::P1; }

private:

	// ���͏���
	void Input(RayConstBufferData& ConstBufferData, const bool& IsBeforeStart);

	// �ړ�����
	void Move(const bool& IsBeforeStart);

	// �h���t�g�Ɋւ���X�V����
	void UpdateDrift(const bool& IsBeforeStart);

	// �����蔻��
	void CheckHit(std::weak_ptr<BaseStage> StageData, bool& IsPassedMiddlePoint, int& RapCount);

	// �ԑ̌X���̏���
	void InclineCarBody();

	// �G���W���̏㉺
	void EngineSineWave();

	// �Q�[���I�����̍X�V����
	void UpdateGameFinish();

	// �h���t�g�p�[�e�B�N���̍X�V����
	void UpdateDriftParticle(RayConstBufferData& ConstBufferData, const bool& IsGameFinish, const bool& IsBeforeStart);

};