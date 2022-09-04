#include "StageObjectMgr.h"
#include "BasicStageObject.h"
#include "FloatingStageObject.h"
#include "ItemBoxObject.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "OBB.h"
#include "BLAS.h"

void StageObjectMgr::Setting()
{

	/*===== 準備 =====*/

	for (auto& index : objects_) {

		index.second = false;

	}

}

int StageObjectMgr::AddObject(const BaseStageObject::OBJECT_ID& ObjectID, const BaseStageObject::COLLISION_ID& CollisionID, const std::string& DirectryPath, const std::string& ModelName, std::vector<LPCWSTR> TexturePath, const std::wstring& HitGroupName, const UINT& ShaderID)
{

	/*===== ギミックを追加 =====*/

	// 空きオブジェクト検索。
	int addIndex = -1;
	for (auto& index : objects_) {

		if (index.second) continue;

		addIndex = static_cast<int>(&index - &objects_[0]);

		break;

	}

	// -1だったらもう空きはないです。
	if (addIndex == -1) {

		assert(0);

	}

	// ふわふわ動く装飾オブジェクトだったら。
	if (ObjectID == BaseStageObject::OBJECT_ID::FLOATING_ORNAMENT) {

		objects_[addIndex].first = std::make_shared<FloatingStageObject>();

	}
	// アイテムボックスオブジェクトだったら
	else if (ObjectID == BaseStageObject::OBJECT_ID::ITEM_BOX) {

		objects_[addIndex].first = std::make_shared<ItemBoxObject>();

	}
	// それ以外の通常のオブジェクトだったら。
	else {

		objects_[addIndex].first = std::make_shared<BasicStageObject>();

	}

	// Blasをロード
	int blasIndex = BLASRegister::Ins()->GenerateObj(DirectryPath, ModelName, HitGroupName, TexturePath);
	// Instanceを生成。
	int instanceIndex = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex, ShaderID, CollisionID == BaseStageObject::COLLISION_ID::MESH);

	// オブジェクトを設定。
	objects_[addIndex].first->Setting(ObjectID, CollisionID, instanceIndex);
	objects_[addIndex].second = true;

	return addIndex;

}

void StageObjectMgr::Update(const int& Timer)
{

	/*===== 更新処理 =====*/

	for (auto& index : objects_) {

		if (!index.second) continue;

		index.first->Update(Timer);

	}

}

