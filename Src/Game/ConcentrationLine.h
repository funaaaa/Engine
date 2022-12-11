#pragma once
#include "Vec.h"
#include <memory>

class Sprite;

// 集中線クラス
class ConcentrationLine {

private:

	/*===== メンバ変数 =====*/

	std::shared_ptr<Sprite> sprite_;	// 描画する画像データ
	float alpha_;						// アルファ値 だんだん薄くしていく。
	int exitTimer_;						// 無効化するまでのタイマー
	const int EXIT_TIME = 10;
	bool isActive_;						// 有効化フラグ


public:

	/*===== メンバ関数 =====*/

	ConcentrationLine();
	void Init();
	void Generate(const Vec3& Pos);
	void Update();
	void Draw();

	bool GetIsActive() { return isActive_; }


};