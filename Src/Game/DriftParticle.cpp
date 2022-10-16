#include "DriftParticle.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"
#include "Camera.h"
#include "PolygonInstance.h"

DriftParticle::DriftParticle()
{

	/*===== コンストラクタ =====*/

	constBufferIndex_ = 0;
	pos_ = Vec3();

	isActive_ = false;

}

void DriftParticle::Setting(std::weak_ptr<BLAS> Blas, const int ConstBufferIndex)
{

	/*===== セッティング =====*/

	blas_ = Blas;
	constBufferIndex_ = ConstBufferIndex;

}

void DriftParticle::Init()
{

	/*===== 初期化処理 =====*/

	if (isActive_) {

		PolygonInstanceRegister::Ins()->DestroyInstance(instance_);

	}

	isActive_ = false;

}

#include "RayEngine.h"
void DriftParticle::GenerateSmoke(std::weak_ptr<BLAS> Blas, const Vec3& Pos, const DirectX::XMMATRIX MatRot, bool IsBoost, Vec3 ForwardVec)
{

	/*===== 生成処理 =====*/

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

	// 奥方向の回転したベクトルを求める。
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

	/*===== 生成処理 =====*/

	mode_ = ID::FIRE;
	pos_ = Pos;
	blas_ = Blas;
	isActive_ = true;
	isAppearingNow_ = true;
	appearingTimer_ = 0;
	instance_ = PolygonInstanceRegister::Ins()->CreateInstance(blas_, PolygonInstanceRegister::SHADER_ID::ALPHA);
	instance_.lock()->ChangeTrans(Pos);
	instance_.lock()->ChangeRotate(MatRot);

	// 奥方向の回転したベクトルを求める。
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

	/*===== オーラの生成処理 =====*/

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

	// 親の回転行列を求める。
	std::weak_ptr<PolygonMeshInstance> parentInstance = TireInstance.lock()->GetParetntInstance();
	DirectX::XMMATRIX parentMatRot = parentInstance.lock()->GetRotate();

	// 座標を求める。
	pos_ = trackedInstance.lock()->GetWorldPos();
	Vec3 trackVec = isTrackRight_ ? Vec3(-1, 0, 0) : Vec3(1, 0, 0);
	trackVec = FHelper::MulRotationMatNormal(trackVec, parentMatRot);
	pos_ += trackVec * AURA_SIDE_SIZE;

	// 移動させる。
	instance_.lock()->ChangeTrans(pos_);

	// 回転させる。
	instance_.lock()->ChangeRotate(parentMatRot);

	// サイズを変える。
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

	/*===== ドリフト時のパーティクルを生成 =====*/

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

	// 親の回転行列を求める。
	std::weak_ptr<PolygonMeshInstance> parentInstance = TireInstance.lock()->GetParetntInstance();
	DirectX::XMMATRIX parentMatRot = parentInstance.lock()->GetRotate();

	// パーティクル特有の変数を初期化。
	particlePos_ = Vec3();
	particlePrevPos_ = Vec3();
	particlePrevMoveVec_ = Vec3();
	particleVec_ = FHelper::GetRandVec3(-100, 100) / Vec3(100.0f, 100.0f, 100.0f);
	// 車体のY軸方向にベクトルを強める。
	Vec3 parentCarUpVec = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), parentMatRot);
	particleVec_ += parentCarUpVec * 0.5f;
	particleVec_.Normalize();

	// パーティクルの移動速度を決定する。
	if (IsLevelChange) {
		particleSpeed_ = FHelper::GetRand(static_cast<int>(MAX_PARTICLE_SPEED_LEVEL_CHANGE / 0.5f) * 100, static_cast<int>(MAX_PARTICLE_SPEED_LEVEL_CHANGE) * 100) / 100.0f;
	}
	else {
		particleSpeed_ = FHelper::GetRand(static_cast<int>(MAX_PARTICLE_SPEED / 0.5f) * 100, static_cast<int>(MAX_PARTICLE_SPEED) * 100) / 100.0f;
	}

	// 座標を求める。
	pos_ = trackedInstance.lock()->GetWorldPos();
	Vec3 trackVec = isTrackRight_ ? Vec3(-1, 0, 0) : Vec3(1, 0, 0);
	trackVec = FHelper::MulRotationMatNormal(trackVec, parentMatRot);
	pos_ += trackVec * AURA_SIDE_SIZE;

	// 移動させる。
	instance_.lock()->ChangeTrans(pos_);

	// 回転させる。
	instance_.lock()->ChangeRotate(parentMatRot);

	// パーティクルの大きさを決める。
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

	// サイズを変える。
	instance_.lock()->ChangeScale(Vec3(particleNowScale_.x_, particleNowScale_.y_, particleNowScale_.x_));

	RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].instanceIndex_ = instance_.lock()->GetInstanceIndex();
	RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f;

}

