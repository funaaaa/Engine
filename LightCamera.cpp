#include "LightCamera.h"
#include "FHelper.h"

LightCamera::LightCamera()
{
}

void LightCamera::Init()
{
	eye = nullptr;
	dir = nullptr;
}

void LightCamera::Generate(XMFLOAT3* eyePtr, XMVECTOR* dirPtr)
{

	/*-- 生成処理 --*/

	// nullチェック
	if (eyePtr == nullptr || dirPtr == nullptr) {

		MessageBoxA(nullptr, "eyePtr or dirPtr がnullptrです。Lightが生成されているかをチェックしてください。", "エラー", MB_OK);

		assert(0);

	}

	// 引数で指定されたライトと座標と方向ベクトルを紐付ける。
	eye = eyePtr;
	dir = dirPtr;

	// 回転行列を求める。
	rotationMat = XMMatrixIdentity();

	//デフォルトの正面ベクトル
	XMFLOAT3 defUpVec = { 0,0,1 };

	//現フレームの上ベクトル
	XMFLOAT3 nowUpVec;
	XMStoreFloat3(&nowUpVec, *dir);

	//回転軸を求める
	XMFLOAT3 axisOfRevolutionBuff = FHelper::Cross3D(defUpVec, nowUpVec);

	//回転軸を正規化する
	if (!(axisOfRevolutionBuff.x == 0 && axisOfRevolutionBuff.y == 0 && axisOfRevolutionBuff.z == 0)) {
		axisOfRevolutionBuff = FHelper::Normalize3D(axisOfRevolutionBuff);
	}

	//回転軸をXMVECTORに治す
	XMVECTOR axisOfRevolution = XMLoadFloat3(&axisOfRevolutionBuff);

	/*-- 内積から回転量を求める --*/

	//回転量を求める
	float amountOfRotation = FHelper::CalAngle3D(defUpVec, nowUpVec);

	//逆余弦を求める関数を使用して求めたcosθをラジアンに変換
	amountOfRotation = acosf(amountOfRotation);


	/*-- クォータニオンを使って回転させる --*/

	//回転軸が{0,0,0}だったら処理を飛ばす
	if (!(axisOfRevolution.m128_f32[0] == 0 && axisOfRevolution.m128_f32[1] == 0 && axisOfRevolution.m128_f32[2] == 0)) {

		//クォータニオンを求める
		XMVECTOR quaternion = XMQuaternionRotationAxis(axisOfRevolution, amountOfRotation);

		//求めたクォータニオンを行列に治す
		rotationMat = XMMatrixRotationQuaternion(quaternion);

	}

	// 回転行列をもとに、上ベクトルを求める。
	up = FHelper::MulRotationMatNormal(XMFLOAT3(0, 1, 0), rotationMat);

	// 注視点座標を求める。
	const XMFLOAT3 eyeTargetDistance = XMFLOAT3(EYE_TARGET_DISTANCE, EYE_TARGET_DISTANCE, EYE_TARGET_DISTANCE);
	target = FHelper::AddXMFLOAT3(*eye, FHelper::MulXMFLOAT3(XMFLOAT3(dir->m128_f32[0], dir->m128_f32[1], dir->m128_f32[2]), eyeTargetDistance));

	// ビュー行列を求める。
	matView = XMMatrixLookAtLH(XMLoadFloat3(eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	// 透視投影変換行列を求める。
	matPerspective = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(20.0f),				//画角(60度)
		(float)window_width / window_height,	//アスペクト比
		0.1f, 1000000.0f							//前端、奥端
	);

}

void LightCamera::GenerateViewMat()
{

	// 回転行列を求める。
	rotationMat = XMMatrixIdentity();

	//デフォルトの正面ベクトル
	XMFLOAT3 defUpVec = { 0,0,1 };

	//現フレームの上ベクトル
	XMFLOAT3 nowUpVec;
	XMStoreFloat3(&nowUpVec, *dir);

	//回転軸を求める
	XMFLOAT3 axisOfRevolutionBuff = FHelper::Cross3D(defUpVec, nowUpVec);

	//回転軸を正規化する
	if (!(axisOfRevolutionBuff.x == 0 && axisOfRevolutionBuff.y == 0 && axisOfRevolutionBuff.z == 0)) {
		axisOfRevolutionBuff = FHelper::Normalize3D(axisOfRevolutionBuff);
	}

	//回転軸をXMVECTORに治す
	XMVECTOR axisOfRevolution = XMLoadFloat3(&axisOfRevolutionBuff);

	/*-- 内積から回転量を求める --*/

	//回転量を求める
	float amountOfRotation = FHelper::CalAngle3D(defUpVec, nowUpVec);

	//逆余弦を求める関数を使用して求めたcosθをラジアンに変換
	amountOfRotation = acosf(amountOfRotation);


	/*-- クォータニオンを使って回転させる --*/

	//回転軸が{0,0,0}だったら処理を飛ばす
	if (!(axisOfRevolution.m128_f32[0] == 0 && axisOfRevolution.m128_f32[1] == 0 && axisOfRevolution.m128_f32[2] == 0)) {

		//クォータニオンを求める
		XMVECTOR quaternion = XMQuaternionRotationAxis(axisOfRevolution, amountOfRotation);

		//求めたクォータニオンを行列に治す
		rotationMat = XMMatrixRotationQuaternion(quaternion);

	}

	// 回転行列をもとに、上ベクトルを求める。
	up = FHelper::MulRotationMatNormal(XMFLOAT3(0, 1, 0), rotationMat);

	// 注視点座標を求める。
	const XMFLOAT3 eyeTargetDistance = XMFLOAT3(EYE_TARGET_DISTANCE, EYE_TARGET_DISTANCE, EYE_TARGET_DISTANCE);
	target = FHelper::AddXMFLOAT3(*eye, FHelper::MulXMFLOAT3(XMFLOAT3(dir->m128_f32[0], dir->m128_f32[1], dir->m128_f32[2]), eyeTargetDistance));

	// ビュー行列を求める。
	matView = XMMatrixLookAtLH(XMLoadFloat3(eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	// 透視投影変換行列を求める。
	matPerspective = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),				//画角(60度)
		(float)window_width / window_height,	//アスペクト比
		0.1f, 1000000.0f							//前端、奥端
	);

}
