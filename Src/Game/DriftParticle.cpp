#include "DriftParticle.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"
#include "Camera.h"
#include "PolygonInstance.h"

DriftParticle::DriftParticle()
{

	/*===== �R���X�g���N�^ =====*/

	constBufferIndex_ = 0;
	pos_ = Vec3();

	isActive_ = false;

}

void DriftParticle::Setting(std::weak_ptr<BLAS> Blas, const int ConstBufferIndex)
{

	/*===== �Z�b�e�B���O =====*/

	blas_ = Blas;
	constBufferIndex_ = ConstBufferIndex;

}

void DriftParticle::Init()
{

	/*===== ���������� =====*/

	if (isActive_) {

		PolygonInstanceRegister::Ins()->DestroyInstance(instance_);

	}

	isActive_ = false;

}

#include "RayEngine.h"
void DriftParticle::GenerateSmoke(std::weak_ptr<BLAS> Blas, const Vec3& Pos, const DirectX::XMMATRIX MatRot, bool IsBoost, Vec3 ForwardVec)
{

	/*===== �������� =====*/

	mode_ = ID::SMOKE;
	pos_ = Pos;
	forardVec_ = ForwardVec;
	blas_ = Blas;
	isActive_ = true;
	isAppearingNow_ = true;
	appearingTimer_ = 0;
	instance_ = PolygonInstanceRegister::Ins()->CreateInstance(blas_, PolygonInstanceRegister::SHADER_ID::ALPHA);
	instance_.lock()->ChangeTrans(Pos);
	instance_.lock()->ChangeRotate(MatRot);

	// �������̉�]�����x�N�g�������߂�B
	Vec3 zRotVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), MatRot);
	float random = FHelper::GetRand(-FHelper::F_PI_F, FHelper::F_PI_F);
	DirectX::XMVECTOR zRotQ = DirectX::XMQuaternionRotationAxis(zRotVec.ConvertXMVECTOR(), random);
	instance_.lock()->AddRotate(DirectX::XMMatrixRotationQuaternion(zRotQ));

	if (IsBoost) {
		instance_.lock()->ChangeScale(Vec3(30, 30, 30));
	}
	else {
		instance_.lock()->ChangeScale(Vec3(15, 15, 15));
	}

	RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].instanceIndex_ = instance_.lock()->GetInstanceIndex();
	RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ = 0.1f;

}

void DriftParticle::GenerateFire(std::weak_ptr<BLAS> Blas, const Vec3& Pos, const DirectX::XMMATRIX MatRot)
{

	/*===== �������� =====*/

	mode_ = ID::FIRE;
	pos_ = Pos;
	blas_ = Blas;
	isActive_ = true;
	isAppearingNow_ = true;
	appearingTimer_ = 0;
	instance_ = PolygonInstanceRegister::Ins()->CreateInstance(blas_, PolygonInstanceRegister::SHADER_ID::ALPHA);
	instance_.lock()->ChangeTrans(Pos);
	instance_.lock()->ChangeRotate(MatRot);

	// �������̉�]�����x�N�g�������߂�B
	Vec3 zRotVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), MatRot);
	float random = FHelper::GetRand(-FHelper::F_PI_F, FHelper::F_PI_F);
	DirectX::XMVECTOR zRotQ = DirectX::XMQuaternionRotationAxis(zRotVec.ConvertXMVECTOR(), random);
	instance_.lock()->AddRotate(DirectX::XMMatrixRotationQuaternion(zRotQ));

	instance_.lock()->ChangeScale(Vec3(5, 5, 5));

	RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].instanceIndex_ = instance_.lock()->GetInstanceIndex();
	RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f;

}

