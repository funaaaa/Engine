#include "BaseGimmick.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"

void BaseGimmick::AddTrans(const Vec3& Trans)
{

	/*===== 移動を加算 =====*/

	PolygonInstanceRegister::Ins()->AddTrans(insIndex_, Trans);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->AddTrans(collider.insIndex, Trans);
#endif

	// OBBも移動させる。
	collider_.pos_ = PolygonInstanceRegister::Ins()->GetPos(insIndex_);

}

void BaseGimmick::ChangeTrans(const Vec3& Trans)
{

	/*===== 移動を代入 =====*/

	PolygonInstanceRegister::Ins()->ChangeTrans(insIndex_, Trans);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->ChangeTrans(collider.insIndex, Trans);
#endif

	// OBBも移動させる。
	collider_.pos_ = PolygonInstanceRegister::Ins()->GetPos(insIndex_);

}

void BaseGimmick::AddScale(const Vec3& Scale)
{

	/*===== スケールを加算 =====*/

	PolygonInstanceRegister::Ins()->AddScale(insIndex_, Scale);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->AddScale(collider.insIndex, Scale);
#endif

	// OBBの大きさも変える。
	collider_.length_ = FHelper::MulMat(collider_.defLength_, PolygonInstanceRegister::Ins()->GetScale(insIndex_));

}

void BaseGimmick::ChangeScale(const Vec3& Scale)
{

	/*===== スケールを代入 =====*/

	PolygonInstanceRegister::Ins()->ChangeScale(insIndex_, Scale);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->ChangeScale(collider.insIndex, Scale);
#endif

	// OBBの大きさも変える。
	collider_.length_ = FHelper::MulMat(collider_.defLength_, PolygonInstanceRegister::Ins()->GetScale(insIndex_));

}

void BaseGimmick::AddRotate(const Vec3& Rotate)
{

	/*===== 回転を加算 =====*/

	PolygonInstanceRegister::Ins()->AddRotate(insIndex_, Rotate);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->AddRotate(collider.insIndex, Rotate);
#endif

	// OBBのベクトルを回転させる。
	DirectX::XMMATRIX matRot_ = PolygonInstanceRegister::Ins()->GetRotate(insIndex_);
	collider_.dir_[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot_);
	collider_.dir_[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot_);
	collider_.dir_[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot_);

}

void BaseGimmick::ChangeRotate(const Vec3& Rotate)
{

	/*===== 回転を代入 =====*/

	PolygonInstanceRegister::Ins()->ChangeRotate(insIndex_, Rotate);

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->ChangeRotate(collider.insIndex, Rotate);
#endif

	// OBBのベクトルを回転させる。
	DirectX::XMMATRIX matRot_ = PolygonInstanceRegister::Ins()->GetRotate(insIndex_);
	collider_.dir_[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot_);
	collider_.dir_[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot_);
	collider_.dir_[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot_);

}
