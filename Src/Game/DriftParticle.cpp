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

void DriftParticle::GenerateSmoke(const int& BlasIndex, const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData, const bool& IsBoost)
{

	/*===== �������� =====*/

	id_ = ID::SMOKE;
	pos_ = Pos;
	blasIndex_ = BlasIndex;
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

void DriftParticle::GenerateFire(const int& BlasIndex, const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData)
{

	/*===== �������� =====*/

	id_ = ID::FIRE;
	pos_ = Pos;
	blasIndex_ = BlasIndex;
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

	PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(5, 5, 5));

	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].instanceIndex_ = particleIns_;
	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f;

}

void DriftParticle::GenerateAura(const int& BlasIndex, const int& TireInsIndex_, const ID& Id, const bool& IsBoostRight, RayConstBufferData& ConstBufferData)
{

	/*===== �I�[���̐������� =====*/

	id_ = Id;
	blasIndex_ = BlasIndex;
	isActive_ = true;
	isAppearingNow_ = true;
	appearingTimer_ = 0;
	particleIns_ = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex_, PolygonInstanceRegister::SHADER_ID::ALPHA);
	trackedID_ = TireInsIndex_;
	isTrackRight_ = IsBoostRight;
	changeScaleTimer_ = 0;

	// �e�̉�]�s������߂�B
	int parentInstanceIndex = PolygonInstanceRegister::Ins()->GetParentInstanceIndex(TireInsIndex_);
	DirectX::XMMATRIX parentMatRot = PolygonInstanceRegister::Ins()->GetRotate(parentInstanceIndex);

	// ���W�����߂�B
	pos_ = PolygonInstanceRegister::Ins()->GetWorldPos(trackedID_);
	Vec3 trackVec = isTrackRight_ ? Vec3(-1, 0, 0) : Vec3(1, 0, 0);
	trackVec = FHelper::MulRotationMatNormal(trackVec, parentMatRot);
	pos_ += trackVec * AURA_SIDE_SIZE;

	// �ړ�������B
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, pos_);

	// ��]������B
	PolygonInstanceRegister::Ins()->ChangeRotate(particleIns_, parentMatRot);

	// �T�C�Y��ς���B
	if (Id == ID::AURA_BIG) {

		PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(AURA_BIG_SCALE, AURA_BIG_SCALE, AURA_BIG_SCALE));
		nowScale_ = 0.0f;
		changeScale_ = 10.0f;

	}
	else if (Id == ID::AURA_SMALL) {

		PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(AURA_SMALL_SCALE, AURA_SMALL_SCALE, AURA_SMALL_SCALE));
		nowScale_ = 5.0f;
		changeScale_ = 10.0f;

	}

	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].instanceIndex_ = particleIns_;
	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f;

}

