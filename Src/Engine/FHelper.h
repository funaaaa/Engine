#pragma once
#include "WindowsAPI.h"

#include <DirectXMath.h>
#include "Vec.h"
#include <vector>
#include <wrl.h>
#include <d3d12.h>

//DirectXに関する便利機能をまとめたもの
namespace FHelper {

	/*----- 便利変数 -----*/

	static const int COLOR_MAX = 255;										// 色の最大値
	static const Vec3 COLOR_RED_VIVID = Vec3(COLOR_MAX, 0, 0);				// RGB振り切りの赤
	static const Vec3 COLOR_GREEN_VIVID = Vec3(0, COLOR_MAX, 0);			// RGB振り切りの緑
	static const Vec3 COLOR_BLUE_VIVID = Vec3(0, 0, COLOR_MAX);				// RGB振り切りの青

	static const float F_PI_F = 3.14159265359f;								// 円周率 180度
	static const float F_PI_DIV_F = F_PI_F / 2.0f;							// 円周率 90度
	static const float F_PI_3DIV_F = F_PI_F + F_PI_3DIV_F;					// 円周率 270度
	static const float F_2PI_F = F_PI_F * 2.0f;								// 円周率 360度

	/*----- 便利関数 -----*/

	// 乱数取得
	inline int GetRand(int Min, int Max) {
		return (rand() % (Max - Min + 1)) + Min;
	}
	inline float GetRand(float Min, float Max) {

		// 一旦最小値と最大値を10000倍してint型にする。
		const float MUL = 10000.0f;
		int intMin = static_cast<int>(Min * MUL);
		int intMax = static_cast<int>(Max * MUL);

		int random = (rand() % (intMax - intMin + 1)) + intMin;

		return static_cast<float>(random) / MUL;

	}
	inline float GetRand(float Max) {

		// 一旦最小値と最大値を10000倍してint型にする。
		const float MUL = 10000.0f;
		int intMax = static_cast<int>(Max * MUL);

		int random = rand() % (intMax + 1);

		return static_cast<float>(random) / MUL;

	}
	inline Vec3 GetRandVec3(int Min, int Max) {
		return Vec3(static_cast<float>(GetRand(Min, Max)), static_cast<float>(GetRand(Min, Max)), static_cast<float>(GetRand(Min, Max)));
	}

	// 指定の値の指定の値の範囲での0~1の割合を求める
	inline float CalPercentageZeroToOne(float NowValue, float MinValue, float MaxValue) {
		float maxBuff = MaxValue;
		float nowValueBuff = NowValue;
		// 最小値が0未満の場合は最大値に足す
		if (MinValue < 0) {
			maxBuff += fabs(MinValue);
			nowValueBuff += fabs(MinValue);
		}
		// 最小値が0より大きい場合は最大値から引く
		else if (0 < MinValue) {
			maxBuff -= fabs(MinValue);
			nowValueBuff -= fabs(MinValue);
		}
		// 丁度0の場合はそのまま
		else {
		}

		float returnBuff = nowValueBuff / maxBuff;

		// 0以下1以上にならないようにする
		if (returnBuff <= 0) returnBuff = 0;
		if (1 <= returnBuff) returnBuff = 1;

		return returnBuff;

	}

	// 01に納める。
	inline float Saturate(float Value) {
		float value = Value;
		if (value < 0.0f) value = 0.0f;
		if (1.0f < value) value = 1.0f;
		return value;
	}
	inline float Clamp(float Value, float Min, float Max) {
		if (Value < Min) Value = Min;
		if (Max < Value) Value = Max;
		return Value;
	}

	// 画面中央
	inline Vec3 WindowCenterPos() {
		return Vec3(static_cast<float>(WINDOW_WIDTH) / 2.0f, static_cast<float>(WINDOW_HEIGHT) / 2.0f, 0);
	}

	// 画面サイズの半分
	inline Vec2 WindowHalfSize() {
		return Vec2(static_cast<float>(WINDOW_WIDTH) / 2.0f, static_cast<float>(WINDOW_HEIGHT) / 2.0f);
	}

	// ベクトルに回転行列をかけたを値を正規化して返す
	inline Vec3 MulRotationMatNormal(const Vec3& FormatVec, const DirectX::XMMATRIX& RotationMat) {
		Vec3 formatVecBuff = FormatVec;
		// 回転行列をかける
		formatVecBuff = DirectX::XMVector3Transform(formatVecBuff.ConvertXMVECTOR(), RotationMat);
		// 正規化する
		formatVecBuff.Normalize();

		return formatVecBuff;
	}
	// ベクトルに行列をかけた値を返す。
	inline Vec3 MulMat(Vec3 Vec, const DirectX::XMMATRIX& Mat) {
		return DirectX::XMVector3Transform(Vec.ConvertXMVECTOR(), Mat);
	}

	// 回転行列を生成する
	inline DirectX::XMMATRIX CalRotationMat(const Vec3& Value) {
		DirectX::XMMATRIX returnBuff = DirectX::XMMatrixIdentity();
		returnBuff *= DirectX::XMMatrixRotationZ(Value.z_);
		returnBuff *= DirectX::XMMatrixRotationX(Value.x_);
		returnBuff *= DirectX::XMMatrixRotationY(Value.y_);
		return returnBuff;
	};

