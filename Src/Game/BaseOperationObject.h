#pragma once
#include "Vec.h"
#include "BaseItem.h"

// ����I�u�W�F�N�g���N���X ������p�������N���X��Character�ɃZ�b�g���ĎԂ𓮂����B
class BaseOperationObject {

protected:

	/*===== �����o�ϐ� =====*/


public:

	enum class OPERATION_ID {

		P1,			// �v���C���[1P
		NORMAL_AI,	// �ʏ��AI
		GHOST,		// �S�[�X�g

	};

	struct Operation {

		float accelerationRate_;	// �����̊���
		float handleDriveRate_;		// ���E�̃n���h������̊��� -1 ~ 1
		bool isDrift_;				// �h���t�g��Ԃ�
		bool isDriftTrigger_;		// �h���t�g��Ԃ̃g���K�[��
		bool isShotBehind_;			// �A�C�e�������ɓ������Ԃ��B
		bool isUseItemTrigger_;		// �A�C�e�����g�p���邩�B
		bool isUseItemRelease_;		// �A�C�e�����g�p���邩�B
		bool isGetItem_;			// �A�C�e�����擾�������ǂ���

		Operation() : accelerationRate_(0), handleDriveRate_(0), isDrift_(false), isShotBehind_(false), isUseItemTrigger_(false), isUseItemRelease_(false), isGetItem_(false), isDriftTrigger_(false) {};
		bool CheckInput() { return accelerationRate_ != 0 || handleDriveRate_ != 0 || isDrift_ || isShotBehind_ || isUseItemTrigger_ || isUseItemRelease_ || isGetItem_ || isDriftTrigger_; }

	};

	// AI����������肷��ۂɕK�v�ȏ��B
	struct OperationInputData
	{

		Vec3 pos_;					// ���݂̃L�����N�^�[�̍��W
		Vec3 forwradVec_;			// �L�����N�^�[�̈ړ������x�N�g��
		bool isHitJumpBoostGimmick_;// �i���W�����v�M�~�b�N�ɂ������Ă��邩�ǂ���
		bool isPrevFrameDrift_;		// �O�t���[���Ƀh���t�g���Ă������B
		BaseItem::ItemID hasItemID_;// �ێ����Ă���A�C�e����ID

		OperationInputData() :pos_({}), forwradVec_(Vec3(0, 0, 1)), isHitJumpBoostGimmick_(false), isPrevFrameDrift_(false), hasItemID_(BaseItem::ItemID::NONE) {};

	};


public:

	/*===== �����o�֐� =====*/

	virtual Operation Input(const OperationInputData& InputData) = 0;

	virtual void OnGoal() = 0;

};