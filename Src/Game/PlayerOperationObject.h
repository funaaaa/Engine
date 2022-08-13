#pragma once
#include "BaseOperationObject.h"
#include <memory>
#include <string>

class Character;

class PlayerOperationObject : public BaseOperationObject {

private:

	/*===== �����o�ϐ� =====*/

	int controllerIndex_;	// ���삷��R���g���[���[�̃C���f�b�N�X
	bool isWriteGhostInfo_;	// �S�[�X�g����ۑ����邩�ǂ����B
	Character* character_;
	std::string filePath_;
	int frameTimer_;		// �n�܂��Ă���̃t���[�����v������p�^�C�}�[ 

public:

	/*===== �����o�֐� =====*/

	PlayerOperationObject(const int& ControllerIndex, const bool& IsWriteGhostInfo, Character* CharacterPointer);

	BaseOperationObject::Operation Input(const BaseOperationObject::OperationInputData& InputData)override;

	void OnGoal()override;

private:

	void DecideFileName();
	void WriteGhostInfo(BaseOperationObject::Operation Operation);

};