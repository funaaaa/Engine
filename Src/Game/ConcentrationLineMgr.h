#pragma once
#include <memory>
#include <array>
#include "Vec.h"

class ConcentrationLine;

// 集中線管理クラス。生成と更新を行う。
class ConcentrationLineMgr {

private:

	/*===== メンバ変数 =====*/

	std::array<std::shared_ptr<ConcentrationLine>, 30> line_;	// 集中線の配列
	int generateDelay_;											// 集中線を生成する遅延
	const int GENERATE_DELAY = 1;								// 集中線を生成する遅延


public:

	ConcentrationLineMgr();
	void Init();
	void Generate();
	void Update();
	void Draw();

};