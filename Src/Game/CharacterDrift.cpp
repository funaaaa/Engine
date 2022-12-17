#include "CharacterDrift.h"
#include "PolygonInstance.h"
#include "TextureManager.h"
#include "BLAS.h"
#include "CharacterInclineBody.h"
#include "DriftParticleMgr.h"
#include "DriftParticle.h"
#include "CharacterGameFinish.h"

CharacterDrift::CharacterDrift()
{

	/*===== �R���X�g���N�^ =====*/

	Init();

}

void CharacterDrift::Init()
{

	/*===== ���������� =====*/

	driftJumpVec_ = Vec3();
	driftJumpSpeed_ = 0.0f;
	driftTimer_ = 0;
	turningIndicatorTimer_ = 0;
	beforeStartSmokeTimer_ = 0;
	isDriftRight_ = false;
	isInputLTPrev_ = false;
	isInputLT_ = false;
	isDriftJump_ = false;
	isDriftJumpPrev_ = false;
	isDrift_ = false;
	isTireMask_ = false;
	IsTurningIndicatorRed_ = false;

}

void CharacterDrift::Input(const InputData& Input, BaseOperationObject::Operation& Operation, std::vector<std::shared_ptr<PlayerTire>>& Tires, PlayerModel& PlayerModel, float& RotY, float& BoostSpeed, Vec3& ForwardVec)
{

	/*===== ���͏��� =====*/

	// �^�C���̃}�X�N�̃t���O������������B
	isTireMask_ = false;

	// �E�X�e�B�b�N�̉��̌X���ʂŃL��������]������B
	if ((Operation.handleDriveRate_ != 0 || isDrift_) && !Input.isBeforeStart_) {

		// ��]�ʂ̍ő�l �ʏ��Ԃƃh���t�g��ԂňႤ�B
		float handleAmount = Input.handleNormal_;

		// [�h���t�g���] [�J�n�O����Ȃ�] ���������]�ʂ𑽂����ɂ���B
		if (isDrift_ && !Input.isBeforeStart_) {

			handleAmount = Input.handleNormal_;

			// �h���t�g�̌����ɂ���ĉ�]�ʂ�ς���B
			if (isDriftRight_ && Operation.handleDriveRate_ < 0.1f) {
				Operation.handleDriveRate_ = 0.1f;
			}
			if (!isDriftRight_ && -0.1f < Operation.handleDriveRate_) {
				Operation.handleDriveRate_ = -0.1f;
			}

		}

		// �^�C�������o���B
		if (0.95f < fabs(Operation.handleDriveRate_) || Operation.isDrift_) {
			isTireMask_ = true;
		}

		// �^�C������]������B
		for (auto& index : Tires) {

			index->Rot(true, static_cast<float>(Operation.handleDriveRate_));

		}

		// �N�H�[�^�j�I�������߂�B
		DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(Vec3(Input.upVec_).ConvertXMVECTOR(), handleAmount * Operation.handleDriveRate_);

		// ���߂��N�H�[�^�j�I�����s��Ɏ����B
		DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(quaternion);

		// ��]�����Z����B
		PlayerModel.carBodyInstance_.lock()->AddRotate(quaternionMat);
		RotY += handleAmount * Operation.handleDriveRate_;

		// ���ʃx�N�g�����Ԃ̉�]�s�񕪉�]������B
		ForwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), PlayerModel.carBodyInstance_.lock()->GetRotate());


		// �E�C���J�[�̐F��ς���^�C�}�[���X�V�B
		++turningIndicatorTimer_;
		if (TURNING_INDICATOR_TIMER < turningIndicatorTimer_) {

			turningIndicatorTimer_ = 0;
			IsTurningIndicatorRed_ = IsTurningIndicatorRed_ ? false : true;

		}

		// �E�C���J�[�̐F��ς���B
		if (IsTurningIndicatorRed_) {

			// �Ȃ����Ă���̂��E��������B
			if (0 < Operation.handleDriveRate_) {

				PlayerModel.carRightLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/blackRed.png"));
				PlayerModel.carLeftLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));

			}
			else {

				PlayerModel.carLeftLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/blackRed.png"));
				PlayerModel.carRightLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));

			}

		}
		else {

			PlayerModel.carRightLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));
			PlayerModel.carLeftLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));

		}

	}
	else {

		// �Ԃ̃��C�g�̐F�����ɖ߂��B
		PlayerModel.carRightLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));
		PlayerModel.carLeftLightBlas_.lock()->ChangeBaseTexture(TextureManager::Ins()->LoadTexture(L"Resource/Game/Car/TurningIndicator/white.png"));

		// �e�ϐ����������B
		IsTurningIndicatorRed_ = false;
		turningIndicatorTimer_ = 100;	// �`�J�`�J����^�C�}�[���������B

	}

	// �h���t�g�̓��͏���ۑ�����B
	isInputLTPrev_ = isInputLT_;
	isInputLT_ = Operation.isDrift_;

	// �h���t�g�W�����v�̃t���O��ۑ����Ă����B
	isDriftJumpPrev_ = isDriftJump_;

	// �h���t�g�{�^���̓��͂��g���K�[��������B
	bool triggerDriftBottom = !isInputLTPrev_ && isInputLT_;
	bool notJump = !isDriftJump_ && driftJumpSpeed_ <= 0.1f;
	bool isOnGround = Input.onGround_ || Input.isBeforeStart_;	// �ݒu���Ă����� �Q�[�����n�܂��Ă��Ȃ��ꍇ�A�L�����͋󒆂ɕ����Ă���̂ŁA�ڒn��������B
	if (triggerDriftBottom && notJump && isOnGround && Input.canNotMoveTimer_ <= 0) {

		isDriftJump_ = true;
		driftJumpVec_ = Input.upVec_;
		driftJumpSpeed_ = DRIFT_JUMP_SPEED;

	}

	// �h���t�g��ԂŃ^�C�}�[���J�E���g����Ă�����B
	bool releaseDriftBottom = isInputLTPrev_ && !isInputLT_;
	if (isDrift_ && 0 < driftTimer_ && releaseDriftBottom) {

		isDrift_ = false;

		// �^�C�}�[�ɉ����ĉ���������B
		int driftLevel = -1;
		for (auto& index : DRIFT_TIMER) {

			if (driftTimer_ < index) break;

			driftLevel = static_cast<int>(&index - &DRIFT_TIMER[0]);

		}

		// �h���t�g���x����0�ȏゾ�������������B
		if (driftLevel != -1) {
			BoostSpeed = DRIFT_BOOST[driftLevel];
		}

	}

	// �o�O�����LT��������Ă��Ȃ�������h���t�g����������B �h���t�g�����̐��K�̏����͂��̏����̏�ɂ���̂Œʏ�͖��Ȃ��B
	// �J�n�O�̏�Ԃł��h���t�g����������B
	if (!isInputLT_ || Input.isBeforeStart_) {

		isDrift_ = false;
		driftTimer_ = 0;

	}

}

