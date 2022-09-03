#include "GhostOperationObject.h"
#include <fstream>
#include <iostream>
#include <sstream>

GhostOperationObject::GhostOperationObject(std::string GhostFilePath)
{

	/*===== �R���X�g���N�^ =====*/

	filePath_ = GhostFilePath;
	frameTimer_ = 0;
	operationCurrentIndex_ = 0;

	// ���߂�ǂݎ��B

	// �t�@�C�����J���B
	std::ifstream file(filePath_);

	// ���s�����o�B
	if (!file) {
		assert(0);
	}

	// 1�s��ǂݎ��B
	std::string cutLine;
	while (std::getline(file, cutLine, '\n')) {

		bool isCurrentFrame = false;

		// �ǂݍ���1�s�̐擪�ɂ��鐔�����t���[�����Ɣ�r����B
		std::istringstream line(cutLine);
		std::string cutElement;
		OperationData operationData;
		while (std::getline(line, cutElement, ' ')) {

			// �����܂łŕ�����𖽗ߒP�ʂŕ������邱�Ƃɐ��������B
			std::istringstream operationElement(cutElement);
			std::string operationString;
			OperationElement operationElementData;
			while (std::getline(operationElement, operationString, ':')) {

				// A��������A�N�Z���B
				if (operationString == "A") {
					operationElementData.operationID_ = OPERATION_ID::ACCEL;
					continue;
				}
				// H��������n���h������B
				if (operationString == "H") {
					operationElementData.operationID_ = OPERATION_ID::HANDLE;
					continue;
				}
				// D��������h���t�g�B
				if (operationString == "D") {
					operationElementData.operationID_ = OPERATION_ID::DRIFT;
					continue;
				}
				// G��������A�C�e���l���B
				if (operationString == "G") {
					operationElementData.operationID_ = OPERATION_ID::GETITEM;
					continue;
				}
				// UT��������A�C�e���g�p�g���K�[�B
				if (operationString == "UT") {
					operationElementData.operationID_ = OPERATION_ID::USETRIGGER;
					continue;
				}
				// UR��������A�C�e���g�p�����[�X�B
				if (operationString == "UR") {
					operationElementData.operationID_ = OPERATION_ID::USERELEASE;
					continue;
				}
				// JT��������W�����v�A�N�V�����g���K�[�����[�X�B
				if (operationString == "JT") {
					operationElementData.operationID_ = OPERATION_ID::JUMPACTIONTRIGGER;
					continue;
				}

				// operationElementData.operationID_��NONE�Ȃ̂ɂ����܂ŗ����Ƃ����炻��̓t���[�����Ȃ̂Ńt���[������ۑ�����B
				if (operationElementData.operationID_ == OPERATION_ID::NONE) {
					operationData.frameTimer_ = std::stof(operationString);
					continue;
				}

				// �����܂ŗ����琔���Ȃ̂ŁA������𐔎��ɕϊ�����B
				operationElementData.operationValue_ = std::stof(operationString);

				// ���߂�ۑ�����B
				operationData.operation_.emplace_back(operationElementData);

			}

		}

		// ���߂�ۑ��B
		operationData_.emplace_back(operationData);

	}

	// �t�@�C�������B
	file.close();

}

BaseOperationObject::Operation GhostOperationObject::Input(const BaseOperationObject::OperationInputData& InputData)
{

	/*===== ���͏��� =====*/

	BaseOperationObject::Operation operation;

	// ���̖��߂ƍ��̃t���[�������r����B
	if (operationData_[operationCurrentIndex_].frameTimer_ == frameTimer_) {

		for (auto& index : operationData_[operationCurrentIndex_].operation_) {

			switch (index.operationID_)
			{
			case GhostOperationObject::OPERATION_ID::NONE:
				break;
			case GhostOperationObject::OPERATION_ID::ACCEL:
				operation.accelerationRate_ = index.operationValue_;
				break;
			case GhostOperationObject::OPERATION_ID::HANDLE:
				operation.handleDriveRate_ = index.operationValue_;
				break;
			case GhostOperationObject::OPERATION_ID::DRIFT:
				operation.isDrift_ = index.operationValue_;
				break;
			case GhostOperationObject::OPERATION_ID::GETITEM:
				operation.isGetItem_ = index.operationValue_;
				break;
			case GhostOperationObject::OPERATION_ID::USETRIGGER:
				operation.isUseItemTrigger_ = index.operationValue_;
				break;
			case GhostOperationObject::OPERATION_ID::USERELEASE:
				operation.isUseItemRelease_ = index.operationValue_;
				break;
			case GhostOperationObject::OPERATION_ID::JUMPACTIONTRIGGER:
				operation.isJumpActionTrigger_ = index.operationValue_;
				break;
			default:
				break;
			}

		}

		// ���̖��߂�
		++operationCurrentIndex_;

		// ���߃C���f�b�N�X�����E�T�C�Y�𒴂��Ȃ��悤�ɂ���B
		if (static_cast<int>(operationData_.size()) <= operationCurrentIndex_) {

			operationCurrentIndex_ = static_cast<int>(operationData_.size()) - 1;

		}

	}

	// �o�߃t���[�����v�Z�B
	++frameTimer_;

	return operation;

}

void GhostOperationObject::OnGoal()
{
}
