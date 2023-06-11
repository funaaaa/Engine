#pragma once
#include "Vec.h"
#include <DirectXMath.h>
#include <array>

// 使用する定数バッファ達

// カメラ用定数バッファ
struct CameraConstBufferData {

	DirectX::XMMATRIX mtxView_;			// ビュー行列。
	DirectX::XMMATRIX mtxProj_;			// プロジェクション行列。
	DirectX::XMMATRIX mtxViewInv_;		// ビュー逆行列。
	DirectX::XMMATRIX mtxProjInv_;		// プロジェクション逆行列。

	// 初期化処理
	void Init();

};

// デバッグ用情報
struct DebugData {

	Vec3 playerPos_;
	int isDebugMesnInfo_;
	int debugMeshMoveX_;
	float timer_;
	Vec2 pad_;

	// 初期化処理
	void Init();

};

//ボリュームテクスチャ用
struct VolumeTextureData {

	Vec3 pos_;                //ボリュームテクスチャのサイズ
	Vec3 color_;              //フォグの色
	float gridSize_;            //サンプリングするグリッドのサイズ
	float wrapCount_;           //サンプリング座標がはみ出した際に何回までWrapするか
	float samplingLength_;      //サンプリング距離
	float sanplingMaxCount_;    //最大サンプリング数
	float density_;             //濃度係数
	float pad;

	// 初期化処理
	void Init();

};

struct RayLightConstBufferData {

	// ディレクショナルライト用定数バッファ
	struct RayDirLightConstBufferData {

		Vec3 lihgtDir_;		// ライトの方向
		int isActive_;		// 有効化するかどうかのフラグ
		Vec3 lightColor_;	// ライトの色
		int seed_;			// 乱数の種

	};

	// 点光源用定数バッファ
	struct RayPointLightData {

		Vec3 lightPos_;			// ライトの座標
		float lightSize_;		// ライトのサイズ
		Vec3 lightColor_;		// ライトの色
		float lightPower_;		// ライトの強さ
		int isActive_;			// 有効化するかどうかのフラグ
		int isShadow_;			// 影を出さないフラグ
		DirectX::XMFLOAT2 pad_;	// パディング

	};

	static const int POINT_LIGHT_COUNT = 30;

	RayDirLightConstBufferData dirLight_;						// 並行光源
	//std::array<RayPointLightData, POINT_LIGHT_COUNT> pointLight_;// 点光源

	// 初期化処理
	void Init();

};

// アルファ用データ
struct AlphaData {

	int instanceIndex_;
	float alpha_;

};
struct AlphaConstBufferData {

	std::array<AlphaData, 120> alphaData_;

	// 初期化処理
	void Init();

};

// すべての定数バッファを纏めたもの。
struct RayConstBufferData {

	// カメラ
	CameraConstBufferData camera_;
	// ライト
	RayLightConstBufferData light_;
	// アルファ用
	AlphaConstBufferData alphaData_;
	// デバッグ用情報
	DebugData debug_;
	// ボリュームテクスチャ用
	VolumeTextureData volumeTextureData_;

	// 初期化処理
	void Init();

};