void CharacterDrift::Update(std::weak_ptr<CharacterInclineBody> InclineBody, Vec3& Pos, Vec3& PrevPos, bool OnGround)
{

	/*===== �X�V���� =====*/

		// �h���t�g�W�����v���Ă�����B
	if (isDriftJump_) {

		// ���W���ړ�������B
		Pos += driftJumpVec_ * driftJumpSpeed_;
		PrevPos += driftJumpVec_ * driftJumpSpeed_;

		// �h���t�g�ʂ����炷�B
		driftJumpSpeed_ -= SUB_DRIFT_JUMP_SPEED;
		if (driftJumpSpeed_ < 0.0f) {

			driftJumpSpeed_ = 0.0f;

		}

		// �X�e�B�b�N���͂���������B
		if (0.1f < std::fabs(InclineBody.lock()->GetHandleAmount())) {

			isDrift_ = true;
			isDriftRight_ = 0.0f < InclineBody.lock()->GetHandleAmount();

		}



	}

	// �h���t�g����������B
	if (isDrift_) {

		// �h���t�g�̌o�ߎ��Ԃ̃^�C�}�[���X�V�B
		++driftTimer_;

	}
	else {

		driftTimer_ = 0;

	}

	// �W�����v����F�ł��̏������ʂ�Ȃ��悤�ɂ��邽�߂̏���
	bool isNotTriggerLT = !(!isInputLTPrev_ && isInputLT_);
	// �ݒu������h���t�g�W�����v����������B
	if (isDriftJump_ && OnGround && isNotTriggerLT) {

		isDriftJump_ = false;
		driftJumpSpeed_ = 0.0f;

	}

}

