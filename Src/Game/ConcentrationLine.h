#pragma once
#include "Vec.h"
#include <memory>

class Sprite;

// �W�����N���X
class ConcentrationLine {

private:

	/*===== �����o�ϐ� =====*/

	std::shared_ptr<Sprite> sprite_;	// �`�悷��摜�f�[�^
	float alpha_;						// �A���t�@�l ���񂾂񔖂����Ă����B
	int exitTimer_;						// ����������܂ł̃^�C�}�[
	const int EXIT_TIME = 10;
	bool isActive_;						// �L�����t���O


public:

	/*===== �����o�֐� =====*/

	ConcentrationLine();
	void Init();
	void Generate(const Vec3& Pos);
	void Update();
	void Draw();

	bool GetIsActive() { return isActive_; }


};