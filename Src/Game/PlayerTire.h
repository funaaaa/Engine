#pragma once
#include "Vec.h"

class PlayerTire{

private:

	/*===== メンバ変数 =====*/

	int tireInsIndex_;	// タイヤのインスタンスのインデックス
	Vec3 pos_;			// 座標
	Vec3 prevPos_;		// 前フレームの座標
	Vec3 rot_;			// 回転量
	bool isBehindTire_;	// 後輪かどうか

	
public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	PlayerTire(const int& TireInsIndex, const bool& IsBehindTire);

	// 更新処理
	void Update();

	// 回転処理
	void Rot(const bool& IsDrift, const float& Rate);

};