#include "DriftParticle.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"

DriftParticle::DriftParticle()
{

	/*===== �R���X�g���N�^ =====*/

	particleIns = 0;
	constBufferIndex = 0;
	pos = Vec3();
	forwardVec = Vec3();
	speed = 0;
	scale = 0;
	isActive = false;

}

void DriftParticle::Setting(const int& BlasIndex, const int ConstBufferIndex)
{

	/*===== �Z�b�e�B���O =====*/

	particleIns = PolygonInstanceRegister::Ins()->CreateInstance(BlasIndex, PolygonInstanceRegister::REFLECTION);
	constBufferIndex = ConstBufferIndex;
	// �ǂ��������ɔ�΂��B
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns, Vec3(-10000000, -10000000, -10000000));
	PolygonInstanceRegister::Ins()->ChangeScale(particleIns, Vec3(0, 0, 0));


}

void DriftParticle::Init()
{

	/*===== ���������� =====*/

	isActive = false;

	// �ǂ��������ɔ�΂��B
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns, Vec3(-10000000, -10000000, -10000000));
	PolygonInstanceRegister::Ins()->ChangeScale(particleIns, Vec3(0, 0, 0));

}

void DriftParticle::Generate(const Vec3& Pos, const Vec3& DriftVec, const DirectX::XMMATRIX& CarMatRot, RayConstBufferData& ConstBufferData)
{

	/*===== �������� =====*/

	pos = Pos;
	speed = SPEED;
	scale = SCALE;
	isActive = true;

	// �i�s�����x�N�g�������߂�B
	forwardVec = DriftVec;

	// �f�t�H���g�̏�x�N�g������]������B
	Vec3 rotUpVec = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), CarMatRot);

	// ��]��������x�N�g���̕����ɗ��������߂Đi�s�����x�N�g�����㉺�ɓ������B
	forwardVec += Vec3(FHelper::GetRand(rotUpVec.x / 2.0f), FHelper::GetRand(rotUpVec.y / 2.0f), FHelper::GetRand(rotUpVec.z / 2.0f));
	forwardVec.Normalize();

	// �f�t�H���g�̐��ʃx�N�g������]������B
	Vec3 rotForwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), CarMatRot);
	rotForwardVec *= Vec3(10.0f, 10.0f, 10.0f);

	// ��]���������ʃx�N�g���̕����ɍ��W�����炷�B
	float randomAmount = FHelper::GetRand(0, 100) % 2 == 0 ? 1.0f : -1.0f;
	pos += rotForwardVec * randomAmount;

	// ���W�ƃX�P�[���������B
	ConstBufferData.light.pointLight[constBufferIndex].lightPos = pos + DriftVec * 10.0f;
	ConstBufferData.light.pointLight[constBufferIndex].isActive = true;
	ConstBufferData.light.pointLight[constBufferIndex].lightSize = scale;
	ConstBufferData.light.pointLight[constBufferIndex].lightPower = 30.0f;

}

void DriftParticle::Update(RayConstBufferData& ConstBufferData)
{

	/*===== �X�V���� =====*/

	if (scale <= SCALE - SUB_SCALE * 2.0f) {
		ConstBufferData.light.pointLight[constBufferIndex].lightPos = Vec3(-1000, -1000, -1000);
		ConstBufferData.light.pointLight[constBufferIndex].isActive = false;
	}

	// �p�[�e�B�N���𓮂����B
	pos += forwardVec * speed;

	// �p�[�e�B�N���̐i�s�����ɏd�͂����Z����B
	forwardVec.y -= 0.08f;
	forwardVec.Normalize();

	// �ړ����x��������B
	speed -= SUB_SPEED;

	// �傫��������������B
	scale -= SUB_SCALE;

	// Instance���X�V�B
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns, pos);
	PolygonInstanceRegister::Ins()->ChangeScale(particleIns, Vec3(scale, scale, scale));

	// �傫����0��菬�����Ȃ����珉��������B
	if (scale < 0) {

		scale = 0;
		Init();

		return;

	}

}
