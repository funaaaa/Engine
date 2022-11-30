#pragma once
#include "Vec.h"
#include <array>

// �e�[�������v�Ɏg�p����4���_��ێ����Ă����N���X�B
class TailLampVertex {

private:

	/*===== �����o�ϐ� =====*/

	std::array<Vec3, 4> vertex_;			// �`�悷��4���_
	bool isActive_;							// �L�����t���O
	bool isFinishScaleDown_;				// �k�����I��������
	int startScaleDownTimer_;				// �k�����J�n����܂ł̃^�C�}�[
	const int START_SCALE_DOWN_TIMER = 1;	// �k�����J�n����܂ł̃^�C�}�[

public:

	// �Q�Ɛ�
	int referenceCount_;


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	TailLampVertex();

	// ����������
	void Init();

	// ��������
	void Generate(std::array<Vec3, 4> Vertex, float ScaleOffsetRate);

	// �X�V����
	void Update();

	// �Q�b�^
	bool GetIsActive() { return isActive_; }
	bool GetIsFinishScaleDown() { return isFinishScaleDown_; }
	std::array<Vec3, 4> GetVertex() { return vertex_; }

};