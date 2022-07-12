#include "BaseGimmick.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"

void BaseGimmick::AddTrans(const Vec3& Trans)
{

	/*===== �ړ������Z =====*/

	PolygonInstanceRegister::Ins()->AddTrans(insIndex, Trans);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->AddTrans(collider.insIndex, Trans);
#endif

	// OBB���ړ�������B
	collider.pos = PolygonInstanceRegister::Ins()->GetPos(insIndex);

}

void BaseGimmick::ChangeTrans(const Vec3& Trans)
{

	/*===== �ړ����� =====*/

	PolygonInstanceRegister::Ins()->ChangeTrans(insIndex, Trans);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->ChangeTrans(collider.insIndex, Trans);
#endif

	// OBB���ړ�������B
	collider.pos = PolygonInstanceRegister::Ins()->GetPos(insIndex);

}

void BaseGimmick::AddScale(const Vec3& Scale)
{

	/*===== �X�P�[�������Z =====*/

	PolygonInstanceRegister::Ins()->AddScale(insIndex, Scale);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->AddScale(collider.insIndex, Scale);
#endif

	// OBB�̑傫�����ς���B
	collider.length = FHelper::MulMat(collider.defLength, PolygonInstanceRegister::Ins()->GetScale(insIndex));

}

void BaseGimmick::ChangeScale(const Vec3& Scale)
{

	/*===== �X�P�[������ =====*/

	PolygonInstanceRegister::Ins()->ChangeScale(insIndex, Scale);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->ChangeScale(collider.insIndex, Scale);
#endif

	// OBB�̑傫�����ς���B
	collider.length = FHelper::MulMat(collider.defLength, PolygonInstanceRegister::Ins()->GetScale(insIndex));

}

void BaseGimmick::AddRotate(const Vec3& Rotate)
{

	/*===== ��]�����Z =====*/

	PolygonInstanceRegister::Ins()->AddRotate(insIndex, Rotate);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->AddRotate(collider.insIndex, Rotate);
#endif

	// OBB�̃x�N�g������]������B
	DirectX::XMMATRIX matRot = PolygonInstanceRegister::Ins()->GetRotate(insIndex);
	collider.dir[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot);
	collider.dir[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot);
	collider.dir[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot);

}

void BaseGimmick::ChangeRotate(const Vec3& Rotate)
{

	/*===== ��]���� =====*/

	PolygonInstanceRegister::Ins()->ChangeRotate(insIndex, Rotate);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->ChangeRotate(collider.insIndex, Rotate);
#endif

	// OBB�̃x�N�g������]������B
	DirectX::XMMATRIX matRot = PolygonInstanceRegister::Ins()->GetRotate(insIndex);
	collider.dir[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot);
	collider.dir[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot);
	collider.dir[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot);

}
