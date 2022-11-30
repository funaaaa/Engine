#pragma once
#include "Vec.h"
#include <memory>

class BLAS;
class PolygonMeshInstance;
class TailLampVertex;

// �e�[�������v�̌`�N���X�B����̃��f���������B
class TailLamp {

private:

	/*===== �����o�ϐ� =====*/

	std::weak_ptr<BLAS> blas_;
	std::weak_ptr<PolygonMeshInstance> ins_;
	std::weak_ptr<TailLampVertex> vertexStart_;
	std::weak_ptr<TailLampVertex> vertexEnd_;
	bool isActive_;


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	TailLamp();

	// ��������
	void Generate(std::weak_ptr<TailLampVertex> VertexStart, std::weak_ptr<TailLampVertex> VertexEnd, int TextureColor);

	// �X�V����
	void Update();

	// �Q�b�^
	bool GetIsActive() { return isActive_; }


};