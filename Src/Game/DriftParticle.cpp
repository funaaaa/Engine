#include "DriftParticle.h"
#include "PolygonInstanceRegister.h"
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

	particleIns = PolygonInstanceRegister::Ins()->CreateInstance(BlasIndex, PolygonInstanceRegister::REFLECTION);
	constBufferIndex = ConstBufferIndex;
	// どこか遠くに飛ばす。
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns, Vec3(-10000000, -10000000, -10000000));
	PolygonInstanceRegister::Ins()->ChangeScale(particleIns, Vec3(0, 0, 0));


}

void DriftParticle::Init()
{

	/*===== 初期化処理 =====*/

	isActive = false;

	// どこか遠くに飛ばす。
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns, Vec3(-10000000, -10000000, -10000000));
	PolygonInstanceRegister::Ins()->ChangeScale(particleIns, Vec3(0, 0, 0));

}

void DriftParticle::Generate(const Vec3& Pos, const Vec3& DriftVec, const DirectX::XMMATRIX& CarMatRot, RayConstBufferData& ConstBufferData)
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
	rotForwardVec *= Vec3(10.0f, 10.0f, 10.0f);

	// 回転させた正面ベクトルの方向に座標をずらす。
	float randomAmount = FHelper::GetRand(0, 100) % 2 == 0 ? 1.0f : -1.0f;
	pos += rotForwardVec * randomAmount;

	// 座標とスケールを実装。
	ConstBufferData.light.pointLight[constBufferIndex].lightPos = pos + DriftVec * 10.0f;
	ConstBufferData.light.pointLight[constBufferIndex].isActive = true;
	ConstBufferData.light.pointLight[constBufferIndex].lightSize = scale;
	ConstBufferData.light.pointLight[constBufferIndex].lightPower = 30.0f;

}

void DriftParticle::Update(RayConstBufferData& ConstBufferData)
{

	/*===== 更新処理 =====*/

	if (scale <= SCALE - SUB_SCALE * 2.0f) {
		ConstBufferData.light.pointLight[constBufferIndex].lightPos = Vec3(-1000, -1000, -1000);
		ConstBufferData.light.pointLight[constBufferIndex].isActive = false;
	}

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
	PolygonInstanceRegister::Ins()->ChangeTrans(particleIns, pos);
	PolygonInstanceRegister::Ins()->ChangeScale(particleIns, Vec3(scale, scale, scale));

	// 大きさが0より小さくなったら初期化する。
	if (scale < 0) {

		scale = 0;
		Init();

		return;

	}

}
