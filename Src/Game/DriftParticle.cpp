#include "DriftParticle.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"

DriftParticle::DriftParticle()
{

	/*===== コンストラクタ =====*/

	particleIns_ = 0;
	constBufferIndex_ = 0;
	pos_ = Vec3();

	isActive_ = false;

}

void DriftParticle::Setting(const int& BlasIndex, const int ConstBufferIndex)
{

	/*===== セッティング =====*/

	blasIndex_ = BlasIndex;
	constBufferIndex_ = ConstBufferIndex;

}

void DriftParticle::Init()
{

	/*===== 初期化処理 =====*/

	if (isActive_) {

		PolygonInstanceRegister::Ins()->DestroyInstance(particleIns_);

	}

	isActive_ = false;

}

void DriftParticle::GenerateSmoke(const int& BlasIndex, const Vec3& Pos, const DirectX::XMMATRIX MatRot, RayConstBufferData& ConstBufferData, const bool& IsBoost)
{

	/*===== 生成処理 =====*/

	id_ = ID::SMOKE;
	pos_ = Pos;
	blasIndex_ = BlasIndex;
	isActive_ = true;
	isAppearingNow_ = true;
	appearingTimer_ = 0;
	particleIns_ = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex_, PolygonInstanceRegister::SHADER_ID::ALPHA);
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, Pos);
	PolygonInstanceRegister::Ins()->ChangeRotate(particleIns_, MatRot);

	// 奥方向の回転したベクトルを求める。
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

	/*===== 生成処理 =====*/

	id_ = ID::FIRE;
	pos_ = Pos;
	blasIndex_ = BlasIndex;
	isActive_ = true;
	isAppearingNow_ = true;
	appearingTimer_ = 0;
	particleIns_ = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex_, PolygonInstanceRegister::SHADER_ID::ALPHA);
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, Pos);
	PolygonInstanceRegister::Ins()->ChangeRotate(particleIns_, MatRot);

	// 奥方向の回転したベクトルを求める。
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

	/*===== オーラの生成処理 =====*/

	id_ = Id;
	blasIndex_ = BlasIndex;
	isActive_ = true;
	isAppearingNow_ = true;
	appearingTimer_ = 0;
	particleIns_ = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex_, PolygonInstanceRegister::SHADER_ID::ALPHA);
	trackedID_ = TireInsIndex_;
	isTrackRight_ = IsBoostRight;
	changeScaleTimer_ = 0;

	// 親の回転行列を求める。
	int parentInstanceIndex = PolygonInstanceRegister::Ins()->GetParentInstanceIndex(TireInsIndex_);
	DirectX::XMMATRIX parentMatRot = PolygonInstanceRegister::Ins()->GetRotate(parentInstanceIndex);

	// 座標を求める。
	pos_ = PolygonInstanceRegister::Ins()->GetWorldPos(trackedID_);
	Vec3 trackVec = isTrackRight_ ? Vec3(-1, 0, 0) : Vec3(1, 0, 0);
	trackVec = FHelper::MulRotationMatNormal(trackVec, parentMatRot);
	pos_ += trackVec * AURA_SIDE_SIZE;

	// 移動させる。
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, pos_);

	// 回転させる。
	PolygonInstanceRegister::Ins()->ChangeRotate(particleIns_, parentMatRot);

	// サイズを変える。
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

	/*===== ドリフト時のパーティクルを生成 =====*/

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

	// 親の回転行列を求める。
	int parentInstanceIndex = PolygonInstanceRegister::Ins()->GetParentInstanceIndex(TireInsIndex_);
	DirectX::XMMATRIX parentMatRot = PolygonInstanceRegister::Ins()->GetRotate(parentInstanceIndex);

	// パーティクル特有の変数を初期化。
	particlePos_ = Vec3();
	particlePrevPos_ = Vec3();
	particlePrevMoveVec_ = Vec3();
	particleVec_ = FHelper::GetRandVec3(-100, 100) / Vec3(100.0f, 100.0f, 100.0f);
	// 車体のY軸方向にベクトルを強める。
	Vec3 parentCarUpVec = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), parentMatRot);
	particleVec_ += parentCarUpVec * 0.5f;
	particleVec_.Normalize();
	particleSpeed_ = FHelper::GetRand(static_cast<int>(MAX_PARTICLE_SPEED / 0.5f) * 100, static_cast<int>(MAX_PARTICLE_SPEED) * 100) / 100.0f;

	// 座標を求める。
	pos_ = PolygonInstanceRegister::Ins()->GetWorldPos(trackedID_);
	Vec3 trackVec = isTrackRight_ ? Vec3(-1, 0, 0) : Vec3(1, 0, 0);
	trackVec = FHelper::MulRotationMatNormal(trackVec, parentMatRot);
	pos_ += trackVec * AURA_SIDE_SIZE;

	// 移動させる。
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, pos_);

	// 回転させる。
	PolygonInstanceRegister::Ins()->ChangeRotate(particleIns_, parentMatRot);

	// パーティクルの大きさを決める。
	const float HUNDRED = 100.0f;
	float random = FHelper::GetRand(static_cast<int>(PARTICLE_SCALE.x_ * HUNDRED / 2), static_cast<int>(PARTICLE_SCALE.x_ * HUNDRED));
	particleNowScale_.x_ = random / HUNDRED;

	random = FHelper::GetRand(static_cast<int>(PARTICLE_SCALE.y_ * HUNDRED / 2), static_cast<int>(PARTICLE_SCALE.y_ * HUNDRED));
	particleNowScale_.y_ = random / HUNDRED;

	// サイズを変える。
	PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(particleNowScale_.x_, particleNowScale_.y_, particleNowScale_.x_));

	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].instanceIndex_ = particleIns_;
	ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f;

}

