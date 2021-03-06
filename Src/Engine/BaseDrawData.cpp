#include "BaseDrawData.h"
#include "Camera.h"
#include "Vec.h"
#include "Pipline.h"

void BaseDrawData::ChangeScale(Vec3 Amount)
{
	scaleMat_ = DirectX::XMMatrixScaling(Amount.x_, Amount.y_, Amount.z_);
}

void BaseDrawData::ChangeScale(float X, float Y, float Z)
{
	scaleMat_ = DirectX::XMMatrixScaling(X, Y, Z);
}

void BaseDrawData::ChangeRotation(Vec3 Amount)
{
	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();
	buff *= DirectX::XMMatrixRotationZ(Amount.z_);
	buff *= DirectX::XMMatrixRotationX(Amount.x_);
	buff *= DirectX::XMMatrixRotationY(Amount.y_);
	rotationMat_ = buff * rotationMat_;
}
void BaseDrawData::ChangeRotation(float X, float Y, float Z)
{
	DirectX::XMMATRIX buff = DirectX::XMMatrixIdentity();
	buff *= DirectX::XMMatrixRotationZ(Z);
	buff *= DirectX::XMMatrixRotationX(X);
	buff *= DirectX::XMMatrixRotationY(Y);
	rotationMat_ = buff * rotationMat_;
}

void BaseDrawData::InitRotation()
{
	rotationMat_ = DirectX::XMMatrixIdentity();
}

void BaseDrawData::AssignmentRotationMat(DirectX::XMMATRIX Amount)
{
	rotationMat_ = Amount;
}

void BaseDrawData::ChangePosition(Vec3 Amount)
{
	positionMat_ = DirectX::XMMatrixTranslation(Amount.x_, Amount.y_, Amount.z_);
	pos_ = Vec3(positionMat_.r[3].m128_f32[0], positionMat_.r[3].m128_f32[1], positionMat_.r[3].m128_f32[2]);
}

void BaseDrawData::ChangePosition(float X, float Y, float Z)
{
	positionMat_ = DirectX::XMMatrixTranslation(X, Y, Z);
	pos_ = Vec3(positionMat_.r[3].m128_f32[0], positionMat_.r[3].m128_f32[1], positionMat_.r[3].m128_f32[2]);
}

void BaseDrawData::MulRotationMat(DirectX::XMMATRIX RotationMat)
{
	this->rotationMat_ *= RotationMat;
}

void BaseDrawData::ChangePositionAdd(Vec3 Amount)
{
	positionMat_ *= DirectX::XMMatrixTranslation(Amount.x_, Amount.y_, Amount.z_);
	pos_ = Vec3(positionMat_.r[3].m128_f32[0], positionMat_.r[3].m128_f32[1], positionMat_.r[3].m128_f32[2]);
}
void BaseDrawData::ChangePositionAdd(float X, float Y, float Z)
{
	positionMat_ *= DirectX::XMMatrixTranslation(X, Y, Z);
	pos_ = Vec3(positionMat_.r[3].m128_f32[0], positionMat_.r[3].m128_f32[1], positionMat_.r[3].m128_f32[2]);
}


void BaseDrawData::AssignmentWorldMatrix(const DirectX::XMMATRIX& PosMat, const DirectX::XMMATRIX& ScaleMat, const DirectX::XMMATRIX& RotationMat)
{
	this->positionMat_ = PosMat;
	this->scaleMat_ = ScaleMat;
	this->rotationMat_ = RotationMat;
}

void BaseDrawData::DoNotDisplay()
{
	isDisplay_ = false;
}

void BaseDrawData::DisplayOnScreen()
{
	isDisplay_ = true;
}

bool BaseDrawData::GetIsDisplay()
{
	return isDisplay_;
}

void BaseDrawData::ChangeTextureID(int TextureID, int Index)
{
	int indexBuff = Index;

	// ?C???f?b?N?X??0??????????????0???????B
	if (indexBuff < 0) indexBuff = 0;

	// ?C???f?b?N?X??textureID?R???e?i?????????????????????l???????B
	if (indexBuff > static_cast<int>(this->textureID_.size()) - 1) indexBuff = static_cast<int>(this->textureID_.size()) - 1;

	// ?w?????C???f?b?N?X???e?N?X?`??ID?????X?????B
	this->textureID_.at(indexBuff) = TextureID;
}

void BaseDrawData::AddTextureID(int TextureID)
{
	// textureID???????B
	this->textureID_.push_back(TextureID);
}

void BaseDrawData::ClearTextureID()
{
	textureID_.clear();
}

