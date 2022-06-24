#pragma once
#include "WindowsAPI.h"

#include <DirectXMath.h>
#include "Vec.h"
#include <vector>

//DirectXに関する便利機能をまとめたもの
namespace FHelper {

	/*----- 便利変数 -----*/

	static const int COLOR_MAX = 255;												//色の最大値
	static const Vec3 COLOR_RED_VIVID = Vec3(COLOR_MAX, 0, 0);				//RGB振り切りの赤
	static const Vec3 COLOR_GREEN_VIVID = Vec3(0, COLOR_MAX, 0);			//RGB振り切りの緑
	static const Vec3 COLOR_BLUE_VIVID = Vec3(0, 0, COLOR_MAX);				//RGB振り切りの青

	static const float F_PI_F = 3.14159265359f;										//円周率 180度
	static const float F_PI_DIV_F = F_PI_F / 2.0f;									//円周率 90度
	static const float F_PI_3DIV_F = F_PI_F + F_PI_3DIV_F;							//円周率 270度
	static const float F_2PI_F = F_PI_F * 2.0f;										//円周率 360度

	/*----- 便利関数 -----*/

	//乱数取得
	inline int GetRand(const int& min, const int& max) {
		return (rand() % (max - min + 1)) + min;
	}
	inline float GetRand(const float& min, const float& max) {

		// 一旦最小値と最大値を10000倍してint型にする。
		const float MUL = 10000.0f;
		int intMin = static_cast<int>(min * MUL);
		int intMax = static_cast<int>(max * MUL);

		int random = (rand() % (intMax - intMin + 1)) + intMin;

		return static_cast<float>(random) / MUL;

	}
	inline float GetRand(const float& max) {

		// 一旦最小値と最大値を10000倍してint型にする。
		const float MUL = 10000.0f;
		int intMax = static_cast<int>(max * MUL);

		int random = rand() % (intMax + 1);

		return static_cast<float>(random) / MUL;

	}
	inline Vec3 GetRandVec3(const int& min, const int& max) {
		return Vec3(static_cast<float>(GetRand(min, max)), static_cast<float>(GetRand(min, max)), static_cast<float>(GetRand(min, max)));
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
	inline Vec3 WindowCenterPos() {
		return Vec3(window_width / 2.0f, window_height / 2.0f, 0);
	}

	//画面サイズの半分
	inline DirectX::XMFLOAT2 WindowHalfSize() {
		return DirectX::XMFLOAT2(window_width / 2.0f, window_height / 2.0f);
	}

	//ベクトルに回転行列をかけたを値を正規化して返す
	inline Vec3 MulRotationMatNormal(const Vec3& formatVec, const DirectX::XMMATRIX& rotationMat) {
		Vec3 formatVecBuff = formatVec;
		//回転行列をかける
		formatVecBuff = DirectX::XMVector3Transform(formatVecBuff.ConvertXMVECTOR(), rotationMat);
		//正規化する
		formatVecBuff.Normalize();

		return formatVecBuff;
	}

	//回転行列を生成する
	inline DirectX::XMMATRIX CalRotationMat(const Vec3& value) {
		DirectX::XMMATRIX returnBuff = DirectX::XMMatrixIdentity();
		returnBuff *= DirectX::XMMatrixRotationZ(value.z);
		returnBuff *= DirectX::XMMatrixRotationX(value.x);
		returnBuff *= DirectX::XMMatrixRotationY(value.y);
		return returnBuff;
	};

	//指定の値を指定の値に近付けた値を返す
	inline float AddDivValue(const float& nowValue, const float& targetValue, const int& divCount) {
		return (targetValue - nowValue) / divCount + nowValue;
	}
	inline DirectX::XMFLOAT2 AddDivValueXMFLOAT2(const DirectX::XMFLOAT2& nowValue, const DirectX::XMFLOAT2& targetValue, const int& divCount) {
		return DirectX::XMFLOAT2((targetValue.x - nowValue.x) / divCount + nowValue.x,
			(targetValue.y - nowValue.y) / divCount + nowValue.y);
	}
	inline Vec3 AddDivValueXMFLOAT3(const Vec3& nowValue, const Vec3& targetValue, const int& divCount) {
		return Vec3(AddDivValue(nowValue.x, targetValue.x, divCount),
			AddDivValue(nowValue.y, targetValue.y, divCount),
			AddDivValue(nowValue.z, targetValue.z, divCount));
	}

	//画面内に収まっているかを判定
	inline bool CheckInScreen(const Vec3& checkPos, const float& virtualWidth, const float& virtualHeight, const DirectX::XMMATRIX& matView, const DirectX::XMMATRIX& matPerspective) {
		float w = window_width / 2.0f;
		float h = window_height / 2.0f;
		DirectX::XMMATRIX viewport = {
		w, 0, 0, 0,
		0, -h, 0, 0,
		0, 0, 1, 0,
		w, h, 0, 1
		};

		// ビュー変換とプロジェクション変換
		Vec3 worldPos = checkPos;
		worldPos = DirectX::XMVector3Transform(worldPos.ConvertXMVECTOR(), matView);
		worldPos = DirectX::XMVector3Transform(worldPos.ConvertXMVECTOR(), matPerspective);

		Vec3 tmp;
		tmp = worldPos;
		// zで割って-1~1の範囲に収める
		// スクリーン変換
		Vec3 viewVec = DirectX::XMVectorSet(tmp.x / tmp.z, tmp.y / tmp.z, 1.0f, 1.0f);
		viewVec = DirectX::XMVector3Transform(viewVec.ConvertXMVECTOR(), viewport);
		Vec3 pos = viewVec;


		if (pos.x > -virtualWidth && pos.x < window_width + virtualWidth && pos.y > -virtualHeight && pos.y < window_height + virtualHeight) {

			return true;
		}
		else {

			return false;

		}
	}

	//レイとオブジェクトの当たり判定
	struct RayToModelCollisionData {

		std::vector<Vec3> targetVertex;
		std::vector<UINT> targetIndex;
		std::vector<Vec3> targetNormal;
		DirectX::XMMATRIX matRot;
		DirectX::XMMATRIX matScale;
		DirectX::XMMATRIX matTrans;
		Vec3 rayPos;
		Vec3 rayDir;

	};
	bool RayToModelCollision(RayToModelCollisionData CollisionData, Vec3& ImpactPos, float& Distance, Vec3& HitNormal);

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