BaseStage::ColliderOutput StageObjectMgr::Collider(BaseStage::ColliderInput Input)
{

	/*==== 当たり判定 =====*/

	// 当たり判定結果用
	BaseStage::ColliderOutput output;
	output.matRot_ = DirectX::XMMatrixIdentity();
	output.isHitBoostGimmick_ = false;
	output.isHitGoal_ = false;
	output.isHitMiddlePoint_ = false;
	output.isHitOrnament_ = false;
	output.isHitStageGrass_ = false;
	output.isHitStage_ = false;
	output.isHitItemBox_ = false;
	output.isHitStepBoostGimmick_ = false;
	output.ornamentHitNormal_ = Vec3(-100, -100, -100);

	for (auto& index : objects_) {

		if (!index.second) continue;
		if (!index.first->GetIsActive()) continue;

		// オブジェクトが当たり判定を行わないものだったら処理を飛ばす。
		BaseStageObject::COLLISION_ID indexCollisionID = index.first->GetCollisionID();
		if (indexCollisionID == BaseStageObject::COLLISION_ID::NONE) continue;

		// 当たり判定がOBBだったら。
		if (indexCollisionID == BaseStageObject::COLLISION_ID::OBB) {

			// 一定以上離れていたら。
			float distance = Vec3(Input.targetOBB_.lock()->pos_ - index.first->GetOBB()->pos_).Length();
			float size = Vec3(Input.targetOBB_.lock()->length_ + index.first->GetOBB()->length_).Length();
			if (size < distance) continue;

			// OBBの当たり判定を行う。
			bool isHit = Input.targetOBB_.lock()->CheckHitOBB(index.first->GetOBB());

			// 当たっていなかったら次の要素へ。
			if (!isHit) continue;

			// オブジェクトのIDを取得して保存。
			BaseStageObject::OBJECT_ID indexObjID = index.first->GetObjectID();

			// オブジェクトごとに保存。
			if (indexObjID == BaseStageObject::OBJECT_ID::BOOST_GIMMICK) {
				output.isHitBoostGimmick_ = true;
			}
			else if (indexObjID == BaseStageObject::OBJECT_ID::GOAL) {
				output.isHitGoal_ = true;
			}
			else if (indexObjID == BaseStageObject::OBJECT_ID::MIDDLE_POINT) {
				output.isHitMiddlePoint_ = true;
			}
			else if (indexObjID == BaseStageObject::OBJECT_ID::STEP_BOOST_GIMMICK) {
				output.isHitStepBoostGimmick_ = true;
			}
			// 当たったオブジェクトがアイテムボックスだったら。
			else if (indexObjID == BaseStageObject::OBJECT_ID::ITEM_BOX && Input.isPlayer_) {

				// アイテムボックスを一時的に無効化。
				index.first->Disable(180);

				output.isHitItemBox_ = true;

			}

		}
		// 当たり判定がMESHだったら。
		else if (indexCollisionID == BaseStageObject::COLLISION_ID::MESH) {

			// 当たり判定用のデータを生成。
			FHelper::RayToModelCollisionData InputRayData;
			InputRayData.targetPolygonData_ = PolygonInstanceRegister::Ins()->GetMeshCollisionData(index.first->GetInstanceIndex());

			// オブジェクトのIDを取得して保存。
			BaseStageObject::OBJECT_ID indexObjID = index.first->GetObjectID();

			// オブジェクトごとに当たり判定を行う
			if (indexObjID == BaseStageObject::OBJECT_ID::STAGE || indexObjID == BaseStageObject::OBJECT_ID::STAGE_GRASS || indexObjID == BaseStageObject::OBJECT_ID::WALL) {
				output = StageMeshCollider(Input, InputRayData, output, indexObjID);
			}
			else if (indexObjID == BaseStageObject::OBJECT_ID::ORNAMENT) {

				// 一定以上離れていたら。 オブジェクトの配置をBlender基準でやっているため距離を頂点から持ってきているが、いずれは手動で配置して座標から距離を持ってこれるようにする
				float distance = Vec3(Input.targetPos_ - PolygonInstanceRegister::Ins()->GetPos(index.first->GetInstanceIndex())).Length();
				float size = Vec3(Input.targetSize_ + index.first->GetOBB()->length_).Length();
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

	objects_[Index].first->AddTrans(Trans);

}

void StageObjectMgr::ChangeTrans(const int& Index, const Vec3& Trans)
{

	/*===== 移動を代入 =====*/

	objects_[Index].first->ChangeTrans(Trans);

}

void StageObjectMgr::AddScale(const int& Index, const Vec3& Scale)
{

	/*===== スケールを加算 =====*/

	objects_[Index].first->AddScale(Scale);

}

void StageObjectMgr::ChangeScale(const int& Index, const Vec3& Scale)
{

	/*===== スケールを代入 =====*/

	objects_[Index].first->ChangeScale(Scale);

}

void StageObjectMgr::AddRotate(const int& Index, const Vec3& Rotate)
{

	/*===== 回転を加算 =====*/

	objects_[Index].first->AddRotate(Rotate);

}

void StageObjectMgr::ChangeRotate(const int& Index, const Vec3& Rotate)
{

	/*===== 回転を代入 =====*/

	objects_[Index].first->ChangeRotate(Rotate);

}

void StageObjectMgr::NonDisplay(const int& Index)
{

	/*===== 非表示 =====*/

	objects_[Index].first->NonDisplay();

}

void StageObjectMgr::Display(const int& Index)
{

	/*===== 表示 =====*/

	objects_[Index].first->Display();

}

void StageObjectMgr::DeleteIndex(const int& Index)
{

	/*===== 要素を削除 =====*/

	if (Index < 0 || static_cast<int>(objects_.size()) <= Index) assert(0);
	if (!objects_[Index].second) assert(0);

	objects_[Index].first->Delete();


}

void StageObjectMgr::ChangeInstanceShaderID(const int& Index, const UINT& ShaderID)
{

	/*===== インスタンスのシェーダーIDを切り替える =====*/

	if (Index < 0 || static_cast<int>(objects_.size()) <= Index) assert(0);
	if (!objects_[Index].second) assert(0);

	// 各要素を保存。
	int blasIndex = PolygonInstanceRegister::Ins()->GetBLASIndex(objects_[Index].first->GetInstanceIndex());
	BaseStageObject::COLLISION_ID CollisionID = objects_[Index].first->GetCollisionID();
	BaseStageObject::OBJECT_ID ObjectID = objects_[Index].first->GetObjectID();

	// Instanceを破棄。
	PolygonInstanceRegister::Ins()->DestroyInstance(objects_[Index].first->GetInstanceIndex());

	// Instanceを生成。
	int instanceIndex = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex, ShaderID, CollisionID == BaseStageObject::COLLISION_ID::MESH);

	// オブジェクトを設定。
	objects_[Index].first->Setting(ObjectID, CollisionID, instanceIndex);

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

	// 前後左右の四方向にレイを飛ばして当たり判定を行う。
	Output = Decision4Way(Input, InputRayData, Output, ObjectID);

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

BaseStage::ColliderOutput StageObjectMgr::Decision4Way(BaseStage::ColliderInput& Input, FHelper::RayToModelCollisionData InputRayData, BaseStage::ColliderOutput Output, BaseStageObject::OBJECT_ID ObjectID)
{

	/*===== 四方向に当たり判定 =====*/

	// 当たり判定の結果保存用変数。
	bool isHit = false;
	Vec3 impactPos;
	float hitDistance;
	Vec3 hitNormal;
	Vec2 hitUV;

	// 貫通防止で正面方向にもレイを飛ばす。
	InputRayData.rayPos_ = Input.targetPos_;
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PolygonInstanceRegister::Ins()->GetRotate(Input.targetInsIndex_));

	// 当たり判定を行う。
	isHit = false;
	isHit = FHelper::RayToModelCollision(InputRayData, impactPos, hitDistance, hitNormal, hitUV);

	// 当たった距離がY軸のサイズよりも小さかったら。
	isHit &= (hitDistance - Input.targetSize_.x_) <= 0;
	isHit &= 0 < hitDistance;

	// 当たっていたら押し戻す。
	if (isHit) {

		Vec3 pushBackVec = (Input.targetPos_ - impactPos).GetNormal();

		// 法線方向に当たった分押し戻す。
		Input.targetPos_ = impactPos + pushBackVec * Input.targetSize_.z_;
		Output.resultPos_ = Input.targetPos_;
		Output.upVec_ = Input.targetUpVec_;

	}

	// 貫通防止で左方向にもレイを飛ばす。
	InputRayData.rayPos_ = Input.targetPos_;
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(Input.targetInsIndex_));

	// 当たり判定を行う。
	isHit = false;
	isHit = FHelper::RayToModelCollision(InputRayData, impactPos, hitDistance, hitNormal, hitUV);

	// 当たった距離がY軸のサイズよりも小さかったら。
	isHit &= (hitDistance - Input.targetSize_.x_) <= 0;
	isHit &= 0 < hitDistance;

	// 当たっていたら押し戻す。
	if (isHit) {

		Vec3 pushBackVec = (Input.targetPos_ - impactPos).GetNormal();

		// 法線方向に当たった分押し戻す。
		Input.targetPos_ = impactPos + pushBackVec * Input.targetSize_.x_;
		Output.resultPos_ = Input.targetPos_;
		Output.upVec_ = Input.targetUpVec_;

	}

	// 貫通防止で右方向にもレイを飛ばす。
	InputRayData.rayPos_ = Input.targetPos_;
	InputRayData.rayDir_ = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), PolygonInstanceRegister::Ins()->GetRotate(Input.targetInsIndex_));

	// 当たり判定を行う。
	isHit = false;
	isHit = FHelper::RayToModelCollision(InputRayData, impactPos, hitDistance, hitNormal, hitUV);

	// 当たった距離がY軸のサイズよりも小さかったら。
	isHit &= (hitDistance - Input.targetSize_.x_) <= 0;
	isHit &= 0 < hitDistance;

	// 当たっていたら押し戻す。
	if (isHit) {

		// 法線方向に当たった分押し戻す。
		Input.targetPos_ += hitNormal * (fabs(hitDistance - Input.targetSize_.x_));
		Output.resultPos_ = Input.targetPos_;
		Output.upVec_ = Input.targetUpVec_;

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
		Output.matRot_ = quaternionMat;
		//PolygonInstanceRegister::Ins()->ChangeRotate(Input.targetInsIndex_, quaternionMat);

		// 法線ベクトル
		Vec3 normal_ = HitNormal;

		if (!Input.isInvalidateRotY_) {

			// 上ベクトルを基準としたクォータニオンを求める。
			DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), Input.targetRotY_);

			// クォータニオンを行列に治す。
			DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

			// プレイヤーを回転させる。
			Output.matRot_ *= upQuaternionMat;
			//PolygonInstanceRegister::Ins()->AddRotate(Input.targetInsIndex_, upQuaternionMat);

		}


		/*-- プレイヤーの回転行列をもとに各ベクトルを回転 --*/

		// 上ベクトルを更新。
		Output.upVec_ = normal_;

		//正面ベクトルを更新。
		Output.forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), Output.matRot_);

	}

	// 当たった法線が上だった場合は行列を初期化して回転させる。
	if (HitNormal == Vec3(0, 1, 0)) {

		// プレイヤーを回転させる。
		Output.matRot_ = DirectX::XMMatrixIdentity();
		//PolygonInstanceRegister::Ins()->ChangeRotate(Input.targetInsIndex_, DirectX::XMMatrixIdentity());

		// 法線ベクトル
		Vec3 normal_ = HitNormal;

		if (!Input.isInvalidateRotY_) {

			// 上ベクトルを基準としたクォータニオンを求める。
			DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), Input.targetRotY_);

			// クォータニオンを行列に治す。
			DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

			// 回転させる。
			Output.matRot_ *= upQuaternionMat;
			//PolygonInstanceRegister::Ins()->AddRotate(Input.targetInsIndex_, upQuaternionMat);

		}


		/*-- 回転行列をもとに各ベクトルを回転 --*/

		// 上ベクトルを更新。
		Output.upVec_ = normal_;

		//正面ベクトルを更新。
		Output.forwardVec_ = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), Output.matRot_);

	}

}


/*

・甲羅を後ろに持つ機能を追加。
・甲羅とプレイヤーの当たり判定を追加。
　→当たった際のエフェクトも追加。
・アイテムボックスを追加。

*/