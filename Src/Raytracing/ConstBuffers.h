#pragma once
#include "Vec.h"
#include <DirectXMath.h>
#include <array>

// 使用する定数バッファ達

// カメラ用定数バッファ
struct CameraConstBufferData {

	DirectX::XMMATRIX mtxView;			// ビュー行列。
	DirectX::XMMATRIX mtxProj;			// プロジェクション行列。
	DirectX::XMMATRIX mtxViewInv;		// ビュー逆行列。
	DirectX::XMMATRIX mtxProjInv;		// プロジェクション逆行列。

	// 初期化処理
	void Init();

};

struct RayLightConstBufferData {

	// ディレクショナルライト用定数バッファ
	struct RayDirLightConstBufferData {

		Vec3 lihgtDir;		// ライトの方向
		int isActive;		// 有効化するかどうかのフラグ
		Vec3 lightColor;	// ライトの色
		float pad;			// パディング

	};

	// 点光源用定数バッファ
	struct RayPointLightData {

		Vec3 lightPos;		// ライトの座標
		float lightSize;	// ライトのサイズ
		Vec3 lightColor;	// ライトの色
		float lightPower;	// ライトの強さ
		int isActive;		// 有効化するかどうかのフラグ
		Vec3 pad;			// パディング

	};

	static const int POINT_LIGHT_COUNT = 10;

	RayDirLightConstBufferData dirLight;						// 並行光源
	std::array<RayPointLightData, POINT_LIGHT_COUNT> pointLight;// 点光源

	// 初期化処理
	void Init();

};

// 大気散乱用定数バッファ
struct AtmosphericScatteringConstBufferData {

	float kr;				// レイリー散乱定数
	float km;				// ミー散乱定数
	float samples;			// 大気散乱サンプル数
	float outerRadius;		// 大気圏の最頂点の高さ
	float innerRadius;		// 地上の高さ
	float eSun;				// 太陽の強さ
	float g;				// 散乱定数を求める際に使用する値
	float aveHeight;		// 平均大気密度を求めるための高さ

	// 初期化処理
	void Init();

};

// デバッグ用定数バッファ
struct DebugConstBufferData {

	int seed;			// 乱数の種
	int counter;		// 累積デノイズ用のサンプル数
	int aoSampleCount;	// AOで何回サンプルするかの数
	int isNoiseScene;	// ノイズのみの画面を描画するか
	int isLightHitScene;// ライトに当たった場所のみを描画するか
	int isNormalScene;	// 法線情報のみを描画するか
	int isMeshScene;	// メッシュ情報のみを描画するか
	int isNoAO;			// AOを行わないか
	int isNoGI;			// GIを行わないか
	int isGIOnlyScene;	// GIのみを行うか
	float pad1;			// パディング
	float pad2;			// パディング

	// 初期化処理
	void Init();

};

// すべての定数バッファを纏めたもの。
struct RayConstBufferData {

	// カメラ
	CameraConstBufferData camera;
	// ライト
	RayLightConstBufferData light;
	// AS用
	AtmosphericScatteringConstBufferData as;
	// デバッグ用
	DebugConstBufferData debug;

	// 初期化処理
	void Init();

};