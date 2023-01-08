#pragma once
#include "BaseStageObject.h"

// �A�C�e���{�b�N�X�N���X
class ItemBoxObject : public BaseStageObject{

private:

	/*===== �����o�ϐ� =====*/

	int timerToActivation_;	// �L�����܂ł̃^�C�}�[
	int activationTimer_;	// �L�����܂ł̃^�C�}�[�v���p
	int charaIndex_;		// �A�C�e���{�b�N�X���擾�����L������CharacterMgr��ł�Index
	float scale_;			// �A�C�e���{�b�N�X�̃X�P�[��
	const float DEF_SCALE = 60.0f;



public:

	/*===== �����o�֐� =====*/

	void Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, std::weak_ptr<PolygonMeshInstance> Instance)override;
	void Destroy()override;
	void Update(int Timer, std::weak_ptr<CharacterMgr> Character)override;
	void Disable(int TimerToActivation, int CharaIndex)override;

};