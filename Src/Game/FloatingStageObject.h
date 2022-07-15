#pragma once
#include "BaseStageObject.h"

// �ӂ�ӂ퓮������p�I�u�W�F�N�g�N���X
class FloatingStageObject : public BaseStageObject{

private:

	/*===== �����o�ϐ� =====*/

	Vec3 floatingMove_;		// �ӂ�ӂ퓮������
	float timerOffset_;		// �^�C�}�[�̃I�t�Z�b�g �ӂ�ӂ��g�̂悤�ɓ��������߂Ɏg�p����B
	const Vec3 FLOATING_MOVE = 10.0f;	// �ӂ�ӂ퓮����


public:

	/*===== �����o�֐� =====*/

	void Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID)override;
	void Destroy()override;
	void Update(const int& Timer)override;

};