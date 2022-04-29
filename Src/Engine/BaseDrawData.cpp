#include "BaseDrawData.h"
#include "Camera.h"
#include "Enum.h"
#include "Vec.h"

void BaseDrawData::ChangeScale(XMFLOAT3 amount)
{
	scaleMat = XMMatrixScaling(amount.x, amount.y, amount.z);
}

void BaseDrawData::ChangeScale(float x, float y, float z)
{
	scaleMat = XMMatrixScaling(x, y, z);
}

void BaseDrawData::ChangeRotation(XMFLOAT3 amount)
{
	XMMATRIX buff = XMMatrixIdentity();
	buff *= XMMatrixRotationZ(amount.z);
	buff *= XMMatrixRotationX(amount.x);
	buff *= XMMatrixRotationY(amount.y);
	rotationMat = buff * rotationMat;
}
void BaseDrawData::ChangeRotation(float x, float y, float z)
{
	XMMATRIX buff = XMMatrixIdentity();
	buff *= XMMatrixRotationZ(z);
	buff *= XMMatrixRotationX(x);
	buff *= XMMatrixRotationY(y);
	rotationMat = buff * rotationMat;
}

void BaseDrawData::InitRotation()
{
	rotationMat = XMMatrixIdentity();
}

void BaseDrawData::AssignmentRotationMat(XMMATRIX amount)
{
	rotationMat = amount;
}

void BaseDrawData::ChangePosition(XMFLOAT3 amount)
{
	positionMat = XMMatrixTranslation(amount.x, amount.y, amount.z);
	pos = XMFLOAT3(positionMat.r[3].m128_f32[0], positionMat.r[3].m128_f32[1], positionMat.r[3].m128_f32[2]);
}

void BaseDrawData::ChangePosition(float x, float y, float z)
{
	positionMat = XMMatrixTranslation(x, y, z);
	pos = XMFLOAT3(positionMat.r[3].m128_f32[0], positionMat.r[3].m128_f32[1], positionMat.r[3].m128_f32[2]);
}

void BaseDrawData::MulRotationMat(XMMATRIX rotationMat)
{
	this->rotationMat *= rotationMat;
}

void BaseDrawData::ChangePositionAdd(XMFLOAT3 amount)
{
	positionMat *= XMMatrixTranslation(amount.x, amount.y, amount.z);
	pos = XMFLOAT3(positionMat.r[3].m128_f32[0], positionMat.r[3].m128_f32[1], positionMat.r[3].m128_f32[2]);
}
void BaseDrawData::ChangePositionAdd(float x, float y, float z)
{
	positionMat *= XMMatrixTranslation(x, y, z);
	pos = XMFLOAT3(positionMat.r[3].m128_f32[0], positionMat.r[3].m128_f32[1], positionMat.r[3].m128_f32[2]);
}


void BaseDrawData::AssignmentWorldMatrix(const XMMATRIX& posMat, const XMMATRIX& scaleMat, const XMMATRIX& rotationMat)
{
	this->positionMat = posMat;
	this->scaleMat = scaleMat;
	this->rotationMat = rotationMat;
}

void BaseDrawData::DoNotDisplay()
{
	isDisplay = false;
}

void BaseDrawData::DisplayOnScreen()
{
	isDisplay = true;
}

bool BaseDrawData::GetIsDisplay()
{
	return isDisplay;
}

void BaseDrawData::ChangeTextureID(int textureID, int index)
{
	int indexBuff = index;

	// インデックスが0より下だったら0にする。
	if (indexBuff < 0) indexBuff = 0;

	// インデックスがtextureIDコンテナより大きかったら最大値にする。
	if (indexBuff > this->textureID.size() - 1) indexBuff = this->textureID.size() - 1;

	//指定のインデックスのテクスチャIDを変更する。
	this->textureID.at(indexBuff) = textureID;
}

void BaseDrawData::AddTextureID(int textureID)
{
	// textureIDを追加。
	this->textureID.push_back(textureID);
}

void BaseDrawData::ClearTextureID()
{
	textureID.clear();
}

