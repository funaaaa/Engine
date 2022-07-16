#include "OBB.h"
#include "BLASRegister.h"
#include "PolygonInstanceRegister.h"
#include "FHelper.h"
#include "HitGroupMgr.h"

void OBB::Setting(const int& BlasIndex, const int& InsIndex)
{

	/*===== OBB���Z�b�e�B���O =====*/

	// OBB���Z�b�g�B
	pos_ = PolygonInstanceRegister::Ins()->GetPos(InsIndex);
	defLength_ = BLASRegister::Ins()->GetVertexLengthMax(BlasIndex);
	length_ = defLength_;
	DirectX::XMMATRIX matRot_ = PolygonInstanceRegister::Ins()->GetRotate(InsIndex);
	dir_[0] = FHelper::MulRotationMatNormal(Vec3(1, 0, 0), matRot_);
	dir_[1] = FHelper::MulRotationMatNormal(Vec3(0, 1, 0), matRot_);
	dir_[2] = FHelper::MulRotationMatNormal(Vec3(0, 0, 1), matRot_);

#ifdef DEBUG

	// �f�o�b�O�p��OBB�{�̂����[�h�B
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

	/*===== InstanceID���w�肵�čs��𐶐� =====*/

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

	/*===== OBB�̓����蔻�� =====*/

	// �e�����x�N�g���̊m��
	// N***:�W���������x�N�g���j
	Vec3 NAe1 = dir_[0], Ae1 = NAe1 * length_.x_;
	Vec3 NAe2 = dir_[1], Ae2 = NAe2 * length_.y_;
	Vec3 NAe3 = dir_[2], Ae3 = NAe3 * length_.z_;
	Vec3 NBe1 = TargetOBB.GetDir()[0], Be1 = NBe1 * TargetOBB.GetLength().x_;
	Vec3 NBe2 = TargetOBB.GetDir()[1], Be2 = NBe2 * TargetOBB.GetLength().y_;
	Vec3 NBe3 = TargetOBB.GetDir()[2], Be3 = NBe3 * TargetOBB.GetLength().z_;
	Vec3 Interval = pos_ - TargetOBB.GetPos();

	// ������ : Ae1
	float rA = Ae1.Length();
	float rB = LenSegOnSeparateAxis(NAe1, Be1, Be2, Be3);
	float L = fabs(Interval.Dot(NAe1));
	if (L > rA + rB) {
		return false; // �Փ˂��Ă��Ȃ�
	}

	// ������ : Ae2
	rA = Ae2.Length();
	rB = LenSegOnSeparateAxis(NAe2, Be1, Be2, Be3);
	L = fabs(Interval.Dot(NAe2));
	if (L > rA + rB) {
		return false;
	}

	// ������ : Ae3
	rA = Ae3.Length();
	rB = LenSegOnSeparateAxis(NAe3, Be1, Be2, Be3);
	L = fabs(Interval.Dot(NAe3));
	if (L > rA + rB) {
		return false;
	}

	// ������ : Be1
	rA = LenSegOnSeparateAxis(NBe1, Ae1, Ae2, Ae3);
	rB = Be1.Length();
	L = fabs(Interval.Dot(NBe1));
	if (L > rA + rB) {
		return false;
	}

	// ������ : Be2
	rA = LenSegOnSeparateAxis(NBe2, Ae1, Ae2, Ae3);
	rB = Be2.Length();
	L = fabs(Interval.Dot(NBe2));
	if (L > rA + rB) {
		return false;
	}

	// ������ : Be3
	rA = LenSegOnSeparateAxis(NBe3, Ae1, Ae2, Ae3);
	rB = Be3.Length();
	L = fabs(Interval.Dot(NBe3));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C11
	Vec3 Cross;
	Cross = NAe1.Cross(NBe1);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C12
	Cross = NAe1.Cross(NBe2);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C13
	Cross = NAe1.Cross(NBe3);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C21
	Cross = NAe2.Cross(NBe1);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C22
	Cross = NAe2.Cross(NBe2);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C23
	Cross = NAe2.Cross(NBe3);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C31
	Cross = NAe3.Cross(NBe1);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C32
	Cross = NAe3.Cross(NBe2);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C33
	Cross = NAe3.Cross(NBe3);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// �������ʂ����݂��Ȃ��̂Łu�Փ˂��Ă���v
	return true;
}

