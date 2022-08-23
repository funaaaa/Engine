#include "ConcentrationLineMgr.h"
#include "ConcentrationLine.h"
#include "FHelper.h"

ConcentrationLineMgr::ConcentrationLineMgr()
{

	/*===== �R���X�g���N�^ =====*/

	for (auto& index : line_) {

		index = std::make_shared<ConcentrationLine>();

	}

	generateDelay_ = 0;

}

void ConcentrationLineMgr::Init()
{

	/*===== ���������� =====*/

	for (auto& index : line_) {

		index->Init();

	}

	generateDelay_ = 0;

}

void ConcentrationLineMgr::Generate()
{

	/*===== �������� =====*/

	++generateDelay_;
	if (generateDelay_ < GENERATE_DELAY) return;

	generateDelay_ = 0;

	// ��������ʒu�����߂�B
	Vec3 pos;
	int random = FHelper::GetRand(0, 3);
	switch (random)
	{
	case 0:
		pos.x_ = FHelper::GetRand(0, 1280);
		pos.y_ = FHelper::GetRand(-400, -100);
		break;
	case 1:
		pos.x_ = FHelper::GetRand(0, 1280);
		pos.y_ = FHelper::GetRand(100, 400) + 720;
		break;
	case 2:
		pos.x_ = FHelper::GetRand(-400, -100);
		pos.y_ = FHelper::GetRand(0, 720);
		break;
	case 3:
		pos.x_ = FHelper::GetRand(100, 400) + 1280;
		pos.y_ = FHelper::GetRand(0, 720);
		break;
	default:
		break;
	}

	for (auto& index : line_) {

		if (index->GetIsActive()) continue;

		index->Generate(pos);

		break;

	}

}

void ConcentrationLineMgr::Update()
{

	/*===== �X�V���� =====*/

	for (auto& index : line_) {

		if (!index->GetIsActive()) continue;

		index->Update();

	}

}

void ConcentrationLineMgr::Draw()
{

	/*===== �`�揈�� =====*/

	for (auto& index : line_) {

		if (!index->GetIsActive()) continue;

		index->Draw();

	}

}