void BaseDrawData::MapConstDataB0(ComPtr<ID3D12Resource> constBuffB0, const ConstBufferDataB0& constBufferDataB0)
{

	// 転送する行列をLightCameraの引数次第で変える。
	XMMATRIX matProjection;
	XMMATRIX matPerspective;
	XMMATRIX matView;
	Vec3 eye;
	Vec3 target;
	Vec3 up;

	XMMATRIX matViewProjShadowMap;


	matProjection = Camera::Ins()->matProjection;
	matPerspective = Camera::Ins()->matPerspective;
	matView = Camera::Ins()->matView;
	eye = Camera::Ins()->Ins()->eye;
	target = Camera::Ins()->Ins()->target;
	up = Camera::Ins()->Ins()->up;



	//定数バッファへのデータ転送
	ConstBufferDataB0* constMap = nullptr;
	HRESULT result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	//投影IDがbackGourndの場合は平行投影変換を行う
	if (projectionID == PROJECTIONID_UI) {
		//ワールド行列の更新
		XMMATRIX matWorld = XMMatrixIdentity();
		matWorld *= scaleMat;
		matWorld *= rotationMat;
		matWorld *= positionMat;
		constMap->mat.world = matWorld;
		constMap->mat.viewproj = Camera::Ins()->matProjection;								//平行投影変換
		constMap->eye = Camera::Ins()->eye;
		constMap->color = constBufferDataB0.color;
	}
	//投影IDがobjectの場合はいろいろな変換を行う
	else if (projectionID == PROJECTIONID_OBJECT) {
		//ワールド行列の更新
		XMMATRIX matWorld = XMMatrixIdentity();
		matWorld *= scaleMat;
		matWorld *= rotationMat;
		matWorld *= positionMat;
		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
		constMap->mat.viewproj = matView * matPerspective;
		constMap->eye = eye;
		constMap->color = constBufferDataB0.color;
	}
	//ビルボードの場合
	else if (projectionID == PROJECTIONID_BILLBOARD) {
		//視点座標
		XMVECTOR eyePosition = eye.ConvertXMVECTOR();
		//注視点座標
		XMVECTOR targetPosition = target.ConvertXMVECTOR();
		//(仮の)上方向
		XMVECTOR upVector = up.ConvertXMVECTOR();
		//カメラZ軸
		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
		//0ベクトルだと向きが定まらないので除外
		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
		assert(!XMVector3IsInfinite(cameraAxisZ));
		assert(!XMVector3Equal(upVector, XMVectorZero()));
		assert(!XMVector3IsInfinite(upVector));
		//ベクトルを正規化
		cameraAxisZ = XMVector3Normalize(cameraAxisZ);
		//カメラのX軸方向(右方向)
		XMVECTOR cameraAxisX{};
		//X軸は上方向->Z軸の外積で求まる
		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
		//ベクトルを正規化
		cameraAxisX = XMVector3Normalize(cameraAxisX);
		//カメラのY軸(上方向)
		XMVECTOR cameraAxisY{};
		//Y軸はZ軸->X軸の外積で求める
		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
		//ベクトルを正規化
		cameraAxisY = XMVector3Normalize(cameraAxisY);
		//カメラ回転行列
		XMMATRIX matCameraRot;
		//カメラ座標系->ワールド座標系の返還行列
		matCameraRot.r[0] = cameraAxisX;
		matCameraRot.r[1] = cameraAxisY;
		matCameraRot.r[2] = cameraAxisZ;
		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
		//転地により逆行列(逆回転)を計算
		XMMATRIX matView = XMMatrixTranspose(matCameraRot);
		//視点座標に-1をかけた座標
		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
		//カメラの位置からワールド原点へのベクトル(カメラ座標系)
		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);		//X成分
		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);		//Y成分
		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);		//Z成分
		//一つのベクトルにまとめる
		XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
		//ビュー行列に平行移動分を設定
		matView.r[3] = translation;

		//ビルボード行列
		XMMATRIX billboardMat = XMMatrixIdentity();
		billboardMat.r[0] = cameraAxisX;
		billboardMat.r[1] = cameraAxisY;
		billboardMat.r[2] = cameraAxisZ;
		billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);
		//ワールド行列の更新
		XMMATRIX matWorld = XMMatrixIdentity();
		matWorld *= billboardMat;
		matWorld *= scaleMat;
		matWorld *= rotationMat;
		matWorld *= positionMat;
		constMap->mat.world = matWorld;												//ワールド変換 * ビュー変換 * 透視投影変換
		constMap->mat.viewproj = matView * matPerspective;
		constMap->eye = eye;
		constMap->color = constBufferDataB0.color;
	}
	constBuffB0->Unmap(0, nullptr);
}