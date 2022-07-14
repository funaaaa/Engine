#include "DriftParticle.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"

DriftParticle::DriftParticle()
{

	/*===== コンストラクタ =====*/

	particleIns_ = 0;
	constBufferIndex_ = 0;
	pos_ = Vec3();
	forwardVec_ = Vec3();
	speed_ = 0;
	scale_ = 0;
	isActive_ = false;

}

void DriftParticle::Setting(const int& BlasIndex, const int ConstBufferIndex)
{

	/*===== セッティング =====*/

	particleIns_ = PolygonInstanceRegister::Ins()->CreateInstance(BlasIndex, PolygonInstanceRegister::REFLECTION);
	constBufferIndex_ = ConstBufferIndex;
	// どこか遠くに飛ばす。
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, Vec3(-10000000, -10000000, -10000000));
	PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(0, 0, 0));


}

void DriftParticle::Init()
{

	/*===== 初期化処理 =====*/

	isActive_ = false;

	// どこか遠くに飛ばす。
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, Vec3(-10000000, -10000000, -10000000));
	PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(0, 0, 0));

}

void DriftParticle::Generate(const Vec3& Pos, const Vec3& DriftVec, const DirectX::XMMATRIX& CarMatRot, RayConstBufferData& ConstBufferData)
{

	/*===== 生成処理 =====*/

	pos_ = Pos;
	speed_ = SPEED;
	scale_ = SCALE;
	isActive_ = true;

	// 進行方向ベクトルを求める。
	forwardVec_ = DriftVec;

	// デフォルトの上ベクトルを回転させる。
	Vec3 rotUpVec = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), CarMatRot);

	// 回転させた上ベクトルの方向に乱数を求めて進行方向ベクトルを上下に動かす。
	forwardVec_ += Vec3(FHelper::GetRand(rotUpVec.x / 2.0f), FHelper::GetRand(rotUpVec.y / 2.0f), FHelper::GetRand(rotUpVec.z / 2.0f));
	forwardVec_.Normalize();

	// デフォルトの正面ベクトルを回転させる。
	Vec3 rotForwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), CarMatRot);
	rotForwardVec *= Vec3(10.0f, 10.0f, 10.0f);

	// 回転させた正面ベクトルの方向に座標をずらす。
	float randomAmount = FHelper::GetRand(0, 100) % 2 == 0 ? 1.0f : -1.0f;
	pos_ += rotForwardVec * randomAmount;

	// 座標とスケールを実装。
	ConstBufferData.light.pointLight[constBufferIndex_].lightPos = pos_ + DriftVec * 10.0f;
	ConstBufferData.light.pointLight[constBufferIndex_].isActive_ = true;
	ConstBufferData.light.pointLight[constBufferIndex_].lightSize = scale_;
	ConstBufferData.light.pointLight[constBufferIndex_].lightPower = 30.0f;

}

void DriftParticle::Update(RayConstBufferData& ConstBufferData)
{

	/*===== 更新処理 =====*/

	if (scale_ <= SCALE - SUB_SCALE * 2.0f) {
		ConstBufferData.light.pointLight[constBufferIndex_].lightPos = Vec3(-1000, -1000, -1000);
		ConstBufferData.light.pointLight[constBufferIndex_].isActive_ = false;
	}

	// パーティクルを動かす。
	pos_ += forwardVec_ * speed_;

	// パーティクルの進行方向に重力を加算する。
	forwardVec_.y -= 0.08f;
	forwardVec_.Normalize();

	// 移動速度を下げる。
	speed_ -= SUB_SPEED;

	// 大きさを小さくする。
	scale_ -= SUB_SCALE;

	// Instanceを更新。
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns_, pos_);
	PolygonInstanceRegister::Ins()->ChangeScale(particleIns_, Vec3(scale_, scale_, scale_));

	// 大きさが0より小さくなったら初期化する。
	if (scale_ < 0) {

		scale_ = 0;
		Init();

		return;

	}

}
