#include "DriftParticle.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"

DriftParticle::DriftParticle()
{

	/*===== �R���X�g���N�^ =====*/

	particleIns_ = 0;
	constBufferIndex_ = 0;
	pos_ = Vec3();
	forwardVec_ = Vec3();
	speed_ = 0;
	scale_ = 0;
	isActive_ = false;

}

void DriftParticle::Setting(const int& BlasIndex, const int ConstBufferIndex)
{

	/*===== �Z�b�e�B���O =====*/

	particleIns_ = PolygonInstanceRegister::Ins()->CreateInstance(BlasIndex, PolygonInstanceRegister::REFLECTION);
	constBufferIndex_ = ConstBufferIndex;
	// �ǂ��������ɔ�΂��B
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, Vec3(-10000000, -10000000, -10000000));
	PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(0, 0, 0));


}

void DriftParticle::Init()
{

	/*===== ���������� =====*/

	isActive_ = false;

	// �ǂ��������ɔ�΂��B
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, Vec3(-10000000, -10000000, -10000000));
	PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(0, 0, 0));

}

void DriftParticle::Generate(const Vec3& Pos, const Vec3& DriftVec, const DirectX::XMMATRIX& CarMatRot, RayConstBufferData& ConstBufferData)
{

	/*===== �������� =====*/

	pos_ = Pos;
	speed_ = SPEED;
	scale_ = SCALE;
	isActive_ = true;

	// �i�s�����x�N�g�������߂�B
	forwardVec_ = DriftVec;

	// �f�t�H���g�̏�x�N�g������]������B
	Vec3 rotUpVec = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), CarMatRot);

	// ��]��������x�N�g���̕����ɗ��������߂Đi�s�����x�N�g�����㉺�ɓ������B
	forwardVec_ += Vec3(FHelper::GetRand(rotUpVec.x / 2.0f), FHelper::GetRand(rotUpVec.y / 2.0f), FHelper::GetRand(rotUpVec.z / 2.0f));
	forwardVec_.Normalize();

	// �f�t�H���g�̐��ʃx�N�g������]������B
	Vec3 rotForwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), CarMatRot);
	rotForwardVec *= Vec3(10.0f, 10.0f, 10.0f);

	// ��]���������ʃx�N�g���̕����ɍ��W�����炷�B
	float randomAmount = FHelper::GetRand(0, 100) % 2 == 0 ? 1.0f : -1.0f;
	pos_ += rotForwardVec * randomAmount;

	// ���W�ƃX�P�[���������B
	ConstBufferData.light.pointLight[constBufferIndex_].lightPos = pos_ + DriftVec * 10.0f;
	ConstBufferData.light.pointLight[constBufferIndex_].isActive_ = true;
	ConstBufferData.light.pointLight[constBufferIndex_].lightSize = scale_;
	ConstBufferData.light.pointLight[constBufferIndex_].lightPower = 30.0f;

}

void DriftParticle::Update(RayConstBufferData& ConstBufferData)
{

	/*===== �X�V���� =====*/

	if (scale_ <= SCALE - SUB_SCALE * 2.0f) {
		ConstBufferData.light.pointLight[constBufferIndex_].lightPos = Vec3(-1000, -1000, -1000);
		ConstBufferData.light.pointLight[constBufferIndex_].isActive_ = false;
	}

	// �p�[�e�B�N���𓮂����B
	pos_ += forwardVec_ * speed_;

	// �p�[�e�B�N���̐i�s�����ɏd�͂����Z����B
	forwardVec_.y -= 0.08f;
	forwardVec_.Normalize();

	// �ړ����x��������B
	speed_ -= SUB_SPEED;

	// �傫��������������B
	scale_ -= SUB_SCALE;

	// Instance���X�V�B
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, pos_);
	PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(scale_, scale_, scale_));

	// �傫����0��菬�����Ȃ����珉��������B
	if (scale_ < 0) {

		scale_ = 0;
		Init();

		return;

	}

}
