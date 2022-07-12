#include "BaseGimmick.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"

void BaseGimmick::AddTrans(const Vec3& Trans)
{

	/*===== 移動を加算 =====*/

	PolygonInstanceRegister::Ins()->AddTrans(insIndex, Trans);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->AddTrans(collider.insIndex, Trans);
#endif

	// OBBも移動させる。
	collider.pos = PolygonInstanceRegister::Ins()->GetPos(insIndex);

}

void BaseGimmick::ChangeTrans(const Vec3& Trans)
{

	/*===== 移動を代入 =====*/

	PolygonInstanceRegister::Ins()->ChangeTrans(insIndex, Trans);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->ChangeTrans(collider.insIndex, Trans);
#endif

	// OBBも移動させる。
	collider.pos = PolygonInstanceRegister::Ins()->GetPos(insIndex);

}

void BaseGimmick::AddScale(const Vec3& Scale)
{

	/*===== スケールを加算 =====*/

	PolygonInstanceRegister::Ins()->AddScale(insIndex, Scale);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->AddScale(collider.insIndex, Scale);
#endif

	// OBBの大きさも変える。
	collider.length = FHelper::MulMat(collider.defLength, PolygonInstanceRegister::Ins()->GetScale(insIndex));

}

void BaseGimmick::ChangeScale(const Vec3& Scale)
{

	/*===== スケールを代入 =====*/

	PolygonInstanceRegister::Ins()->ChangeScale(insIndex, Scale);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->ChangeScale(collider.insIndex, Scale);
#endif

	// OBBの大きさも変える。
	collider.length = FHelper::MulMat(collider.defLength, PolygonInstanceRegister::Ins()->GetScale(insIndex));

}

void BaseGimmick::AddRotate(const Vec3& Rotate)
{

	/*===== 回転を加算 =====*/

	PolygonInstanceRegister::Ins()->AddRotate(insIndex, Rotate);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->AddRotate(collider.insIndex, Rotate);
#endif

	// OBBのベクトルを回転させる。
	DirectX::XMMATRIX matRot = PolygonInstanceRegister::Ins()->GetRotate(insIndex);
	collider.dir[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot);
	collider.dir[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot);
	collider.dir[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot);

}

void BaseGimmick::ChangeRotate(const Vec3& Rotate)
{

	/*===== 回転を代入 =====*/

	PolygonInstanceRegister::Ins()->ChangeRotate(insIndex, Rotate);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->ChangeRotate(collider.insIndex, Rotate);
#endif

	// OBBのベクトルを回転させる。
	DirectX::XMMATRIX matRot = PolygonInstanceRegister::Ins()->GetRotate(insIndex);
	collider.dir[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot);
	collider.dir[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot);
	collider.dir[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot);

}