void CharacterDrift::UpdateDriftParticle(const DriftParticleInData& InData, DriftParticleOutData& OutData)
{

	/*===== �h���t�g�p�[�e�B�N���̍X�V���� =====*/

	// �u�[�X�g����^�C�}�[�����ȏゾ�����������������B
	if (0 < OutData.boostTimer_) {

		--OutData.boostTimer_;

		OutData.boostSpeed_ = InData.maxBoostSpeed_;

		// �����̊��������߂�B
		float boostRate = OutData.boostSpeed_ / InData.maxBoostSpeed_;

		// ���̑傫�������߂�B
		bool IsSmokeBig = 0.5f < boostRate;

		// �ݒu���Ă����牌�𐶐��B
		if (InData.onGround_) {
			Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), OutData.playerData_.carBodyInstance_.lock()->GetRotate());
			DriftParticleMgr::Ins()->GenerateSmoke(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, OutData.playerData_.carBodyInstance_.lock()->GetRotate(), IsSmokeBig, DriftParticleMgr::DELAY_ID::DASH);
			driftVec = FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), OutData.playerData_.carBodyInstance_.lock()->GetRotate());
			DriftParticleMgr::Ins()->GenerateSmoke(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, OutData.playerData_.carBodyInstance_.lock()->GetRotate(), IsSmokeBig, DriftParticleMgr::DELAY_ID::DASH);
		}

		// �ݒu���Ă��Ĉړ����x�����ȏゾ�����牊�𐶐��B
		if (15.0f < OutData.boostSpeed_) {
			DriftParticleMgr::Ins()->GenerateFire(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos(), OutData.playerData_.carBodyInstance_.lock()->GetRotate());
		}

	}

	// �J�n�O�ŃA�N�Z���𓥂�ł�����B
	if (InData.isAccel_ && InData.isBeforeStart_) {

		// �����o���^�C�}�[���X�V�B
		++beforeStartSmokeTimer_;
		if (BEFORE_START_SMOKE_TIMER < beforeStartSmokeTimer_) {

			beforeStartSmokeTimer_ = 0;

			Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), OutData.playerData_.carBodyInstance_.lock()->GetRotate());
			// ���E�ɎU�炷�B
			Vec3 generatePos = OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * static_cast<float>(FHelper::GetRand(-2, 2));
			// �������Ɏ����Ă���B
			generatePos += -InData.forwardVec_ * 20.0f;
			DriftParticleMgr::Ins()->GenerateSmoke(generatePos, OutData.playerData_.carBodyInstance_.lock()->GetRotate(), false, DriftParticleMgr::DELAY_ID::NONE_DELAY, -InData.forwardVec_);

		}

	}

	// ���n�����u�Ԃ�������B
	bool isJumpDriftRelease = (isDriftJumpPrev_ && !isDriftJump_);
	bool onGroundTrigger = (!InData.onGroundPrev_ && InData.onGround_);
	if ((isJumpDriftRelease || onGroundTrigger) && !InData.isBeforeStart_ && !OutData.gameFinish_.lock()->GetIsGameFinish()) {

		// �O�񃉃��_���Ɉʒu�����炵�Đ�������B
		Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), OutData.playerData_.carBodyInstance_.lock()->GetRotate());
		for (int index = 0; index < 3; ++index) {

			DriftParticleMgr::Ins()->GenerateSmoke(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, OutData.playerData_.carBodyInstance_.lock()->GetRotate(), false, DriftParticleMgr::DELAY_ID::NONE_DELAY);

		}
		// �O�񃉃��_���Ɉʒu�����炵�Đ�������B
		for (int index = 0; index < 3; ++index) {
			DriftParticleMgr::Ins()->GenerateSmoke(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() - driftVec * 30.0f, OutData.playerData_.carBodyInstance_.lock()->GetRotate(), false, DriftParticleMgr::DELAY_ID::NONE_DELAY);

		}

	}

	// �h���t�g���͉����o���B
	if (isDrift_ && InData.onGround_) {


		// ���݂̃��x���B
		int nowLevel = 0;
		for (auto& index : DRIFT_TIMER) {

			if (driftTimer_ < index) continue;

			nowLevel = static_cast<int>(&index - &DRIFT_TIMER[0]);

		}

		// ���݂̃h���t�g���x����1�ȏゾ������p�[�e�B�N���ƃI�[�����o���B
		if (1 <= nowLevel) {

			if (!DriftParticleMgr::Ins()->IsAuraGenerated(InData.charaIndex_)) {
				DriftParticleMgr::Ins()->GenerateAura(InData.charaIndex_, OutData.playerData_.carBehindTireInstance_, static_cast<int>(DriftParticle::ID::AURA_BIG), isDriftRight_, 2 <= nowLevel);
				DriftParticleMgr::Ins()->GenerateAura(InData.charaIndex_, OutData.playerData_.carBehindTireInstance_, static_cast<int>(DriftParticle::ID::AURA_SMALL), isDriftRight_, 2 <= nowLevel);
			}

			// ���[�g�����߂�B
			float rate = 0;
			rate = FHelper::Saturate(static_cast<float>(driftTimer_) / static_cast<float>(DRIFT_TIMER[0]));

			// �p�[�e�B�N���𐶐�����B
			DriftParticleMgr::Ins()->GenerateDriftParticle(OutData.playerData_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), rate, false, DriftParticleMgr::DELAY_ID::DELAY1);

			// ���x����������u�Ԃ��������C�Ƀp�[�e�B�N���𐶐�����B
			if (driftTimer_ == DRIFT_TIMER[0] || driftTimer_ == DRIFT_TIMER[1] || driftTimer_ == DRIFT_TIMER[2]) {

				DriftParticleMgr::Ins()->GenerateDriftParticle(OutData.playerData_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1);
				DriftParticleMgr::Ins()->GenerateDriftParticle(OutData.playerData_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1);
				DriftParticleMgr::Ins()->GenerateDriftParticle(OutData.playerData_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1);
				DriftParticleMgr::Ins()->GenerateDriftParticle(OutData.playerData_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1);
				DriftParticleMgr::Ins()->GenerateDriftParticle(OutData.playerData_.carBehindTireInstance_, isDriftRight_, 2 <= nowLevel, static_cast<int>(DriftParticle::ID::PARTICLE), 1.0f, true, DriftParticleMgr::DELAY_ID::DELAY1);

				// �I�[������U�j��
				DriftParticleMgr::Ins()->DestroyAura(InData.charaIndex_);

			}

		}

		// �����o���B
		Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(isDriftRight_ ? -1.0f : 1.0f, 0, 0), OutData.playerData_.carBodyInstance_.lock()->GetRotate());
		DriftParticleMgr::Ins()->GenerateSmoke(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, OutData.playerData_.carBodyInstance_.lock()->GetRotate(), nowLevel < 1, DriftParticleMgr::DELAY_ID::DEF);



	}
	else {

		// �I�[����j�� �֐����ɓ�d����΍�̏�����������B
		DriftParticleMgr::Ins()->DestroyAura(InData.charaIndex_);

	}

	// �Q�[�����I�����Ă����� �C�[�W���O���I�����Ă��Ȃ�������B
	if (InData.isGameFinish_ && OutData.gameFinish_.lock()->GetGameFinishEasingTimer() < 0.9f) {

		// �����o���B
		Vec3 driftVec = FHelper::MulRotationMatNormal(Vec3(OutData.gameFinish_.lock()->GetIsGameFinishDriftLeft() ? -1.0f : 1.0f, 0, 0), OutData.playerData_.carBodyInstance_.lock()->GetRotate());
		DriftParticleMgr::Ins()->GenerateSmoke(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, OutData.gameFinish_.lock()->GetGameFinishTriggerMatRot(), false, DriftParticleMgr::DELAY_ID::NONE_DELAY);
		DriftParticleMgr::Ins()->GenerateSmoke(OutData.playerData_.carBehindTireInstance_.lock()->GetWorldPos() + driftVec * 30.0f, OutData.gameFinish_.lock()->GetGameFinishTriggerMatRot(), false, DriftParticleMgr::DELAY_ID::NONE_DELAY);
		DriftParticleMgr::Ins()->GenerateSmoke(OutData.gameFinish_.lock()->GetIsGameFinishDriftLeft() ? OutData.playerData_.carLeftTireInstance_.lock()->GetWorldPos() : OutData.playerData_.carRightTireInstance_.lock()->GetWorldPos(), OutData.gameFinish_.lock()->GetGameFinishTriggerMatRot(), false, DriftParticleMgr::DELAY_ID::NONE_DELAY);


	}

	// �W�����v�A�N�V�����̃p�[�e�B�N���𐶐��B
	if (InData.IsJumpAction_) {

		DriftParticleMgr::Ins()->GenerateJumpEffect(OutData.playerData_.carBodyInstance_);

	}

}
