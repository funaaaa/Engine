#include "OBB.h"

bool OBB::CheckHitOBB(OBB TargetOBB)
{
	// �e�����x�N�g���̊m��
	// N***:�W���������x�N�g���j
	Vec3 NAe1 = dir[0], Ae1 = NAe1 * length.x;
	Vec3 NAe2 = dir[1], Ae2 = NAe2 * length.y;
	Vec3 NAe3 = dir[2], Ae3 = NAe3 * length.z;
	Vec3 NBe1 = TargetOBB.GetDir()[0], Be1 = NBe1 * TargetOBB.GetLength().x;
	Vec3 NBe2 = TargetOBB.GetDir()[1], Be2 = NBe2 * TargetOBB.GetLength().y;
	Vec3 NBe3 = TargetOBB.GetDir()[2], Be3 = NBe3 * TargetOBB.GetLength().z;
	Vec3 Interval = pos - TargetOBB.GetPos();

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
	float r3 = E3.Length() == 0 ? (fabs(Sep.Dot(E3))) : 0;
	return r1 + r2 + r3;
}
