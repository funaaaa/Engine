#include "RayDenoiser.h"
#include "RaytracingOutput.h"
#include "RayComputeShader.h"
#include "DynamicConstBuffer.h"
#include "DirectXBase.h"
#include "WindowsAPI.h"

void Denoiser::Setting()
{

	/*===== �R���X�g���N�^ =====*/

	// �u���[�o�͗p�N���X�𐶐��B
	blurXOutput = std::make_shared<RaytracingOutput>();
	blurYOutput = std::make_shared<RaytracingOutput>();

	// �u���[�o�̓e�X�g�p�N���X���Z�b�g�B
	blurXOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseBlurXOutput");
	blurYOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseBlurYOutput");

	// �f�m�C�Y���ɔr�o����p�N���X���Z�b�g�B
	denoiseOutput = std::make_shared<RaytracingOutput>();
	denoiseOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"DenoiseBlurBuffOutput");

	// �g�p����R���s���[�g�V�F�[�_�[�𐶐��B
	blurX = std::make_shared<RayComputeShader>();
	blurY = std::make_shared<RayComputeShader>();
	blurFinal = std::make_shared<RayComputeShader>();
	mixColorAndLuminance = std::make_shared<RayComputeShader>();

	// �K�E�V�A���u���[�Ɏg�p����R���s���[�g�V�F�[�_�[���Z�b�g�B
	blurX->Setting(L"Resource/ShaderFiles/RayTracing/DenoiseBlurX.hlsl", 0, 1, 2, { 0 });
	blurY->Setting(L"Resource/ShaderFiles/RayTracing/DenoiseBlurY.hlsl", 0, 1, 2, { blurXOutput->GetUAVIndex() });
	blurFinal->Setting(L"Resource/ShaderFiles/RayTracing/DenoiseFinal.hlsl", 0, 0, 1, { blurYOutput->GetUAVIndex() });
	mixColorAndLuminance->Setting(L"Resource/ShaderFiles/RayTracing/MixColorAndLuminance.hlsl", 0, 0, 4, { 0,0 });

	// �K�E�V�A���u���[�̏d�݃e�[�u�����v�Z�B
	CalcWeightsTableFromGaussian(10);

	// �萔�o�b�t�@�𐶐��B
	weightTableCBX = std::make_shared<DynamicConstBuffer>();
	weightTableCBX->Generate(sizeof(float) * GAUSSIAN_WEIGHTS_COUNT, L"GaussianWeightCBX");
	weightTableCBY = std::make_shared<DynamicConstBuffer>();
	weightTableCBY->Generate(sizeof(float) * GAUSSIAN_WEIGHTS_COUNT, L"GaussianWeightCBY");

}