void DriftParticle::Update(RayConstBufferData& ConstBufferData)
{

	/*===== 更新処理 =====*/

	switch (id_)
	{
	case DriftParticle::ID::SMOKE:

		if (isAppearingNow_) {

			// アルファ値を加算。
			ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ += APPEARING_ALPHA;
			if (1.0f < ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_) {

				ConstBufferData.alphaData_.alphaData_[constBufferIndex_].alpha_ = 1.0f;
				++appearingTimer_;

				// 出現タイマーが一定値に達したらアルファ値を下げていく。
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

		// 親の回転行列を求める。
		int parentInstanceIndex = PolygonInstanceRegister::Ins()->GetParentInstanceIndex(trackedID_);
		DirectX::XMMATRIX parentMatRot = PolygonInstanceRegister::Ins()->GetRotate(parentInstanceIndex);

		// 座標を求める。
		pos_ = PolygonInstanceRegister::Ins()->GetWorldPos(trackedID_);
		Vec3 trackVec = isTrackRight_ ? Vec3(-1, 0, 0) : Vec3(1, 0, 0);
		trackVec = FHelper::MulRotationMatNormal(trackVec, parentMatRot);
		pos_ += trackVec * AURA_SIDE_SIZE;

		// 移動させる。
		PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, pos_);

		// 回転させる。
		PolygonInstanceRegister::Ins()->ChangeRotate(particleIns_, parentMatRot);

		// サイズを変える。
		++changeScaleTimer_;
		const int MAX_CHANGE_TIMER = (id_ == ID::AURA_BIG ? CHANGE_SCALE_TIMER_BIG : CHANGE_SCALE_TIMER_SMALL);
		if (MAX_CHANGE_TIMER < changeScaleTimer_) {

			changeScaleTimer_ = 0;

			// 補間先のスケールをランダムで求める。
			const float HUNDRED = 100.0f;
			const int CHANGE_SCALE = (id_ == ID::AURA_BIG ? AURA_BIG_SCALE * HUNDRED : AURA_SMALL_SCALE * HUNDRED);
			float random = FHelper::GetRand(CHANGE_SCALE / 2, CHANGE_SCALE);
			changeScale_ = random / 100.0f;

		}

		// スケールを補完する。
		nowScale_ += (changeScale_ - nowScale_) / 2.0f;
		PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(nowScale_, nowScale_, nowScale_));


	}

	break;

	case DriftParticle::ID::PARTICLE:

	{

		// 親の回転行列を求める。
		int parentInstanceIndex = PolygonInstanceRegister::Ins()->GetParentInstanceIndex(trackedID_);
		DirectX::XMMATRIX parentMatRot = PolygonInstanceRegister::Ins()->GetRotate(parentInstanceIndex);

		// 座標を求める。
		pos_ = PolygonInstanceRegister::Ins()->GetWorldPos(trackedID_);
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
		PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, pos_ + particlePos_);

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
			PolygonInstanceRegister::Ins()->ChangeRotate(particleIns_, quaternionMat);

		}


		// 今フレームの移動したベクトルを保存しておく。
		particlePrevMoveVec_ = nowFrameMovedVec;

		// サイズを変える。
		PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(particleNowScale_.x_, particleNowScale_.y_, particleNowScale_.x_));

		// 消えるまでのタイマーを更新。
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