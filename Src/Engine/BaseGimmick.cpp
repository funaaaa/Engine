#include "BaseGimmick.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"

void BaseGimmick::AddTrans(const Vec3& Trans)
{

	/*===== �ړ������Z =====*/

	PolygonInstanceRegister::Ins()->AddTrans(insIndex_, Trans);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->AddTrans(collider.insIndex, Trans);
#endif

	// OBB���ړ�������B
	collider_.pos_ = PolygonInstanceRegister::Ins()->GetPos(insIndex_);

}

void BaseGimmick::ChangeTrans(const Vec3& Trans)
{

	/*===== �ړ����� =====*/

	PolygonInstanceRegister::Ins()->ChangeTrans(insIndex_, Trans);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->ChangeTrans(collider.insIndex, Trans);
#endif

	// OBB���ړ�������B
	collider_.pos_ = PolygonInstanceRegister::Ins()->GetPos(insIndex_);

}

void BaseGimmick::AddScale(const Vec3& Scale)
{

	/*===== �X�P�[�������Z =====*/

	PolygonInstanceRegister::Ins()->AddScale(insIndex_, Scale);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->AddScale(collider.insIndex, Scale);
#endif

	// OBB�̑傫�����ς���B
	collider_.length = FHelper::MulMat(collider_.defLength, PolygonInstanceRegister::Ins()->GetScale(insIndex_));

}

void BaseGimmick::ChangeScale(const Vec3& Scale)
{

	/*===== �X�P�[������ =====*/

	PolygonInstanceRegister::Ins()->ChangeScale(insIndex_, Scale);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->ChangeScale(collider.insIndex, Scale);
#endif

	// OBB�̑傫�����ς���B
	collider_.length = FHelper::MulMat(collider_.defLength, PolygonInstanceRegister::Ins()->GetScale(insIndex_));

}

void BaseGimmick::AddRotate(const Vec3& Rotate)
{

	/*===== ��]�����Z =====*/

	PolygonInstanceRegister::Ins()->AddRotate(insIndex_, Rotate);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->AddRotate(collider.insIndex, Rotate);
#endif

	// OBB�̃x�N�g������]������B
	DirectX::XMMATRIX matRot = PolygonInstanceRegister::Ins()->GetRotate(insIndex_);
	collider_.dir[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot);
	collider_.dir[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot);
	collider_.dir[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot);

}

void BaseGimmick::ChangeRotate(const Vec3& Rotate)
{

	/*===== ��]���� =====*/

	PolygonInstanceRegister::Ins()->ChangeRotate(insIndex_, Rotate);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->ChangeRotate(collider.insIndex, Rotate);
#endif

	// OBB�̃x�N�g������]������B
	DirectX::XMMATRIX matRot = PolygonInstanceRegister::Ins()->GetRotate(insIndex_);
	collider_.dir[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot);
	collider_.dir[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot);
	collider_.dir[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot);

}
