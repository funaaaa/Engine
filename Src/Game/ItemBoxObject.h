#pragma once
#include "BaseStageObject.h"

// �A�C�e���{�b�N�X�N���X
class ItemBoxObject : public BaseStageObject{

private:

	/*===== �����o�ϐ� =====*/

	int timerToActivation_;	// �L�����܂ł̃^�C�}�[
	int activationTimer_;	// �L�����܂ł̃^�C�}�[�v���p



public:

	/*===== �����o�֐� =====*/

	void Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID)override;
	void Destroy()override;
	void Update(const int& Timer)override;
	void Disable(const int& TimerToActivation)override;

};