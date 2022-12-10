#include "CharacterGameFinish.h"
#include "FHelper.h"
#include "PolygonInstance.h"
#include "CharacterInclineBody.h"

CharacterGameFinish::CharacterGameFinish()
{

	/*===== �R���X�g���N�^ =====*/

	Init();

}

void CharacterGameFinish::Init()
{

	/*===== ���������� =====*/

	isPrevGameFinish_ = false;
	isGameFinish_ = false;
	gameFinishTriggerMatRot_ = DirectX::XMMatrixIdentity();
	gameFinishTriggerRotY_ = 0;
	gameFinishEasingTimer_ = 0;
	gameFinishRotStopAmount_ = 0;
	gameFinishRotStopReturnAmount_ = 0;
	isGameFinish_ = false;
	isPrevGameFinish_ = false;
	isGameFinishDriftLeft_ = false;

}

void CharacterGameFinish::Update(const UpdateInputData& Input, int& BoostTimer, float& Speed, float& RotY, Vec3& ForwardVec)
{

	/*===== �Q�[���I�����̃^�C�}�[���̊�{�I�ȍX�V���� =====*/

	// �Q�[�����I����Ă��Ȃ������珈�����΂��B
	if (!isGameFinish_) return;

	// �Q�[���I���g���K�[�̏ꍇ�B
	bool isFinishTrigger = !isPrevGameFinish_ && isGameFinish_;
	if (isFinishTrigger) {

		// �e�ϐ���ݒ�B
		BoostTimer = 0;
		Speed = Input.maxSpeed_;
		gameFinishTriggerRotY_ = RotY;
		gameFinishEasingTimer_ = 0;
		gameFinishTruggerForardVec_ = ForwardVec;
		gameFinishTriggerMatRot_ = Input.playerModel_.carBodyInstance_.lock()->GetRotate();

		// ���o�łǂ���Ƀh���t�g�����邩���擾�B
		isGameFinishDriftLeft_ = Input.inclineBody_.lock()->GetHandleAmount() < 0;
		if (Input.inclineBody_.lock()->GetHandleAmount() == 0) {

			isGameFinishDriftLeft_ = true;

		}

		return;

	}


	/*-- �Q�[���I�����̍X�V�����{�� --*/

	// ���ʃx�N�g�����X�V����B
	ForwardVec = gameFinishTruggerForardVec_;

	// �Q�[���I�����̃C�[�W���O�^�C�}�[���X�V���ăI�[�o�[�t���[���Ȃ��悤�ɂ���B
	gameFinishEasingTimer_ = FHelper::Saturate(gameFinishEasingTimer_ + GAME_FINISH_EASING_TIMER);

	// �C�[�W���O�ʂ����߂�B
	float easingAmount = FEasing::EaseOutQuint(gameFinishEasingTimer_);

	// �ړ��ʂ��X�V����B
	Speed = Input.maxSpeed_ * (1.0f - easingAmount);

	// ��]���X�V����B
	const float ADD_ROT_Y = DirectX::XM_PIDIV2 * (isGameFinishDriftLeft_ ? -1.0f : 1.0f);
	RotY = gameFinishTriggerRotY_;
	RotY += ADD_ROT_Y * easingAmount;

}

void CharacterGameFinish::AKIRAERotation(PlayerModel& Model)
{

	/*===== �Q�[���I������AKIRA�����𕗉��o�̍X�V���� =====*/

	if (isGameFinish_) {

		// �C�[�W���O���I�������B
		if (0.7f <= gameFinishEasingTimer_) {

			// ��]�������߂�B
			Vec3 axisOfRevolution = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), Model.carBodyInstance_.lock()->GetRotate());

			// �N�H�[�^�j�I�������߂�B
			DirectX::XMVECTOR gameFinishRotQ = DirectX::XMQuaternionRotationAxis(axisOfRevolution.ConvertXMVECTOR(), gameFinishRotStopAmount_);

			// �N�H�[�^�j�I�������]�s������߂�B
			DirectX::XMMATRIX gameFinishRotMat = DirectX::XMMatrixRotationQuaternion(gameFinishRotQ);

			// ��]������B
			Model.carBodyInstance_.lock()->AddRotate(gameFinishRotMat);

			// ��]�ʂ����炷�B
			if (isGameFinishDriftLeft_) {

				gameFinishRotStopReturnAmount_ -= -0.02f;
				gameFinishRotStopAmount_ += gameFinishRotStopReturnAmount_;
				if (0 <= gameFinishRotStopAmount_) {

					gameFinishRotStopAmount_ = 0.0f;

				}

			}
			else {

				gameFinishRotStopReturnAmount_ -= 0.02f;
				gameFinishRotStopAmount_ += gameFinishRotStopReturnAmount_;
				if (gameFinishRotStopAmount_ <= 0) {

					gameFinishRotStopAmount_ = 0.0f;

				}

			}

		}
		else {

			// �C�[�W���O�ʂ����߂�B
			float easingAmount = FEasing::EaseInSine(gameFinishEasingTimer_) * (isGameFinishDriftLeft_ ? -1.0f : 1.0f);

			if (GAME_FINISH_STOP_ROT_LIMIT < fabs(easingAmount)) easingAmount = GAME_FINISH_STOP_ROT_LIMIT * (easingAmount < 0 ? -1.0f : 1.0f);

			// ��]�������߂�B
			Vec3 axisOfRevolution = FHelper::MulRotationMatNormal(Vec3(0, 0, -1), Model.carBodyInstance_.lock()->GetRotate());

			// �N�H�[�^�j�I�������߂�B
			DirectX::XMVECTOR gameFinishRotQ = DirectX::XMQuaternionRotationAxis(axisOfRevolution.ConvertXMVECTOR(), easingAmount);

			// �N�H�[�^�j�I�������]�s������߂�B
			DirectX::XMMATRIX gameFinishRotMat = DirectX::XMMatrixRotationQuaternion(gameFinishRotQ);

			// ��]������B
			Model.carBodyInstance_.lock()->AddRotate(gameFinishRotMat);

			gameFinishRotStopAmount_ = easingAmount;
			gameFinishRotStopReturnAmount_ = 0;

		}

	}

}

void CharacterGameFinish::UpdateGameFinishFlag(bool IsGameFinish)
{

	/*===== �Q�[���I�����̃t���O���X�V =====*/

	isPrevGameFinish_ = isGameFinish_;
	isGameFinish_ = IsGameFinish;

}