void DriftParticle::GenerateAura(std::weak_ptr<BLAS> Blas, std::weak_ptr<PolygonMeshInstance> TireInstance, const ID& Id, bool IsBoostRight)
{

	/*===== �I�[���̐������� =====*/

	mode_ = static_cast<DriftParticle::ID>(Id);
	blas_ = Blas;
	isActive_ = true;
	isAppearingNow_ = true;
	appearingTimer_ = 0;
	instance_ = PolygonInstanceRegister::Ins()->CreateInstance(blas_, PolygonInstanceRegister::SHADER_ID::ADD);
	trackedInstance = TireInstance;
	isTrackRight_ = IsBoostRight;
	changeScaleTimer_ = 0;
	isAuraBig_ = true;

	// �e�̉�]�s������߂�B
	std::weak_ptr<PolygonMeshInstance> parentInstance = TireInstance.lock()->GetParetntInstance();
	DirectX::XMMATRIX parentMatRot = parentInstance.lock()->GetRotate();

	// ���W�����߂�B
	pos_ = trackedInstance.lock()->GetWorldPos();
	Vec3 trackVec = isTrackRight_ ? Vec3(-1, 0, 0) : Vec3(1, 0, 0);
	trackVec = FHelper::MulRotationMatNormal(trackVec, parentMatRot);
	pos_ += trackVec * AURA_SIDE_SIZE;

	// �ړ�������B
	instance_.lock()->ChangeTrans(pos_);

	// ��]������B
	instance_.lock()->ChangeRotate(parentMatRot);

	// �T�C�Y��ς���B
	if (Id == ID::AURA_BIG) {

		instance_.lock()->ChangeScale(Vec3(AURA_BIG_SCALE + AURA_ADD_SCALE, AURA_BIG_SCALE + AURA_ADD_SCALE, AURA_BIG_SCALE + AURA_ADD_SCALE));
		nowScale_ = 0;
		changeScale_ = AURA_BIG_SCALE;

	}
	else if (Id == ID::AURA_SMALL) {

		instance_.lock()->ChangeScale(Vec3(AURA_SMALL_SCALE + AURA_ADD_SCALE, AURA_SMALL_SCALE + AURA_ADD_SCALE, AURA_SMALL_SCALE + AURA_ADD_SCALE));
		nowScale_ = 0;
		changeScale_ = AURA_SMALL_SCALE;

	}

	RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].instanceIndex_ = instance_.lock()->GetInstanceIndex();
	RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f;

}

