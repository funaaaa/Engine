#pragma once
#include "Struct.h"
#include "Object3D.h"
#include "WindowsAPI.h"

//DirectXに関する便利機能をまとめたもの
namespace FHelper {

	/*----- 便利変数 -----*/

	static const int COLOR_MAX = 255;												//色の最大値
	static const XMFLOAT3 COLOR_RED_VIVID = XMFLOAT3(COLOR_MAX, 0, 0);				//RGB振り切りの赤
	static const XMFLOAT3 COLOR_GREEN_VIVID = XMFLOAT3(0, COLOR_MAX, 0);			//RGB振り切りの緑
	static const XMFLOAT3 COLOR_BLUE_VIVID = XMFLOAT3(0, 0, COLOR_MAX);				//RGB振り切りの青

	static const float F_PI_F = 3.14159265359;										//円周率 180度
	static const float F_PI_DIV_F = F_PI_F / 2.0f;									//円周率 90度
	static const float F_PI_3DIV_F = F_PI_F + F_PI_3DIV_F;							//円周率 270度
	static const float F_2PI_F = F_PI_F * 2.0f;										//円周率 360度

	/*----- 便利関数 -----*/

	//2D座標軸での二点間の距離を求める
	inline float Distance2D(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return sqrtf(powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f));
	}
	//3D座標軸での二点間の距離を求める
	inline float Distance3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return sqrtf(powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f) + powf(lhs.z - rhs.z, 2.0f));
	}

	//内積を求める
	inline float Dot2D(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}
	inline float Dot3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	//外積を求める
	inline float Cross2D(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}
	inline XMFLOAT3 Cross3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		//タイプミスしてる可能性あり
		return XMFLOAT3(lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x);
	}

	//ベクトルの長さを求める
	inline float Length2D(const XMFLOAT2& rhs) {
		return sqrtf(Dot2D(rhs, rhs));
	}
	inline float Length3D(const XMFLOAT3& rhs) {
		return sqrtf(Dot3D(rhs, rhs));
	}

	//角度を求める
	inline float CalAngle3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return Dot3D(lhs, rhs) / (Length3D(lhs) * Length3D(rhs));
	}

	//ベクトルを正規化する
	inline XMFLOAT2 Normalize2D(const XMFLOAT2& rhs) {
		float len = Length2D(rhs);
		XMFLOAT2 buff = rhs;
		buff.x /= len;
		buff.y /= len;
		return buff;
	}
	inline XMFLOAT3 Normalize3D(const XMFLOAT3& rhs) {
		float len = Length3D(rhs);
		XMFLOAT3 buff = rhs;
		buff.x /= len;
		buff.y /= len;
		buff.z /= len;
		return buff;
	}

	//引数を足す
	inline XMFLOAT2 AddXMFLOAT2(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return XMFLOAT2(lhs.x + rhs.x, lhs.y + rhs.y);
	}
	inline XMFLOAT3 AddXMFLOAT3(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return XMFLOAT3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	//引数を引く
	inline XMFLOAT2 SubXMFLOAT2(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return XMFLOAT2(lhs.x - rhs.x, lhs.y - rhs.y);
	}
	inline XMFLOAT3 SubXMFLOAT3(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		XMFLOAT3 buff = XMFLOAT3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);

		//限りなく0に近かったら0にする
		if (fabs(buff.x) < 0.00001f) {
			buff.x = 0;
		}
		if (fabs(buff.y) < 0.00001f) {
			buff.y = 0;
		}
		if (fabs(buff.z) < 0.00001f) {
			buff.z = 0;
		}

		return buff;
	}

	//引数をかける
	inline XMFLOAT2 MulXMFLOAT2(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return XMFLOAT2(lhs.x * rhs.x, lhs.y * rhs.y);
	}
	inline XMFLOAT3 MulXMFLOAT3(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return XMFLOAT3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
	}

	//引数を割る
	inline XMFLOAT2 DivXMFLOAT2(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
		return XMFLOAT2(lhs.x / rhs.x, lhs.y / rhs.y);
	}
	inline XMFLOAT3 DivXMFLOAT3(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
		return XMFLOAT3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
	}

	//XMVECTOR -> XMFLOAT3
	inline XMFLOAT3 ConvertXMFLOAT3(const XMVECTOR& rhs) {
		XMFLOAT3 returnBuff;
		XMStoreFloat3(&returnBuff, rhs);
		return returnBuff;
	}

	//乱数取得
	inline int GetRand(const int& min, const int& max) {
		return (rand() % (max - min + 1)) + min;
	}
	inline XMFLOAT3 GetRandXMFLOAT3(const int& min, const int& max) {
		return XMFLOAT3(GetRand(min, max), GetRand(min, max), GetRand(min, max));
	}

	//指定の値の指定の値の範囲での0~1の割合を求める
	inline float CalPercentageZeroToOne(const float& nowValue, const float& minValue, const float& maxValue) {
		float maxBuff = maxValue;
		float nowValueBuff = nowValue;
		//最小値が0未満の場合は最大値に足す
		if (minValue < 0) {
			maxBuff += fabs(minValue);
			nowValueBuff += fabs(minValue);
		}
		//最小値が0より大きい場合は最大値から引く
		else if (minValue > 0) {
			maxBuff -= fabs(minValue);
			nowValueBuff -= fabs(minValue);
		}
		//丁度0の場合はそのまま
		else {
		}

		float returnBuff = nowValueBuff / maxBuff;

		//0以下1以上にならないようにする
		if (returnBuff <= 0) returnBuff = 0;
		if (returnBuff >= 1) returnBuff = 1;

		return returnBuff;

	}

	//画面中央
	inline XMFLOAT3 WindowCenterPos() {
		return XMFLOAT3(window_width / 2.0f, window_height / 2.0f, 0);
	}

	//画面サイズの半分
	inline XMFLOAT2 WindowHalfSize() {
		return XMFLOAT2(window_width / 2.0f, window_height / 2.0f);
	}

	//ベクトルに回転行列をかけたを値を正規化して返す
	inline XMFLOAT3 MulRotationMatNormal(const XMFLOAT3& formatVec, const XMMATRIX& rotationMat) {
		XMFLOAT3 formatVecBuff = formatVec;
		XMVECTOR formatVectorBuff = XMLoadFloat3(&formatVecBuff);
		//回転行列をかける
		formatVectorBuff = XMVector3Transform(formatVectorBuff, rotationMat);
		//正規化する
		formatVectorBuff = XMVector3Normalize(formatVectorBuff);
		//ベクトルを更新する
		XMStoreFloat3(&formatVecBuff, formatVectorBuff);

		return formatVecBuff;
	}

	//回転行列を生成する
	inline XMMATRIX CalRotationMat(const XMFLOAT3& value) {
		XMMATRIX returnBuff = XMMatrixIdentity();
		returnBuff *= XMMatrixRotationZ(value.z);
		returnBuff *= XMMatrixRotationX(value.x);
		returnBuff *= XMMatrixRotationY(value.y);
		return returnBuff;
	};

	//ベクトルに回転行列をかける
	inline XMFLOAT3 MulRotationMat(const XMFLOAT3& formatVec, const XMMATRIX& rotationMat) {
		XMFLOAT3 formatVecBuff = formatVec;
		XMVECTOR formatVectorBuff = XMLoadFloat3(&formatVecBuff);
		//回転行列をかける
		formatVectorBuff = XMVector3Transform(formatVectorBuff, rotationMat);
		//ベクトルを更新する
		XMStoreFloat3(&formatVecBuff, formatVectorBuff);

		return formatVecBuff;
	}

	//指定の値を指定の値に近付けた値を返す
	inline float AddDivValue(const float& nowValue, const float& targetValue, const int& divCount) {
		return (targetValue - nowValue) / divCount + nowValue;
	}
	inline XMFLOAT2 AddDivValueXMFLOAT2(const XMFLOAT2& nowValue, const XMFLOAT2& targetValue, const int& divCount) {
		return XMFLOAT2((targetValue.x - nowValue.x) / divCount + nowValue.x,
			(targetValue.y - nowValue.y) / divCount + nowValue.y);
	}
	inline XMFLOAT3 AddDivValueXMFLOAT3(const XMFLOAT3& nowValue, const XMFLOAT3& targetValue, const int& divCount) {
		return XMFLOAT3(AddDivValue(nowValue.x, targetValue.x, divCount),
			AddDivValue(nowValue.y, targetValue.y, divCount),
			AddDivValue(nowValue.z, targetValue.z, divCount));
	}

	//レイとObject3Dの当たり判定
	inline bool RayToModelCollision(const Object3D& target, const Ray& ray, XMFLOAT3& impactPos, float& distance, XMFLOAT3& hitNormal) {
		//あたっているポリゴンのデータを保存するよう変数	衝突地点、距離、衝突面の法線
		vector<tuple<XMFLOAT3, float, XMFLOAT3>> buff{};

		//一時保存用
		Object3D targetBuff = target;
		vector<BaseDrawData::Vertex> targetVertex = targetBuff.GetVertex();
		vector<unsigned short> targetIndex = targetBuff.GetIndex();

		float epsilon = 1.0e-5f;
		/*-----targetからポリゴンを抜き取る-----*/

		//レイとの当たり判定用のポリゴン構造体
		struct Porygon {
			bool isActive = true;
			BaseDrawData::Vertex p1;
			BaseDrawData::Vertex p2;
			BaseDrawData::Vertex p3;
		};

		vector<Porygon> targetPorygon;		//ポリゴン保存用コンテナ

		//targetのポリゴン数に合わせてリサイズ
		targetPorygon.resize(targetBuff.GetIndexSize() / 3.0f);

		//ポリゴンの中身を代入
		int targetPorygonSize = targetPorygon.size();
		for (int i = 0; i < targetPorygonSize; ++i) {
			targetPorygon.at(i).p1 = targetVertex.at(targetIndex.at(i * 3));
			targetPorygon.at(i).p2 = targetVertex.at(targetIndex.at((i * 3) + 1));
			targetPorygon.at(i).p3 = targetVertex.at(targetIndex.at((i * 3) + 2));
			targetPorygon.at(i).isActive = true;
		}

		/*-----保存したポリゴンの頂点座標にワールド変換行列をかける-----*/
		//ワールド行列
		XMMATRIX matWorld = XMMatrixIdentity();
		matWorld *= targetBuff.GetScaleMat();
		matWorld *= targetBuff.GetRotationMat();
		matWorld *= targetBuff.GetPositionMat();
		targetPorygonSize = targetPorygon.size();
		for (int i = 0; i < targetPorygonSize; ++i) {
			//頂点を変換
			XMStoreFloat3(&targetPorygon.at(i).p1.pos, XMVector3Transform(XMLoadFloat3(&targetPorygon.at(i).p1.pos), matWorld));
			XMStoreFloat3(&targetPorygon.at(i).p2.pos, XMVector3Transform(XMLoadFloat3(&targetPorygon.at(i).p2.pos), matWorld));
			XMStoreFloat3(&targetPorygon.at(i).p3.pos, XMVector3Transform(XMLoadFloat3(&targetPorygon.at(i).p3.pos), matWorld));
			//法線を回転行列分だけ変換
			XMStoreFloat3(&targetPorygon.at(i).p1.normal, XMVector3Normalize(XMVector3Transform(XMLoadFloat3(&targetPorygon.at(i).p1.normal), targetBuff.GetRotationMat())));
			XMStoreFloat3(&targetPorygon.at(i).p2.normal, XMVector3Normalize(XMVector3Transform(XMLoadFloat3(&targetPorygon.at(i).p2.normal), targetBuff.GetRotationMat())));
			XMStoreFloat3(&targetPorygon.at(i).p3.normal, XMVector3Normalize(XMVector3Transform(XMLoadFloat3(&targetPorygon.at(i).p3.normal), targetBuff.GetRotationMat())));
		}

		/*-----レイの方向と法線が同じ方向なら除外-----*/
		for (int i = 0; i < targetPorygonSize; ++i) {
			//まずは1つ目の頂点をチェック
			float kari = XMVector3Dot(XMLoadFloat3(&targetPorygon.at(i).p1.normal), ray.dir).m128_f32[0];
			if (XMVector3Dot(XMLoadFloat3(&targetPorygon.at(i).p1.normal), ray.dir).m128_f32[0] > -0.0001f) {
				//ここまで来たら完全に反対側を向いているので、削除フラグを建てる
				targetPorygon.at(i).isActive = false;
				continue;
			}
		}

		/*-----ポリゴンの3頂点と境界球の当たり判定を行い、一定以上距離があった場合は処理を飛ばす-----*/

		/*-----ポリゴンごとに当たり判定-----*/
		//此処から先はポリゴンごとに計算する
		for (int i = 0; i < targetPorygonSize; ++i) {
			/*-----レイと平面の衝突点を計算する-----*/

			//ポリゴンが無効化されていたら次の処理へ
			if (targetPorygon.at(i).isActive == false) continue;

			//レイの開始地点から平面におろした垂線の長さを求める
			XMFLOAT3 planeNorm = { -targetPorygon.at(i).p1.normal.x,-targetPorygon.at(i).p1.normal.y,-targetPorygon.at(i).p1.normal.z };
			float rayToOriginLength = XMVector3Dot(ray.pos, XMLoadFloat3(&planeNorm)).m128_f32[0];
			float planeToOriginLength = XMVector3Dot(XMLoadFloat3(&targetPorygon.at(i).p1.pos), XMLoadFloat3(&planeNorm)).m128_f32[0];
			//視点から平面におろした垂線の長さ
			float perpendicularLine = rayToOriginLength - planeToOriginLength;

			//三角関数を利用して視点から衝突店までの距離を求める
			float dist = XMVector3Dot(XMLoadFloat3(&planeNorm), ray.dir).m128_f32[0];
			float impDistance = perpendicularLine / -dist;

			//distanceが0未満の場合交わっていない
			/*if (impDistance < 0) {
				targetPorygon.at(i).isActive = false;
				continue;
			}*/

			//衝突地点
			XMFLOAT3 impactPoint = { ray.pos.m128_f32[0] + ray.dir.m128_f32[0] * impDistance,
									ray.pos.m128_f32[1] + ray.dir.m128_f32[1] * impDistance,
									ray.pos.m128_f32[2] + ray.dir.m128_f32[2] * impDistance };

			/*-----衝突点がポリゴンの内側にあるかを調べる-----*/
			XMVECTOR m;

			/*辺1本目*/
			XMVECTOR impactToP1 = XMLoadFloat3(&XMFLOAT3(targetPorygon.at(i).p1.pos.x - impactPoint.x, targetPorygon.at(i).p1.pos.y - impactPoint.y, targetPorygon.at(i).p1.pos.z - impactPoint.z));
			XMVECTOR P2ToP1 = XMLoadFloat3(&XMFLOAT3(targetPorygon.at(i).p2.pos.x - targetPorygon.at(i).p1.pos.x, targetPorygon.at(i).p2.pos.y - targetPorygon.at(i).p1.pos.y, targetPorygon.at(i).p2.pos.z - targetPorygon.at(i).p1.pos.z));
			m = XMVector3Cross(impactToP1, P2ToP1);
			//正規化
			m = XMVector3Normalize(m);
			//法線と一致しているか調べる
			float result = XMVector3Dot(m, XMLoadFloat3(&targetPorygon.at(i).p1.normal)).m128_f32[0];
			if (result <= 0) {
				//結果が0以外だったら衝突地点がポリゴンの内側ではないので、衝突していない
				targetPorygon.at(i).isActive = false;
				continue;
			}

			/*辺2本目*/
			XMVECTOR impactToP2 = XMLoadFloat3(&XMFLOAT3(targetPorygon.at(i).p2.pos.x - impactPoint.x, targetPorygon.at(i).p2.pos.y - impactPoint.y, targetPorygon.at(i).p2.pos.z - impactPoint.z));
			XMVECTOR P3ToP2 = XMLoadFloat3(&XMFLOAT3(targetPorygon.at(i).p3.pos.x - targetPorygon.at(i).p2.pos.x, targetPorygon.at(i).p3.pos.y - targetPorygon.at(i).p2.pos.y, targetPorygon.at(i).p3.pos.z - targetPorygon.at(i).p2.pos.z));
			m = XMVector3Cross(impactToP2, P3ToP2);
			//正規化
			m = XMVector3Normalize(m);
			//法線と一致しているか調べる
			result = XMVector3Dot(m, XMLoadFloat3(&targetPorygon.at(i).p1.normal)).m128_f32[0];
			if (result <= 0) {
				//結果が0以外だったら衝突地点がポリゴンの内側ではないので、衝突していない
				targetPorygon.at(i).isActive = false;
				continue;
			}

			/*辺3本目*/
			XMVECTOR impactToP3 = XMLoadFloat3(&XMFLOAT3(targetPorygon.at(i).p3.pos.x - impactPoint.x, targetPorygon.at(i).p3.pos.y - impactPoint.y, targetPorygon.at(i).p3.pos.z - impactPoint.z));
			XMVECTOR P1ToP3 = XMLoadFloat3(&XMFLOAT3(targetPorygon.at(i).p1.pos.x - targetPorygon.at(i).p3.pos.x, targetPorygon.at(i).p1.pos.y - targetPorygon.at(i).p3.pos.y, targetPorygon.at(i).p1.pos.z - targetPorygon.at(i).p3.pos.z));
			m = XMVector3Cross(impactToP3, P1ToP3);
			//正規化
			m = XMVector3Normalize(m);
			//法線と一致しているか調べる
			result = XMVector3Dot(m, XMLoadFloat3(&targetPorygon.at(i).p1.normal)).m128_f32[0];
			if (result <= 0) {
				//結果が0以外だったら衝突地点がポリゴンの内側ではないので、衝突していない
				targetPorygon.at(i).isActive = false;
				continue;
			}

			/*ここまで来たらポリゴンに衝突してる！*/
			tuple<XMFLOAT3, float, XMFLOAT3> buffer{};
			get<0>(buffer) = impactPoint;
			get<1>(buffer) = impDistance;
			get<2>(buffer) = targetPorygon.at(i).p1.normal;
			buff.push_back(buffer);
		}

		//buffの値が1以上だったら距離が最小の要素を検索
		if (buff.size() >= 1) {
			//距離が最小の要素を検索
			int min = 0;
			float minDistance = 100000;
			for (int i = 0; i < buff.size(); ++i) {
				if (fabs(get<1>(buff.at(i))) < fabs(minDistance)) {
					minDistance = get<1>(buff.at(i));
					min = i;
				}
			}

			//検索した最小値を代入してreturn
			impactPos = get<0>(buff.at(min));
			distance = get<1>(buff.at(min));
			hitNormal = get<2>(buff.at(min));
			return true;
		}
		else {
			impactPos = { 0,0,0 };
			distance = 0;
			hitNormal = { 0,0,0 };
			return false;
		}
	}

	//画面内に収まっているかを判定
	inline bool CheckInScreen(const XMFLOAT3& checkPos, const float& virtualWidth, const float& virtualHeight, const XMMATRIX& matView, const XMMATRIX& matPerspective) {
		float w = window_width / 2.0f;
		float h = window_height / 2.0f;
		XMMATRIX viewport = {
		w, 0, 0, 0,
		0, -h, 0, 0,
		0, 0, 1, 0,
		w, h, 0, 1
		};

		// ビュー変換とプロジェクション変換
		XMVECTOR worldPos = XMLoadFloat3(&checkPos);
		worldPos = XMVector3Transform(worldPos, matView);
		worldPos = XMVector3Transform(worldPos, matPerspective);

		XMFLOAT3 temp;
		XMStoreFloat3(&temp, worldPos);
		// zで割って-1~1の範囲に収める
		// スクリーン変換
		XMVECTOR viewVec = XMVectorSet(temp.x / temp.z, temp.y / temp.z, 1.0f, 1.0f);
		viewVec = XMVector3Transform(viewVec, viewport);
		XMFLOAT3 pos{};
		XMStoreFloat3(&pos, viewVec);


		if (pos.x > -virtualWidth && pos.x < window_width + virtualWidth && pos.y > -virtualHeight && pos.y < window_height + virtualHeight) {

			return true;
		}
		else {

			return false;

		}
	}

	//モデルの頂点を法線方向に膨張させる
	inline void ExpansionVertexNormal(Object3D& targetData, const XMFLOAT3& cameraPos, const XMMATRIX& matView, const XMMATRIX& matPerspective, const float expantionValue) {

		/*-- すべての頂点分ループさせる --*/
		Object3D targetDataBuff = targetData;
		int vertexSize = targetDataBuff.GetVertex().size();
		vector<BaseDrawData::Vertex> vertex = targetDataBuff.GetVertex();

		for (int i = 0; i < vertexSize; ++i) {

			//本来の座標にビュー行列を掛ける
			XMVECTOR worldPos = XMLoadFloat3(&vertex[i].pos);
			worldPos = XMVector3Transform(worldPos, matView);
			worldPos = XMVector3Transform(worldPos, matPerspective);

			//膨張させるレートを求める
			float rate = worldPos.m128_f32[2] / worldPos.m128_f32[3];

			if (rate < 0) {
				rate = 0;
			}
			if (rate > 1) {
				rate = 1;
			}

			//移動させる量を求める
			XMFLOAT3 moveValue = {};

			moveValue = { expantionValue * rate, expantionValue * rate, expantionValue * rate };


			//法線方向に移動させる
			vertex[i].pos = FHelper::AddXMFLOAT3(vertex[i].pos, FHelper::MulXMFLOAT3(vertex[i].normal, moveValue));

		}

		//マップ処理を行う
		targetDataBuff.MapVertexData();

	}

	//モデルの頂点を法線方向に膨張させる
	inline void ExpansionVertex(Object3D& targetData, const float expantionValue) {

		/*-- すべての頂点分ループさせる --*/
		Object3D targetDataBuff = targetData;
		int vertexSize = targetDataBuff.GetVertex().size();
		vector<BaseDrawData::Vertex> vertex = targetDataBuff.GetVertex();

		for (int i = 0; i < vertexSize; ++i) {

			//移動させる量を求める
			XMFLOAT3 moveValue = {};

			moveValue = { expantionValue , expantionValue , expantionValue };


			//法線方向に移動させる
			vertex[i].pos = FHelper::AddXMFLOAT3(vertex[i].pos, FHelper::MulXMFLOAT3(vertex[i].normal, moveValue));

		}

		//マップ処理を行う
		targetDataBuff.MapVertexData();

	}

}

//イージング関係
namespace FEasing {

	/*----- イージング関数 -----*/

	//tは0~1の範囲

	inline float EaseInSine(const float& time) {

		return 1.0f - cosf((time * FHelper::F_PI_F) / 2.0f);

	}

	inline float EaseInOutSine(const float& time) {
		return -(cosf(FHelper::F_PI_F * time) - 1.0f) / 2.0f;
	}

	inline float EaseOutSine(const float& time) {

		return sinf((time * FHelper::F_PI_F) / 2.0f);

	}

	inline float EaseOutCubic(const float& time) {
		return 1.0f - powf(1 - time, 3.0f);
	}

	inline float EaseOutQuar(const float& time) {
		return 1.0f - powf(1 - time, 4.0f);
	}

	inline float EaseInOutQuint(const float& time) {
		return time < 0.5f ? 16.0f * time * time * time * time * time : 1.0f - powf(-2.0f * time + 2.0f, 5.0f) / 2.0f;
	}

}