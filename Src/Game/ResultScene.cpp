#include "ResultScene.h"
#include "Input.h"
#include "FHelper.h"
#include "Pipeline.h"
#include "SceneTransition.h"

ResultScene::ResultScene()
{

	/*===== コンストラクタ =====*/

	isTransition_ = false;
	nextScene_ = SCENE_ID::TITLE;

	result_.GenerateForTexture(FHelper::WindowCenterPos(), FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Result/result.png");

}

void ResultScene::Init()
{

	/*===== 初期化処理 =====*/

	isTransition_ = false;
	isFinishTransition_ = false;
	isStartTransition_ = false;
	nextScene_ = SCENE_ID::TITLE;

}

void ResultScene::Update()
{

	/*===== 更新処理 =====*/

	// 遷移が終わってなかったら終わらせる。
	if (!isFinishTransition_) {
		isFinishTransition_ = true;
		SceneTransition::Ins()->Exit();
	}

	if (Input::Ins()->IsPadBottomTrigger(XINPUT_GAMEPAD_A) || Input::Ins()->IsKeyTrigger(DIK_RETURN)) {

		isTransition_ = true;

	}

	ImGui::Text("Press A or Enter to return to the title!");

}

#include "Engine.h"
void ResultScene::Draw()
{


	result_.Draw();
	SceneTransition::Ins()->Draw();

	UINT bbIndex = Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex();
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Engine::Ins()->swapchain_.backBuffers_[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	Engine::Ins()->copyResourceCmdList_->ResourceBarrier(1, &resourceBarrier);


}
