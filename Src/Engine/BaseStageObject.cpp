#include "BaseStageObject.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"
#include "OBB.h"
#include "BLASRegister.h"
#include "PolygonInstance.h"
#include "BLAS.h"

void BaseStageObject::Display()
{

	/*===== 表示 =====*/

	PolygonInstanceRegister::Ins()->Display(instance_);

}

void BaseStageObject::NonDisplay()
{

	/*===== 非表示 =====*/

	PolygonInstanceRegister::Ins()->NonDisplay(instance_);

}

int BaseStageObject::GetBLASIndex()
{
	return blas_.lock()->GetBlasIndex();
}

int BaseStageObject::GetInstanceIndex()
{
	return instance_.lock()->GetInstanceIndex();
}

void BaseStageObject::ChangeMapTexture(int Index, int MapParam)
{

	/*===== 法線マップを変更 =====*/

	blas_.lock()->ChangeMapTexture(Index, static_cast<BLAS::MAP_PARAM>(MapParam));

}

void BaseStageObject::AddTrans(const Vec3& Trans)
{

	/*===== 移動を加算 =====*/

	// オブジェクトのIDが中間地点のときは、描画しないので処理を通さない。
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		instance_.lock()->AddTrans(Trans);

	}


#ifdef DEBUG
	PolygonInstanceRegister::Ins()->AddTrans(obb_->insIndex_, Trans);
#endif

	// OBBも移動させる。
	obb_->pos_ += Trans;

}

void BaseStageObject::ChangeTrans(const Vec3& Trans)
{

	/*===== 移動を代入 =====*/

	// オブジェクトのIDが中間地点のときは、描画しないので処理を通さない。
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		instance_.lock()->ChangeTrans(Trans);

	}

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->ChangeTrans(obb_->insIndex_, Trans);
#endif

	// OBBも移動させる。
	obb_->pos_ = instance_.lock()->GetPos();

}

void BaseStageObject::AddScale(const Vec3& Scale)
{

	/*===== スケールを加算 =====*/

	// オブジェクトのIDが中間地点のときは、描画しないので処理を通さない。
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		instance_.lock()->AddScale(Scale);

	}

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->AddScale(obb_->insIndex_, Scale);
#endif

	// OBBの大きさも変える。
	obb_->length_ = FHelper::MulMat(obb_->defLength_, FHelper::CalScaleMat(Scale));

}

void BaseStageObject::ChangeScale(const Vec3& Scale)
{

	/*===== スケールを代入 =====*/

	// オブジェクトのIDが中間地点のときは、描画しないので処理を通さない。
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		instance_.lock()->ChangeScale(Scale);

	}

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->ChangeScale(obb_->insIndex_, Scale);
#endif

	// OBBの大きさも変える。
	obb_->length_ = FHelper::MulMat(obb_->defLength_, instance_.lock()->GetScale());

}

void BaseStageObject::AddRotate(const Vec3& Rotate)
{

	/*===== 回転を加算 =====*/

	// オブジェクトのIDが中間地点のときは、描画しないので処理を通さない。
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		instance_.lock()->AddRotate(Rotate);

	}

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->AddRotate(obb_->insIndex_, Rotate);
#endif

	// OBBのベクトルを回転させる。
	DirectX::XMMATRIX matRot_ = instance_.lock()->GetRotate();
	obb_->dir_[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot_);
	obb_->dir_[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot_);
	obb_->dir_[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot_);

}

void BaseStageObject::ChangeRotate(const Vec3& Rotate)
{

	/*===== 回転を代入 =====*/

	// オブジェクトのIDが中間地点のときは、描画しないので処理を通さない。
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		instance_.lock()->ChangeRotate(Rotate);

	}

#ifdef DEBUG
	PolygonInstanceRegister::Ins()->ChangeRotate(obb_->insIndex_, Rotate);
#endif

	// OBBのベクトルを回転させる。
	DirectX::XMMATRIX matRot_ = instance_.lock()->GetRotate();
	obb_->dir_[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot_);
	obb_->dir_[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot_);
	obb_->dir_[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot_);

}

void BaseStageObject::Delete()
{

	/*===== 削除 =====*/

	isActive_ = false;

	blas_.lock()->Init();
	PolygonInstanceRegister::Ins()->DestroyInstance(instance_);

}

void BaseStageObject::BasicInit(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, std::weak_ptr<PolygonMeshInstance> Instance)
{

	/*===== 基底クラスのメンバ変数の初期化 =====*/

	// オブジェクトをロード
	instance_ = Instance;
	blas_ = Instance.lock()->GetBLAS();

	// 各種変数を初期化。
	collisionID_ = CollisionID;
	objID_ = ObjectID;
	isActive_ = true;

	// OBBをセット。
	obb_ = std::make_shared<OBB>();
	obb_->Setting(blas_, Instance);

	// オブジェクトのIDが中間地点だったら、OBBを生成した時点で描画するInstanceは不要になるので破棄する。
	if (ObjectID == BaseStageObject::OBJECT_ID::MIDDLE_POINT || ObjectID == BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK || ObjectID == BaseStageObject::OBJECT_ID::GOAL) {

		PolygonInstanceRegister::Ins()->DestroyInstance(Instance);

	}

}