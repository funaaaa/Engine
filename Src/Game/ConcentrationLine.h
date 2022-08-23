#pragma once
#include "Vec.h"
#include <memory>

class Sprite;

// �W�����N���X
class ConcentrationLine {

private:

	/*===== �����o�ϐ� =====*/

	std::shared_ptr<Sprite> sprite_;
	float alpha_;
	int exitTimer_;
	const int EXIT_TIME = 30;
	bool isActive_;


public:

	/*===== �����o�֐� =====*/

	ConcentrationLine();
	void Init();
	void Generate(const Vec3& Pos);
	void Update();
	void Draw();

	bool GetIsActive() { return isActive_; }


};