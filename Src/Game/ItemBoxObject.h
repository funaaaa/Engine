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

	void Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, std::weak_ptr<PolygonMeshInstance> Instance)override;
	void Destroy()override;
	void Update(int Timer)override;
	void Disable(int TimerToActivation)override;

};