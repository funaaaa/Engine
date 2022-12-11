#include "CharacterTireMask.h"
#include "FHelper.h"
#include "CharacterGameFinish.h"
#include "CharacterFlags.h"
#include "CharacterDrift.h"
#include "PolygonInstanceRegister.h"
#include "PolygonInstance.h"
#include "BaseStage.h"
#include "StageObjectMgr.h"

CharacterTireMask::CharacterTireMask()
{

	/*===== �R���X�g���N�^ =====*/

	Init();

}

void CharacterTireMask::Init()
{

	/*===== ���������� =====*/

	tireMaskUV_ = TireMaskUV();

}

bool CharacterTireMask::CheckTireMask(const TireMaskInData& InData, TireMaskUV& TireMaskUVData)
{

	/*===== �^�C�������������� =====*/

	// �Q�[�����I�������g���K�[���肾������B
	if (InData.gameFinish_.lock()->GetIsGameFinishTrigger()) {

		tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

	}

	// �󒆂ɂ�����A�h���t�g�W�����v����������B
	if (!InData.flags_.lock()->onGround_ || InData.drift_.lock()->GetIsDriftJump()) {

		tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

	}

	const float RAY_LENGTH = 80.0f;

	// �Q�[�����I�����Ă�����B
	if (InData.gameFinish_.lock()->GetIsGameFinish()) {

		// �����Ƀh���t�g���Ă�����B
		if (!InData.gameFinish_.lock()->GetIsGameFinishDriftLeft()) {

			FHelper::RayToModelCollisionData InputRayData;
			InputRayData.targetPolygonData_ = PolygonInstanceRegister::Ins()->GetMeshCollisionData(InData.stageData_.lock()->stageObjectMgr_->GetInstanceIndex(1));

			// �߂�n�ۑ��p
			Vec3 ImpactPos;
			Vec3 HitNormal;
			Vec2 HitUV;
			float HitDistance;

			// ���O�^�C��
			InputRayData.rayPos_ = InData.playerModel_.carRightTireInstance_.lock()->GetWorldPos();
			InputRayData.rayDir_ = InData.bottomVec_;

			// �^�C�����̈ʒu�����o
			bool isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

			// �����蔻�肪���������ǂ������`�F�b�N�B
			if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

				tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
				tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);

				return false;

			}
			else {

				tireMaskUV_.forwardLeftUV_.prevuv_ = tireMaskUV_.forwardLeftUV_.uv_;
				tireMaskUV_.forwardLeftUV_.uv_ = HitUV;
				TireMaskUVData.forwardLeftUV_.prevuv_ = tireMaskUV_.forwardLeftUV_.prevuv_;
				TireMaskUVData.forwardLeftUV_.uv_ = HitUV;

			}

			// �E�O�^�C��
			InputRayData.rayPos_ = InData.playerModel_.carBehindTireInstance_.lock()->GetWorldPos();
			InputRayData.rayPos_ += FHelper::MulRotationMatNormal(Vec3(1, 0, 0), InData.playerModel_.carBodyInstance_.lock()->GetRotate()) * 20.0f;
			InputRayData.rayDir_ = InData.bottomVec_;

			// �^�C�����̈ʒu�����o
			isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

			if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

				tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
				tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

				return false;

			}
			else {

				tireMaskUV_.forwardRightUV_.prevuv_ = tireMaskUV_.forwardRightUV_.uv_;
				tireMaskUV_.forwardRightUV_.uv_ = HitUV;
				TireMaskUVData.forwardRightUV_.prevuv_ = tireMaskUV_.forwardRightUV_.prevuv_;
				TireMaskUVData.forwardRightUV_.uv_ = HitUV;

			}

		}
		else {

			FHelper::RayToModelCollisionData InputRayData;
			InputRayData.targetPolygonData_ = PolygonInstanceRegister::Ins()->GetMeshCollisionData(InData.stageData_.lock()->stageObjectMgr_->GetInstanceIndex(1));

			// �߂�n�ۑ��p
			Vec3 ImpactPos;
			Vec3 HitNormal;
			Vec2 HitUV;
			float HitDistance;

			// ���O�^�C��
			InputRayData.rayPos_ = InData.playerModel_.carLeftTireInstance_.lock()->GetWorldPos();
			InputRayData.rayDir_ = InData.bottomVec_;

			// �^�C�����̈ʒu�����o
			bool isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

			// �����蔻�肪���������ǂ������`�F�b�N�B
			if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

				tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
				tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);

				return false;

			}
			else {

				tireMaskUV_.forwardLeftUV_.prevuv_ = tireMaskUV_.forwardLeftUV_.uv_;
				tireMaskUV_.forwardLeftUV_.uv_ = HitUV;
				TireMaskUVData.forwardLeftUV_.prevuv_ = tireMaskUV_.forwardLeftUV_.prevuv_;
				TireMaskUVData.forwardLeftUV_.uv_ = HitUV;

			}

			// �E�O�^�C��
			InputRayData.rayPos_ = InData.playerModel_.carBehindTireInstance_.lock()->GetWorldPos();
			InputRayData.rayPos_ += FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), InData.playerModel_.carBodyInstance_.lock()->GetRotate()) * 20.0f;
			InputRayData.rayDir_ = InData.bottomVec_;

			// �^�C�����̈ʒu�����o
			isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

			if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

				tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
				tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

				return false;

			}
			else {

				tireMaskUV_.forwardRightUV_.prevuv_ = tireMaskUV_.forwardRightUV_.uv_;
				tireMaskUV_.forwardRightUV_.uv_ = HitUV;
				TireMaskUVData.forwardRightUV_.prevuv_ = tireMaskUV_.forwardRightUV_.prevuv_;
				TireMaskUVData.forwardRightUV_.uv_ = HitUV;

			}


		}


		return true;

	}

	if (!InData.drift_.lock()->GetIsTireMask()) {

		tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

		return false;

	}

	FHelper::RayToModelCollisionData InputRayData;
	InputRayData.targetPolygonData_ = PolygonInstanceRegister::Ins()->GetMeshCollisionData(InData.stageData_.lock()->stageObjectMgr_->GetInstanceIndex(1));

	// �߂�n�ۑ��p
	Vec3 ImpactPos;
	Vec3 HitNormal;
	Vec2 HitUV;
	float HitDistance;
	bool isHit = false;

	// �E�h���t�g���Ă����炱�̏����͒ʂ��Ȃ��B
	if (!(InData.drift_.lock()->GetIsDrift() && !InData.drift_.lock()->GetIsDriftRight())) {

		// ���O�^�C��
		InputRayData.rayPos_ = InData.playerModel_.carLeftTireInstance_.lock()->GetWorldPos();
		InputRayData.rayDir_ = InData.bottomVec_;

		// �^�C�����̈ʒu�����o
		isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

		// �����蔻�肪���������ǂ������`�F�b�N�B
		if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

			tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
			tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);

			return false;

		}
		else {

			tireMaskUV_.forwardLeftUV_.prevuv_ = tireMaskUV_.forwardLeftUV_.uv_;
			tireMaskUV_.forwardLeftUV_.uv_ = HitUV;
			TireMaskUVData.forwardLeftUV_.prevuv_ = tireMaskUV_.forwardLeftUV_.prevuv_;
			TireMaskUVData.forwardLeftUV_.uv_ = HitUV;

		}

	}


	// ���h���t�g���Ă����炱�̏����͒ʂ��Ȃ��B
	if (!(InData.drift_.lock()->GetIsDrift() && InData.drift_.lock()->GetIsDriftRight())) {

		// �E�O�^�C��
		InputRayData.rayPos_ = InData.playerModel_.carRightTireInstance_.lock()->GetWorldPos();
		InputRayData.rayDir_ = InData.bottomVec_;

		// �^�C�����̈ʒu�����o
		isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

		if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

			tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
			tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

			return false;

		}
		else {

			tireMaskUV_.forwardRightUV_.prevuv_ = tireMaskUV_.forwardRightUV_.uv_;
			tireMaskUV_.forwardRightUV_.uv_ = HitUV;
			TireMaskUVData.forwardRightUV_.prevuv_ = tireMaskUV_.forwardRightUV_.prevuv_;
			TireMaskUVData.forwardRightUV_.uv_ = HitUV;

		}

	}

	// �E���^�C��
	InputRayData.rayPos_ = InData.playerModel_.carRightTireInstance_.lock()->GetWorldPos();
	InputRayData.rayDir_ = InData.bottomVec_;

	// ��]�������x�N�g�������߂�B
	Vec3 behindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), InData.playerModel_.carBodyInstance_.lock()->GetRotate());
	InputRayData.rayPos_ += behindVec * 70.0f;

	// �^�C�����̈ʒu�����o
	isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

	if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

		tireMaskUV_.behindRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.behindRightUV_.uv_ = Vec2(0, 0);

		return false;

	}
	else {

		tireMaskUV_.behindRightUV_.prevuv_ = tireMaskUV_.behindRightUV_.uv_;
		tireMaskUV_.behindRightUV_.uv_ = HitUV;
		TireMaskUVData.behindRightUV_.prevuv_ = tireMaskUV_.behindRightUV_.prevuv_;
		TireMaskUVData.behindRightUV_.uv_ = HitUV;

	}

	// �����^�C��
	InputRayData.rayPos_ = InData.playerModel_.carLeftTireInstance_.lock()->GetWorldPos();
	InputRayData.rayDir_ = InData.bottomVec_;

	// ��]�������x�N�g�������߂�B
	behindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), InData.playerModel_.carBodyInstance_.lock()->GetRotate());
	InputRayData.rayPos_ += behindVec * 70.0f;

	// �^�C�����̈ʒu�����o
	isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

	if (!isHit || fabs(HitDistance) < 0 || RAY_LENGTH < fabs(HitDistance)) {

		tireMaskUV_.behindLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.behindLeftUV_.uv_ = Vec2(0, 0);

		return false;

	}
	else {

		tireMaskUV_.behindLeftUV_.prevuv_ = tireMaskUV_.behindLeftUV_.uv_;
		tireMaskUV_.behindLeftUV_.uv_ = HitUV;
		TireMaskUVData.behindLeftUV_.prevuv_ = tireMaskUV_.behindLeftUV_.prevuv_;
		TireMaskUVData.behindLeftUV_.uv_ = HitUV;

	}

	return true;

}
