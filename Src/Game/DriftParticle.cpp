#include "DriftParticle.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"

DriftParticle::DriftParticle()
{

	/*===== �R���X�g���N�^ =====*/

	particleIns_ = 0;
	constBufferIndex_ = 0;
	pos_ = Vec3();

	isActive_ = false;

}

void DriftParticle::Setting(const int& BlasIndex, const int ConstBufferIndex)
{

	/*===== �Z�b�e�B���O =====*/

	blasIndex_ = BlasIndex;
	constBufferIndex_ = ConstBufferIndex;

}

void DriftParticle::Init()
{

	/*===== ���������� =====*/

	if (isActive_) {

		PolygonInstanceRegister::Ins()->DestroyInstance(particleIns_);

	}

	isActive_ = false;

}

void DriftParticle::GenerateSmoke(const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData, const bool& IsBoost, const bool& IsDash)
{

	/*===== �������� =====*/

	pos_ = Pos;
	isActive_ = true;
	isAppearingNow_ = true;
	appearingTimer_ = 0;
	particleIns_ = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex_, PolygonInstanceRegister::SHADER_ID::ALPHA);
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, Pos);
	PolygonInstanceRegister::Ins()->ChangeRotate(particleIns_, MatRot);

	// �������̉�]�����x�N�g�������߂�B
	Vec3 zRotVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), MatRot);
	float random = FHelper::GetRand(-FHelper::F_PI_F, FHelper::F_PI_F);
	DirectX::XMVECTOR zRotQ = DirectX::XMQuaternionRotationAxis(zRotVec.ConvertXMVECTOR(), random);
	PolygonInstanceRegister::Ins()->AddRotate(particleIns_, DirectX::XMMatrixRotationQuaternion(zRotQ));

	if (IsBoost) {
		PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(30, 30, 30));
	}
	else {
		PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(15, 15, 15));
	}

	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].instanceIndex_ = particleIns_;
	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ = 0.1f;

}

void DriftParticle::Update(RayConstBufferData& ConstBufferData)
{

	/*===== �X�V���� =====*/

	if (isAppearingNow_) {

		// �A���t�@�l�����Z�B
		ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ += APPEARING_ALPHA;
		if (1.0f < ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_) {

			ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f;
			++appearingTimer_;

			// �o���^�C�}�[�����l�ɒB������A���t�@�l�������Ă����B
			if (APPEARING_TIMER < appearingTimer_) {

				isAppearingNow_ = false;

			}

		}

	}
	else {

		ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ -= EXIT_ALPHA;

	}

	if (ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ < 0.0f) {

		Init();

	}

}