void DriftParticle::GenerateDriftParticle(std::weak_ptr<BLAS> Blas, std::weak_ptr<PolygonMeshInstance> TireInstance, const ID& Id, bool IsBoostRight, bool IsLevelChange)
{

	/*===== �h���t�g���̃p�[�e�B�N���𐶐� =====*/

	mode_ = Id;
	blas_ = Blas;
	isActive_ = true;
	isAppearingNow_ = true;
	appearingTimer_ = 0;
	particleMatQ_ = DirectX::XMQuaternionIdentity();
	instance_ = PolygonInstanceRegister::Ins()->CreateInstance(blas_, PolygonInstanceRegister::SHADER_ID::ADD);
	trackedInstance = TireInstance;
	isTrackRight_ = IsBoostRight;
	changeScaleTimer_ = 0;
	grav = 0;

	// �e�̉�]�s������߂�B
	std::weak_ptr<PolygonMeshInstance> parentInstance = TireInstance.lock()->GetParetntInstance();
	DirectX::XMMATRIX parentMatRot = parentInstance.lock()->GetRotate();

	// �p�[�e�B�N�����L�̕ϐ����������B
	particlePos_ = Vec3();
	particlePrevPos_ = Vec3();
	particlePrevMoveVec_ = Vec3();
	particleVec_ = FHelper::GetRandVec3(-100, 100) / Vec3(100.0f, 100.0f, 100.0f);
	// �ԑ̂�Y�������Ƀx�N�g�������߂�B
	Vec3 parentCarUpVec = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), parentMatRot);
	particleVec_ += parentCarUpVec * 0.5f;
	particleVec_.Normalize();

	// �p�[�e�B�N���̈ړ����x�����肷��B
	if (IsLevelChange) {
		particleSpeed_ = FHelper::GetRand(static_cast<int>(MAX_PARTICLE_SPEED_LEVEL_CHANGE / 0.5f) * 100, static_cast<int>(MAX_PARTICLE_SPEED_LEVEL_CHANGE) * 100) / 100.0f;
	}
	else {
		particleSpeed_ = FHelper::GetRand(static_cast<int>(MAX_PARTICLE_SPEED / 0.5f) * 100, static_cast<int>(MAX_PARTICLE_SPEED) * 100) / 100.0f;
	}

	// ���W�����߂�B
	pos_ = trackedInstance.lock()->GetWorldPos();
	Vec3 trackVec = isTrackRight_ ? Vec3(-1, 0, 0) : Vec3(1, 0, 0);
	trackVec = FHelper::MulRotationMatNormal(trackVec, parentMatRot);
	pos_ += trackVec * AURA_SIDE_SIZE;

	// �ړ�������B
	instance_.lock()->ChangeTrans(pos_);

	// ��]������B
	instance_.lock()->ChangeRotate(parentMatRot);

	// �p�[�e�B�N���̑傫�������߂�B
	const float HUNDRED = 100.0f;

	if (IsLevelChange) {
		float random = static_cast<float>(FHelper::GetRand(static_cast<int>(PARTICLE_SCALE_LEVEL_CHANGE.x_ * HUNDRED / 2), static_cast<int>(PARTICLE_SCALE_LEVEL_CHANGE.x_ * HUNDRED)));
		particleNowScale_.x_ = random / HUNDRED;

		random = static_cast<float>(FHelper::GetRand(static_cast<int>(PARTICLE_SCALE_LEVEL_CHANGE.y_ * HUNDRED / 2), static_cast<int>(PARTICLE_SCALE_LEVEL_CHANGE.y_ * HUNDRED)));
		particleNowScale_.y_ = random / HUNDRED;
	}
	else {
		float random = static_cast<float>(FHelper::GetRand(static_cast<int>(PARTICLE_SCALE.x_ * HUNDRED / 2), static_cast<int>(PARTICLE_SCALE.x_ * HUNDRED)));
		particleNowScale_.x_ = random / HUNDRED;

		random = static_cast<float>(FHelper::GetRand(static_cast<int>(PARTICLE_SCALE.y_ * HUNDRED / 2), static_cast<int>(PARTICLE_SCALE.y_ * HUNDRED)));
		particleNowScale_.y_ = random / HUNDRED;
	}

	// �T�C�Y��ς���B
	instance_.lock()->ChangeScale(Vec3(particleNowScale_.x_, particleNowScale_.y_, particleNowScale_.x_));

	RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].instanceIndex_ = instance_.lock()->GetInstanceIndex();
	RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f;

}

void DriftParticle::GenerateJumpEffect(std::weak_ptr<BLAS> Blas, std::weak_ptr<PolygonMeshInstance> CarBodyInstance)
{

	/*===== �W�����v�G�t�F�N�g�𐶐� =====*/

	mode_ = ID::JUMP_EFFECT;
	blas_ = Blas;
	isActive_ = true;
	isAppearingNow_ = true;
	jumpActionEasingTimer_ = 0;
	instance_ = PolygonInstanceRegister::Ins()->CreateInstance(blas_, PolygonInstanceRegister::SHADER_ID::ALPHA);
	trackedInstance = CarBodyInstance;

	// ���W�����߂�B
	pos_ = trackedInstance.lock()->GetWorldPos();

	// �ړ�������B
	instance_.lock()->ChangeTrans(pos_);

	// �T�C�Y��ς���B
	instance_.lock()->ChangeScale(Vec3(particleNowScale_.x_, particleNowScale_.y_, particleNowScale_.x_));

	RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].instanceIndex_ = instance_.lock()->GetInstanceIndex();
	RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f;

}

