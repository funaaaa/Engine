#include "OBB.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"
#include "HitGroupMgr.h"

void OBB::Setting(const int& BlasIndex, const int& InsIndex)
{

	/*===== OBBをセッティング =====*/

	// OBBをセット。
	pos_ = PolygonInstanceRegister::Ins()->GetPos(InsIndex);
	defLength_ = BLASRegister::Ins()->GetVertexLengthMax(BlasIndex);
	length_ = defLength_;
	DirectX::XMMATRIX matRot_ = PolygonInstanceRegister::Ins()->GetRotate(InsIndex);
	dir_[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot_);
	dir_[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot_);
	dir_[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot_);

#ifdef DEBUG

	// デバッグ用のOBB本体をロード。
	ModelDataManager::ObjectData objectData;
	int hitGroupID = static_cast<int>(HitGroupMgr::Ins()->DENOISE_AO_HIT_GROUP);
	blasIndex = BLASRegister::Ins()->GenerateObj("Resource/Game/", "wireFrameBox.obj", HitGroupMgr::Ins()->hitGroupNames[hitGroupID], { L"Resource/Game/black.png" }, false, true, true);

	BLASRegister::Ins()->MulVec3Vertex(blasIndex, length);
	BLASRegister::Ins()->Update(blasIndex);

	insIndex = PolygonInstanceRegister::Ins()->CreateInstance(blasIndex, PolygonInstanceRegister::SHADER_ID::DEF);

#endif

}

void OBB::SetMat(const int& InsIndex)
{

	/*===== InstanceIDを指定して行列を生成 =====*/

	pos_ = PolygonInstanceRegister::Ins()->GetPos(InsIndex);
	length_ = FHelper::MulMat(defLength_, PolygonInstanceRegister::Ins()->GetScale(InsIndex));
	DirectX::XMMATRIX matRot_ = PolygonInstanceRegister::Ins()->GetRotate(InsIndex);
	dir_[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot_);
	dir_[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot_);
	dir_[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot_);

#ifdef DEBUG

	PolygonInstanceRegister::Ins()->ChangeRotate(insIndex, PolygonInstanceRegister::Ins()->GetRotate(InsIndex));
	PolygonInstanceRegister::Ins()->ChangeScale(insIndex, PolygonInstanceRegister::Ins()->GetScale(InsIndex));
	PolygonInstanceRegister::Ins()->ChangeTrans(insIndex, PolygonInstanceRegister::Ins()->GetTrans(InsIndex));

#endif

}

bool OBB::CheckHitOBB(OBB TargetOBB)
{

	/*===== OBBの当たり判定 =====*/

	// 各方向ベクトルの確保
	// N***:標準化方向ベクトル）
	Vec3 NAe1 = dir_[0], Ae1 = NAe1 * length_.x_;
	Vec3 NAe2 = dir_[1], Ae2 = NAe2 * length_.y_;
	Vec3 NAe3 = dir_[2], Ae3 = NAe3 * length_.z_;
	Vec3 NBe1 = TargetOBB.GetDir()[0], Be1 = NBe1 * TargetOBB.GetLength().x_;
	Vec3 NBe2 = TargetOBB.GetDir()[1], Be2 = NBe2 * TargetOBB.GetLength().y_;
	Vec3 NBe3 = TargetOBB.GetDir()[2], Be3 = NBe3 * TargetOBB.GetLength().z_;
	Vec3 Interval = pos_ - TargetOBB.GetPos();

	// 分離軸 : Ae1
	float rA = Ae1.Length();
	float rB = LenSegOnSeparateAxis(NAe1, Be1, Be2, Be3);
	float L = fabs(Interval.Dot(NAe1));
	if (L > rA + rB) {
		return false; // 衝突していない
	}

	// 分離軸 : Ae2
	rA = Ae2.Length();
	rB = LenSegOnSeparateAxis(NAe2, Be1, Be2, Be3);
	L = fabs(Interval.Dot(NAe2));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : Ae3
	rA = Ae3.Length();
	rB = LenSegOnSeparateAxis(NAe3, Be1, Be2, Be3);
	L = fabs(Interval.Dot(NAe3));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : Be1
	rA = LenSegOnSeparateAxis(NBe1, Ae1, Ae2, Ae3);
	rB = Be1.Length();
	L = fabs(Interval.Dot(NBe1));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : Be2
	rA = LenSegOnSeparateAxis(NBe2, Ae1, Ae2, Ae3);
	rB = Be2.Length();
	L = fabs(Interval.Dot(NBe2));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : Be3
	rA = LenSegOnSeparateAxis(NBe3, Ae1, Ae2, Ae3);
	rB = Be3.Length();
	L = fabs(Interval.Dot(NBe3));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C11
	Vec3 Cross;
	Cross = NAe1.Cross(NBe1);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C12
	Cross = NAe1.Cross(NBe2);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C13
	Cross = NAe1.Cross(NBe3);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C21
	Cross = NAe2.Cross(NBe1);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C22
	Cross = NAe2.Cross(NBe2);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C23
	Cross = NAe2.Cross(NBe3);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C31
	Cross = NAe3.Cross(NBe1);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C32
	Cross = NAe3.Cross(NBe2);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C33
	Cross = NAe3.Cross(NBe3);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離平面が存在しないので「衝突している」
	return true;
}

