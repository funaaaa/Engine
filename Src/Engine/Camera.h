#pragma once
#include <DirectXMath.h>
#include "Singleton.h"
#include "Vec.h"

using namespace DirectX;

class Camera : public Singleton<Camera> {
public:
	XMMATRIX matView;		//ビュー行列
	Vec3 eye;			//ゲームワールド内でのカメラ座標
	Vec3 target;			//ゲームワールド内でカメラが見ている座標
	Vec3 up;				//ゲームワールド内でカメラから見て上方向を指すベクトル

	Vec3 forwardVec;

	Vec3 honraiEye;		//本来あるべき視点座標
	Vec3 honraiTarget;	//本来あるべき注視点座標
	Vec3 honraiUp;		//本来あるべき上ベクトル

	XMMATRIX rotationMat;	//カメラの回転行列
	XMMATRIX upRotationMat;	//カメラの上方向ベクトルの回転行列

	XMMATRIX matPerspective;
	XMMATRIX matProjection;

	float angleOfView;		//画角

	float angleXZ;		// XZ平面での角度

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
	void Init();

	//ビュー行列生成
	void GenerateMatView();

	//プレイヤーのスピードに応じて視野角を変えた視点行列を生成する
	void GenerateMatViewSpeed(const float& nowSpeed, const float& maxSpeed);

	//更新処理
	void UpdateRacingGame(const Vec3& playerPos, const Vec3& playerForwardVec, const Vec3& playerUpVec, const float& nowSpeed, const float& maxSpeed);
	void Update();

	// 回転
	void AddRotation(const float& RotX, const float& RotY, const float& RotZ);
	void AddRotationXZ(const float& Rot);

	// 移動
	void Move(const float& Speed);
	void MoveRight(const float& Speed);

	//アクセッタ
	Vec3 GetEyeVector();
};