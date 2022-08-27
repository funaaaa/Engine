#include "StageObjectMgr.h"
#include "BasicStageObject.h"
#include "FloatingStageObject.h"
#include "ItemBoxObject.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "OBB.h"
#include "BLAS.h"

int StageObjectMgr::AddObject(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID)
{

	/*===== ギミックを追加 =====*/

	// ふわふわ動く装飾オブジェクトだったら。
	if (ObjectID == BaseStageObject::OBJECT_ID::FLOATING_ORNAMENT) {

		objects_.emplace_back(std::make_shared<FloatingStageObject>());

	}
	// アイテムボックスオブジェクトだったら
	else if (ObjectID == BaseStageObject::OBJECT_ID::ITEM_BOX) {

		objects_.emplace_back(std::make_shared<ItemBoxObject>());

	}
	// それ以外の通常のオブジェクトだったら。
	else {

		objects_.emplace_back(std::make_shared<BasicStageObject>());

	}

	// Blasをロード
	int blasIndex = BLASRegister::Ins()->GenerateObj(DirectryPath, ModelName, HitGroupName, TexturePath);
	// Instanceを生成。
	int instanceIndex = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex, ShaderID);

	// オブジェクトを設定。
	objects_.back()->Setting(ObjectID, CollisionID, instanceIndex);

	return static_cast<int>(objects_.size()) - 1;

}

void StageObjectMgr::Update(const int& Timer)
{

	/*===== 更新処理 =====*/

	for (auto& index : objects_) {

		index->Update(Timer);

	}

}

BaseStage::ColliderOutput StageObjectMgr::Collider(BaseStage::ColliderInput Input)
{

	/*==== 当たり判定 =====*/

	// 当たり判定結果用
	BaseStage::ColliderOutput output;
	output.isHitBoostGimmick_ = false;
	output.isHitGoal_ = false;
	output.isHitMiddlePoint_ = false;
	output.isHitOrnament_ = false;
	output.isHitStageGrass_ = false;
	output.isHitStage_ = false;
	output.isHitItemBox_ = false;
	output.ornamentHitNormal_ = Vec3(-100, -100, -100);

	for (auto& index : objects_) {

		// オブジェクトが有効化されていなかったら処理を飛ばす。
		if (!index->GetIsActive())continue;

		// オブジェクトが当たり判定を行わないものだったら処理を飛ばす。
		BaseStageObject::COLLISION_ID indexCollisionID = index->GetCollisionID();
		if (indexCollisionID == BaseStageObject::COLLISION_ID::NONE) continue;

		// 当たり判定がOBBだったら。
		if (indexCollisionID == BaseStageObject::COLLISION_ID::OBB) {

			// 一定以上離れていたら。
			float distance = Vec3(Input.targetOBB_.lock()->pos_ - index->GetOBB()->pos_).Length();
			float size = Vec3(Input.targetOBB_.lock()->length_ + index->GetOBB()->length_).Length();
			if (size < distance) continue;

			// OBBの当たり判定を行う。
			bool isHit = Input.targetOBB_.lock()->CheckHitOBB(index->GetOBB());

			// 当たっていなかったら次の要素へ。
			if (!isHit) continue;

			// オブジェクトのIDを取得して保存。
			BaseStageObject::OBJECT_ID indexObjID = index->GetObjectID();

			// オブジェクトごとに保存。
			if (indexObjID == BaseStageObject::OBJECT_ID::BOOS_GIMMICK) {
				output.isHitBoostGimmick_ = true;
			}
			else if (indexObjID == BaseStageObject::OBJECT_ID::GOAL) {
				output.isHitGoal_ = true;
			}
			else if (indexObjID == BaseStageObject::OBJECT_ID::MIDDLE_POINT) {
				output.isHitMiddlePoint_ = true;
			}
			// 当たったオブジェクトがアイテムボックスだったら。
			else if (indexObjID == BaseStageObject::OBJECT_ID::ITEM_BOX && Input.isPlayer_) {

				// アイテムボックスを一時的に無効化。
				index->Disable(180);

				output.isHitItemBox_ = true;

			}

		}
		// 当たり判定がMESHだったら。
		else if (indexCollisionID == BaseStageObject::COLLISION_ID::MESH) {

			// 当たり判定用のデータを生成。
			FHelper::RayToModelCollisionData InputRayData;
			InputRayData.targetVertex_ = BLASRegister::Ins()->GetBLAS()[index->GetBLASIndex()]->GetVertexPos();
			InputRayData.targetNormal_ = BLASRegister::Ins()->GetBLAS()[index->GetBLASIndex()]->GetVertexNormal();
			InputRayData.targetIndex_ = BLASRegister::Ins()->GetBLAS()[index->GetBLASIndex()]->GetVertexIndex();
			InputRayData.targetUV_ = BLASRegister::Ins()->GetBLAS()[index->GetBLASIndex()]->GetVertexUV();
			InputRayData.matTrans_ = PolygonInstanceRegister::Ins()->GetTrans(index->GetINSTANCEIndex());
			InputRayData.matScale_ = PolygonInstanceRegister::Ins()->GetScale(index->GetINSTANCEIndex());
			InputRayData.matRot_ = PolygonInstanceRegister::Ins()->GetRotate(index->GetINSTANCEIndex());

			// オブジェクトのIDを取得して保存。
			BaseStageObject::OBJECT_ID indexObjID = index->GetObjectID();

			// オブジェクトごとに当たり判定を行う
			if (indexObjID == BaseStageObject::OBJECT_ID::STAGE || indexObjID == BaseStageObject::OBJECT_ID::STAGE_GRASS || indexObjID == BaseStageObject::OBJECT_ID::WALL) {
				output = StageMeshCollider(Input, InputRayData, output, indexObjID);
			}
			else if (indexObjID == BaseStageObject::OBJECT_ID::ORNAMENT) {

				// 一定以上離れていたら。 オブジェクトの配置をBlender基準でやっているため距離を頂点から持ってきているが、いずれは手動で配置して座標から距離を持ってこれるようにする
				float distance = Vec3(Input.targetPos_ - PolygonInstanceRegister::Ins()->GetPos(index->GetINSTANCEIndex())).Length();
				float size = Vec3(Input.targetSize_ + index->GetOBB()->length_).Length();
				if (size < distance) continue;

				output = OrnamentMeshCollider(Input, InputRayData, output);
			}

		}

	}

	// 最終結果の座標を保存。
	output.resultPos_ = Input.targetPos_;

	return output;

}

