#include "DriftParticle.h"
#include "PorygonInstanceRegister.h"
#include "FHelper.h"

DriftParticle::DriftParticle()
{

	/*===== コンストラクタ =====*/

	particleIns = 0;
	constBufferIndex = 0;
	pos = Vec3();
	forwardVec = Vec3();
	speed = 0;
	scale = 0;
	isActive = false;

}

void DriftParticle::Setting(const int& BlasIndex, const int ConstBufferIndex)
{

	/*===== セッティング =====*/

	particleIns = PorygonInstanceRegister::Ins()->CreateInstance(BlasIndex, PorygonInstanceRegister::SHADER_ID_TEXCOLOR);
	constBufferIndex = ConstBufferIndex;
	// どこか遠くに飛ばす。
	PorygonInstanceRegister::Ins()->ChangeTrans(particleIns, Vec3(-100000, -100000, -10000));
	PorygonInstanceRegister::Ins()->ChangeScale(particleIns, Vec3(0, 0, 0));


}

void DriftParticle::Init()
{

	/*===== 初期化処理 =====*/

	isActive = false;

	// どこか遠くに飛ばす。
	PorygonInstanceRegister::Ins()->ChangeTrans(particleIns, Vec3(-100000, -100000, -10000));
	PorygonInstanceRegister::Ins()->ChangeScale(particleIns, Vec3(0, 0, 0));

}

void DriftParticle::Generate(const Vec3& Pos, const Vec3& DriftVec, const DirectX::XMMATRIX& CarMatRot)
{

	/*===== 生成処理 =====*/

	pos = Pos;
	speed = SPEED;
	scale = SCALE;
	isActive = true;

	// 進行方向ベクトルを求める。
	forwardVec = DriftVec;

	// デフォルトの上ベクトルを回転させる。
	Vec3 rotUpVec = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), CarMatRot);

	// 回転させた上ベクトルの方向に乱数を求めて進行方向ベクトルを上下に動かす。
	forwardVec += Vec3(FHelper::GetRand(rotUpVec.x / 2.0f), FHelper::GetRand(rotUpVec.y / 2.0f), FHelper::GetRand(rotUpVec.z / 2.0f));
	forwardVec.Normalize();

	// デフォルトの正面ベクトルを回転させる。
	Vec3 rotForwardVec = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), CarMatRot);

	// 回転させた正面ベクトルの方向に座標をずらす。
	float randomAmount = static_cast<float>(FHelper::GetRand(-1000, 1000)) / 100.0f;
	pos += rotForwardVec + randomAmount;

}

void DriftParticle::Update(RayConstBufferData& ConstBufferData)
{

	/*===== 更新処理 =====*/

	// パーティクルを動かす。
	pos += forwardVec * speed;

	// パーティクルの進行方向に重力を加算する。
	forwardVec.y -= 0.08f;
	forwardVec.Normalize();

	// 移動速度を下げる。
	speed -= SUB_SPEED;

	// 大きさを小さくする。
	scale -= SUB_SCALE;

	// Instanceを更新。
	PorygonInstanceRegister::Ins()->ChangeTrans(particleIns, pos);
	PorygonInstanceRegister::Ins()->ChangeScale(particleIns, Vec3(scale, scale, scale));

	// 大きさが0より小さくなったら初期化する。
	if (scale < 0) {

		scale = 0;
		Init();

	}
	ConstBufferData;
}
