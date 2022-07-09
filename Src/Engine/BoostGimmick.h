#pragma once
#include "BaseGimmick.h"
#include "ModelDataManager.h"

class BoostGimmick : public BaseGimmick {

private:

	/*===== �����o�ϐ� =====*/


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	BoostGimmick() {};

	// ����������
	void Setting(const ID& GimmickID, const UINT& InstanceID) override;

	// �X�V����
	void Update() override;

private:

	// OBB�p��Vertex�𐶐��B
	ModelDataManager::ObjectData GenerateVertex(OBB Obb);

};