void StageObjectMgr::AddTrans(const int& Index, const Vec3& Trans)
{

	/*===== 移動を加算 =====*/

	objects_[Index]->AddTrans(Trans);

}

void StageObjectMgr::ChangeTrans(const int& Index, const Vec3& Trans)
{

	/*===== 移動を代入 =====*/

	objects_[Index]->ChangeTrans(Trans);

}

void StageObjectMgr::AddScale(const int& Index, const Vec3& Scale)
{

	/*===== スケールを加算 =====*/

	objects_[Index]->AddScale(Scale);

}

void StageObjectMgr::ChangeScale(const int& Index, const Vec3& Scale)
{

	/*===== スケールを代入 =====*/

	objects_[Index]->ChangeScale(Scale);

}

void StageObjectMgr::AddRotate(const int& Index, const Vec3& Rotate)
{

	/*===== 回転を加算 =====*/

	objects_[Index]->AddRotate(Rotate);

}

void StageObjectMgr::ChangeRotate(const int& Index, const Vec3& Rotate)
{

	/*===== 回転を代入 =====*/

	objects_[Index]->ChangeRotate(Rotate);

}

void StageObjectMgr::NonDisplay(const int& Index)
{

	/*===== 非表示 =====*/

	objects_[Index]->NonDisplay();

}

void StageObjectMgr::Display(const int& Index)
{

	/*===== 表示 =====*/

	objects_[Index]->Display();

}

BaseStage::ColliderOutput StageObjectMgr::StageMeshCollider(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output, BaseStageObject::OBJECT_ID ObjectID)
{

	/*===== ステージ、草とのメッシュの当たり判定 =====*/

	// 当たり判定の結果保存用変数。
	bool isHit = false;
	Vec3 impactPos;
	float hitDistance;
	Vec3 hitNormal;
	Vec2 hitUV;

	if (ObjectID != BaseStageObject::OBJECT_ID::WALL) {

		// レイ用の設定を追加。
		InputRayData.rayPos_ = Input.targetPos_;
		InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(Input.targetInsIndex_));

		// 当たり判定を行う。
		isHit = FHelper::RayToModelCollision(InputRayData, impactPos, hitDistance, hitNormal, hitUV);

		// 当たった距離がY軸のサイズよりも小さかったら。
		isHit &= (hitDistance - Input.targetSize_.y_) <= 0;
		isHit &= 0 < hitDistance;

		// 当たっていたら押し戻す。
		if (isHit) {

			// ぴったり押し戻すと次のフレームで空中判定になってしまうので、若干オフセットを設ける。
			const float PUSH_BACK_OFFSET = 1.0f;

			// 法線方向に当たった分押し戻す。
			Input.targetPos_ += hitNormal * (Input.targetSize_.y_ - (hitDistance + PUSH_BACK_OFFSET));

			// 斜め床の回転処理。
			RotObliqueFloor(Input, hitNormal, Output);

			// ステージだったら
			if (ObjectID == BaseStageObject::OBJECT_ID::STAGE) {

				Output.isHitStage_ = true;

			}
			// 草だったら
			else if (ObjectID == BaseStageObject::OBJECT_ID::STAGE_GRASS) {

				Output.isHitStageGrass_ = true;

			}

		}

	}

	// 貫通防止で正面方向にもレイを飛ばす。
	InputRayData.rayPos_ = Input.targetPrevPos_;
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(Input.targetInsIndex_));

	// 当たり判定を行う。
	isHit = false;
	isHit = FHelper::RayToModelCollision(InputRayData, impactPos, hitDistance, hitNormal, hitUV);

	// 当たった距離がY軸のサイズよりも小さかったら。
	isHit &= (hitDistance - Input.targetSize_.x_) <= 0;
	isHit &= 0 < hitDistance;

	// 当たっていたら押し戻す。
	if (isHit) {

		// 法線方向に当たった分押し戻す。
		Input.targetPos_ = impactPos + hitNormal * hitDistance;

	}

	return Output;

}

