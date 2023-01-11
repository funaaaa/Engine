#include "RadialBlur.h"
#include "RaytracingOutput.h"
#include "RayComputeShader.h"
#include "DynamicConstBuffer.h"
#include "WindowsAPI.h"
#include "Engine.h"

void RadialBlur::Setting()
{

	/*===== �������� =====*/

	// �g�p����R���s���[�g�V�F�[�_�[�𐶐��B
	blur_ = std::make_shared<RayComputeShader>();

	// �K�E�V�A���u���[�Ɏg�p����R���s���[�g�V�F�[�_�[���Z�b�g�B
	blur_->Setting(L"Resource/ShaderFiles/RadialBlur.hlsl", 0, 1, 1, {}, { 0 });

	// �萔�o�b�t�@�𐶐��B
	weight_ = std::make_shared<DynamicConstBuffer>();
	weight_->Generate(sizeof(float) * WEIGHTS_COUNT, L"RadialBlurWeight");

}

void RadialBlur::Blur(int InImg, int OutImg)
{

	/*===== �u���[�������� =====*/

	ApplyRadialBlur(InImg, OutImg);
	ApplyRadialBlur(OutImg, InImg);
	ApplyRadialBlur(InImg, OutImg);

}

void RadialBlur::ApplyRadialBlur(int InImg, int OutImg)
{

	/*===== �u���[�����s =====*/

	// �d�݃e�[�u�����������ށB
	blurWeight_[0] = blurPower_;
	weight_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), blurWeight_.data(), sizeof(float) * WEIGHTS_COUNT);

	// �R���s���[�g�V�F�[�_�[�����s�B
	blur_->ChangeInputUAVIndex({ InImg });
	blur_->Dispatch(static_cast<UINT>(WINDOW_WIDTH / 32) + 1, static_cast<UINT>(WINDOW_HEIGHT / 32) + 1, static_cast<UINT>(1), OutImg, { weight_->GetBuffer(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() }, Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]);
	
}
