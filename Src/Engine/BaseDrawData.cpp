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

	// �C���f�b�N�X��0��艺��������0�ɂ���B
	if (indexBuff < 0) indexBuff = 0;

	// �C���f�b�N�X��textureID�R���e�i���傫��������ő�l�ɂ���B
	if (indexBuff > static_cast<int>(this->textureID.size()) - 1) indexBuff = static_cast<int>(this->textureID.size()) - 1;

	// �w��̃C���f�b�N�X�̃e�N�X�`��ID��ύX����B
	this->textureID.at(indexBuff) = TextureID;
}

void BaseDrawData::AddTextureID(int TextureID)
{
	// textureID��ǉ��B
	this->textureID.push_back(TextureID);
}

void BaseDrawData::ClearTextureID()
{
	textureID.clear();
}

void BaseDrawData::MapConstDataB0(Microsoft::WRL::ComPtr<ID3D12Resource> ConstBuffB0, const ConstBufferDataB0& ConstBufferB0)
{

	// �]������s���LightCamera�̈�������ŕς���B
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



	// �萔�o�b�t�@�ւ̃f�[�^�]��
	ConstBufferDataB0* constMap = nullptr;
	ConstBuffB0->Map(0, nullptr, (void**)&constMap);
	// ���eID��backGournd�̏ꍇ�͕��s���e�ϊ����s��
	if (projectionID == Pipline::PROJECTIONID::UI) {
		// ���[���h�s��̍X�V
		DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
		matWorld *= scaleMat;
		matWorld *= rotationMat;
		matWorld *= positionMat;
		constMap->mat.world = matWorld;
		constMap->mat.viewproj = Camera::Ins()->matProjection;		// ���s���e�ϊ�
		constMap->eye = Camera::Ins()->eye;
		constMap->color = ConstBufferB0.color;
	}
	// ���eID��object�̏ꍇ�͂��낢��ȕϊ����s��
	else if (projectionID == Pipline::PROJECTIONID::OBJECT) {
		// ���[���h�s��̍X�V
		DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
		matWorld *= scaleMat;
		matWorld *= rotationMat;
		matWorld *= positionMat;
		constMap->mat.world = matWorld;								// ���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
		constMap->mat.viewproj = matView * matPerspective;
		constMap->eye = eye;
		constMap->color = ConstBufferB0.color;
	}
	// �r���{�[�h�̏ꍇ
	else if (projectionID == Pipline::PROJECTIONID::BILLBOARD) {
		// ���_���W
		DirectX::XMVECTOR eyePosition = eye.ConvertXMVECTOR();
		// �����_���W
		DirectX::XMVECTOR targetPosition = target.ConvertXMVECTOR();
		// (����)�����
		DirectX::XMVECTOR upVector = up.ConvertXMVECTOR();
		// �J����Z��
		DirectX::XMVECTOR cameraAxisZ = DirectX::XMVectorSubtract(targetPosition, eyePosition);
		// 0�x�N�g�����ƌ�������܂�Ȃ��̂ŏ��O
		assert(!DirectX::XMVector3Equal(cameraAxisZ, DirectX::XMVectorZero()));
		assert(!DirectX::XMVector3IsInfinite(cameraAxisZ));
		assert(!DirectX::XMVector3Equal(upVector, DirectX::XMVectorZero()));
		assert(!DirectX::XMVector3IsInfinite(upVector));
		// �x�N�g���𐳋K��
		cameraAxisZ = DirectX::XMVector3Normalize(cameraAxisZ);
		// �J������X������(�E����)
		DirectX::XMVECTOR cameraAxisX{};
		// X���͏����->Z���̊O�ςŋ��܂�
		cameraAxisX = DirectX::XMVector3Cross(upVector, cameraAxisZ);
		// �x�N�g���𐳋K��
		cameraAxisX = DirectX::XMVector3Normalize(cameraAxisX);
		// �J������Y��(�����)
		DirectX::XMVECTOR cameraAxisY{};
		// Y����Z��->X���̊O�ςŋ��߂�
		cameraAxisY = DirectX::XMVector3Cross(cameraAxisZ, cameraAxisX);
		// �x�N�g���𐳋K��
		cameraAxisY = DirectX::XMVector3Normalize(cameraAxisY);
		// �J������]�s��
		DirectX::XMMATRIX matCameraRot;
		// �J�������W�n->���[���h���W�n�̕Ԋҍs��
		matCameraRot.r[0] = cameraAxisX;
		matCameraRot.r[1] = cameraAxisY;
		matCameraRot.r[2] = cameraAxisZ;
		matCameraRot.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);
		// �]�n�ɂ��t�s��(�t��])���v�Z
		DirectX::XMMATRIX invMatView = DirectX::XMMatrixTranspose(matCameraRot);
		// ���_���W��-1�����������W
		DirectX::XMVECTOR reverseEyePosition = DirectX::XMVectorNegate(eyePosition);
		// �J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��(�J�������W�n)
		DirectX::XMVECTOR tX = DirectX::XMVector3Dot(cameraAxisX, reverseEyePosition);		// X����
		DirectX::XMVECTOR tY = DirectX::XMVector3Dot(cameraAxisY, reverseEyePosition);		// Y����
		DirectX::XMVECTOR tZ = DirectX::XMVector3Dot(cameraAxisZ, reverseEyePosition);		// Z����
		// ��̃x�N�g���ɂ܂Ƃ߂�
		DirectX::XMVECTOR translation = DirectX::XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
		// �r���[�s��ɕ��s�ړ�����ݒ�
		invMatView.r[3] = translation;

		// �r���{�[�h�s��
		DirectX::XMMATRIX billboardMat = DirectX::XMMatrixIdentity();
		billboardMat.r[0] = cameraAxisX;
		billboardMat.r[1] = cameraAxisY;
		billboardMat.r[2] = cameraAxisZ;
		billboardMat.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);
		// ���[���h�s��̍X�V
		DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();
		matWorld *= billboardMat;
		matWorld *= scaleMat;
		matWorld *= rotationMat;
		matWorld *= positionMat;
		constMap->mat.world = matWorld;												// ���[���h�ϊ� * �r���[�ϊ� * �������e�ϊ�
		constMap->mat.viewproj = invMatView * matPerspective;
		constMap->eye = eye;
		constMap->color = ConstBufferB0.color;
	}
	ConstBuffB0->Unmap(0, nullptr);
}