#pragma once
#include "Struct.h"
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