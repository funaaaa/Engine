#include "SceneTransition.h"
#include "FHelper.h"
#include "PipelineManager.h"

void SceneTransition::Setting()
{

	/*===== �Z�b�e�B���O =====*/

	titleLogo_.GenerateForTexture(FHelper::WindowCenterPos(), FHelper::WindowHalfSize(), Pipeline::PROJECTIONID::UI, Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, L"Resource/Title/titleLogo.png");
	alpha_ = 0.0f;
	isAppear_ = false;
	easingTimer_ = 1.0f;

}

void SceneTransition::Appear()
{

	/*===== �J�n���� =====*/

	alpha_ = 0.0f;
	isAppear_ = true;
	easingTimer_ = 0.0f;

}

void SceneTransition::Exit()
{

	/*===== �I������ =====*/

	alpha_ = 1.0f;
	isAppear_ = false;
	easingTimer_ = 0.0f;

}

void SceneTransition::Update()
{

	/*===== �X�V���� =====*/

	easingTimer_ = FHelper::Clamp(easingTimer_ + ADD_EASING_TIMER, 0.0f, 1.0f);

	// �C�[�W���O�ʂ����߂�B
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

	/*===== �`�揈�� =====*/

	titleLogo_.Draw();

}
