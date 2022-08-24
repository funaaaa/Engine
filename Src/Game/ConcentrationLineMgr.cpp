#include "ConcentrationLineMgr.h"
#include "ConcentrationLine.h"
#include "FHelper.h"

ConcentrationLineMgr::ConcentrationLineMgr()
{

	/*===== コンストラクタ =====*/

	for (auto& index : line_) {

		index = std::make_shared<ConcentrationLine>();

	}

	generateDelay_ = 0;

}

void ConcentrationLineMgr::Init()
{

	/*===== 初期化処理 =====*/

	for (auto& index : line_) {

		index->Init();

	}

	generateDelay_ = 0;

}

void ConcentrationLineMgr::Generate()
{

	/*===== 生成処理 =====*/

	++generateDelay_;
	if (generateDelay_ < GENERATE_DELAY) return;

	generateDelay_ = 0;

	// 生成する位置を求める。
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

	/*===== 更新処理 =====*/

	for (auto& index : line_) {

		if (!index->GetIsActive()) continue;

		index->Update();

	}

}

void ConcentrationLineMgr::Draw()
{

	/*===== 描画処理 =====*/

	for (auto& index : line_) {

		if (!index->GetIsActive()) continue;

		index->Draw();

	}

}
