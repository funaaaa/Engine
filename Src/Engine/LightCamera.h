#pragma once
#include "Struct.h"

// ライト視点でのカメラクラス
class LightCamera {

private:

	/*-- メンバ変数 --*/

	XMFLOAT3* eye;				// 視点座標
	XMVECTOR* dir;				// 方向ベクトル

	XMFLOAT3 up;				// 上ベクトル
	XMFLOAT3 target;			// 注視点座標

	XMMATRIX rotationMat;		// 回転行列

	XMMATRIX matView;			// ビュー行列
	XMMATRIX matPerspective;	// 透視投影行列


private:

	/*-- 定数 --*/

	const float EYE_TARGET_DISTANCE = 10.0f;	// 視点と注視点の距離


public:

	/*-- メンバ関数 --*/

	// コンストラクタ
	LightCamera();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(XMFLOAT3* eyePtr, XMVECTOR* dirPtr);

	// 行列生成処理
	void GenerateViewMat();

	// アクセッタ
	inline const XMFLOAT3& GetEye() { return *eye; }
	inline const XMFLOAT3& GetTarget() { return target; }
	inline const XMFLOAT3& GetUp() { return  up; }
	inline const XMMATRIX& GetPerspectiveMat() { return matPerspective; }
	inline const XMMATRIX& GetViewMat() { return matView; }

};