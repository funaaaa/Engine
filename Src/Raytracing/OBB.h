#pragma once
#include "Vec.h"
#include <array>
#include <memory>

class PolygonMeshInstance;

class OBB
{

public:

	/*===== メンバ変数 =====*/

	Vec3 pos_;					// 位置
	std::array<Vec3, 3> dir_;	// 方向ベクトル
	Vec3 length_;				// 各軸方向の長さ
	Vec3 defLength_;				// デフォルトの各軸方向の長さ

	// デバッグ用
	int blasIndex_;
	int insIndex_;


public:

	/*===== メンバ関数 =====*/

	// OBBを生成
	void Setting(const int& BlasIndex, std::weak_ptr<PolygonMeshInstance> Instance);

	// Instanceを指定して各種行列を設定。
	void SetMat(std::weak_ptr<PolygonMeshInstance> Instance);

	// OBBとの当たり判定
	bool CheckHitOBB(OBB TargetOBB);
	bool CheckHitOBB(std::weak_ptr<OBB> TargetOBB);

	inline const std::array<Vec3, 3>& GetDir() { return dir_; };		// 指定軸番号の方向ベクトルを取得
	inline const Vec3 GetLength() { return length_; };				// 指定軸方向の長さを取得
	inline const Vec3& GetPos() { return pos_; };					// 位置を取得

private:

	// 分離軸に投影された軸成分から投影線分長を算出
	float LenSegOnSeparateAxis(Vec3 Sep, Vec3 E1, Vec3 E2, Vec3 E3 = 0);

};