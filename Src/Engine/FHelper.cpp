#include "FHelper.h"

bool FHelper::RayToModelCollision(RayToModelCollisionData CollisionData, Vec3& ImpactPos, float& Distance, Vec3& HitNormal)
{

	// あたっているポリゴンのデータを保存するよう変数	衝突地点、距離、衝突面の法線
	struct HitPorygonData
	{
		Vec3 pos_;
		float distance;
		Vec3 normal_;
	};

	std::vector<HitPorygonData> hitPorygon{};

	/*----- targetからポリゴンを抜き取る -----*/

	struct CheckHitVertex {

		Vec3 pos_;
		Vec3 normal_;

	};

	// レイとの当たり判定用のポリゴン構造体
	struct CheckHitPorygon {
		bool isActive_ = true;
		CheckHitVertex p1;
		CheckHitVertex p2;
		CheckHitVertex p3;
	};

	std::vector<CheckHitPorygon> targetPorygon;		//ポリゴン保存用コンテナ

	// targetのポリゴン数に合わせてリサイズ
	targetPorygon.resize(static_cast<unsigned __int64>(static_cast<float>(CollisionData.targetVertex.size()) / 3.0f));

	// ポリゴンの中身を代入
	int targetPorygonSize = static_cast<int>(targetPorygon.size());
	for (int index_ = 0; index_ < targetPorygonSize; ++index_) {

		// 頂点1
		targetPorygon[index_].p1.pos_ = CollisionData.targetVertex[static_cast<UINT>(CollisionData.targetIndex[static_cast<UINT>(index_ * 3)])];
		targetPorygon[index_].p1.normal_ = CollisionData.targetNormal[static_cast<UINT>(CollisionData.targetIndex[static_cast<UINT>(index_ * 3)])];
		// 頂点2
		targetPorygon[index_].p2.pos_ = CollisionData.targetVertex[static_cast<UINT>(CollisionData.targetIndex[static_cast<UINT>(index_ * 3 + 1)])];
		targetPorygon[index_].p2.normal_ = CollisionData.targetNormal[static_cast<UINT>(CollisionData.targetIndex[static_cast<UINT>(index_ * 3 + 1)])];
		// 頂点3
		targetPorygon[index_].p3.pos_ = CollisionData.targetVertex[static_cast<UINT>(CollisionData.targetIndex[static_cast<UINT>(index_ * 3 + 2)])];
		targetPorygon[index_].p3.normal_ = CollisionData.targetNormal[static_cast<UINT>(CollisionData.targetIndex[static_cast<UINT>(index_ * 3 + 2)])];
		// 有効化フラグ
		targetPorygon[index_].isActive_ = true;
	}

	/*----- 保存したポリゴンの頂点座標にワールド変換行列をかける -----*/
	// ワールド行列
	DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
	matWorld *= CollisionData.matScale;
	matWorld *= CollisionData.matRot;
	matWorld *= CollisionData.matTrans;
	targetPorygonSize = static_cast<int>(targetPorygon.size());
	for (int index_ = 0; index_ < targetPorygonSize; ++index_) {
		// 頂点を変換
		targetPorygon[index_].p1.pos_ = DirectX::XMVector3Transform(targetPorygon[index_].p1.pos_.ConvertXMVECTOR(), matWorld);
		targetPorygon[index_].p2.pos_ = DirectX::XMVector3Transform(targetPorygon[index_].p2.pos_.ConvertXMVECTOR(), matWorld);
		targetPorygon[index_].p3.pos_ = DirectX::XMVector3Transform(targetPorygon[index_].p3.pos_.ConvertXMVECTOR(), matWorld);
		// 法線を回転行列分だけ変換
		targetPorygon[index_].p1.normal_ = XMVector3Transform(targetPorygon[index_].p1.normal_.ConvertXMVECTOR(), CollisionData.matRot);
		targetPorygon[index_].p1.normal_.Normalize();
		targetPorygon[index_].p2.normal_ = XMVector3Transform(targetPorygon[index_].p2.normal_.ConvertXMVECTOR(), CollisionData.matRot);
		targetPorygon[index_].p2.normal_.Normalize();
		targetPorygon[index_].p3.normal_ = XMVector3Transform(targetPorygon[index_].p3.normal_.ConvertXMVECTOR(), CollisionData.matRot);
		targetPorygon[index_].p3.normal_.Normalize();
	}

	/*----- レイの方向と法線が同じ方向なら除外 -----*/
	for (int index_ = 0; index_ < targetPorygonSize; ++index_) {
		// まずは1つ目の頂点をチェック
		if (-0.0001f < targetPorygon[index_].p1.normal_.Dot(CollisionData.rayDir)) {
			// ここまで来たら完全に反対側を向いているので、削除フラグを建てる
			targetPorygon[index_].isActive_ = false;
			continue;
		}
	}

	/*----- ポリゴンの3頂点と境界球の当たり判定を行い、一定以上距離があった場合は処理を飛ばす -----*/

	/*----- ポリゴンごとに当たり判定 -----*/
	// 此処から先はポリゴンごとに計算する
	for (int index_ = 0; index_ < targetPorygonSize; ++index_) {
		/*----- レイと平面の衝突点を計算する -----*/

		// ポリゴンが無効化されていたら次の処理へ
		if (!targetPorygon[index_].isActive_) continue;

		// レイの開始地点から平面におろした垂線の長さを求める
		Vec3 planeNorm = -targetPorygon[index_].p1.normal_;
		float rayToOriginLength = CollisionData.rayPos.Dot(planeNorm);
		float planeToOriginLength = targetPorygon[index_].p1.pos_.Dot(planeNorm);
		// 視点から平面におろした垂線の長さ
		float perpendicularLine = rayToOriginLength - planeToOriginLength;

		// 三角関数を利用して視点から衝突店までの距離を求める
		float dist = planeNorm.Dot(CollisionData.rayDir);
		float impDistance = perpendicularLine / -dist;

		// 衝突地点
		Vec3 impactPoint = CollisionData.rayPos + CollisionData.rayDir * impDistance;

		/*----- 衝突点がポリゴンの内側にあるかを調べる -----*/
		Vec3 m;

		/* 辺1本目 */
		Vec3 P1ToImpactPos = (impactPoint - targetPorygon[index_].p1.pos_).GetNormal();
		Vec3 P1ToP2 = (targetPorygon[index_].p2.pos_ - targetPorygon[index_].p1.pos_).GetNormal();
		Vec3 P1ToP3 = (targetPorygon[index_].p3.pos_ - targetPorygon[index_].p1.pos_).GetNormal();

		// 衝突点と辺1の内積
		float impactDot = P1ToImpactPos.Dot(P1ToP2);
		// 点1と点3の内積
		float P1Dot = P1ToP2.Dot(P1ToP3);

		// 衝突点と辺1の内積が点1と点3の内積より小さかったらアウト
		if (impactDot < P1Dot) {
			targetPorygon.at(index_).isActive_ = false;
			continue;
		}

		/* 辺2本目 */
		Vec3 P2ToImpactPos = (impactPoint - targetPorygon[index_].p2.pos_).GetNormal();
		Vec3 P2ToP3 = (targetPorygon[index_].p3.pos_ - targetPorygon[index_].p2.pos_).GetNormal();
		Vec3 P2ToP1 = (targetPorygon[index_].p1.pos_ - targetPorygon[index_].p2.pos_).GetNormal();

		// 衝突点と辺2の内積
		impactDot = P2ToImpactPos.Dot(P2ToP3);
		// 点2と点1の内積
		float P2Dot = P2ToP3.Dot(P2ToP1);

		// 衝突点と辺2の内積が点2と点1の内積より小さかったらアウト
		if (impactDot < P2Dot) {
			targetPorygon.at(index_).isActive_ = false;
			continue;
		}

		/* 辺3本目 */
		Vec3 P3ToImpactPos = (impactPoint - targetPorygon[index_].p3.pos_).GetNormal();
		Vec3 P3ToP1 = (targetPorygon[index_].p1.pos_ - targetPorygon[index_].p3.pos_).GetNormal();
		Vec3 P3ToP2 = (targetPorygon[index_].p2.pos_ - targetPorygon[index_].p3.pos_).GetNormal();

		// 衝突点と辺3の内積
		impactDot = P3ToImpactPos.Dot(P3ToP1);
		// 点3と点2の内積
		float P3Dot = P3ToP1.Dot(P3ToP2);

		// 衝突点と辺3の内積が点3と点2の内積より小さかったらアウト
		if (impactDot < P3Dot) {
			targetPorygon.at(index_).isActive_ = false;
			continue;
		}

		/* ここまで来たらポリゴンに衝突してる！ */
		HitPorygonData hitPorygonData;
		hitPorygonData.pos_ = impactPoint;
		hitPorygonData.distance = impDistance;
		hitPorygonData.normal_ = targetPorygon[index_].p1.normal_;
		hitPorygon.push_back(hitPorygonData);
	}

	// hitPorygonの値が1以上だったら距離が最小の要素を検索
	if (1 <= hitPorygon.size()) {
		// 距離が最小の要素を検索
		int min = 0;
		float minDistance = 100000;
		int counter = 0;
		for (auto& index_ : hitPorygon) {
			if (fabs(index_.distance) < fabs(minDistance)) {
				minDistance = index_.distance;
				min = counter;
				++counter;
			}
		}

		//検索した最小値を代入してreturn
		ImpactPos = hitPorygon[min].pos_;
		Distance = hitPorygon[min].distance;
		HitNormal = hitPorygon[min].normal_;
		return true;
	}
	else {
		ImpactPos = Vec3{ -1,-1,-1 };
		Distance = -1;
		HitNormal = Vec3{ -1,-1,-1 };
		return false;
	}
}
