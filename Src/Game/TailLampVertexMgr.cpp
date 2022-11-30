#include "TailLampVertexMgr.h"
#include "TailLampVertex.h"

TailLampVertexMgr::TailLampVertexMgr()
{

	/*===== �R���X�g���N�^ =====*/

	for (auto& index : tailLampVertex_) {

		index = std::make_shared<TailLampVertex>();

	}

	prevGenerateIndex_ = 0;
	nowGenerateIndex_ = 0;

}

void TailLampVertexMgr::Init()
{

	/*===== ���������� =====*/

	for (auto& index : tailLampVertex_) {

		index->Init();

	}

	prevGenerateIndex_ = 0;
	nowGenerateIndex_ = 0;

}

void TailLampVertexMgr::Generate(std::array<Vec3, 4> Vertex, float ScaleOffsetRate)
{

	/*===== �������� =====*/

	prevGenerateIndex_ = nowGenerateIndex_;
	for (auto& index : tailLampVertex_) {

		if (index->GetIsActive()) continue;

		index->Generate(Vertex, ScaleOffsetRate);

		nowGenerateIndex_ = static_cast<int>(&index - &tailLampVertex_[0]);

		break;

	}

}

void TailLampVertexMgr::Update()
{

	/*===== �X�V���� =====*/

	for (auto& index : tailLampVertex_) {

		if (!index->GetIsActive()) continue;

		index->Update();

	}

}
