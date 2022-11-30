#pragma once
#include <array>
#include <memory>
#include "Vec.h"

class TailLampVertex;

// �e�[�������v���_�Ǘ��N���X
class TailLampVertexMgr {

private:

	/*===== �����o�ϐ� =====*/

	static const int TAIL_LAMP_VERTEX_SIZE = 30;
	std::array<std::shared_ptr<TailLampVertex>, TAIL_LAMP_VERTEX_SIZE> tailLampVertex_;
	int prevGenerateIndex_;		// �O�񐶐�����Index


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	TailLampVertexMgr();

	// ����������
	void Init();

	// ��������
	void Generate(std::array<Vec3, 4> Vertex, int TextureHandle);

	// �X�V����
	void Update();

};