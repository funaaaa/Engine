#include "BaseStageObject.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"
#include "OBB.h"

void BaseStageObject::AddTrans(const Vec3& Trans)
{

	/*===== 移動を加算 =====*/

	// オブジェクトのIDが中間地点のときは、描画しないので処理を通さない。
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		PolygonInstanceRegister::Ins()->AddTrans(insIndex_, Trans);

	}

	// 当たり判定がOBBの時のみ処理を通す。
	if (collisionID_ == BaseStageObject::COLLISION_ID::OBB) {

#ifdef DEBUG
		PolygonInstanceRegister::Ins()->AddTrans(obb_->insIndex, Trans);
#endif

		// OBBも移動させる。
		obb_->pos_ = PolygonInstanceRegister::Ins()->GetPos(insIndex_);

	}

}

void BaseStageObject::ChangeTrans(const Vec3& Trans)
{

	/*===== 移動を代入 =====*/

	// オブジェクトのIDが中間地点のときは、描画しないので処理を通さない。
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		PolygonInstanceRegister::Ins()->ChangeTrans(insIndex_, Trans);

	}

	// 当たり判定がOBBの時のみ処理を通す。
	if (collisionID_ == BaseStageObject::COLLISION_ID::OBB) {

#ifdef DEBUG
		PolygonInstanceRegister::Ins()->ChangeTrans(obb_->insIndex, Trans);
#endif

		// OBBも移動させる。
		obb_->pos_ = PolygonInstanceRegister::Ins()->GetPos(insIndex_);

	}

}

void BaseStageObject::AddScale(const Vec3& Scale)
{

	/*===== スケールを加算 =====*/

	// オブジェクトのIDが中間地点のときは、描画しないので処理を通さない。
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		PolygonInstanceRegister::Ins()->AddScale(insIndex_, Scale);

	}

	// 当たり判定がOBBの時のみ処理を通す。
	if (collisionID_ == BaseStageObject::COLLISION_ID::OBB) {

#ifdef DEBUG
		PolygonInstanceRegister::Ins()->AddScale(obb_->insIndex, Scale);
#endif

		// OBBの大きさも変える。
		obb_->length_ = FHelper::MulMat(obb_->defLength_, PolygonInstanceRegister::Ins()->GetScale(insIndex_));

	}

}

void BaseStageObject::ChangeScale(const Vec3& Scale)
{

	/*===== スケールを代入 =====*/

	// オブジェクトのIDが中間地点のときは、描画しないので処理を通さない。
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		PolygonInstanceRegister::Ins()->ChangeScale(insIndex_, Scale);

	}

	// 当たり判定がOBBの時のみ処理を通す。
	if (collisionID_ == BaseStageObject::COLLISION_ID::OBB) {

#ifdef DEBUG
		PolygonInstanceRegister::Ins()->ChangeScale(obb_->insIndex, Scale);
#endif

		// OBBの大きさも変える。
		obb_->length_ = FHelper::MulMat(obb_->defLength_, PolygonInstanceRegister::Ins()->GetScale(insIndex_));

	}

}

void BaseStageObject::AddRotate(const Vec3& Rotate)
{

	/*===== 回転を加算 =====*/

	// オブジェクトのIDが中間地点のときは、描画しないので処理を通さない。
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		PolygonInstanceRegister::Ins()->AddRotate(insIndex_, Rotate);

	}

	// 当たり判定がOBBの時のみ処理を通す。
	if (collisionID_ == BaseStageObject::COLLISION_ID::OBB) {

#ifdef DEBUG
		PolygonInstanceRegister::Ins()->AddRotate(obb_->insIndex, Rotate);
#endif

		// OBBのベクトルを回転させる。
		DirectX::XMMATRIX matRot_ = PolygonInstanceRegister::Ins()->GetRotate(insIndex_);
		obb_->dir_[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot_);
		obb_->dir_[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot_);
		obb_->dir_[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot_);

	}

}

void BaseStageObject::ChangeRotate(const Vec3& Rotate)
{

	/*===== 回転を代入 =====*/

	// オブジェクトのIDが中間地点のときは、描画しないので処理を通さない。
	if (objID_ != BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		PolygonInstanceRegister::Ins()->ChangeRotate(insIndex_, Rotate);

	}

	// 当たり判定がOBBの時のみ処理を通す。
	if (collisionID_ == BaseStageObject::COLLISION_ID::OBB) {

#ifdef DEBUG
		PolygonInstanceRegister::Ins()->ChangeRotate(obb_->insIndex, Rotate);
#endif

		// OBBのベクトルを回転させる。
		DirectX::XMMATRIX matRot_ = PolygonInstanceRegister::Ins()->GetRotate(insIndex_);
		obb_->dir_[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot_);
		obb_->dir_[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot_);
		obb_->dir_[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot_);

}

}

void BaseStageObject::BasicInit(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const int& InstanceID)
{

	/*===== 基底クラスのメンバ変数の初期化 =====*/

	// オブジェクトをロード
	insIndex_ = InstanceID;
	blasIndex_ = PolygonInstanceRegister::Ins()->GetBLASIndex(InstanceID);

	// 各種変数を初期化。
	collisionID_ = CollisionID;
	objID_ = ObjectID;
	isActive_ = true;

	// 当たり判定のIDがOBBだったら。
	if (CollisionID == BaseStageObject::COLLISION_ID::OBB) {

		// OBBをセット。
		obb_ = std::make_shared<OBB>();
		obb_->Setting(blasIndex_, insIndex_);

	}

	// オブジェクトのIDが中間地点だったら、OBBを生成した時点で描画するInstanceは不要になるので破棄する。
	if (ObjectID == BaseStageObject::OBJECT_ID::MIDDLE_POINT) {

		PolygonInstanceRegister::Ins()->DestroyInstance(insIndex_);

	}

}