void DriftParticle::GenerateDriftParticle(const int& BlasIndex, const int& TireInsIndex_, const ID& Id, const bool& IsBoostRight, RayConstBufferData& ConstBufferData)
{

	/*===== �h���t�g���̃p�[�e�B�N���𐶐� =====*/

	id_ = Id;
	blasIndex_ = BlasIndex;
	isActive_ = true;
	isAppearingNow_ = true;
	appearingTimer_ = 0;
	particleMatQ_ = DirectX::XMQuaternionIdentity();
	particleIns_ = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex_, PolygonInstanceRegister::SHADER_ID::TEXCOLOR);
	trackedID_ = TireInsIndex_;
	isTrackRight_ = IsBoostRight;
	changeScaleTimer_ = 0;
	grav = 0;

	// �e�̉�]�s������߂�B
	int parentInstanceIndex = PolygonInstanceRegister::Ins()->GetParentInstanceIndex(TireInsIndex_);
	DirectX::XMMATRIX parentMatRot = PolygonInstanceRegister::Ins()->GetRotate(parentInstanceIndex);

	// �p�[�e�B�N�����L�̕ϐ����������B
	particlePos_ = Vec3();
	particlePrevPos_ = Vec3();
	particlePrevMoveVec_ = Vec3();
	particleVec_ = FHelper::GetRandVec3(-100, 100) / Vec3(100.0f, 100.0f, 100.0f);
	// �ԑ̂�Y�������Ƀx�N�g�������߂�B
	Vec3 parentCarUpVec = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), parentMatRot);
	particleVec_ += parentCarUpVec * 0.5f;
	particleVec_.Normalize();
	particleSpeed_ = FHelper::GetRand(static_cast<int>(MAX_PARTICLE_SPEED / 0.5f) * 100, static_cast<int>(MAX_PARTICLE_SPEED) * 100) / 100.0f;

	// ���W�����߂�B
	pos_ = PolygonInstanceRegister::Ins()->GetWorldPos(trackedID_);
	Vec3 trackVec = isTrackRight_ ? Vec3(-1, 0, 0) : Vec3(1, 0, 0);
	trackVec = FHelper::MulRotationMatNormal(trackVec, parentMatRot);
	pos_ += trackVec * AURA_SIDE_SIZE;

	// �ړ�������B
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, pos_);

	// ��]������B
	PolygonInstanceRegister::Ins()->ChangeRotate(particleIns_, parentMatRot);

	// �p�[�e�B�N���̑傫�������߂�B
	const float HUNDRED = 100.0f;
	float random = FHelper::GetRand(static_cast<int>(PARTICLE_SCALE.x_ * HUNDRED / 2), static_cast<int>(PARTICLE_SCALE.x_ * HUNDRED));
	particleNowScale_.x_ = random / HUNDRED;

	random = FHelper::GetRand(static_cast<int>(PARTICLE_SCALE.y_ * HUNDRED / 2), static_cast<int>(PARTICLE_SCALE.y_ * HUNDRED));
	particleNowScale_.y_ = random / HUNDRED;

	// �T�C�Y��ς���B
	PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(particleNowScale_.x_, particleNowScale_.y_, particleNowScale_.x_));

	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].instanceIndex_ = particleIns_;
	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f;

}

