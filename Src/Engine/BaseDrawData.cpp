#include "BaseDrawData.h"
#include "Camera.h"
#include "Vec.h"
#include "Pipline.h"

void BaseDrawData::ChangeScale(Vec3 Amount)
{
	scaleMat = DirectX::XMMatrixScaling(Amount.x_, Amount.y_, Amount.z_);
}

void BaseDrawData::ChangeScale(float X, float Y, float Z)
{
	scaleMat = DirectX::XMMatrixScaling(X, Y, Z);
}

void BaseDrawData::ChangeRotation(Vec3 Amount)
{
	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();
	buff *= DirectX::XMMatrixRotationZ(Amount.z_);
	buff *= DirectX::XMMatrixRotationX(Amount.x_);
	buff *= DirectX::XMMatrixRotationY(Amount.y_);
	rotationMat = buff * rotationMat;
}
void BaseDrawData::ChangeRotation(float X, float Y, float Z)
{
	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();
	buff *= DirectX::XMMatrixRotationZ(Z);
	buff *= DirectX::XMMatrixRotationX(X);
	buff *= DirectX::XMMatrixRotationY(Y);
	rotationMat = buff * rotationMat;
}

void BaseDrawData::InitRotation()
{
	rotationMat = DirectX::XMMatrixIdentity();
}

void BaseDrawData::AssignmentRotationMat(DirectX::XMMATRIX Amount)
{
	rotationMat = Amount;
}

void BaseDrawData::ChangePosition(Vec3 Amount)
{
	positionMat = DirectX::XMMatrixTranslation(Amount.x_, Amount.y_, Amount.z_);
	pos_ = Vec3(positionMat.r[3].m128_f32[0], positionMat.r[3].m128_f32[1], positionMat.r[3].m128_f32[2]);
}

void BaseDrawData::ChangePosition(float X, float Y, float Z)
{
	positionMat = DirectX::XMMatrixTranslation(X, Y, Z);
	pos_ = Vec3(positionMat.r[3].m128_f32[0], positionMat.r[3].m128_f32[1], positionMat.r[3].m128_f32[2]);
}

void BaseDrawData::MulRotationMat(DirectX::XMMATRIX RotationMat)
{
	this->rotationMat *= RotationMat;
}

void BaseDrawData::ChangePositionAdd(Vec3 Amount)
{
	positionMat *= DirectX::XMMatrixTranslation(Amount.x_, Amount.y_, Amount.z_);
	pos_ = Vec3(positionMat.r[3].m128_f32[0], positionMat.r[3].m128_f32[1], positionMat.r[3].m128_f32[2]);
}
void BaseDrawData::ChangePositionAdd(float X, float Y, float Z)
{
	positionMat *= DirectX::XMMatrixTranslation(X, Y, Z);
	pos_ = Vec3(positionMat.r[3].m128_f32[0], positionMat.r[3].m128_f32[1], positionMat.r[3].m128_f32[2]);
}


