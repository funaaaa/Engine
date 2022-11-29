#include "RayDenoiser.h"
#include "RaytracingOutput.h"
#include "RayComputeShader.h"
#include "DynamicConstBuffer.h"
#include "WindowsAPI.h"
#include "Engine.h"

void Denoiser::Setting()
{

	/*===== �R���X�g���N�^ =====*/

	// �u���[�o�͗p�N���X�𐶐��B
	blurXOutput_ = std::make_shared<RaytracingOutput>();
	blurYOutput_ = std::make_shared<RaytracingOutput>();

	// �u���[�o�̓e�X�g�p�N���X���Z�b�g�B
	blurXOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseBlurXOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	blurYOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseBlurYOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// �f�m�C�Y���ɔr�o����p�N���X���Z�b�g�B
	denoiseOutput_ = std::make_shared<RaytracingOutput>();
	denoiseOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseBlurBuffOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// �g�p����R���s���[�g�V�F�[�_�[�𐶐��B
	blurX_ = std::make_shared<RayComputeShader>();
	blurY_ = std::make_shared<RayComputeShader>();
	blurFinal_ = std::make_shared<RayComputeShader>();
	mixColorAndLuminance_ = std::make_shared<RayComputeShader>();

	// �K�E�V�A���u���[�Ɏg�p����R���s���[�g�V�F�[�_�[���Z�b�g�B
	blurX_->Setting(L"Resource/ShaderFiles/RayTracing/DenoiseBlurX.hlsl", 0, 1, 2, { 0 });
	blurY_->Setting(L"Resource/ShaderFiles/RayTracing/DenoiseBlurY.hlsl", 0, 1, 2, { blurXOutput_->GetUAVIndex() });
	blurFinal_->Setting(L"Resource/ShaderFiles/RayTracing/DenoiseFinal.hlsl", 0, 0, 1, { blurYOutput_->GetUAVIndex() });
	mixColorAndLuminance_->Setting(L"Resource/ShaderFiles/RayTracing/MixColorAndLuminance.hlsl", 0, 0, 5, { 0,0 });

	// �K�E�V�A���u���[�̏d�݃e�[�u�����v�Z�B
	CalcDenoiseWeightsTableFromGaussian(10);

	// �萔�o�b�t�@�𐶐��B
	denoiseWeightTableCBX_ = std::make_shared<DynamicConstBuffer>();
	denoiseWeightTableCBX_->Generate(sizeof(float) * DENOISE_GAUSSIAN_WEIGHTS_COUNT, L"GaussianWeightCBX");
	denoiseWeightTableCBY_ = std::make_shared<DynamicConstBuffer>();
	denoiseWeightTableCBY_->Generate(sizeof(float) * DENOISE_GAUSSIAN_WEIGHTS_COUNT, L"GaussianWeightCBY");

}

void Denoiser::ApplyDenoiseGaussianBlur(int InputUAVIndex, int DenoiseMaskIndex, int OutputUAVIndex, int BlurPower)
{

	/*===== �f�m�C�Y���� =====*/

	// �d�݃e�[�u�����v�Z�B
	CalcDenoiseWeightsTableFromGaussian(static_cast<float>(BlurPower));

	// �d�݃e�[�u�����������ށB
	denoiseWeightTableCBX_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), denoiseGaussianWeights_.data(), sizeof(float) * DENOISE_GAUSSIAN_WEIGHTS_COUNT);
	denoiseWeightTableCBY_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), denoiseGaussianWeights_.data(), sizeof(float) * DENOISE_GAUSSIAN_WEIGHTS_COUNT);

	// �R���s���[�g�V�F�[�_�[�����s�B
	blurX_->ChangeInputUAVIndex({ InputUAVIndex, DenoiseMaskIndex });
	blurY_->ChangeInputUAVIndex({ blurXOutput_->GetUAVIndex(), DenoiseMaskIndex });
	blurX_->Dispatch(static_cast<UINT>(WINDOW_WIDTH / 32) + 1, static_cast<UINT>(WINDOW_HEIGHT / 32) + 1, static_cast<UINT>(1), blurXOutput_->GetUAVIndex(), { denoiseWeightTableCBX_->GetBuffer(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() }, Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]);
	blurY_->Dispatch(static_cast<UINT>((WINDOW_WIDTH / 1.0f) / 32) + 1, static_cast<UINT>((WINDOW_HEIGHT / 1.0f) / 32) + 1, static_cast<UINT>(1), OutputUAVIndex, { denoiseWeightTableCBY_->GetBuffer(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() }, Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]);

}

