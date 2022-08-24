#pragma once
#include <memory>
#include <array>
#include "Vec.h"

class ConcentrationLine;

class ConcentrationLineMgr {

private:

	/*===== ƒƒ“ƒo•Ï” =====*/

	std::array<std::shared_ptr<ConcentrationLine>, 30> line_;
	int generateDelay_;
	const int GENERATE_DELAY = 1;


public:

	ConcentrationLineMgr();
	void Init();
	void Generate();
	void Update();
	void Draw();

};