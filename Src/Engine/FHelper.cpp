#include "FHelper.h"

Vec3 FHelper::CalBary(const Vec3& PosA, const Vec3& PosB, const Vec3& PosC, const Vec3& TargetPos)
{

	/*===== 重心座標を求める =====*/

	Vec3 uvw = Vec3();

	// 三角形の面積を求める。
	float areaABC = (PosC - PosA).Cross(PosB - PosA).Length() / 2.0f;

	// 重心座標を求める。
	uvw.x_ = ((PosA - TargetPos).Cross(PosB - TargetPos).Length() / 2.0f) / areaABC;
	uvw.y_ = ((PosB - TargetPos).Cross(PosC - TargetPos).Length() / 2.0f) / areaABC;
	uvw.z_ = ((PosC - TargetPos).Cross(PosA - TargetPos).Length() / 2.0f) / areaABC;

	return uvw;
}

bool FHelper::RayToModelCollision(RayToModelCollisionData CollisionData, Vec3& ImpactPos, float& Distance, Vec3& HitNormal, Vec2& HitUV)
{

	/*----- targetからポリゴンを抜き取る -----*/

	struct CheckHitVertex {

		Vec3 pos_;
		Vec3 normal_;
		Vec2 uv_;

	};

	// レイとの当たり判定用のポリゴン構造体
	struct CheckHitPorygon {
		bool isActive_ = true;
		CheckHitVertex p1_;
		CheckHitVertex p2_;
		CheckHitVertex p3_;
	};

	// あたっているポリゴンのデータを保存するよう変数	衝突地点、距離、衝突面の法線
	struct HitPorygonData
	{
		Vec3 pos_;
		float distance_;
		Vec3 normal_;
		CheckHitPorygon targetPolygon_;
	};

	std::vector<HitPorygonData> hitPolygon{};

	std::vector<CheckHitPorygon> targetPolygon;		//ポリゴン保存用コンテナ

	// targetのポリゴン数に合わせてリサイズ
	targetPolygon.resize(static_cast<unsigned __int64>(static_cast<float>(CollisionData.targetVertex_.size()) / 3.0f));

	// ポリゴンの中身を代入
	for (auto& index : targetPolygon) {

		// 頂点1
		index.p1_.pos_ = CollisionData.targetVertex_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3)])];
		index.p1_.normal_ = CollisionData.targetNormal_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3)])];
		index.p1_.uv_ = CollisionData.targetUV_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3)])];
		// 頂点2
		index.p2_.pos_ = CollisionData.targetVertex_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3 + 1)])];
		index.p2_.normal_ = CollisionData.targetNormal_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3 + 1)])];
		index.p2_.uv_ = CollisionData.targetUV_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3 + 1)])];
		// 頂点3
		index.p3_.pos_ = CollisionData.targetVertex_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3 + 2)])];
		index.p3_.normal_ = CollisionData.targetNormal_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3 + 2)])];
		index.p3_.uv_ = CollisionData.targetUV_[static_cast<UINT>(CollisionData.targetIndex_[static_cast<UINT>((&index - &targetPolygon[0]) * 3 + 2)])];
		// 有効化フラグ
		index.isActive_ = true;
	}

	/*----- 保存したポリゴンの頂点座標にワールド変換行列をかける -----*/
	// ワールド行列
	DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
	matWorld *= CollisionData.matScale_;
	matWorld *= CollisionData.matRot_;
	matWorld *= CollisionData.matTrans_;
	for (auto& index : targetPolygon) {
		// 頂点を変換
		index.p1_.pos_ = DirectX::XMVector3Transform(index.p1_.pos_.ConvertXMVECTOR(), matWorld);
		index.p2_.pos_ = DirectX::XMVector3Transform(index.p2_.pos_.ConvertXMVECTOR(), matWorld);
		index.p3_.pos_ = DirectX::XMVector3Transform(index.p3_.pos_.ConvertXMVECTOR(), matWorld);
		// 法線を回転行列分だけ変換
		index.p1_.normal_ = XMVector3Transform(index.p1_.normal_.ConvertXMVECTOR(), CollisionData.matRot_);
		index.p1_.normal_.Normalize();
		index.p2_.normal_ = XMVector3Transform(index.p2_.normal_.ConvertXMVECTOR(), CollisionData.matRot_);
		index.p2_.normal_.Normalize();
		index.p3_.normal_ = XMVector3Transform(index.p3_.normal_.ConvertXMVECTOR(), CollisionData.matRot_);
		index.p3_.normal_.Normalize();
	}

	/*----- レイの方向と法線が同じ方向なら除外 -----*/
	for (auto& index : targetPolygon) {
		// まずは1つ目の頂点をチェック
		if (-0.0001f < index.p1_.normal_.Dot(CollisionData.rayDir_)) {
			// ここまで来たら完全に反対側を向いているので、削除フラグを建てる
			index.isActive_ = false;
			continue;
		}
	}

	/*----- ポリゴンの3頂点と境界球の当たり判定を行い、一定以上距離があった場合は処理を飛ばす -----*/

	/*----- ポリゴンごとに当たり判定 -----*/
	// 此処から先はポリゴンごとに計算する
	for (auto& index : targetPolygon) {
		/*----- レイと平面の衝突点を計算する -----*/

		// ポリゴンが無効化されていたら次の処理へ
		if (!index.isActive_) continue;

		// レイの開始地点から平面におろした垂線の長さを求める
		Vec3 planeNorm = -index.p1_.normal_;
		float rayToOriginLength = CollisionData.rayPos_.Dot(planeNorm);
		float planeToOriginLength = index.p1_.pos_.Dot(planeNorm);
		// 視点から平面におろした垂線の長さ
		float perpendicularLine = rayToOriginLength - planeToOriginLength;

		// 三角関数を利用して視点から衝突店までの距離を求める
		float dist = planeNorm.Dot(CollisionData.rayDir_);
		float impDistance = perpendicularLine / -dist;

		// 衝突地点
		Vec3 impactPoint = CollisionData.rayPos_ + CollisionData.rayDir_ * impDistance;

		/*----- 衝突点がポリゴンの内側にあるかを調べる -----*/
		Vec3 m;

		/* 辺1本目 */
		Vec3 P1ToImpactPos = (impactPoint - index.p1_.pos_).GetNormal();
		Vec3 P1ToP2 = (index.p2_.pos_ - index.p1_.pos_).GetNormal();
		Vec3 P1ToP3 = (index.p3_.pos_ - index.p1_.pos_).GetNormal();

		// 衝突点と辺1の内積
		float impactDot = P1ToImpactPos.Dot(P1ToP2);
		// 点1と点3の内積
		float P1Dot = P1ToP2.Dot(P1ToP3);

		// 衝突点と辺1の内積が点1と点3の内積より小さかったらアウト
		if (impactDot < P1Dot) {
			index.isActive_ = false;
			continue;
		}

		/* 辺2本目 */
		Vec3 P2ToImpactPos = (impactPoint - index.p2_.pos_).GetNormal();
		Vec3 P2ToP3 = (index.p3_.pos_ - index.p2_.pos_).GetNormal();
		Vec3 P2ToP1 = (index.p1_.pos_ - index.p2_.pos_).GetNormal();

		// 衝突点と辺2の内積
		impactDot = P2ToImpactPos.Dot(P2ToP3);
		// 点2と点1の内積
		float P2Dot = P2ToP3.Dot(P2ToP1);

		// 衝突点と辺2の内積が点2と点1の内積より小さかったらアウト
		if (impactDot < P2Dot) {
			index.isActive_ = false;
			continue;
		}

		/* 辺3本目 */
		Vec3 P3ToImpactPos = (impactPoint - index.p3_.pos_).GetNormal();
		Vec3 P3ToP1 = (index.p1_.pos_ - index.p3_.pos_).GetNormal();
		Vec3 P3ToP2 = (index.p2_.pos_ - index.p3_.pos_).GetNormal();

		// 衝突点と辺3の内積
		impactDot = P3ToImpactPos.Dot(P3ToP1);
		// 点3と点2の内積
		float P3Dot = P3ToP1.Dot(P3ToP2);

		// 衝突点と辺3の内積が点3と点2の内積より小さかったらアウト
		if (impactDot < P3Dot) {
			index.isActive_ = false;
			continue;
		}

		/* ここまで来たらポリゴンに衝突してる！ */
		HitPorygonData hitPorygonData;
		hitPorygonData.pos_ = impactPoint;
		hitPorygonData.distance_ = impDistance;
		hitPorygonData.normal_ = index.p1_.normal_;
		hitPorygonData.targetPolygon_ = index;
		hitPolygon.push_back(hitPorygonData);
	}

	// hitPorygonの値が1以上だったら距離が最小の要素を検索
	if (1 <= hitPolygon.size()) {
		// 距離が最小の要素を検索
		int min = 0;
		float minDistance = 100000;
		for (auto& index : hitPolygon) {
			if (fabs(index.distance_) < fabs(minDistance)) {
				minDistance = index.distance_;
				min = &index - &hitPolygon[0];
			}
		}

		// 検索した最小値を代入してreturn
		ImpactPos = hitPolygon[min].pos_;
		Distance = hitPolygon[min].distance_;
		HitNormal = hitPolygon[min].normal_;

		// 重心座標を求める。
		Vec3 bary = FHelper::CalBary(hitPolygon[min].targetPolygon_.p1_.pos_, hitPolygon[min].targetPolygon_.p2_.pos_, hitPolygon[min].targetPolygon_.p3_.pos_, ImpactPos);

		Vec3 baryBuff = bary;

		// UVWの値がずれるので修正。
		bary.x_ = baryBuff.y_;
		bary.y_ = baryBuff.z_;
		bary.z_ = baryBuff.x_;

		Vec2 uv = Vec2();

		// 重心座標からUVを求める。
		uv.x_ += hitPolygon[min].targetPolygon_.p1_.uv_.x_ * bary.x_;
		uv.x_ += hitPolygon[min].targetPolygon_.p2_.uv_.x_ * bary.y_;
		uv.x_ += hitPolygon[min].targetPolygon_.p3_.uv_.x_ * bary.z_;
		
		uv.y_ += hitPolygon[min].targetPolygon_.p1_.uv_.y_ * bary.x_;
		uv.y_ += hitPolygon[min].targetPolygon_.p2_.uv_.y_ * bary.y_;
		uv.y_ += hitPolygon[min].targetPolygon_.p3_.uv_.y_ * bary.z_;

		HitUV = uv;

		return true;
	}
	else {
		ImpactPos = Vec3{ -1,-1,-1 };
		Distance = -1;
		HitNormal = Vec3{ -1,-1,-1 };
		return false;
	}
}
