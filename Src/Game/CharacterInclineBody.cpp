#include "CharacterInclineBody.h"
#include "FHelper.h"
#include "Character.h"
#include "PolygonInstance.h"

CharacterInclineBody::CharacterInclineBody()
{

	/*===== �R���X�g���N�^ =====*/

	boostRotQ_ = DirectX::XMVECTOR();
	handleRotQ_ = DirectX::XMVECTOR();
	nowBoostRotQ_ = DirectX::XMVECTOR();
	nowHandleRotQ_ = DirectX::XMVECTOR();
	handleAmount_ = 0;
	baseHandleAmount_ = 0;
	tireLollingAmount_ = 0;
	isRotRightSide_ = false;
	nowDriftRot_ = 0;
	baseDriftRot_ = 0;
	driftRotTimer_ = 0;
	baseBoostRot_ = 0;
	nowBoostRot_ = 0;
	nowFrameHandleAmount_ = 0;

}

void CharacterInclineBody::Init()
{

	/*===== ���������� =====*/

	boostRotQ_ = DirectX::XMVECTOR();
	handleRotQ_ = DirectX::XMVECTOR();
	nowBoostRotQ_ = DirectX::XMVECTOR();
	nowHandleRotQ_ = DirectX::XMVECTOR();
	handleAmount_ = 0;
	baseHandleAmount_ = 0;
	tireLollingAmount_ = 0;
	isRotRightSide_ = false;
	nowDriftRot_ = 0;
	baseDriftRot_ = 0;
	driftRotTimer_ = 0;
	baseBoostRot_ = 0;
	nowBoostRot_ = 0;
	nowFrameHandleAmount_ = 0;

}