	// スケール行列を生成する
	inline DirectX::XMMATRIX CalScaleMat(const Vec3& Value) {
		DirectX::XMMATRIX returnBuff = DirectX::XMMatrixIdentity();
		returnBuff *= DirectX::XMMatrixScaling(Value.x_, Value.y_, Value.z_);
		return returnBuff;
	};

	// 画面内に収まっているかを判定
	inline bool CheckInScreen(const Vec3& CheckPos, float VirtualWidth, float VirtualHeight, const DirectX::XMMATRIX& MatView, const DirectX::XMMATRIX& MatPerspective) {
		float w = WINDOW_WIDTH / 2.0f;
		float h = WINDOW_HEIGHT / 2.0f;
		DirectX::XMMATRIX viewport = {
		w, 0, 0, 0,
		0, -h, 0, 0,
		0, 0, 1, 0,
		w, h, 0, 1
		};

		// ビュー変換とプロジェクション変換
		Vec3 worldPos = CheckPos;
		worldPos = DirectX::XMVector3Transform(worldPos.ConvertXMVECTOR(), MatView);
		worldPos = DirectX::XMVector3Transform(worldPos.ConvertXMVECTOR(), MatPerspective);

		Vec3 tmp;
		tmp = worldPos;

		// Zが-だと視錐台に収まっていない。
		if (tmp.z_ < -1000) {
			return false;
		}

		// zで割って-1~1の範囲に収める
		// スクリーン変換
		Vec3 viewVec = DirectX::XMVectorSet(tmp.x_ / tmp.z_, tmp.y_ / tmp.z_, 1.0f, 1.0f);
		viewVec = DirectX::XMVector3Transform(viewVec.ConvertXMVECTOR(), viewport);
		Vec3 pos_ = viewVec;


		if (-VirtualWidth < pos_.x_ && pos_.x_ < WINDOW_WIDTH + VirtualWidth && -VirtualHeight < pos_.y_ && pos_.y_ < WINDOW_HEIGHT + VirtualHeight) {

			return true;
		}
		else {

			return false;

		}
	}

	// FPSを計測
	inline int FPS() {

		static DWORD prevTime = timeGetTime();	// 前回の時間
		static int frameCount = 0;			// フレームカウント
		static int fps = 0;
		DWORD nowTime = timeGetTime();		// 今回のフレームの時間

		frameCount++;	// フレーム数をカウントする

		// 経過時間が１秒を超えたらカウントと時間をリセット
		if (1000 <= nowTime - prevTime)
		{
			prevTime = nowTime;
			fps = frameCount;
			frameCount = 0;
		}

		return fps;

	}

	// バッファを生成。
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType, const wchar_t* Name = nullptr);

	// 三角形の重心座標を求める。
	Vec3 CalBary(const Vec3& PosA, const Vec3& PosB, const Vec3& PosC, const Vec3& TargetPos);

	// レイとオブジェクトの当たり判定
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
	struct RayToModelCollisionData {

		std::vector<FHelper::CheckHitPorygon> targetPolygonData_;
		Vec3 rayPos_;
		Vec3 rayDir_;

	};
	bool RayToModelCollision(RayToModelCollisionData CollisionData, Vec3& ImpactPos, float& Distance, Vec3& HitNormal, Vec2& HitUV);

}

//イージング関係
namespace FEasing {

	/*----- イージング関数 -----*/

	//tは0~1の範囲

	inline float EaseInSine(float Time) {

		return 1.0f - cosf((Time * FHelper::F_PI_F) / 2.0f);

	}
	inline float EaseInCubic(float Time) {

		return Time * Time * Time;

	}

	inline float EaseInOutSine(float Time) {
		return -(cosf(FHelper::F_PI_F * Time) - 1.0f) / 2.0f;
	}

	inline float EaseOutSine(float Time) {

		return sinf((Time * FHelper::F_PI_F) / 2.0f);

	}

	inline float EaseOutCubic(float Time) {
		return 1.0f - powf(1 - Time, 3.0f);
	}

	inline float EaseOutQuar(float Time) {
		return 1.0f - powf(1 - Time, 4.0f);
	}

	inline float EaseInOutQuint(float Time) {
		return Time < 0.5f ? 16.0f * Time * Time * Time * Time * Time : 1.0f - powf(-2.0f * Time + 2.0f, 5.0f) / 2.0f;
	}

	inline float EaseInQuint(float Time) {
		return Time * Time * Time * Time * Time;
	}

	inline float EaseOutQuint(float Time) {
		return 1.0f - powf(1.0f - Time, 5);
	}

	inline float EaseInBack(float Time) {
		float c1 = 1.70158f;
		float c3 = c1 + 1.0f;

		return c3 * Time * Time * Time - c1 * Time * Time;
	}

	inline float EaseOutBack(float Time) {
		float c1 = 1.70158f;
		float c3 = c1 + 1.0f;

		return 1.0f + c3 * std::powf(Time - 1.0f, 3.0f) + c1 * std::powf(Time - 1.0f, 2.0f);
	}

	inline float EaseInOutExpo(float Time) {
		return Time == 0
			? 0
			: Time == 1
			? 1
			: Time < 0.5 ? powf(2, 20 * Time - 10) / 2
			: (2 - powf(2, -20 * Time + 10)) / 2;
	}

	//inline float EaseOutExpo(float Time) {
	//	return Time == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * Time);
	//}

	//inline float EaseInExpo(float Time) {
	//	return Time == 0.0f ? 0.0f : 1.0f - powf(2.0f, 10.0f * Time);
	//}

}