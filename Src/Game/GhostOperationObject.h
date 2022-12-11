#pragma once
#include "BaseOperationObject.h"
#include <string>
#include <vector>

// �S�[�X�g�̑���I�u�W�F�N�g�N���X�B
class GhostOperationObject : public BaseOperationObject {

private:

	/*===== �����o�ϐ� =====*/

	std::string filePath_;
	int frameTimer_;		// �n�܂��Ă���̃t���[�����v������p�^�C�}�[
	int operationCurrentIndex_;	// ���݂̖��߂̃C���f�b�N�X�B

	// ���߂��e�L�X�g�ɓ��o�͂���ۂɕK�v�Ȏ��ʎq
	enum class OPERATION_ID {
		NONE,
		ACCEL,			// �A�N�Z���̏��ł��B
		HANDLE,			// �n���h���̏��ł��B
		DRIFT,			// �h���t�g�̏��ł��B
		GETITEM,		// Item���擾�������̏��ł��B
		USETRIGGER,		// Item���g�p�����g���K�[���ǂ����̏��ł��B
		USERELEASE,		// Item���g�p���������[�X���ǂ����̏��ł��B
		DRIFTTRIGGER,	// �h���t�g�̃g���K�[���ǂ����̏��ł��B
	};
	// ���߂�ID�Ƃ��̃p�����[�^�[��ۑ����邽�߂̍\����
	struct OperationElement {
		OPERATION_ID operationID_;
		float operationValue_;
		OperationElement() : operationID_(OPERATION_ID::NONE) {};
	};
	// ���߂̔z��
	struct OperationData {
		std::vector<OperationElement> operation_;
		int frameTimer_;
	};
	std::vector<OperationData> operationData_;

public:

	/*===== �����o�֐� =====*/

	GhostOperationObject(std::string GhostFilePath);

	BaseOperationObject::Operation Input(const BaseOperationObject::OperationInputData& InputData)override;

	void OnGoal()override;

};