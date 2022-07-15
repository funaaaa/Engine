#include "BaseStageObject.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"
#include "OBB.h"

void BaseStageObject::AddTrans(const Vec3& Trans)
{

	/*===== �ړ������Z =====*/

	// �I�u�W�F�N�g��ID�����Ԓn�_�̂Ƃ��́A�`�悵�Ȃ��̂ŏ�����ʂ��Ȃ��B
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		PolygonInstanceRegister::Ins()->AddTrans(insIndex_, Trans);

	}

	// �����蔻�肪OBB�̎��̂ݏ�����ʂ��B
	if (collisionID_ == BaseStageObject::COLLISION_ID::OBB) {

#ifdef DEBUG
		PolygonInstanceRegister::Ins()->AddTrans(obb_->insIndex, Trans);
#endif

		// OBB���ړ�������B
		obb_->pos_ = PolygonInstanceRegister::Ins()->GetPos(insIndex_);

	}

}

void BaseStageObject::ChangeTrans(const Vec3& Trans)
{

	/*===== �ړ����� =====*/

	// �I�u�W�F�N�g��ID�����Ԓn�_�̂Ƃ��́A�`�悵�Ȃ��̂ŏ�����ʂ��Ȃ��B
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		PolygonInstanceRegister::Ins()->ChangeTrans(insIndex_, Trans);

	}

	// �����蔻�肪OBB�̎��̂ݏ�����ʂ��B
	if (collisionID_ == BaseStageObject::COLLISION_ID::OBB) {

#ifdef DEBUG
		PolygonInstanceRegister::Ins()->ChangeTrans(obb_->insIndex, Trans);
#endif

		// OBB���ړ�������B
		obb_->pos_ = PolygonInstanceRegister::Ins()->GetPos(insIndex_);

	}

}

void BaseStageObject::AddScale(const Vec3& Scale)
{

	/*===== �X�P�[�������Z =====*/

	// �I�u�W�F�N�g��ID�����Ԓn�_�̂Ƃ��́A�`�悵�Ȃ��̂ŏ�����ʂ��Ȃ��B
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		PolygonInstanceRegister::Ins()->AddScale(insIndex_, Scale);

	}

	// �����蔻�肪OBB�̎��̂ݏ�����ʂ��B
	if (collisionID_ == BaseStageObject::COLLISION_ID::OBB) {

#ifdef DEBUG
		PolygonInstanceRegister::Ins()->AddScale(obb_->insIndex, Scale);
#endif

		// OBB�̑傫�����ς���B
		obb_->length_ = FHelper::MulMat(obb_->defLength_, PolygonInstanceRegister::Ins()->GetScale(insIndex_));

	}

}

void BaseStageObject::ChangeScale(const Vec3& Scale)
{

	/*===== �X�P�[������ =====*/

	// �I�u�W�F�N�g��ID�����Ԓn�_�̂Ƃ��́A�`�悵�Ȃ��̂ŏ�����ʂ��Ȃ��B
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		PolygonInstanceRegister::Ins()->ChangeScale(insIndex_, Scale);

	}

	// �����蔻�肪OBB�̎��̂ݏ�����ʂ��B
	if (collisionID_ == BaseStageObject::COLLISION_ID::OBB) {

#ifdef DEBUG
		PolygonInstanceRegister::Ins()->ChangeScale(obb_->insIndex, Scale);
#endif

		// OBB�̑傫�����ς���B
		obb_->length_ = FHelper::MulMat(obb_->defLength_, PolygonInstanceRegister::Ins()->GetScale(insIndex_));

	}

}

void BaseStageObject::AddRotate(const Vec3& Rotate)
{

	/*===== ��]�����Z =====*/

	// �I�u�W�F�N�g��ID�����Ԓn�_�̂Ƃ��́A�`�悵�Ȃ��̂ŏ�����ʂ��Ȃ��B
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		PolygonInstanceRegister::Ins()->AddRotate(insIndex_, Rotate);

	}

	// �����蔻�肪OBB�̎��̂ݏ�����ʂ��B
	if (collisionID_ == BaseStageObject::COLLISION_ID::OBB) {

#ifdef DEBUG
		PolygonInstanceRegister::Ins()->AddRotate(obb_->insIndex, Rotate);
#endif

		// OBB�̃x�N�g������]������B
		DirectX::XMMATRIX matRot_ = PolygonInstanceRegister::Ins()->GetRotate(insIndex_);
		obb_->dir_[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot_);
		obb_->dir_[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot_);
		obb_->dir_[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot_);

	}

}

void BaseStageObject::ChangeRotate(const Vec3& Rotate)
{

	/*===== ��]���� =====*/

	// �I�u�W�F�N�g��ID�����Ԓn�_�̂Ƃ��́A�`�悵�Ȃ��̂ŏ�����ʂ��Ȃ��B
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		PolygonInstanceRegister::Ins()->ChangeRotate(insIndex_, Rotate);

	}

	// �����蔻�肪OBB�̎��̂ݏ�����ʂ��B
	if (collisionID_ == BaseStageObject::COLLISION_ID::OBB) {

#ifdef DEBUG
		PolygonInstanceRegister::Ins()->ChangeRotate(obb_->insIndex, Rotate);
#endif

		// OBB�̃x�N�g������]������B
		DirectX::XMMATRIX matRot_ = PolygonInstanceRegister::Ins()->GetRotate(insIndex_);
		obb_->dir_[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot_);
		obb_->dir_[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot_);
		obb_->dir_[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot_);

}

}

void BaseStageObject::BasicInit(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID)
{

	/*===== ���N���X�̃����o�ϐ��̏����� =====*/

	// �I�u�W�F�N�g�����[�h
	insIndex_ = InstanceID;
	blasIndex_ = PolygonInstanceRegister::Ins()->GetBLASIndex(InstanceID);

	// �e��ϐ����������B
	collisionID_ = CollisionID;
	objID_ = ObjectID;
	isActive_ = true;

	// �����蔻���ID��OBB��������B
	if (CollisionID == BaseStageObject::COLLISION_ID::OBB) {

		// OBB���Z�b�g�B
		obb_ = std::make_shared<OBB>();
		obb_->Setting(blasIndex_, insIndex_);

	}

	// �I�u�W�F�N�g��ID�����Ԓn�_��������AOBB�𐶐��������_�ŕ`�悷��Instance�͕s�v�ɂȂ�̂Ŕj������B
	if (ObjectID == BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		PolygonInstanceRegister::Ins()->DestroyInstance(insIndex_);

	}

}