void BaseDrawData::MapConstDataB0(Microsoft::WRL::ComPtr<ID3D12Resource> ConstBuffB0, const ConstBufferDataB0& ConstBufferB0)
{

	// ?]???????s????LightCamera???????????????????B
	DirectX::XMMATRIX matProjection_;
	DirectX::XMMATRIX matPerspective_;
	DirectX::XMMATRIX matView_;
	Vec3 eye_;
	Vec3 target_;
	Vec3 up_;


	matProjection_ = Camera::Ins()->matProjection_;
	matPerspective_ = Camera::Ins()->matPerspective_;
	matView_ = Camera::Ins()->matView_;
	eye_ = Camera::Ins()->Ins()->eye_;
	target_ = Camera::Ins()->Ins()->target_;
	up_ = Camera::Ins()->Ins()->up_;



	// ?????o?b?t?@?????f?[?^?]??
	ConstBufferDataB0* constMap = nullptr;
	ConstBuffB0->Map(0, nullptr, (void**)&constMap);
	// ???eID??backGournd???????????s???e???????s??
	if (projectionID_ == Pipline::PROJECTIONID::UI) {
		// ???[???h?s?????X?V
		DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
		matWorld *= scaleMat_;
		matWorld *= rotationMat_;
		matWorld *= positionMat_;
		constMap->mat.world_ = matWorld;
		constMap->mat.viewproj = Camera::Ins()->matProjection_;		// ???s???e????
		constMap->eye_ = Camera::Ins()->eye_;
		constMap->color = ConstBufferB0.color;
	}
	// ???eID??object?????????????????????????s??
	else if (projectionID_ == Pipline::PROJECTIONID::OBJECT) {
		// ???[???h?s?????X?V
		DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
		matWorld *= scaleMat_;
		matWorld *= rotationMat_;
		matWorld *= positionMat_;
		constMap->mat.world_ = matWorld;								// ???[???h???? * ?r???[???? * ???????e????
		constMap->mat.viewproj = matView_ * matPerspective_;
		constMap->eye_ = eye_;
		constMap->color = ConstBufferB0.color;
	}
	// ?r???{?[?h??????
	else if (projectionID_ == Pipline::PROJECTIONID::BILLBOARD) {
		// ???_???W
		DirectX::XMVECTOR eyePosition = eye_.ConvertXMVECTOR();
		// ?????_???W
		DirectX::XMVECTOR targetPosition = target_.ConvertXMVECTOR();
		// (????)??????
		DirectX::XMVECTOR upVector = up_.ConvertXMVECTOR();
		// ?J????Z??
		DirectX::XMVECTOR cameraAxisZ = DirectX::XMVectorSubtract(targetPosition, eyePosition);
		// 0?x?N?g?????????????????????????????O
		assert(!DirectX::XMVector3Equal(cameraAxisZ, DirectX::XMVectorZero()));
		assert(!DirectX::XMVector3IsInfinite(cameraAxisZ));
		assert(!DirectX::XMVector3Equal(upVector, DirectX::XMVectorZero()));
		assert(!DirectX::XMVector3IsInfinite(upVector));
		// ?x?N?g???????K??
		cameraAxisZ = DirectX::XMVector3Normalize(cameraAxisZ);
		// ?J??????X??????(?E????)
		DirectX::XMVECTOR cameraAxisX{};
		// X??????????->Z?????O??????????
		cameraAxisX = DirectX::XMVector3Cross(upVector, cameraAxisZ);
		// ?x?N?g???????K??
		cameraAxisX = DirectX::XMVector3Normalize(cameraAxisX);
		// ?J??????Y??(??????)
		DirectX::XMVECTOR cameraAxisY{};
		// Y????Z??->X?????O??????????
		cameraAxisY = DirectX::XMVector3Cross(cameraAxisZ, cameraAxisX);
		// ?x?N?g???????K??
		cameraAxisY = DirectX::XMVector3Normalize(cameraAxisY);
		// ?J???????]?s??
		DirectX::XMMATRIX matCameraRot;
		// ?J???????W?n->???[???h???W?n???????s??
		matCameraRot.r[0] = cameraAxisX;
		matCameraRot.r[1] = cameraAxisY;
		matCameraRot.r[2] = cameraAxisZ;
		matCameraRot.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);
		// ?]?n???????t?s??(?t???])???v?Z
		DirectX::XMMATRIX invMatView = DirectX::XMMatrixTranspose(matCameraRot);
		// ???_???W??-1???????????W
		DirectX::XMVECTOR reverseEyePosition = DirectX::XMVectorNegate(eyePosition);
		// ?J?????????u???????[???h???_?????x?N?g??(?J???????W?n)
		DirectX::XMVECTOR tX = DirectX::XMVector3Dot(cameraAxisX, reverseEyePosition);		// X????
		DirectX::XMVECTOR tY = DirectX::XMVector3Dot(cameraAxisY, reverseEyePosition);		// Y????
		DirectX::XMVECTOR tZ = DirectX::XMVector3Dot(cameraAxisZ, reverseEyePosition);		// Z????
		// ???????x?N?g????????????
		DirectX::XMVECTOR translation_ = DirectX::XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
		// ?r???[?s???????s????????????
		invMatView.r[3] = translation_;

		// ?r???{?[?h?s??
		DirectX::XMMATRIX billboardMat = DirectX::XMMatrixIdentity();
		billboardMat.r[0] = cameraAxisX;
		billboardMat.r[1] = cameraAxisY;
		billboardMat.r[2] = cameraAxisZ;
		billboardMat.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);
		// ???[???h?s?????X?V
		DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
		matWorld *= billboardMat;
		matWorld *= scaleMat_;
		matWorld *= rotationMat_;
		matWorld *= positionMat_;
		constMap->mat.world_ = matWorld;												// ???[???h???? * ?r???[???? * ???????e????
		constMap->mat.viewproj = invMatView * matPerspective_;
		constMap->eye_ = eye_;
		constMap->color = ConstBufferB0.color;
	}
	ConstBuffB0->Unmap(0, nullptr);
}