void DriftParticle::Update(RayConstBufferData& ConstBufferData)
{

	/*===== �X�V���� =====*/

	switch (id_)
	{
	case DriftParticle::ID::SMOKE:

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

		break;
	case DriftParticle::ID::FIRE:

		ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ -= FIRE_ALPHA;
		PolygonInstanceRegister::Ins()->AddScale(particleIns_, Vec3(1.0f, 1.0f, 1.0f));

		break;

	case DriftParticle::ID::AURA_BIG:
	case DriftParticle::ID::AURA_SMALL:

	{

		// �e�̉�]�s������߂�B
		int parentInstanceIndex = PolygonInstanceRegister::Ins()->GetParentInstanceIndex(trackedID_);
		DirectX::XMMATRIX parentMatRot = PolygonInstanceRegister::Ins()->GetRotate(parentInstanceIndex);

		// ���W�����߂�B
		pos_ = PolygonInstanceRegister::Ins()->GetWorldPos(trackedID_);
		Vec3 trackVec = isTrackRight_ ? Vec3(-1, 0, 0) : Vec3(1, 0, 0);
		trackVec = FHelper::MulRotationMatNormal(trackVec, parentMatRot);
		pos_ += trackVec * AURA_SIDE_SIZE;

		// �ړ�������B
		PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, pos_);

		// ��]������B
		PolygonInstanceRegister::Ins()->ChangeRotate(particleIns_, parentMatRot);

		// �T�C�Y��ς���B
		++changeScaleTimer_;
		const int MAX_CHANGE_TIMER = (id_ == ID::AURA_BIG ? CHANGE_SCALE_TIMER_BIG : CHANGE_SCALE_TIMER_SMALL);
		if (MAX_CHANGE_TIMER < changeScaleTimer_) {

			changeScaleTimer_ = 0;

			// ��Ԑ�̃X�P�[���������_���ŋ��߂�B
			const float HUNDRED = 100.0f;
			const int CHANGE_SCALE = (id_ == ID::AURA_BIG ? AURA_BIG_SCALE * HUNDRED : AURA_SMALL_SCALE * HUNDRED);
			float random = FHelper::GetRand(CHANGE_SCALE / 2, CHANGE_SCALE);
			changeScale_ = random / 100.0f;

		}

		// �X�P�[����⊮����B
		nowScale_ += (changeScale_ - nowScale_) / 2.0f;
		PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(nowScale_, nowScale_, nowScale_));


	}

	break;

	case DriftParticle::ID::PARTICLE:

	{

		// �e�̉�]�s������߂�B
		int parentInstanceIndex = PolygonInstanceRegister::Ins()->GetParentInstanceIndex(trackedID_);
		DirectX::XMMATRIX parentMatRot = PolygonInstanceRegister::Ins()->GetRotate(parentInstanceIndex);

		// ���W�����߂�B
		pos_ = PolygonInstanceRegister::Ins()->GetWorldPos(trackedID_);
		Vec3 trackVec = isTrackRight_ ? Vec3(-1, 0, 0) : Vec3(1, 0, 0);
		trackVec = FHelper::MulRotationMatNormal(trackVec, parentMatRot);
		pos_ += trackVec * AURA_SIDE_SIZE;

		// �OF�̍��W��ۑ��B
		particlePrevPos_ = particlePos_;

		// �p�[�e�B�N�����L�̍��W���ړ�������B
		particlePos_ += particleVec_ * particleSpeed_;
		particleSpeed_ -= 0.4f;
		if (particleSpeed_ < 1.0f) particleSpeed_ = 1.0f;

		// �d�͂��X�V�B
		grav += ADD_GRAV;
		if (MAX_GRAV < grav) grav = MAX_GRAV;
		particlePos_.y_ -= grav;


		// �ړ�������B
		PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, pos_ + particlePos_);

		// ���t���[���̈ړ������x�N�g�������߂�B
		Vec3 nowFrameMovedVec = (particlePos_ - particlePrevPos_).GetNormal();

		// �e�̉�]�s����N�H�[�^�j�I���Ɏ����B
		DirectX::XMVECTOR parentRotQ = DirectX::XMQuaternionRotationMatrix(parentMatRot);

		// �ړ��x�N�g�������Ƀp�[�e�B�N������]������B
		// ��]�������߂�B
		Vec3 axisOfRevolution = particlePrevMoveVec_.Cross(nowFrameMovedVec);

		// ��]���𐳋K������B
		if (axisOfRevolution.Length() != 0) {
			axisOfRevolution.Normalize();
		}

		// ��]�ʂ����߂�B
		float amountOfRotation = particlePrevMoveVec_.Dot(nowFrameMovedVec);

		// �t�]�������߂�֐����g�p���ċ��߂�cos�Ƃ����W�A���ɕϊ��B
		amountOfRotation = acosf(amountOfRotation);

		// ��]����{0,0,0}�������珈�����΂��B
		if (axisOfRevolution.Length() != 0 && amountOfRotation != 0) {

			// �N�H�[�^�j�I�������߂�B
			DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(axisOfRevolution.ConvertXMVECTOR(), amountOfRotation);

			// �p�[�e�B�N���̃N�H�[�^�j�I���ɂ�����B
			particleMatQ_ = DirectX::XMQuaternionMultiply(particleMatQ_, quaternion);

			// �N�H�[�^�j�I����������B
			parentRotQ = DirectX::XMQuaternionMultiply(parentRotQ, particleMatQ_);

			// ���߂��N�H�[�^�j�I�����s��Ɏ����B
			DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(parentRotQ);

			// ��]������B
			PolygonInstanceRegister::Ins()->ChangeRotate(particleIns_, quaternionMat);

		}


		// ���t���[���̈ړ������x�N�g����ۑ����Ă����B
		particlePrevMoveVec_ = nowFrameMovedVec;

		// �T�C�Y��ς���B
		PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(particleNowScale_.x_, particleNowScale_.y_, particleNowScale_.x_));

		// ������܂ł̃^�C�}�[���X�V�B
		++appearingTimer_;
		if (10 < appearingTimer_) {

			particleNowScale_.y_ -= 0.5f;
			if (particleNowScale_.y_ <= 0.1f) particleNowScale_.y_ = 0.1f;
			ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ -= EXIT_ALPHA;

		}

	}

	break;

	default:
		break;
	}


	if (ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ < 0.0f) {

		Init();

	}

}