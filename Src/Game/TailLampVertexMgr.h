#pragma once
#include <array>
#include <memory>
#include "Vec.h"

class TailLampVertex;

// �e�[�������v���_�Z�b�g�̔z���ێ�����N���X�B
class TailLampVertexMgr {

private:

	/*===== �����o�ϐ� =====*/

	static const int TAIL_LAMP_VERTEX_SIZE = 10;	// �e�[�������v�̒��_�Z�b�g�̕ۑ���(�ő�10���_�܂ŕۑ�����B)
	std::array<std::shared_ptr<TailLampVertex>, TAIL_LAMP_VERTEX_SIZE> tailLampVertex_;
	int nowGenerateIndex_;		// ���񐶐�����Index
	int prevGenerateIndex_;		// �O�񐶐�����Index


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	TailLampVertexMgr();

	// ����������
	void Init();

	// ��������
	void Generate(std::array<Vec3, 4> Vertex, float ScaleOffsetRate);

	// �X�V����
	void Update();

	// �Q�b�^
	int GetNowGenerateIndex() { return nowGenerateIndex_; }
	int GetPrevGenerateIndex() { return prevGenerateIndex_; }
	std::weak_ptr<TailLampVertex> GetNowGenerateVertexData() { return tailLampVertex_[nowGenerateIndex_]; }
	std::weak_ptr<TailLampVertex> GetPrevGenerateVertexData() { return tailLampVertex_[prevGenerateIndex_]; }

};