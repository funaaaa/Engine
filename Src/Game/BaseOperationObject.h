#pragma once
#include "Vec.h"

// ����I�u�W�F�N�g���N���X
class BaseOperationObject {

protected:

	/*===== �����o�ϐ� =====*/


public:

	enum class OPERATION_ID {

		P1,			// �v���C���[1P
		NORMAL_AI,	// �ʏ��AI

	};

	struct Operation {

		float accelerationRate_;	// �����̊���
		float handleDriveRate_;		// ���E�̃n���h������̊��� -1 ~ 1
		bool isDrift_;				// �h���t�g��Ԃ�
		bool isShotBehind_;			// �A�C�e�������ɓ������Ԃ��B
		bool isUseItemTrigger_;		// �A�C�e�����g�p���邩�B
		bool isUseItemRelease_;		// �A�C�e�����g�p���邩�B

		Operation() : accelerationRate_(0), handleDriveRate_(0), isDrift_(false), isShotBehind_(false), isUseItemTrigger_(false), isUseItemRelease_(false) {};
		bool CheckInput() { return accelerationRate_ != 0 || handleDriveRate_ != 0 || isDrift_ || isShotBehind_ || isUseItemTrigger_ || isUseItemRelease_; }

	};

	struct OperationInputData
	{

		Vec3 pos_;
		Vec3 forwradVec_;

	};


public:

	/*===== �����o�֐� =====*/

	virtual Operation Input(const OperationInputData& InputData) = 0;

	virtual void OnGoal() = 0;

};