BaseStage::ColliderOutput StageObjectMgr::OrnamentMeshCollider(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output)
{

	/*===== ステージ装飾オブジェクトとの当たり判定 =====*/

	InputRayData.rayPos_ = Input.targetPrevPos_;
	InputRayData.rayDir_ = (Input.targetPos_ - Input.targetPrevPos_).GetNormal();

	// 当たり判定を行う。
	bool isHit = false;
	Vec3 impactPos;
	float hitDistance;
	Vec3 hitNormal;
	Vec2 hitUV;
	isHit = FHelper::RayToModelCollision(InputRayData, impactPos, hitDistance, hitNormal, hitUV);

	// 当たった距離がY軸のサイズよりも小さかったら。
	isHit &= fabs(hitDistance) < (Input.targetPos_ - Input.targetPrevPos_).Length();

	// 当たっていたら押し戻す。
	if (isHit) {

		// 法線方向に当たった分押し戻す。
		Input.targetPos_ = impactPos + hitNormal * hitDistance;
		Output.isHitOrnament_ = true;
		Output.ornamentHitNormal_ = hitNormal;

	}

	return Output;

}

void StageObjectMgr::RotObliqueFloor(BaseStage::ColliderInput Input, const Vec3& HitNormal, BaseStage::ColliderOutput& Output)
{

	/*===== 斜め床の回転処理 =====*/

	/*-- 外積から回転軸を取得 --*/

	// デフォルトの上ベクトル。
	Vec3 defUpVec = Vec3(0, 1, 0);

	// 回転軸を求める。
	Vec3 axisOfRevolution = defUpVec.Cross(HitNormal);

	// 回転軸を正規化する。
	if (axisOfRevolution.Length() != 0) {
		axisOfRevolution.Normalize();
	}

	/*-- 内積から回転量を取得 --*/

	// 回転量を求める。
	float amountOfRotation = defUpVec.Dot(HitNormal);

	// 逆余弦を求める関数を使用して求めたcosθをラジアンに変換。
	amountOfRotation = acosf(amountOfRotation);


	/*-- クォータニオンを使って回転 --*/

	// 回転軸が{0,0,0}だったら処理を飛ばす。
	if (axisOfRevolution.Length() != 0 && Vec3(HitNormal).Length() != 0) {

		// クォータニオンを求める。
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(axisOfRevolution.ConvertXMVECTOR(), amountOfRotation);

		// 求めたクォータニオンを行列に治す。
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// 回転させる。
		PolygonInstanceRegister::Ins()->ChangeRotate(Input.targetInsIndex_, quaternionMat);

		// 法線ベクトル
		Vec3 normal_ = HitNormal;

		if (!Input.isInvalidateRotY_) {

			// 上ベクトルを基準としたクォータニオンを求める。
			DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), Input.targetRotY_);

			// クォータニオンを行列に治す。
			DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

			// プレイヤーを回転させる。
			PolygonInstanceRegister::Ins()->AddRotate(Input.targetInsIndex_, upQuaternionMat);

		}


		/*-- プレイヤーの回転行列をもとに各ベクトルを回転 --*/

		// 回転行列を取得。
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(Input.targetInsIndex_);

		// 上ベクトルを更新。
		Output.upVec_ = normal_;

		//正面ベクトルを更新。
		Output.forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

	// 当たった法線が上だった場合は行列を初期化して回転させる。
	if (HitNormal == Vec3(0, 1, 0)) {

		// プレイヤーを回転させる。
		PolygonInstanceRegister::Ins()->ChangeRotate(Input.targetInsIndex_, DirectX::XMMatrixIdentity());

		// 法線ベクトル
		Vec3 normal_ = HitNormal;

		if (!Input.isInvalidateRotY_) {

			// 上ベクトルを基準としたクォータニオンを求める。
			DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), Input.targetRotY_);

			// クォータニオンを行列に治す。
			DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

			// 回転させる。
			PolygonInstanceRegister::Ins()->AddRotate(Input.targetInsIndex_, upQuaternionMat);

		}


		/*-- 回転行列をもとに各ベクトルを回転 --*/

		// 回転行列を取得。
		DirectX::XMMATRIX rotationMatBuff = PolygonInstanceRegister::Ins()->GetRotate(Input.targetInsIndex_);

		// 上ベクトルを更新。
		Output.upVec_ = normal_;

		//正面ベクトルを更新。
		Output.forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), rotationMatBuff);

	}

}


/*

・甲羅を後ろに持つ機能を追加。
・甲羅とプレイヤーの当たり判定を追加。
　→当たった際のエフェクトも追加。
・アイテムボックスを追加。

*/