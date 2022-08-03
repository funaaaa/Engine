#pragma once

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

		Operation() : accelerationRate_(0), handleDriveRate_(0), isDrift_(false), isShotBehind_(false) {};

	};


public:

	/*===== �����o�֐� =====*/

	virtual Operation Input() = 0;

};