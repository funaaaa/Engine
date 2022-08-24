#include "ShellObject.h"
#include "OBB.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "HitGroupMgr.h"
#include "BaseStage.h"
#include "FHelper.h"

ShellObject::ShellObject()
{

	/*===== �R���X�g���N�^ =====*/

	isDestroyed_ = true;
	isActive_ = false;

}

void ShellObject::Destroy()
{

	/*===== �j������ =====*/

	isActive_ = false;

	if (isDestroyed_) return;

	PolygonInstanceRegister::Ins()->DestroyInstance(insIndex_);
	isDestroyed_ = true;


}

void ShellObject::Generate(const Vec3& Pos, const Vec3& ForwardVec, const float& CharaRotY, const int& ShellID, const int& CharaInsIndex)
{

	/*===== �������� =====*/

	// BLAS��Instance�𐶐��B
	blasIndex_ = BLASRegister::Ins()->GenerateObj("Resource/Game/Carapace/", "Carapace.obj", HitGroupMgr::Ins()->hitGroupNames[HitGroupMgr::HITGROUP_ID::DEF], { L"Resource/Game/blackRed.png" });
	insIndex_ = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex_, PolygonInstanceRegister::SHADER_ID::DEF);

	size_ = Vec3(20.0f, 20.0f, 20.0f);
	PolygonInstanceRegister::Ins()->AddScale(insIndex_, size_);

	// OBB��ݒ�B
	obb_ = std::make_shared<OBB>();
	obb_->Setting(blasIndex_, insIndex_);

	// �ϐ���ݒ�B
	pos_ = Pos;
	prevPos_ = Pos;
	forwardVec_ = ForwardVec;
	rotY_ = CharaRotY;
	charaInsIndex_ = CharaInsIndex;
	shellID_ = static_cast<SHELL_ID>(ShellID);
	exitTimer_ = 0;
	speed_ = SPEED;
	isActive_ = true;
	isDestroyed_ = false;

}

void ShellObject::CheckHit(std::weak_ptr<BaseStage> StageData)
{

	/*===== �����蔻�� =====*/

	// �I�u�W�F�N�g�̏�Ԃ�THROW�ȊO�������瓖���蔻����s��Ȃ��B
	if (shellID_ == SHELL_ID::BEHIND) return;

	// �����蔻��֐��ɓ����l��ݒ�B
	BaseStage::ColliderInput input;
	input.targetInsIndex_ = insIndex_;
	input.targetOBB_ = obb_;
	input.targetPos_ = pos_;
	input.targetPrevPos_ = prevPos_;
	input.targetRotY_ = rotY_;
	input.targetSize_ = size_;
	input.isInvalidateRotY_ = false;
	input.isPlayer_ = false;

	// �����蔻��֐�����Ԃ��Ă���l�B
	BaseStage::ColliderOutput output;

	// �����蔻����s���B
	output = StageData.lock()->Collider(input);

	// �����蔻��̌��ʂ��珈�����s���B
	if (output.isHitOrnament_) {

		// �����I�u�W�F�N�g�Ƃ̓�����������
		speed_ = 0;

	}

	// �ݒu������������B
	onGround_ = false;
	if (output.isHitStage_) {

		// �X�e�[�W�Ƃ̓����蔻��
		onGround_ = true;

		forwardVec_ = output.forwardVec_;

		// ���ɔ�΂��Ă����Ԃ�������x�N�g�����t�ɂ���B
		if (shellID_ == SHELL_ID::BEHIND_THROW) {

			forwardVec_ *= Vec3(-1, -1, -1);

		}

	}
	if (output.isHitStageGrass_) {

		// �X�e�[�W�Ƃ̓����蔻��
		onGround_ = true;

		forwardVec_ = output.forwardVec_;

		// ���ɔ�΂��Ă����Ԃ�������x�N�g�����t�ɂ���B
		if (shellID_ == SHELL_ID::BEHIND_THROW) {

			forwardVec_ *= Vec3(-1, -1, -1);

		}

	}

	// �����I�u�W�F�N�g�ƏՓ˂��Ă�����B
	if (output.isHitOrnament_) {

		// �x�N�g���𔽎˂�����B
		Vec3 reflectVec = forwardVec_.Reflection(output.ornamentHitNormal_);

		// �x�N�g���Ԃ̊p�x�����߂�B
		float vecDot = forwardVec_.Dot(reflectVec);
		vecDot = acosf(vecDot);

		// ���E����p
		Vec3 cross = forwardVec_.Cross(reflectVec);

		// RotY����]������B
		rotY_ += vecDot * (cross.y_ < 0 ? -1.0f : 1.0f);

		// ���˃x�N�g������
		forwardVec_ = reflectVec;

	}

	// ���̑��̕ϐ����������B
	pos_ = output.resultPos_;

}

void ShellObject::Update(std::weak_ptr<BaseStage> StageData)
{

	/*===== �X�V���� =====*/

	// �I�u�W�F�N�g���������Ă����Ԃ�������B
	if (shellID_ == SHELL_ID::BEHIND_THROW || shellID_ == SHELL_ID::FORWARD_THROW) {

		// �ړ�������B
		pos_ += forwardVec_ * SPEED;

		// �d�͂̏���
		if (!onGround_) {

			// �d�͂��X�V
			gravity_ += ADD_GRAV;
			if (MAX_GRAV < gravity_) {

				gravity_ = MAX_GRAV;

			}

			// ���ł܂ł̎��Ԃ��X�V�B
			++exitTimer_;
			if (EXIT_TIMER < exitTimer_) {

				isActive_ = false;

			}

		}
		else {

			gravity_ = 0;
			exitTimer_ = 0;

		}

		// �d�͂����Z�B
		pos_.y_ -= gravity_;

		// �����蔻����s���B
		CheckHit(StageData);

	}
	// �I�u�W�F�N�g�����ɕێ�����Ă����Ԃ�������B
	else if (shellID_ == SHELL_ID::BEHIND) {

		// �ێ����Ă���L�����̍��W�B
		Vec3 charaPos = PolygonInstanceRegister::Ins()->GetPos(charaInsIndex_);

		// �ێ����Ă���L�����̌��x�N�g��
		Vec3 charaBehindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), PolygonInstanceRegister::Ins()->GetRotate(charaInsIndex_));

		// �L�����ƍb���̋���
		static const float CHARA_SHELL_DISTANCE = 70.0f;

		// �b�����ړ�������B
		pos_ = charaPos + charaBehindVec * CHARA_SHELL_DISTANCE;

		// �ێ����Ă���L�����̂����x�N�g��
		Vec3 underVec = FHelper::MulRotationMatNormal(Vec3(0, -1, 0), PolygonInstanceRegister::Ins()->GetRotate(charaInsIndex_));

		// �ʒu�����p�ς���
		static const float UNDER_DISTANCCE = 10.0f;

		pos_ += underVec * UNDER_DISTANCCE;


	}

	// �C���X�^���X���ړ�������B
	PolygonInstanceRegister::Ins()->ChangeTrans(insIndex_, pos_);

	// OBB��Ǐ]������B
	obb_->SetMat(insIndex_);

	// ���W��ۑ�
	prevPos_ = pos_;

}

bool ShellObject::CheckHitOBB(std::weak_ptr<OBB> CharaOBB)
{

	if (shellID_ == SHELL_ID::BEHIND) {

		return false;

	}

	return obb_->CheckHitOBB(CharaOBB);
}
