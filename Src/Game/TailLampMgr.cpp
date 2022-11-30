#include "TailLampMgr.h"
#include "TailLamp.h"
#include "TailLampVertexMgr.h"

TailLampMgr::TailLampMgr()
{

	/*===== �R���X�g���N�^ =====*/

	for (auto& index : tailLamp_) {
		index = std::make_shared<TailLamp>();
	}
	tailLampVertex_ = std::make_shared<TailLampVertexMgr>();
	isContinuity_ = false;
	continuityTimer_ = 0;

}

void TailLampMgr::Init()
{

	/*===== ���������� =====*/

	tailLampVertex_->Init();
	isContinuity_ = false;
	continuityTimer_ = 0;

}

void TailLampMgr::Generate(std::array<Vec3, 4> Vertex, int TextureColor, float ScaleOffsetRate)
{

	/*===== �������� =====*/

	// ���_��ۑ��B
	tailLampVertex_->Generate(Vertex, ScaleOffsetRate);

	for (auto& index : tailLamp_) {

		if (index->GetIsActive()) continue;

		// �A���Ő�������Ă�����A�O�t���[���ɐ������ꂽ���_�ƂȂ���B
		if (isContinuity_) {
			index->Generate(tailLampVertex_->GetNowGenerateVertexData(), tailLampVertex_->GetPrevGenerateVertexData(), TextureColor);
		}
		else {
			index->Generate(tailLampVertex_->GetNowGenerateVertexData(), tailLampVertex_->GetNowGenerateVertexData(), TextureColor);
		}

		break;

	}

	continuityTimer_ = CONTINUITY_TIMER;

}

void TailLampMgr::Update()
{

	/*===== �X�V���� =====*/

	// ���̃t���[���Ő�������Ă�����A�����Ă���B
	if (0 < continuityTimer_) {
		--continuityTimer_;
		isContinuity_ = true;
	}
	else {
		isContinuity_ = false;
	}

	tailLampVertex_->Update();

	for (auto& index : tailLamp_) {

		if (!index->GetIsActive()) continue;

		index->Update();

	}

}
