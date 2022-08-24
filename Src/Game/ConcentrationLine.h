#pragma once
#include "Vec.h"
#include <memory>

class Sprite;

// 集中線クラス
class ConcentrationLine {

private:

	/*===== メンバ変数 =====*/

	std::shared_ptr<Sprite> sprite_;
	float alpha_;
	int exitTimer_;
	const int EXIT_TIME = 30;
	bool isActive_;


public:

	/*===== メンバ関数 =====*/

	ConcentrationLine();
	void Init();
	void Generate(const Vec3& Pos);
	void Update();
	void Draw();

	bool GetIsActive() { return isActive_; }


};