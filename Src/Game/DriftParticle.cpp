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

void DriftParticle::Generate(const Vec3& Pos, RayConstBufferData& ConstBufferData)
{

	/*===== �������� =====*/

	pos_ = Pos;
	isActive_ = true;
	particleIns_ = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex_, PolygonInstanceRegister::SHADER_ID::ALPHA);
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, Pos);
	PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(30, 30, 30));

	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].instanceIndex_ = particleIns_;
	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f;

}

void DriftParticle::Update(RayConstBufferData& ConstBufferData)
{

	/*===== �X�V���� =====*/

	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ -= 0.05f;

	if (ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ < 0.3f) {

		Init();

	}

}
