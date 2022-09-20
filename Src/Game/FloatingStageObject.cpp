#include "FloatingStageObject.h"
#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "OBB.h"

void FloatingStageObject::Setting(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*===== �Z�b�e�B���O���� =====*/

	BasicInit(ObjectID, CollisionID, Instance);

	// �ړ��ʂ��������B
	timerOffset_ = static_cast<float>(Instance.lock()->GetInstanceIndex());	// InstanceID���^�C�}�[�̃I�t�Z�b�g�Ɏg�p����B����ɂ���ĘA���Ő�������FloatingStageObject�͔g�̂悤�ɓ����B
	floatingMove_ = Vec3();

}

void FloatingStageObject::Destroy()
{

	/*===== �C���X�^���X�j�� =====*/

	PolygonInstanceRegister::Ins()->DestroyInstance(instance_);
	isActive_ = false;

}

void FloatingStageObject::Update(const int& Timer)
{

	/*===== �X�V���� =====*/

	// �O�t���[���̈ړ������������B
	AddTrans(-floatingMove_);

	// �V���Ɉړ��ʂ����߂�B Timer��int�^�Ȃ̂�20.0f�Ŋ����ď����ɂ���B
	floatingMove_.y_ = sinf(static_cast<float>(Timer) / 20.0f + timerOffset_) * FLOATING_MOVE;

	// �ړ�������B
	AddTrans(floatingMove_);

	// ��]������B
	AddRotate(Vec3(0.01f, 0.01f, 0));

}

void FloatingStageObject::Disable(const int& TimerToActivation)
{

	/*===== ���������čĐݒ�܂ł̃^�C�}�[���Z�b�g =====*/

	TimerToActivation;

}
