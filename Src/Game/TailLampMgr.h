#pragma once
#include <array>
#include <memory>
#include "Vec.h"

class TailLamp;
class TailLampVertexMgr;

// �e�[�������v�Ǘ��N���X �e�[�������v�𐶐����钸�_���w�肷�邱�ƂŁA���_�̕ۑ��ƃe�[�������v�̐������s���B
class TailLampMgr {

private:

	/*===== �����o�ϐ� =====*/

	static const int LAMP_COUNT = 10;
	std::array<std::shared_ptr<TailLamp>, LAMP_COUNT> tailLamp_;	// �e�[�������v�{�̃N���X
	std::shared_ptr<TailLampVertexMgr> tailLampVertex_;				// �e�[�������v���쐬����ۂɎQ�Ƃ��钸�_�N���X
	bool isContinuity_;			// �A�����Ă��邩
	int continuityTimer_;		// �A�����Ă��锻��̎���
	const int CONTINUITY_TIMER = 4;	// �A�����肵�Ă��鎞�ԁB


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	TailLampMgr();

	// ������
	void Init();

	// �X�V����
	void Generate(std::array<Vec3, 4> Vertex, int TextureColor, float ScaleOffsetRate);

	// �X�V����
	void Update();

};