void BaseDrawData::AssignmentWorldMatrix(const DirectX::XMMATRIX& PosMat, const DirectX::XMMATRIX& ScaleMat, const DirectX::XMMATRIX& RotationMat)
{
	this->positionMat = PosMat;
	this->scaleMat = ScaleMat;
	this->rotationMat = RotationMat;
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

void BaseDrawData::ChangeTextureID(int TextureID, int Index)
{
	int indexBuff = Index;

	// インデックスが0より下だったら0にする。
	if (indexBuff < 0) indexBuff = 0;

	// インデックスがtextureIDコンテナより大きかったら最大値にする。
	if (indexBuff > static_cast<int>(this->textureID.size()) - 1) indexBuff = static_cast<int>(this->textureID.size()) - 1;

	// 指定のインデックスのテクスチャIDを変更する。
	this->textureID.at(indexBuff) = TextureID;
}

void BaseDrawData::AddTextureID(int TextureID)
{
	// textureIDを追加。
	this->textureID.push_back(TextureID);
}

void BaseDrawData::ClearTextureID()
{
	textureID.clear();
}

void BaseDrawData::MapConstDataB0(Microsoft::WRL::ComPtr<ID3D12Resource> ConstBuffB0, const ConstBufferDataB0& ConstBufferB0)
{

	// 転送する行列をLightCameraの引数次第で変える。
	DirectX::XMMATRIX matProjection;
	DirectX::XMMATRIX matPerspective;
	DirectX::XMMATRIX matView;
	Vec3 eye;
	Vec3 target;
	Vec3 up;


	matProjection = Camera::Ins()->matProjection;
	matPerspective = Camera::Ins()->matPerspective;
	matView = Camera::Ins()->matView;
	eye = Camera::Ins()->Ins()->eye;
	target = Camera::Ins()->Ins()->target;
	up = Camera::Ins()->Ins()->up;



	// 定数バッファへのデータ転送
	ConstBufferDataB0* constMap = nullptr;
	ConstBuffB0->Map(0, nullptr, (void**)&constMap);
	// 投影IDがbackGourndの場合は平行投影変換を行う
	if (projectionID == Pipline::PROJECTIONID::UI) {
		// ワールド行列の更新
		DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
		matWorld *= scaleMat;
		matWorld *= rotationMat;
		matWorld *= positionMat;
		constMap->mat.world = matWorld;
		constMap->mat.viewproj = Camera::Ins()->matProjection;		// 平行投影変換
		constMap->eye = Camera::Ins()->eye;
		constMap->color = ConstBufferB0.color;
	}
	// 投影IDがobjectの場合はいろいろな変換を行う
	else if (projectionID == Pipline::PROJECTIONID::OBJECT) {
		// ワールド行列の更新
		DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
		matWorld *= scaleMat;
		matWorld *= rotationMat;
		matWorld *= positionMat;
		constMap->mat.world = matWorld;								// ワールド変換 * ビュー変換 * 透視投影変換
		constMap->mat.viewproj = matView * matPerspective;
		constMap->eye = eye;
		constMap->color = ConstBufferB0.color;
	}
	// ビルボードの場合
	else if (projectionID == Pipline::PROJECTIONID::BILLBOARD) {
		// 視点座標
		DirectX::XMVECTOR eyePosition = eye.ConvertXMVECTOR();
		// 注視点座標
		DirectX::XMVECTOR targetPosition = target.ConvertXMVECTOR();
		// (仮の)上方向
		DirectX::XMVECTOR upVector = up.ConvertXMVECTOR();
		// カメラZ軸
		DirectX::XMVECTOR cameraAxisZ = DirectX::XMVectorSubtract(targetPosition, eyePosition);
		// 0ベクトルだと向きが定まらないので除外
		assert(!DirectX::XMVector3Equal(cameraAxisZ, DirectX::XMVectorZero()));
		assert(!DirectX::XMVector3IsInfinite(cameraAxisZ));
		assert(!DirectX::XMVector3Equal(upVector, DirectX::XMVectorZero()));
		assert(!DirectX::XMVector3IsInfinite(upVector));
		// ベクトルを正規化
		cameraAxisZ = DirectX::XMVector3Normalize(cameraAxisZ);
		// カメラのX軸方向(右方向)
		DirectX::XMVECTOR cameraAxisX{};
		// X軸は上方向->Z軸の外積で求まる
		cameraAxisX = DirectX::XMVector3Cross(upVector, cameraAxisZ);
		// ベクトルを正規化
		cameraAxisX = DirectX::XMVector3Normalize(cameraAxisX);
		// カメラのY軸(上方向)
		DirectX::XMVECTOR cameraAxisY{};
		// Y軸はZ軸->X軸の外積で求める
		cameraAxisY = DirectX::XMVector3Cross(cameraAxisZ, cameraAxisX);
		// ベクトルを正規化
		cameraAxisY = DirectX::XMVector3Normalize(cameraAxisY);
		// カメラ回転行列
		DirectX::XMMATRIX matCameraRot;
		// カメラ座標系->ワールド座標系の返還行列
		matCameraRot.r[0] = cameraAxisX;
		matCameraRot.r[1] = cameraAxisY;
		matCameraRot.r[2] = cameraAxisZ;
		matCameraRot.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);
		// 転地により逆行列(逆回転)を計算
		DirectX::XMMATRIX invMatView = DirectX::XMMatrixTranspose(matCameraRot);
		// 視点座標に-1をかけた座標
		DirectX::XMVECTOR reverseEyePosition = DirectX::XMVectorNegate(eyePosition);
		// カメラの位置からワールド原点へのベクトル(カメラ座標系)
		DirectX::XMVECTOR tX = DirectX::XMVector3Dot(cameraAxisX, reverseEyePosition);		// X成分
		DirectX::XMVECTOR tY = DirectX::XMVector3Dot(cameraAxisY, reverseEyePosition);		// Y成分
		DirectX::XMVECTOR tZ = DirectX::XMVector3Dot(cameraAxisZ, reverseEyePosition);		// Z成分
		// 一つのベクトルにまとめる
		DirectX::XMVECTOR translation = DirectX::XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
		// ビュー行列に平行移動分を設定
		invMatView.r[3] = translation;

		// ビルボード行列
		DirectX::XMMATRIX billboardMat = DirectX::XMMatrixIdentity();
		billboardMat.r[0] = cameraAxisX;
		billboardMat.r[1] = cameraAxisY;
		billboardMat.r[2] = cameraAxisZ;
		billboardMat.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);
		// ワールド行列の更新
		DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
		matWorld *= billboardMat;
		matWorld *= scaleMat;
		matWorld *= rotationMat;
		matWorld *= positionMat;
		constMap->mat.world = matWorld;												// ワールド変換 * ビュー変換 * 透視投影変換
		constMap->mat.viewproj = invMatView * matPerspective;
		constMap->eye = eye;
		constMap->color = ConstBufferB0.color;
	}
	ConstBuffB0->Unmap(0, nullptr);
}