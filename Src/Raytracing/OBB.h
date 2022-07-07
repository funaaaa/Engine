#pragma once
#include "Vec.h"
#include <array>

class OBB
{
public:

	/*===== メンバ変数 =====*/

	Vec3 pos;						// 位置
	std::array<Vec3, 3> dir;		// 方向ベクトル
	Vec3 length;	// 各軸方向の長さ


public:

	/*===== メンバ関数 =====*/

	// OBBとの当たり判定
	bool CheckHitOBB(OBB TargetOBB);

	inline const std::array<Vec3, 3>& GetDir() { return dir; };		// 指定軸番号の方向ベクトルを取得
	inline const Vec3 GetLength() { return length; };				// 指定軸方向の長さを取得
	inline const Vec3& GetPos() { return pos; };					// 位置を取得

private:

	// 分離軸に投影された軸成分から投影線分長を算出
	float LenSegOnSeparateAxis(Vec3 Sep, Vec3 E1, Vec3 E2, Vec3 E3 = 0);

};