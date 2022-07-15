#include "FloatingStageObject.h"
#include "PolygonInstanceRegister.h"
#include "OBB.h"

void FloatingStageObject::Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID)
{

	/*===== �Z�b�e�B���O���� =====*/

	BasicInit(ObjectID, CollisionID, InstanceID);

	// �ړ��ʂ��������B
	timerOffset_ = InstanceID;	// InstanceID���^�C�}�[�̃I�t�Z�b�g�Ɏg�p����B����ɂ���ĘA���Ő�������FloatingStageObject�͔g�̂悤�ɓ����B
	floatingMove_ = Vec3();

}

void FloatingStageObject::Destroy()
{

	/*===== �C���X�^���X�j�� =====*/

	PolygonInstanceRegister::Ins()->DestroyInstance(insIndex_);
	isActive_ = false;

}

void FloatingStageObject::Update(const int& Timer)
{

	/*===== �X�V���� =====*/

	// �O�t���[���̈ړ������������B
	AddTrans(-floatingMove_);

	// �V���Ɉړ��ʂ����߂�B Timer��int�^�Ȃ̂�10.0f�Ŋ����ď����ɂ���B
	floatingMove_.y_ = sinf(static_cast<float>(Timer) / 10.0f + timerOffset_);

	// �ړ�������B
	AddTrans(floatingMove_);

}
