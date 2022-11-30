#pragma once
#include "Vec.h"
#include <array>

// �e�[�������v�Ɏg�p����4���_��ێ����Ă����N���X�B
class TailLampVertex {

private:

	/*===== �����o�ϐ� =====*/

	std::array<Vec3, 4> vertex_;			// �`�悷��4���_
	bool isActive_;							// �L�����t���O
	int startScaleDownTimer_;				// �k�����J�n����܂ł̃^�C�}�[
	const int START_SCALE_DOWN_TIMER = 30;	// �k�����J�n����܂ł̃^�C�}�[


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	TailLampVertex();

	// ����������
	void Init();

	// ��������
	void Generate(std::array<Vec3, 4> Vertex, int TextureHandle);

	// �X�V����
	void Update();

	// �Q�b�^
	bool GetIsActive() { return isActive_; }

};