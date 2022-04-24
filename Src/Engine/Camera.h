#pragma once
#include <DirectXMath.h>
#include "Singleton.h"

using namespace DirectX;

class Camera : public Singleton<Camera> {
public:
	static XMMATRIX matView;		//ビュー行列
	static XMFLOAT3 eye;			//ゲームワールド内でのカメラ座標
	static XMFLOAT3 target;			//ゲームワールド内でカメラが見ている座標
	static XMFLOAT3 up;				//ゲームワールド内でカメラから見て上方向を指すベクトル

	static XMFLOAT3 honraiEye;		//本来あるべき視点座標
	static XMFLOAT3 honraiTarget;	//本来あるべき注視点座標
	static XMFLOAT3 honraiUp;		//本来あるべき上ベクトル

	static XMMATRIX rotationMat;	//カメラの回転行列
	static XMMATRIX upRotationMat;	//カメラの上方向ベクトルの回転行列

	static XMMATRIX matPerspective;
	static XMMATRIX matProjection;

	static float angleOfView;		//画角

private:

	const float EYE_PLAYER_DISTANCE = 100;			//プレイヤーと視点の距離
	const float TARGET_PLAYER_DISTNACE = 50;		//プレイヤーと注視点の距離
	const float TARGET_UPPER = 50;					//ターゲットを上にずらす量
#define MAX_ANGLEOFVIEW 100.0f						//最大画角

public:

	//コンストラクタ
	friend Singleton<Camera>;
	Camera();

	//初期化
	static void Init();

	//ビュー行列生成
	static void GenerateMatView();

	//プレイヤーのスピードに応じて視野角を変えた視点行列を生成する
	static void GenerateMatViewSpeed(const float& nowSpeed, const float& maxSpeed);

	//更新処理
	void Update(const XMFLOAT3& playerPos, const XMFLOAT3& playerForwardVec, const XMFLOAT3& playerUpVec, const float& nowSpeed, const float& maxSpeed);

	//アクセッタ
	XMFLOAT3 GetEyeVector();
};