bool OBB::CheckHitOBB(std::weak_ptr<OBB> TargetOBB)
{

	/*===== OBB�̓����蔻�� =====*/

	// �e�����x�N�g���̊m��
	// N***:�W���������x�N�g���j
	Vec3 NAe1 = dir_[0], Ae1 = NAe1 * length_.x_;
	Vec3 NAe2 = dir_[1], Ae2 = NAe2 * length_.y_;
	Vec3 NAe3 = dir_[2], Ae3 = NAe3 * length_.z_;
	Vec3 NBe1 = TargetOBB.lock()->GetDir()[0], Be1 = NBe1 * TargetOBB.lock()->GetLength().x_;
	Vec3 NBe2 = TargetOBB.lock()->GetDir()[1], Be2 = NBe2 * TargetOBB.lock()->GetLength().y_;
	Vec3 NBe3 = TargetOBB.lock()->GetDir()[2], Be3 = NBe3 * TargetOBB.lock()->GetLength().z_;
	Vec3 Interval = pos_ - TargetOBB.lock()->GetPos();

	// ������ : Ae1
	float rA = Ae1.Length();
	float rB = LenSegOnSeparateAxis(NAe1, Be1, Be2, Be3);
	float L = fabs(Interval.Dot(NAe1));
	if (L > rA + rB) {
		return false; // �Փ˂��Ă��Ȃ�
	}

	// ������ : Ae2
	rA = Ae2.Length();
	rB = LenSegOnSeparateAxis(NAe2, Be1, Be2, Be3);
	L = fabs(Interval.Dot(NAe2));
	if (L > rA + rB) {
		return false;
	}

	// ������ : Ae3
	rA = Ae3.Length();
	rB = LenSegOnSeparateAxis(NAe3, Be1, Be2, Be3);
	L = fabs(Interval.Dot(NAe3));
	if (L > rA + rB) {
		return false;
	}

	// ������ : Be1
	rA = LenSegOnSeparateAxis(NBe1, Ae1, Ae2, Ae3);
	rB = Be1.Length();
	L = fabs(Interval.Dot(NBe1));
	if (L > rA + rB) {
		return false;
	}

	// ������ : Be2
	rA = LenSegOnSeparateAxis(NBe2, Ae1, Ae2, Ae3);
	rB = Be2.Length();
	L = fabs(Interval.Dot(NBe2));
	if (L > rA + rB) {
		return false;
	}

	// ������ : Be3
	rA = LenSegOnSeparateAxis(NBe3, Ae1, Ae2, Ae3);
	rB = Be3.Length();
	L = fabs(Interval.Dot(NBe3));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C11
	Vec3 Cross;
	Cross = NAe1.Cross(NBe1);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C12
	Cross = NAe1.Cross(NBe2);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C13
	Cross = NAe1.Cross(NBe3);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C21
	Cross = NAe2.Cross(NBe1);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C22
	Cross = NAe2.Cross(NBe2);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C23
	Cross = NAe2.Cross(NBe3);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C31
	Cross = NAe3.Cross(NBe1);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C32
	Cross = NAe3.Cross(NBe2);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// ������ : C33
	Cross = NAe3.Cross(NBe3);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Interval.Dot(Cross));
	if (L > rA + rB) {
		return false;
	}

	// �������ʂ����݂��Ȃ��̂Łu�Փ˂��Ă���v
	return true;
}

float OBB::LenSegOnSeparateAxis(Vec3 Sep, Vec3 E1, Vec3 E2, Vec3 E3)
{
	// 3�̓��ς̐�Βl�̘a�œ��e���������v�Z
	// ������Sep�͕W��������Ă��邱��
	float r1 = fabs(Sep.Dot(E1));
	float r2 = fabs(Sep.Dot(E2));
	float r3 = 0 < E3.Length() ? (fabs(Sep.Dot(E3))) : 0;
	return r1 + r2 + r3;
}
