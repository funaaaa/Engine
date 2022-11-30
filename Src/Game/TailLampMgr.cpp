#include "TailLampMgr.h"
#include "TailLamp.h"
#include "TailLampVertexMgr.h"

TailLampMgr::TailLampMgr()
{

	/*===== コンストラクタ =====*/

	for (auto& index : tailLamp_) {
		index = std::make_shared<TailLamp>();
	}
	tailLampVertex_ = std::make_shared<TailLampVertexMgr>();
	isContinuity_ = false;
	continuityTimer_ = 0;

}

void TailLampMgr::Init()
{

	/*===== 初期化処理 =====*/

	tailLampVertex_->Init();
	isContinuity_ = false;
	continuityTimer_ = 0;

}

void TailLampMgr::Generate(std::array<Vec3, 4> Vertex, int TextureColor, float ScaleOffsetRate)
{

	/*===== 生成処理 =====*/

	// 頂点を保存。
	tailLampVertex_->Generate(Vertex, ScaleOffsetRate);

	for (auto& index : tailLamp_) {

		if (index->GetIsActive()) continue;

		// 連続で生成されていたら、前フレームに生成された頂点とつなげる。
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

	/*===== 更新処理 =====*/

	// このフレームで生成されていたら連続している。
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