void DriftParticle::Update()
{

	/*===== �X�V���� =====*/

	switch (mode_)
	{
	case DriftParticle::ID::SMOKE:

		// �ړ�������B
		pos_ += forardVec_ * SMOKE_SPEED;

		// �ړ�������B
		instance_.lock()->ChangeTrans(pos_);

		if (isAppearingNow_) {

			// �A���t�@�l�����Z�B
			RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ += APPEARING_ALPHA;
			if (1.0f < RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_) {

				RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f;
				++appearingTimer_;

				// �o���^�C�}�[�����l�ɒB������A���t�@�l�������Ă����B
				if (APPEARING_TIMER < appearingTimer_) {

					isAppearingNow_ = false;

				}

			}

		}
		else {

			RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ -= EXIT_ALPHA;

		}

		break;
	case DriftParticle::ID::FIRE:

		RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ -= FIRE_ALPHA;
		instance_.lock()->AddScale(Vec3(1.0f, 1.0f, 1.0f));

		break;

	case DriftParticle::ID::AURA_BIG:
	case DriftParticle::ID::AURA_SMALL:

	{

		// �e�̉�]�s������߂�B
		std::weak_ptr<PolygonMeshInstance> parentInstance = trackedInstance.lock()->GetParetntInstance();
		DirectX::XMMATRIX parentMatRot = parentInstance.lock()->GetRotate();

		// ���W�����߂�B
		pos_ = trackedInstance.lock()->GetWorldPos();
		Vec3 trackVec = isTrackRight_ ? Vec3(-1, 0, 0) : Vec3(1, 0, 0);
		trackVec = FHelper::MulRotationMatNormal(trackVec, parentMatRot);
		pos_ += trackVec * AURA_SIDE_SIZE;

		// �ړ�������B
		instance_.lock()->ChangeTrans(pos_);

		// ��]������B
		instance_.lock()->ChangeRotate(parentMatRot);

		// �T�C�Y��ς���B
		if (isAuraBig_) {

			float CHANGE_SCALE = (mode_ == ID::AURA_BIG ? AURA_BIG_SCALE + AURA_ADD_SCALE : AURA_SMALL_SCALE + AURA_ADD_SCALE);

			// �⊮����B
			nowScale_ += (CHANGE_SCALE - nowScale_) / 2.0f;

			// ������x�ۊǂ��ꂽ��B
			if (fabs(nowScale_ - CHANGE_SCALE) < 0.1f) {

				isAuraBig_ = false;

			}

		}
		else {

			++changeScaleTimer_;
			int MAX_CHANGE_TIMER = (mode_ == ID::AURA_BIG ? CHANGE_SCALE_TIMER_BIG : CHANGE_SCALE_TIMER_SMALL);
			if (MAX_CHANGE_TIMER < changeScaleTimer_) {

				changeScaleTimer_ = 0;

				// ��Ԑ�̃X�P�[���������_���ŋ��߂�B
				const float HUNDRED = 100.0f;
				int CHANGE_SCALE = static_cast<int>((mode_ == ID::AURA_BIG ? AURA_BIG_SCALE * HUNDRED : AURA_SMALL_SCALE * HUNDRED));
				float random = static_cast<float>(FHelper::GetRand(CHANGE_SCALE / 2, CHANGE_SCALE));
				changeScale_ = random / 100.0f;

			}

			// �X�P�[����⊮����B
			nowScale_ += (changeScale_ - nowScale_) / 2.0f;

		}


		instance_.lock()->ChangeScale(Vec3(nowScale_, nowScale_, nowScale_));


	}

	break;

	case DriftParticle::ID::PARTICLE:

	{

		// �e�̉�]�s������߂�B
		std::weak_ptr<PolygonMeshInstance> parentInstanceIndex = trackedInstance.lock()->GetParetntInstance();
		DirectX::XMMATRIX parentMatRot = parentInstanceIndex.lock()->GetRotate();

		// ���W�����߂�B
		pos_ = trackedInstance.lock()->GetWorldPos();
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
		instance_.lock()->ChangeTrans(pos_ + particlePos_);

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
			instance_.lock()->ChangeRotate(quaternionMat);

		}


		// ���t���[���̈ړ������x�N�g����ۑ����Ă����B
		particlePrevMoveVec_ = nowFrameMovedVec;

		// �T�C�Y��ς���B
		instance_.lock()->ChangeScale(Vec3(particleNowScale_.x_, particleNowScale_.y_, particleNowScale_.x_));

		// ������܂ł̃^�C�}�[���X�V�B
		++appearingTimer_;
		if (10 < appearingTimer_) {

			particleNowScale_.y_ -= 0.5f;
			if (particleNowScale_.y_ <= 0.1f) particleNowScale_.y_ = 0.1f;
			RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ -= EXIT_ALPHA;

		}

	}

	break;

	case DriftParticle::ID::JUMP_EFFECT:

	{

		// �Ԃ̍��W�����߂�B
		Vec3 carPos = trackedInstance.lock()->GetWorldPos();

		// ���W��ݒ�B
		instance_.lock()->ChangeTrans(carPos);

		// ��]��ݒ�B
		DirectX::XMMATRIX charaRot = trackedInstance.lock()->GetRotate();
		instance_.lock()->ChangeRotate(charaRot);

		// �C�[�W���O�^�C�}�[���X�V���A�C�[�W���O�ʂ����߂�B
		jumpActionEasingTimer_ = FHelper::Saturate(jumpActionEasingTimer_ + ADD_JUMP_ACTION_EASING_TIMER_APPE);
		float easingAmount = FEasing::EaseInSine(jumpActionEasingTimer_);

		// �X�P�[����ݒ�B
		float nowScale = JUMP_DEF_SCALE + (JUMP_BIG_SCALE - JUMP_DEF_SCALE) * easingAmount;
		instance_.lock()->ChangeScale(Vec3(nowScale, nowScale, nowScale));

		// �A���t�@�����߂�B
		RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f - (1.1f * easingAmount);

		//// �o������������B
		//if (isAppearingNow_) {

		//	// �C�[�W���O�^�C�}�[���X�V���A�C�[�W���O�ʂ����߂�B
		//	jumpActionEasingTimer_ = FHelper::Saturate(jumpActionEasingTimer_ + ADD_JUMP_ACTION_EASING_TIMER_APPE);
		//	float easingAmount = FEasing::EaseOutCubic(jumpActionEasingTimer_);

		//	// �X�P�[����ݒ�B
		//	float nowScale = JUMP_DEF_SCALE + (JUMP_BIG_SCALE - JUMP_DEF_SCALE) * easingAmount;
		//	PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(nowScale, nowScale, nowScale));

		//	// �A���t�@�����߂�B
		//	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ = 0.1f + (1.0f - 0.1f) * easingAmount;

		//	// �^�C�}�[���J���X�g�����玟�ցB
		//	if (1.0f <= jumpActionEasingTimer_) {

		//		jumpActionEasingTimer_ = 0;
		//		isAppearingNow_ = false;

		//	}

		//}
		//else {

		//	// �C�[�W���O�^�C�}�[���X�V���A�C�[�W���O�ʂ����߂�B
		//	jumpActionEasingTimer_ = FHelper::Saturate(jumpActionEasingTimer_ + ADD_JUMP_ACTION_EASING_TIMER_EXIT);
		//	float easingAmount = FEasing::EaseInSine(jumpActionEasingTimer_);

		//	// �X�P�[����ݒ�B
		//	float nowScale = JUMP_BIG_SCALE + (JUMP_EXIT_SCALE - JUMP_BIG_SCALE) * easingAmount;
		//	PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(nowScale, nowScale, nowScale));

		//	// �A���t�@�����߂�B
		//	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f + (-1.1 * easingAmount);

		//}

	}

	break;

	default:
		break;
	}


	if (RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ < 0.0f) {

		Init();

	}

}