void CharacterInclineBody::Update(const InputData& Input)
{

	/*===== �X�V���� =====*/

	// �n���h���̗ʂ��Ԃ���B
	handleAmount_ += (baseHandleAmount_ - handleAmount_) / 30.0f;

	// �󒆂ɋ�����B
	if (!Input.onGround_ && !Input.isDriftJump_) {

		// �ړ����������x�N�g��
		Vec3 movedVec = (Input.pos_ - Input.prevPos_).GetNormal();

		if (!std::isnan(movedVec.x_) && Input.isPlayer_) {

			// �f�t�H���g�̏�x�N�g���B
			Vec3 defUpVec = Vec3(0, -1, 0);

			// ��]�������߂�B
			Vec3 axisOfRevolution = defUpVec.Cross(movedVec);

			// ��]���𐳋K������B
			if (axisOfRevolution.Length() != 0) {
				axisOfRevolution.Normalize();
			}

			/*-- ���ς����]�ʂ��擾 --*/

			// ��]�ʂ����߂�B
			float amountOfRotation = defUpVec.Dot(movedVec);

			// �t�]�������߂�֐����g�p���ċ��߂�cos�Ƃ����W�A���ɕϊ��B
			amountOfRotation = acosf(amountOfRotation) - DirectX::XM_PIDIV2;


			/*-- �N�H�[�^�j�I�����g���ĉ�] --*/

			// ��]����{0,0,0}�������珈�����΂��B
			if (axisOfRevolution.Length() != 0 && amountOfRotation != 0) {

				// �N�H�[�^�j�I�������߂�B
				DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(axisOfRevolution.ConvertXMVECTOR(), amountOfRotation);

				// ���߂��N�H�[�^�j�I�����s��Ɏ����B
				DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

				// �v���C���[����]������B
				Input.playerModel_.carBodyInstance_.lock()->ChangeRotate(quaternionMat);

				// ��x�N�g������Ƃ����N�H�[�^�j�I�������߂�B
				Vec3 normal_ = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), Input.playerModel_.carBodyInstance_.lock()->GetRotate());
				DirectX::XMVECTOR upQuaternion = DirectX::XMQuaternionRotationNormal(normal_.ConvertXMVECTOR(), Input.rotY_);

				// �N�H�[�^�j�I�����s��Ɏ����B
				DirectX::XMMATRIX upQuaternionMat = DirectX::XMMatrixRotationQuaternion(upQuaternion);

				// �v���C���[����]������B
				Input.playerModel_.carBodyInstance_.lock()->AddRotate(upQuaternionMat);

			}
		}

	}

	// �u�[�X�g���̉�]�B
	if (0 < Input.boostSpeed_) {

		float boostRate = Input.boostSpeed_ / Input.maxBoostSpeed_;
		const float MAX_ROT = 0.1f;

		baseBoostRot_ = MAX_ROT * boostRate;

		// �o�����X�����^�C�}�[���X�V�B
		++forwardTireLollingTimer_;
		if (FORWARD_TIMER_LOLLING_TIMER < forwardTireLollingTimer_) {

			forwardTireLollingTimer_ = 0;

			// �o�����X������Ă��镗�ɂ��邽�߂Ƀ����_���ŉ�]�ʂ�ύX����B
			const float MAX_RANDOM = 0.4f;
			baseBoostRot_ += FHelper::GetRand(-MAX_RANDOM * boostRate, MAX_RANDOM * boostRate);

		}

	}
	else {

		boostRotQ_ = DirectX::XMVECTOR();
		forwardTireLollingTimer_ = 0;
		baseBoostRot_ = 0;

	}

	{

		nowBoostRot_ += (baseBoostRot_ - nowBoostRot_) / 5.0f;

		Vec3 horiVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), Input.playerModel_.carBodyInstance_.lock()->GetRotate());

		// �N�H�[�^�j�I�������߂�B
		boostRotQ_ = DirectX::XMQuaternionRotationAxis(horiVec.ConvertXMVECTOR(), nowBoostRot_);

		// �u�[�X�g�̎ԑ̂̃N�H�[�^�j�I�����ԁB
		nowBoostRotQ_ = DirectX::XMQuaternionSlerp(nowBoostRotQ_, boostRotQ_, 0.2f);
		// ��]�s������߂�B
		DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(nowBoostRotQ_);
		Input.playerModel_.carBodyInstance_.lock()->AddRotate(mat);

	}


	// �h���t�g���̉�]�B
	if (Input.isDrift_ && 0 != handleAmount_) {

		// �O�t���[���Ɖ�]�������Ⴉ�����珉��������B
		if (isRotRightSide_ && handleAmount_ < 0) {
			handleRotQ_ = DirectX::XMVECTOR();
			baseDriftRot_ = 0;
			driftRotTimer_ = 0;
		}

		++driftRotTimer_;
		if (MAX_DRIFT_ROT_TIMER < driftRotTimer_) driftRotTimer_ = MAX_DRIFT_ROT_TIMER;

		float timerRate = static_cast<float>(driftRotTimer_) / static_cast<float>(MAX_DRIFT_ROT_TIMER);

		// ��]�ʂɃC�[�W���O��������B �}�W�b�N�i���o�[�͌X���̔������p�B
		if (0.35f < fabs(handleAmount_)) {
			baseDriftRot_ = 0.6f * (0 < handleAmount_ ? 1.0f : -1.0f);
		}
		else {
			baseDriftRot_ = std::fabs(FEasing::EaseOutCubic(timerRate) * MAX_DRIFT_ROT) * handleAmount_;
		}

		// ��]������ۑ��B
		isRotRightSide_ = 0 < handleAmount_;

		// �o�����X������Ă��镗�ɂ��邽�߂Ƀ����_���ŉ�]�ʂ�ύX����B
		const float MAX_RANDOM = 0.1f;
		tireLollingAmount_ = FHelper::GetRand(-MAX_RANDOM * timerRate, MAX_RANDOM * timerRate);
		baseDriftRot_ += tireLollingAmount_;

	}
	else {

		handleRotQ_ = DirectX::XMVECTOR();

		baseDriftRot_ = 0;
		driftRotTimer_ = 0;

	}

	// ��]������B
	{
		nowDriftRot_ += (baseDriftRot_ - nowDriftRot_) / 10.0f;

		Vec3 horiVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), Input.playerModel_.carBodyInstance_.lock()->GetRotate());
		Vec3 forwardVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), Input.playerModel_.carBodyInstance_.lock()->GetRotate());
		Vec3 upVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), Input.playerModel_.carBodyInstance_.lock()->GetRotate());

		// ���N�H�[�^�j�I�������߂�B
		handleRotQ_ = DirectX::XMQuaternionRotationAxis(horiVec.ConvertXMVECTOR(), nowDriftRot_);
		// ���ʃN�H�[�^�j�I�������߂�B ���������Ɖ�]��������Ƃ�������Ă��邽�߁B
		DirectX::XMVECTOR forwardQ = DirectX::XMQuaternionRotationAxis(forwardVec.ConvertXMVECTOR(), 0.01f);
		// ��N�H�[�^�j�I�������߂�B
		DirectX::XMVECTOR upQ = DirectX::XMQuaternionRotationAxis(upVec.ConvertXMVECTOR(), -0.2f * handleAmount_);

		// �N�H�[�^�j�I����������B
		handleRotQ_ = DirectX::XMQuaternionMultiply(handleRotQ_, forwardQ);
		handleRotQ_ = DirectX::XMQuaternionMultiply(handleRotQ_, upQ);
	}

	// �u�[�X�g�̎ԑ̂̃N�H�[�^�j�I�����ԁB
	nowHandleRotQ_ = DirectX::XMQuaternionSlerp(nowHandleRotQ_, handleRotQ_, 0.2f);
	// ��]�s������߂�B
	DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(nowHandleRotQ_);
	Input.playerModel_.carBodyInstance_.lock()->AddRotate(mat);

}

void CharacterInclineBody::InitQuaternion()
{

	/*===== �ԑ̂̉�]�Ɏg�p���Ă���N�H�[�^�j�I�������������� =====*/

	boostRotQ_ = DirectX::XMVECTOR();
	handleRotQ_ = DirectX::XMVECTOR();
	nowBoostRotQ_ = DirectX::XMVECTOR();
	nowHandleRotQ_ = DirectX::XMVECTOR();

}
