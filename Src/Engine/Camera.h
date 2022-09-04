#pragma once
#include <DirectXMath.h>
#include "Singleton.h"
#include "Vec.h"

class Camera : public Singleton<Camera> {

public:

	DirectX::XMMATRIX matView_;	// ビュー行列
	Vec3 eye_;					// ゲームワールド内でのカメラ座標
	Vec3 target_;				// ゲームワールド内でカメラが見ている座標
	Vec3 up_;					// ゲームワールド内でカメラから見て上方向を指すベクトル

	Vec3 baseEye_;		// 基準となる視点座標 eyeはこの値に向かって補間される。
	Vec3 baseTarget_;	// 基準となる注視点座標 targetはこの値に向かって補間される。
	Vec3 baseUp_;		// 基準となる上ベクトル upはこの値に向かって補間される。

	DirectX::XMMATRIX matPerspective_;
	DirectX::XMMATRIX matProjection_;

	float angleOfView_;		// 画角
	float baseAngleOfView_;	// 基準となる画角の値 angleOfViewをこの値に向かって補間する。

	int convTimer_;		// ゲームが始まってからのフレームを計算する。

private:

	const float BEFORE_START_CAMERA_DISTANCE = 100;	// 開始前のカメラ演出の際のプレイヤーと視点の距離
	const float EYE_PLAYER_DISTANCE = 150;			// プレイヤーと視点の距離
	const float TARGET_PLAYER_DISTNACE = 50;		// プレイヤーと注視点の距離
	const float TARGET_UPPER = 40;					// ターゲットを上にずらす量
	const float DEF_ANGLEOFVIEW = 60.0f;			// 画角のデフォルト値
	const float MAX_ANGLEOFVIEW = 120.0f;			// 最大画角

public:

	// コンストラクタ
	Camera();

	// 初期化
	void Init();

	// ビュー行列生成
	void GenerateMatView();

	// 更新処理
	void Update(const Vec3& CharaPos, const Vec3& CharaForwardVec, const Vec3& CharaUpVec, const float& CharaSpeedPer, const bool& IsBeforeStart, const bool& IsGameFinish);

};