void DriftParticle::GenerateJumpEffect(std::weak_ptr<BLAS> Blas, std::weak_ptr<PolygonMeshInstance> CarBodyInstance)
{

	/*===== ジャンプエフェクトを生成 =====*/

	mode_ = ID::JUMP_EFFECT;
	blas_ = Blas;
	isActive_ = true;
	isAppearingNow_ = true;
	jumpActionEasingTimer_ = 0;
	instance_ = PolygonInstanceRegister::Ins()->CreateInstance(blas_, PolygonInstanceRegister::SHADER_ID::ALPHA);
	trackedInstance = CarBodyInstance;

	// 座標を求める。
	pos_ = trackedInstance.lock()->GetWorldPos();

	// 移動させる。
	instance_.lock()->ChangeTrans(pos_);

	// サイズを変える。
	instance_.lock()->ChangeScale(Vec3(particleNowScale_.x_, particleNowScale_.y_, particleNowScale_.x_));

	RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].instanceIndex_ = instance_.lock()->GetInstanceIndex();
	RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f;

}

void DriftParticle::Update()
{

	/*===== 更新処理 =====*/

	switch (mode_)
	{
	case DriftParticle::ID::SMOKE:

		// 移動させる。
		pos_ += forardVec_ * SMOKE_SPEED;

		// 移動させる。
		instance_.lock()->ChangeTrans(pos_);

		if (isAppearingNow_) {

			// アルファ値を加算。
			RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ += APPEARING_ALPHA;
			if (1.0f < RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_) {

				RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f;
				++appearingTimer_;

				// 出現タイマーが一定値に達したらアルファ値を下げていく。
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

		// 親の回転行列を求める。
		std::weak_ptr<PolygonMeshInstance> parentInstance = trackedInstance.lock()->GetParetntInstance();
		DirectX::XMMATRIX parentMatRot = parentInstance.lock()->GetRotate();

		// 座標を求める。
		pos_ = trackedInstance.lock()->GetWorldPos();
		Vec3 trackVec = isTrackRight_ ? Vec3(-1, 0, 0) : Vec3(1, 0, 0);
		trackVec = FHelper::MulRotationMatNormal(trackVec, parentMatRot);
		pos_ += trackVec * AURA_SIDE_SIZE;

		// 移動させる。
		instance_.lock()->ChangeTrans(pos_);

		// 回転させる。
		instance_.lock()->ChangeRotate(parentMatRot);

		// サイズを変える。
		if (isAuraBig_) {

			float CHANGE_SCALE = (mode_ == ID::AURA_BIG ? AURA_BIG_SCALE + AURA_ADD_SCALE : AURA_SMALL_SCALE + AURA_ADD_SCALE);

			// 補完する。
			nowScale_ += (CHANGE_SCALE - nowScale_) / 2.0f;

			// ある程度保管されたら。
			if (fabs(nowScale_ - CHANGE_SCALE) < 0.1f) {

				isAuraBig_ = false;

			}

		}
		else {

			++changeScaleTimer_;
			int MAX_CHANGE_TIMER = (mode_ == ID::AURA_BIG ? CHANGE_SCALE_TIMER_BIG : CHANGE_SCALE_TIMER_SMALL);
			if (MAX_CHANGE_TIMER < changeScaleTimer_) {

				changeScaleTimer_ = 0;

				// 補間先のスケールをランダムで求める。
				const float HUNDRED = 100.0f;
				int CHANGE_SCALE = static_cast<int>((mode_ == ID::AURA_BIG ? AURA_BIG_SCALE * HUNDRED : AURA_SMALL_SCALE * HUNDRED));
				float random = static_cast<float>(FHelper::GetRand(CHANGE_SCALE / 2, CHANGE_SCALE));
				changeScale_ = random / 100.0f;

			}

			// スケールを補完する。
			nowScale_ += (changeScale_ - nowScale_) / 2.0f;

		}


		instance_.lock()->ChangeScale(Vec3(nowScale_, nowScale_, nowScale_));


	}

	break;

	case DriftParticle::ID::PARTICLE:

	{

		// 親の回転行列を求める。
		std::weak_ptr<PolygonMeshInstance> parentInstanceIndex = trackedInstance.lock()->GetParetntInstance();
		DirectX::XMMATRIX parentMatRot = parentInstanceIndex.lock()->GetRotate();

		// 座標を求める。
		pos_ = trackedInstance.lock()->GetWorldPos();
		Vec3 trackVec = isTrackRight_ ? Vec3(-1, 0, 0) : Vec3(1, 0, 0);
		trackVec = FHelper::MulRotationMatNormal(trackVec, parentMatRot);
		pos_ += trackVec * AURA_SIDE_SIZE;

		// 前Fの座標を保存。
		particlePrevPos_ = particlePos_;

		// パーティクル特有の座標を移動させる。
		particlePos_ += particleVec_ * particleSpeed_;
		particleSpeed_ -= 0.4f;
		if (particleSpeed_ < 1.0f) particleSpeed_ = 1.0f;

		// 重力を更新。
		grav += ADD_GRAV;
		if (MAX_GRAV < grav) grav = MAX_GRAV;
		particlePos_.y_ -= grav;


		// 移動させる。
		instance_.lock()->ChangeTrans(pos_ + particlePos_);

		// 今フレームの移動したベクトルを求める。
		Vec3 nowFrameMovedVec = (particlePos_ - particlePrevPos_).GetNormal();

		// 親の回転行列をクォータニオンに治す。
		DirectX::XMVECTOR parentRotQ = DirectX::XMQuaternionRotationMatrix(parentMatRot);

		// 移動ベクトルを元にパーティクルを回転させる。
		// 回転軸を求める。
		Vec3 axisOfRevolution = particlePrevMoveVec_.Cross(nowFrameMovedVec);

		// 回転軸を正規化する。
		if (axisOfRevolution.Length() != 0) {
			axisOfRevolution.Normalize();
		}

		// 回転量を求める。
		float amountOfRotation = particlePrevMoveVec_.Dot(nowFrameMovedVec);

		// 逆余弦を求める関数を使用して求めたcosθをラジアンに変換。
		amountOfRotation = acosf(amountOfRotation);

		// 回転軸が{0,0,0}だったら処理を飛ばす。
		if (axisOfRevolution.Length() != 0 && amountOfRotation != 0) {

			// クォータニオンを求める。
			DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(axisOfRevolution.ConvertXMVECTOR(), amountOfRotation);

			// パーティクルのクォータニオンにかける。
			particleMatQ_ = DirectX::XMQuaternionMultiply(particleMatQ_, quaternion);

			// クォータニオンをかける。
			parentRotQ = DirectX::XMQuaternionMultiply(parentRotQ, particleMatQ_);

			// 求めたクォータニオンを行列に治す。
			DirectX::XMMATRIX quaternionMat = DirectX::XMMatrixRotationQuaternion(parentRotQ);

			// 回転させる。
			instance_.lock()->ChangeRotate(quaternionMat);

		}


		// 今フレームの移動したベクトルを保存しておく。
		particlePrevMoveVec_ = nowFrameMovedVec;

		// サイズを変える。
		instance_.lock()->ChangeScale(Vec3(particleNowScale_.x_, particleNowScale_.y_, particleNowScale_.x_));

		// 消えるまでのタイマーを更新。
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

		// 車の座標を求める。
		Vec3 carPos = trackedInstance.lock()->GetWorldPos();

		// 座標を設定。
		instance_.lock()->ChangeTrans(carPos);

		// 回転を設定。
		DirectX::XMMATRIX charaRot = trackedInstance.lock()->GetRotate();
		instance_.lock()->ChangeRotate(charaRot);

		// イージングタイマーを更新し、イージング量を求める。
		jumpActionEasingTimer_ = FHelper::Saturate(jumpActionEasingTimer_ + ADD_JUMP_ACTION_EASING_TIMER_APPE);
		float easingAmount = FEasing::EaseInSine(jumpActionEasingTimer_);

		// スケールを設定。
		float nowScale = JUMP_DEF_SCALE + (JUMP_BIG_SCALE - JUMP_DEF_SCALE) * easingAmount;
		instance_.lock()->ChangeScale(Vec3(nowScale, nowScale, nowScale));

		// アルファを求める。
		RayEngine::Ins()->GetConstBufferData().alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f - (1.1f * easingAmount);

		//// 出現中だったら。
		//if (isAppearingNow_) {

		//	// イージングタイマーを更新し、イージング量を求める。
		//	jumpActionEasingTimer_ = FHelper::Saturate(jumpActionEasingTimer_ + ADD_JUMP_ACTION_EASING_TIMER_APPE);
		//	float easingAmount = FEasing::EaseOutCubic(jumpActionEasingTimer_);

		//	// スケールを設定。
		//	float nowScale = JUMP_DEF_SCALE + (JUMP_BIG_SCALE - JUMP_DEF_SCALE) * easingAmount;
		//	PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(nowScale, nowScale, nowScale));

		//	// アルファを求める。
		//	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ = 0.1f + (1.0f - 0.1f) * easingAmount;

		//	// タイマーがカンストしたら次へ。
		//	if (1.0f <= jumpActionEasingTimer_) {

		//		jumpActionEasingTimer_ = 0;
		//		isAppearingNow_ = false;

		//	}

		//}
		//else {

		//	// イージングタイマーを更新し、イージング量を求める。
		//	jumpActionEasingTimer_ = FHelper::Saturate(jumpActionEasingTimer_ + ADD_JUMP_ACTION_EASING_TIMER_EXIT);
		//	float easingAmount = FEasing::EaseInSine(jumpActionEasingTimer_);

		//	// スケールを設定。
		//	float nowScale = JUMP_BIG_SCALE + (JUMP_EXIT_SCALE - JUMP_BIG_SCALE) * easingAmount;
		//	PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(nowScale, nowScale, nowScale));

		//	// アルファを求める。
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