void Denoiser::MixColorAndLuminance(int InputColorIndex, int InputLuminanceIndex, int InputLightLuminanceIndex, int InputGIIndex, int InputEmissiveIndex, int OutputUAVIndex)
{

	/*===== �F���Ɩ��邳���̏�Z =====*/

	// �R���s���[�g�V�F�[�_�[�����s�B
	mixColorAndLuminance_->ChangeInputUAVIndex({ InputColorIndex,InputLuminanceIndex, InputLightLuminanceIndex, InputGIIndex, InputEmissiveIndex });
	mixColorAndLuminance_->Dispatch(WINDOW_WIDTH / 32, WINDOW_HEIGHT / 32 + 1, 1, OutputUAVIndex, {}, Engine::Ins()->denoiseCmdList_[Engine::Ins()->currentQueueIndex_]);

}

void Denoiser::Denoise(int InImg, int OutImg, int DenoiseMaskIndex, int DenoisePower, int DenoiseCount)
{

	/*===== �f�m�C�Y =====*/

	// �f�m�C�Y���鐔��1�񂾂�����B
	if (DenoiseCount == 1) {

		// �K�E�V�A���u���[��������B
		ApplyDenoiseGaussianBlur(InImg, DenoiseMaskIndex, OutImg, DenoisePower);

	}
	// �f�m�C�Y���鐔��2�񂾂�����B
	else if (DenoiseCount == 2) {

		// �K�E�V�A���u���[��������B
		ApplyDenoiseGaussianBlur(InImg, DenoiseMaskIndex, denoiseOutput_->GetUAVIndex(), DenoisePower);
		ApplyDenoiseGaussianBlur(denoiseOutput_->GetUAVIndex(), DenoiseMaskIndex, OutImg, DenoisePower);

	}
	else {

		for (int index = 0; index < DenoiseCount; ++index) {

			// �f�m�C�Y���ŏ��̈�񂾂�����B
			if (index == 0) {

				ApplyDenoiseGaussianBlur(InImg, DenoiseMaskIndex, denoiseOutput_->GetUAVIndex(), DenoisePower);

			}
			// �f�m�C�Y�̍ŏI�i�K��������B
			else if (index == DenoiseCount - 1) {

				ApplyDenoiseGaussianBlur(denoiseOutput_->GetUAVIndex(), DenoiseMaskIndex, OutImg, DenoisePower);

			}
			else {

				ApplyDenoiseGaussianBlur(denoiseOutput_->GetUAVIndex(), DenoiseMaskIndex, denoiseOutput_->GetUAVIndex(), DenoisePower);

			}

		}

	}


}

void Denoiser::CalcDenoiseWeightsTableFromGaussian(float Power)
{

	/*===== �K�E�V�A���u���[�̏d�݂��v�Z =====*/

	// �d�݂̍��v���L�^����ϐ����`����B
	float total = 0;

	// ��������K�E�X�֐���p���ďd�݂��v�Z���Ă���B
	// ���[�v�ϐ���x����e�N�Z������̋����B
	for (int x = 0; x < DENOISE_GAUSSIAN_WEIGHTS_COUNT; x++)
	{
		denoiseGaussianWeights_[x] = expf(-0.5f * static_cast<float>(x * x) / Power);
		total += 2.0f * denoiseGaussianWeights_.at(x);
	}

	// �d�݂̍��v�ŏ��Z���邱�ƂŁA�d�݂̍��v��1�ɂ��Ă���B
	for (int i = 0; i < DENOISE_GAUSSIAN_WEIGHTS_COUNT; i++)
	{
		denoiseGaussianWeights_[i] /= total;
	}

}
