#include "PlayerOperationObject.h"
#include "Input.h"
#include "Character.h"
#include <ostream>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include "FileName.h"

PlayerOperationObject::PlayerOperationObject(const int& ControllerIndex, const bool& IsWriteGhostInfo, Character* CharacterPointer)
{

	/*===== �R���X�g���N�^ =====*/

	controllerIndex_ = ControllerIndex;
	isWriteGhostInfo_ = IsWriteGhostInfo;
	character_ = CharacterPointer;

	// �S�[�X�g���L��������Ă�����B
	if (isWriteGhostInfo_) {

		// �������ރt�@�C�������擾�B
		DecideFileName();

		frameTimer_ = 0;

	}



}

BaseOperationObject::Operation PlayerOperationObject::Input(const BaseOperationObject::OperationInputData& InputData)
{

	/*===== ���͏��� =====*/

	BaseOperationObject::Operation operation;
	InputData;

	// �O�i�ɂ��Ă̓��͏���
	operation.accelerationRate_ = Input::Ins()->PadTrigger(XINPUT_TRIGGER_RIGHT);
	if (Input::Ins()->IsKey(DIK_W)) {
		operation.accelerationRate_ = 1.0f;
	}

	// �A�C�e�������Ɍ����̓��͏���
	if (Input::Ins()->IsKey(DIK_O)) {
		operation.isShotBehind_ = true;
	}
	// �A�C�e�����g�p���邩�̓��͏���
	if (Input::Ins()->IsKeyTrigger(DIK_P)) {
		operation.isUseItemTrigger_ = true;
	}
	// �A�C�e�����g�p���邩�̓��͏���
	if (Input::Ins()->IsKeyRelease(DIK_P)) {
		operation.isUseItemRelease_ = true;
	}

	// ���E�̃n���h������ɂ��Ă̓��͏���
	operation.handleDriveRate_ = Input::Ins()->PadStick(XINPUT_THUMB_LEFTSIDE);
	// �f�b�h���C����݂���B
	if (std::fabs(operation.handleDriveRate_) < 0.1f) operation.handleDriveRate_ = 0.0f;
	if (Input::Ins()->IsKey(DIK_D)) {
		operation.handleDriveRate_ = 1.0f;
	}
	if (Input::Ins()->IsKey(DIK_A)) {
		operation.handleDriveRate_ = -1.0f;
	}

	// �h���t�g�ɂ��Ă̓��͏������s���B
	operation.isDrift_ = 0.9f < Input::Ins()->PadTrigger(XINPUT_TRIGGER_LEFT);
	if (Input::Ins()->IsKey(DIK_LSHIFT)) {
		operation.isDrift_ = true;
	}

	// �A�C�e�������肳��Ă�����B
	if (character_->GetIsGetItem()) {
		operation.isGetItem_ = true;
	}

	// �W�����v�A�N�V�����̃g���K�[����B
	if (Input::Ins()->IsKeyTrigger(DIK_SPACE) || Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		operation.isJumpActionTrigger_ = true;
	}

	// �S�[�X�g���̏������݂��L��������Ă�����B
	if (isWriteGhostInfo_) {

		WriteGhostInfo(operation);

		++frameTimer_;

	}


	return operation;

}

void PlayerOperationObject::OnGoal()
{
}

void PlayerOperationObject::DecideFileName()
{

	/*===== �t�@�C���������� =====*/

	std::vector<std::string> fileNames;
	std::vector<int> fileNumbers;

	// �t�@�C������ǂݍ��݁B
	bool isSuccess = FFILE::GetFileNames("Resource/Game/CharaGhostData/", fileNames);

	// �t�@�C�����̓ǂݍ��݂����s���Ă�����B
	if (!isSuccess) assert(0);

	// �S�t�@�C��������B
	for (auto& index : fileNames) {

		std::istringstream iss(index);
		std::string cutString;

		// '_'��؂�ŕ�����𕪉��B
		while (std::getline(iss, cutString, '_')) {

			// .txt�����O�������̂�'.'��T���B
			std::istringstream commaIss(cutString);
			std::string commaCutString;
			while (std::getline(commaIss, commaCutString, '.')) {

				// ������"Dev"��������J���҃S�[�X�g�B
				//if (cutString == "Dev");

				// �����񂪐������ǂ����𔻒肷��B
				if (std::all_of(commaCutString.cbegin(), commaCutString.cend(), isdigit)) {

					// �����ɕϊ�����B
					fileNumbers.emplace_back(std::stoi(commaCutString));

				}

			}


		}

	}

	// �擾���ꂽ�l�̍ő�̐������߂�B
	int maxNumber = 0;

	// �ۑ����ꂽ������1�ȏゾ������B
	if (0 < static_cast<int>(fileNumbers.size())) {

		std::sort(fileNumbers.begin(), fileNumbers.end(), std::greater<int>());

		// �ő�̐�����ۑ��B
		maxNumber = fileNumbers.front() + 1;

	}

	// �p�X��ݒ肷��B
	std::string directoryPath = "Resource/Game/CharaGhostData/";
	std::string ghostCode = "Dev_";
	std::string number = std::to_string(maxNumber);
	std::string extension = ".txt";
	filePath_ = directoryPath + ghostCode + number + extension;

}

void PlayerOperationObject::WriteGhostInfo(BaseOperationObject::Operation Operation)
{

	/*===== �S�[�X�g���̏������� =====*/

	// ���͂����������ǂ������`�F�b�N�B
	if (!Operation.CheckInput()) return;

	std::ofstream wfile;

	// �t�@�C�����J���B��������������t�@�C���𐶐�����B
	wfile.open(filePath_, std::ios::app);

	// �擪�Ƀt���[�������������ށB
	wfile << std::to_string(frameTimer_) << " ";

	// �e������������ł����B
	if (Operation.accelerationRate_ != 0) {

		wfile << "A:" << std::to_string(Operation.accelerationRate_) << " ";

	}
	if (Operation.handleDriveRate_ != 0) {

		wfile << "H:" << std::to_string(Operation.handleDriveRate_) << " ";

	}
	if (Operation.isDrift_) {

		wfile << "D:" << std::to_string(static_cast<int>(Operation.isDrift_)) << " ";

	}
	if (Operation.isGetItem_) {

		wfile << "G:" << std::to_string(static_cast<int>(Operation.isGetItem_)) << " ";

	}
	if (Operation.isUseItemTrigger_) {

		wfile << "UT:" << std::to_string(static_cast<int>(Operation.isUseItemTrigger_)) << " ";

	}
	if (Operation.isUseItemRelease_) {

		wfile << "UR:" << std::to_string(static_cast<int>(Operation.isUseItemRelease_)) << " ";

	}

	wfile << std::endl;


	// �t�@�C�������B
	wfile.close();

}
