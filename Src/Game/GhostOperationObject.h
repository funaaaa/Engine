#pragma once
#include "BaseOperationObject.h"
#include <string>
#include <vector>

class GhostOperationObject : public BaseOperationObject {

private:

	/*===== �����o�ϐ� =====*/

	std::string filePath_;
	int frameTimer_;		// �n�܂��Ă���̃t���[�����v������p�^�C�}�[
	int operationCurrentIndex_;	// ���݂̖��߂̃C���f�b�N�X�B

	enum class OPERATION_ID {
		NONE,
		ACCEL,
		HANDLE,
		DRIFT,
		GETITEM,
		USETRIGGER,
		USERELEASE,
		JUMPACTIONTRIGGER,
	};
	struct OperationElement {
		OPERATION_ID operationID_;
		float operationValue_;
		OperationElement() : operationID_(OPERATION_ID::NONE) {};
	};
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