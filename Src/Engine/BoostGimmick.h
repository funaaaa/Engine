#pragma once
#include "BaseGimmick.h"

class BoostGimmick : public BaseGimmick {

private:

	/*===== �����o�ϐ� =====*/


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	BoostGimmick() {};

	// ����������
	void Setting(const ID& GimmickID, const UINT& InstanceID, const UINT& ShaderID) override;

	// �X�V����
	void Update() override;

};