bool OBB::CheckHitOBB(std::weak_ptr<OBB> TargetOBB)
{

	/*===== OBBの当たり判定 =====*/

	// 各方向ベクトルの確保
	// N***:標準化方向ベクトル）
	Vec3 NAe1 = dir_[0], Ae1 = NAe1 * length_.x_;
	Vec3 NAe2 = dir_[1], Ae2 = NAe2 * length_.y_;
	Vec3 NAe3 = dir_[2], Ae3 = NAe3 * length_.z_;
	Vec3 NBe1 = TargetOBB.lock()->GetDir()[0], Be1 = NBe1 * TargetOBB.lock()->GetLength().x_;
	Vec3 NBe2 = TargetOBB.lock()->GetDir()[1], Be2 = NBe2 * TargetOBB.lock()->GetLength().y_;
	Vec3 NBe3 = TargetOBB.lock()->GetDir()[2], Be3 = NBe3 * TargetOBB.lock()->GetLength().z_;
	Vec3 Interval = pos_ - TargetOBB.lock()->GetPos();

	// 分離軸 : Ae1
	float rA = Ae1.Length();
	float rB = LenSegOnSeparateAxis(NAe1, Be1, Be2, Be3);
	float L = fabs(Interval.Dot(NAe1));
	if (L > rA + rB) {
		return false; // 衝突していない
	}

	// 分離軸 : Ae2
	rA = Ae2.Length();
	rB = LenSegOnSeparateAxis(NAe2, Be1, Be2, Be3);
	L = fabs(Interval.Dot(NAe2));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : Ae3
	rA = Ae3.Length();
	rB = LenSegOnSeparateAxis(NAe3, Be1, Be2, Be3);
	L = fabs(Interval.Dot(NAe3));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : Be1
	rA = LenSegOnSeparateAxis(NBe1, Ae1, Ae2, Ae3);
	rB = Be1.Length();
	L = fabs(Interval.Dot(NBe1));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : Be2
	rA = LenSegOnSeparateAxis(NBe2, Ae1, Ae2, Ae3);
	rB = Be2.Length();
	L = fabs(Interval.Dot(NBe2));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : Be3
	rA = LenSegOnSeparateAxis(NBe3, Ae1, Ae2, Ae3);
	rB = Be3.Length();
	L = fabs(Interval.Dot(NBe3));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C11
	Vec3 Cross;
	Cross = NAe1.Cross(NBe1);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C12
	Cross = NAe1.Cross(NBe2);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C13
	Cross = NAe1.Cross(NBe3);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C21
	Cross = NAe2.Cross(NBe1);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C22
	Cross = NAe2.Cross(NBe2);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C23
	Cross = NAe2.Cross(NBe3);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C31
	Cross = NAe3.Cross(NBe1);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C32
	Cross = NAe3.Cross(NBe2);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C33
	Cross = NAe3.Cross(NBe3);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離平面が存在しないので「衝突している」
	return true;
}

float OBB::LenSegOnSeparateAxis(Vec3 Sep, Vec3 E1, Vec3 E2, Vec3 E3)
{
	// 3つの内積の絶対値の和で投影線分長を計算
	// 分離軸Sepは標準化されていること
	float r1 = fabs(Sep.Dot(E1));
	float r2 = fabs(Sep.Dot(E2));
	float r3 = 0 < E3.Length() ? (fabs(Sep.Dot(E3))) : 0;
	return r1 + r2 + r3;
}
