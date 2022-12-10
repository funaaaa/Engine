#pragma once
#include "Vec.h"
#include <memory>

class BLAS;
class PolygonMeshInstance;
class TailLampVertex;

// �e�[�������v�̌`�N���X�B�e�[�������v���_�N���X���瓛��̃��f���������B
class TailLamp {

private:

	/*===== �����o�ϐ� =====*/

	std::weak_ptr<BLAS> blas_;					// �e�[�������v�̌`��ۑ�����BLAS�B�e�[�������v���_�N���X2���Q�Ƃ��ē���̌`��𐶐�����B
	std::weak_ptr<PolygonMeshInstance> ins_;	// �e�[�������v��Instance�B
	std::weak_ptr<TailLampVertex> vertexStart_;	// �e�[�������v�̓��`��̊J�n�n�_�B
	std::weak_ptr<TailLampVertex> vertexEnd_;	// �e�[�������v�̓��`��̏I���n�_�B
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