void Denoiser::ApplyGaussianBlur(const int& InputUAVIndex, const int& DenoiseMaskIndex, const int& OutputUAVIndex, const int& BlurPower)
{

	/*===== �f�m�C�Y���� =====*/

	// �d�݃e�[�u�����v�Z�B
	CalcWeightsTableFromGaussian(static_cast<float>(BlurPower));

	// �d�݃e�[�u�����������ށB
	weightTableCBX->Write(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex(), gaussianWeights.data(), sizeof(float) * GAUSSIAN_WEIGHTS_COUNT);
	weightTableCBY->Write(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex(), gaussianWeights.data(), sizeof(float) * GAUSSIAN_WEIGHTS_COUNT);

	// �o�͗pUAV�̏�Ԃ�ς���B
	blurXOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	blurYOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	denoiseOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// �R���s���[�g�V�F�[�_�[�����s�B
	blurX->ChangeInputUAVIndex({ InputUAVIndex, DenoiseMaskIndex });
	blurY->ChangeInputUAVIndex({ blurXOutput->GetUAVIndex(), DenoiseMaskIndex });
	blurX->Dispatch(static_cast<UINT>(window_width / 32) + 1, static_cast<UINT>(window_height / 32) + 1, static_cast<UINT>(1), blurXOutput->GetUAVIndex(), { weightTableCBX->GetBuffer(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() });
	blurY->Dispatch(static_cast<UINT>((window_width / 1.0f) / 32) + 1, static_cast<UINT>((window_height / 1.0f) / 32) + 1, static_cast<UINT>(1), OutputUAVIndex, { weightTableCBY->GetBuffer(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() });

	// �o�͗pUAV�̏�Ԃ�ς���B
	blurXOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	blurYOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	denoiseOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

}

void Denoiser::MixColorAndLuminance(const int& InputColorIndex, const int& InputLuminanceIndex, const int& InputLightLuminanceIndex, const int& InputGIIndex, const int& OutputUAVIndex)
{

	/*===== �F���Ɩ��邳���̏�Z =====*/

	// �R���s���[�g�V�F�[�_�[�����s�B
	mixColorAndLuminance->ChangeInputUAVIndex({ InputColorIndex,InputLuminanceIndex, InputLightLuminanceIndex, InputGIIndex });
	mixColorAndLuminance->Dispatch(window_width / 32, window_height / 32, 1, OutputUAVIndex, {});

}

void Denoiser::Denoise(const int& InImg, const int& OutImg, const int& DenoiseMaskIndex, const int& DenoisePower, const int& DenoiseCount)
{

	/*===== �f�m�C�Y =====*/

	// �f�m�C�Y���鐔��1�񂾂�����B
	if (DenoiseCount == 1) {

		// �K�E�V�A���u���[��������B
		ApplyGaussianBlur(InImg, DenoiseMaskIndex, OutImg, DenoisePower);

	}
	// �f�m�C�Y���鐔��2�񂾂�����B
	else if (DenoiseCount == 2) {

		// �K�E�V�A���u���[��������B
		ApplyGaussianBlur(InImg, DenoiseMaskIndex, denoiseOutput->GetUAVIndex(), DenoisePower);
		ApplyGaussianBlur(denoiseOutput->GetUAVIndex(), DenoiseMaskIndex, OutImg, DenoisePower);

	}
	else {

		for (int index = 0; index < DenoiseCount; ++index) {

			// �f�m�C�Y���ŏ��̈�񂾂�����B
			if (index == 0) {

				ApplyGaussianBlur(InImg, DenoiseMaskIndex, denoiseOutput->GetUAVIndex(), DenoisePower);

			}
			// �f�m�C�Y�̍ŏI�i�K��������B
			else if (index == DenoiseCount - 1) {

				ApplyGaussianBlur(denoiseOutput->GetUAVIndex(), DenoiseMaskIndex, OutImg, DenoisePower);

			}
			else {

				ApplyGaussianBlur(denoiseOutput->GetUAVIndex(), DenoiseMaskIndex, denoiseOutput->GetUAVIndex(), DenoisePower);

			}

		}

	}


}

void Denoiser::CalcWeightsTableFromGaussian(float Power)
{

	/*===== �K�E�V�A���u���[�̏d�݂��v�Z =====*/

	// �d�݂̍��v���L�^����ϐ����`����B
	float total = 0;

	// ��������K�E�X�֐���p���ďd�݂��v�Z���Ă���B
	// ���[�v�ϐ���x����e�N�Z������̋����B
	for (int x_ = 0; x_ < GAUSSIAN_WEIGHTS_COUNT; x_++)
	{
		gaussianWeights[x_] = expf(-0.5f * static_cast<float>(x_ * x_) / Power);
		total += 2.0f * gaussianWeights.at(x_);
	}

	// �d�݂̍��v�ŏ��Z���邱�ƂŁA�d�݂̍��v��1�ɂ��Ă���B
	for (int i = 0; i < GAUSSIAN_WEIGHTS_COUNT; i++)
	{
		gaussianWeights[i] /= total;
	}

}
