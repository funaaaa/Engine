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

	/*===== コンストラクタ =====*/

	Init();

}

void CharacterTireMask::Init()
{

	/*===== 初期化処理 =====*/

	tireMaskUV_ = TireMaskUV();

}

bool CharacterTireMask::CheckTireMask(const TireMaskInData& InData, TireMaskUV& TireMaskUVData)
{

	/*===== タイヤ痕を書き込む =====*/

	// ゲームが終了したトリガー判定だったら。
	if (InData.gameFinish_.lock()->GetIsGameFinishTrigger()) {

		tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

	}

	// 空中にいたら、ドリフトジャンプ中だったら。
	if (!InData.flags_.lock()->onGround_ || InData.drift_.lock()->GetIsDriftJump()) {

		tireMaskUV_.forwardLeftUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardLeftUV_.uv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.prevuv_ = Vec2(0, 0);
		tireMaskUV_.forwardRightUV_.uv_ = Vec2(0, 0);

	}

	const float RAY_LENGTH = 80.0f;

	// ゲームが終了していたら。
	if (InData.gameFinish_.lock()->GetIsGameFinish()) {

		// 左側にドリフトしていたら。
		if (!InData.gameFinish_.lock()->GetIsGameFinishDriftLeft()) {

			FHelper::RayToModelCollisionData InputRayData;
			InputRayData.targetPolygonData_ = PolygonInstanceRegister::Ins()->GetMeshCollisionData(InData.stageData_.lock()->stageObjectMgr_->GetInstanceIndex(1));

			// 戻り地保存用
			Vec3 ImpactPos;
			Vec3 HitNormal;
			Vec2 HitUV;
			float HitDistance;

			// 左前タイヤ
			InputRayData.rayPos_ = InData.playerModel_.carRightTireInstance_.lock()->GetWorldPos();
			InputRayData.rayDir_ = InData.bottomVec_;

			// タイヤ痕の位置を検出
			bool isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

			// 当たり判定が正しいかどうかをチェック。
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

			// 右前タイヤ
			InputRayData.rayPos_ = InData.playerModel_.carBehindTireInstance_.lock()->GetWorldPos();
			InputRayData.rayPos_ += FHelper::MulRotationMatNormal(Vec3(1, 0, 0), InData.playerModel_.carBodyInstance_.lock()->GetRotate()) * 20.0f;
			InputRayData.rayDir_ = InData.bottomVec_;

			// タイヤ痕の位置を検出
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

			// 戻り地保存用
			Vec3 ImpactPos;
			Vec3 HitNormal;
			Vec2 HitUV;
			float HitDistance;

			// 左前タイヤ
			InputRayData.rayPos_ = InData.playerModel_.carLeftTireInstance_.lock()->GetWorldPos();
			InputRayData.rayDir_ = InData.bottomVec_;

			// タイヤ痕の位置を検出
			bool isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

			// 当たり判定が正しいかどうかをチェック。
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

			// 右前タイヤ
			InputRayData.rayPos_ = InData.playerModel_.carBehindTireInstance_.lock()->GetWorldPos();
			InputRayData.rayPos_ += FHelper::MulRotationMatNormal(Vec3(-1, 0, 0), InData.playerModel_.carBodyInstance_.lock()->GetRotate()) * 20.0f;
			InputRayData.rayDir_ = InData.bottomVec_;

			// タイヤ痕の位置を検出
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

	// 戻り地保存用
	Vec3 ImpactPos;
	Vec3 HitNormal;
	Vec2 HitUV;
	float HitDistance;
	bool isHit = false;

	// 右ドリフトしていたらこの処理は通さない。
	if (!(InData.drift_.lock()->GetIsDrift() && !InData.drift_.lock()->GetIsDriftRight())) {

		// 左前タイヤ
		InputRayData.rayPos_ = InData.playerModel_.carLeftTireInstance_.lock()->GetWorldPos();
		InputRayData.rayDir_ = InData.bottomVec_;

		// タイヤ痕の位置を検出
		isHit = FHelper::RayToModelCollision(InputRayData, ImpactPos, HitDistance, HitNormal, HitUV);

		// 当たり判定が正しいかどうかをチェック。
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


	// 左ドリフトしていたらこの処理は通さない。
	if (!(InData.drift_.lock()->GetIsDrift() && InData.drift_.lock()->GetIsDriftRight())) {

		// 右前タイヤ
		InputRayData.rayPos_ = InData.playerModel_.carRightTireInstance_.lock()->GetWorldPos();
		InputRayData.rayDir_ = InData.bottomVec_;

		// タイヤ痕の位置を検出
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

	// 右後ろタイヤ
	InputRayData.rayPos_ = InData.playerModel_.carRightTireInstance_.lock()->GetWorldPos();
	InputRayData.rayDir_ = InData.bottomVec_;

	// 回転した後ろベクトルを求める。
	Vec3 behindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), InData.playerModel_.carBodyInstance_.lock()->GetRotate());
	InputRayData.rayPos_ += behindVec * 70.0f;

	// タイヤ痕の位置を検出
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

	// 左後ろタイヤ
	InputRayData.rayPos_ = InData.playerModel_.carLeftTireInstance_.lock()->GetWorldPos();
	InputRayData.rayDir_ = InData.bottomVec_;

	// 回転した後ろベクトルを求める。
	behindVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), InData.playerModel_.carBodyInstance_.lock()->GetRotate());
	InputRayData.rayPos_ += behindVec * 70.0f;

	// タイヤ痕の位置を検出
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
