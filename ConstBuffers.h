#pragma once
#include "Vec.h"
#include <DirectXMath.h>

// 使用する定数バッファ達

// カメラ用定数バッファ
struct CameraConstBufferData {

	DirectX::XMMATRIX mtxView;			// ビュー行列。
	DirectX::XMMATRIX mtxProj;			// プロジェクション行列。
	DirectX::XMMATRIX mtxViewInv;		// ビュー逆行列。
	DirectX::XMMATRIX mtxProjInv;		// プロジェクション逆行列。
	DirectX::XMVECTOR ambientColor;		// 環境光。

	// 初期化処理
	void Init();

};

// ライト用定数バッファ
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

	RayDirLightConstBufferData dirLight;	// 並行光源
	RayPointLightData pointLight;			// 点光源

	// 初期化処理
	void Init();

};

// 大気散乱用定数バッファ
struct AtmosphericScattering {

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
struct DebugConstBufferData{

	int seed;				// 乱数の種
	int counter;			// 累積デノイズ用のサンプリング数
	int aoSampleCount;		// AOで何回サンプリングするか
	int isNoiseScene;		// ノイズのみの画面を描画するか
	int isLightHitScene;	// ライトに当たった面だけ描画するか
	int isNormalScene;		// 法線だけ描画するか
	int isMeshScene;		// メッシュ情報だけ描画するか
	int isNoAO;				// AOを行わないか
	int isNoGI;				// GIを行わないか
	int isGIOnlyScene;		// GIのみを描画するか
	float pad1;				// パディング
	float pad2;				// パディング

	// 初期化処理
	void Init();

};