#pragma once
#include "Vec.h"
#include <memory>

class PolygonMeshInstance;

class PlayerTire{

private:

	/*===== メンバ変数 =====*/

	std::weak_ptr<PolygonMeshInstance> tireInstance;	// タイヤのインスタンスのインデックス
	Vec3 pos_;			// 座標
	Vec3 prevPos_;		// 前フレームの座標
	Vec3 rot_;			// 回転量
	bool isBehindTire_;	// 後輪かどうか

	
public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	PlayerTire(std::weak_ptr<PolygonMeshInstance> TireInstance, bool IsBehindTire);

	// 更新処理
	void Update();

	// 回転処理
	void Rot(bool IsDrift, float Rate);

};