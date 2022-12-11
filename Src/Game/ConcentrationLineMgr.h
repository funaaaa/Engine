#pragma once
#include <memory>
#include <array>
#include "Vec.h"

class ConcentrationLine;

// �W�����Ǘ��N���X�B�����ƍX�V���s���B
class ConcentrationLineMgr {

private:

	/*===== �����o�ϐ� =====*/

	std::array<std::shared_ptr<ConcentrationLine>, 30> line_;	// �W�����̔z��
	int generateDelay_;											// �W�����𐶐�����x��
	const int GENERATE_DELAY = 1;								// �W�����𐶐�����x��


public:

	ConcentrationLineMgr();
	void Init();
	void Generate();
	void Update();
	void Draw();

};