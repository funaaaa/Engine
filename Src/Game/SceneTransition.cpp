#include "SceneTransition.h"
#include "FHelper.h"
#include "PipelineManager.h"

void SceneTransition::Setting()
{

	/*===== セッティング =====*/

	titleLogo_.GenerateForTexture(FHelper::WindowCenterPos(), FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/titleLogo.png");
	alpha_ = 0.0f;
	isAppear_ = false;
	easingTimer_ = 1.0f;

}

void SceneTransition::Appear()
{

	/*===== 開始処理 =====*/

	alpha_ = 0.0f;
	isAppear_ = true;
	easingTimer_ = 0.0f;

}

void SceneTransition::Exit()
{

	/*===== 終了処理 =====*/

	alpha_ = 1.0f;
	isAppear_ = false;
	easingTimer_ = 0.0f;

}

void SceneTransition::Update()
{

	/*===== 更新処理 =====*/

	easingTimer_ = FHelper::Clamp(easingTimer_ + ADD_EASING_TIMER, 0.0f, 1.0f);

	// イージング量を求める。
	float easingAmount = 0.0f;
	if (isAppear_) {

		easingAmount = FEasing::EaseOutQuint(easingTimer_);

	}
	else {

		easingAmount = 1.0f - FEasing::EaseInSine(easingTimer_);

	}

	alpha_ = easingAmount;
	titleLogo_.SetColor(DirectX::XMFLOAT4(1, 1, 1, alpha_));

}

void SceneTransition::Draw()
{

	/*===== 描画処理 =====*/

	titleLogo_.Draw();

}
