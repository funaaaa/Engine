#include "RayDenoiser.h"
#include "RaytracingOutput.h"
#include "RayComputeShader.h"
#include "DynamicConstBuffer.h"
#include "DirectXBase.h"

void Denoiser::Setting()
{

	/*===== �R���X�g���N�^ =====*/

	// �u���[�o�͗p�N���X�𐶐��B
	blurXOutput = std::make_shared<RaytracingOutput>();
	blurYOutput = std::make_shared<RaytracingOutput>();

	// �u���[�o�̓e�X�g�p�N���X���Z�b�g�B
	blurXOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);
	blurYOutput->Setting(DXGI_FORMAT_R8G8B8A8_UNORM);

	// �g�p����R���s���[�g�V�F�[�_�[�𐶐��B
	blurX = std::make_shared<RayComputeShader>();
	blurY = std::make_shared<RayComputeShader>();
	blurFinal = std::make_shared<RayComputeShader>();
	mixColorAndLuminance = std::make_shared<RayComputeShader>();

	// �K�E�V�A���u���[�Ɏg�p����R���s���[�g�V�F�[�_�[���Z�b�g�B
	blurX->Setting(L"Resource/ShaderFiles/RayTracing/DenoiseBlurX.hlsl", 0, 1, 1, { 0 });
	blurY->Setting(L"Resource/ShaderFiles/RayTracing/DenoiseBlurY.hlsl", 0, 1, 1, { blurXOutput->GetUAVIndex() });
	blurFinal->Setting(L"Resource/ShaderFiles/RayTracing/DenoiseFinal.hlsl", 0, 0, 1, { blurYOutput->GetUAVIndex() });
	mixColorAndLuminance->Setting(L"Resource/ShaderFiles/RayTracing/MixColorAndLuminance.hlsl", 0, 0, 2, { 0,0 });

	// �K�E�V�A���u���[�̏d�݃e�[�u�����v�Z�B
	CalcWeightsTableFromGaussian(10);

	// �萔�o�b�t�@�𐶐��B
	weightTableCBX = std::make_shared<DynamicConstBuffer>();
	weightTableCBX->Generate(sizeof(float) * GAUSSIAN_WEIGHTS_COUNT, L"GaussianWeightCBX");
	weightTableCBY = std::make_shared<DynamicConstBuffer>();
	weightTableCBY->Generate(sizeof(float) * GAUSSIAN_WEIGHTS_COUNT, L"GaussianWeightCBY");

}

void Denoiser::ApplyGaussianBlur(const int& InputUAVIndex, const int& OutputUAVIndex, const int& BlurPower)
{

	/*===== �f�m�C�Y���� =====*/

	// �d�݃e�[�u�����v�Z�B
	CalcWeightsTableFromGaussian(BlurPower);

	// �d�݃e�[�u�����������ށB
	weightTableCBX->Write(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex(), gaussianWeights.data(), sizeof(float) * GAUSSIAN_WEIGHTS_COUNT);
	weightTableCBY->Write(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex(), gaussianWeights.data(), sizeof(float) * GAUSSIAN_WEIGHTS_COUNT);

	// �o�͗pUAV�̏�Ԃ�ς���B
	blurXOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	blurYOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// �R���s���[�g�V�F�[�_�[�����s�B
	/*blurX->ChangeInputUAVIndex({ InputUAVIndex });
	blurX->Dispatch((window_width / 2.0f) / 4, window_height / 4, 1, blurXOutput->GetUAVIndex(), { weightTableCBX->GetBuffer(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() });
	blurY->Dispatch((window_width / 2.0f) / 4, (window_height / 2.0f) / 4, 1, blurYOutput->GetUAVIndex(), { weightTableCBY->GetBuffer(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() });
	blurFinal->Dispatch(window_width / 4, window_height / 4, 1, OutputUAVIndex, {});*/
	blurX->ChangeInputUAVIndex({ InputUAVIndex });
	blurX->Dispatch((window_width / 1.0f) / 4, window_height / 4, 1, blurXOutput->GetUAVIndex(), { weightTableCBX->GetBuffer(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() });
	blurY->Dispatch((window_width / 1.0f) / 4, (window_height /1.0f) / 4, 1, blurYOutput->GetUAVIndex(), { weightTableCBY->GetBuffer(DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() });
	blurFinal->Dispatch(window_width / 4, window_height / 4, 1, OutputUAVIndex, {});

	// �o�͗pUAV�̏�Ԃ�ς���B
	blurXOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	blurYOutput->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

}

void Denoiser::MixColorAndLuminance(const int& InputColorIndex, const int& InputLuminanceIndex, const int& OutputUAVIndex)
{

	/*===== �F���Ɩ��邳���̏�Z =====*/

	// �R���s���[�g�V�F�[�_�[�����s�B
	mixColorAndLuminance->ChangeInputUAVIndex({InputColorIndex,InputLuminanceIndex});
	mixColorAndLuminance->Dispatch(window_width / 4, window_height / 4, 1, OutputUAVIndex, {});

}

void Denoiser::CalcWeightsTableFromGaussian(float Pwer)
{

	/*===== �K�E�V�A���u���[�̏d�݂��v�Z =====*/

	// �d�݂̍��v���L�^����ϐ����`����B
	float total = 0;

	// ��������K�E�X�֐���p���ďd�݂��v�Z���Ă���B
	// ���[�v�ϐ���x����e�N�Z������̋����B
	for (int x = 0; x < GAUSSIAN_WEIGHTS_COUNT; x++)
	{
		gaussianWeights[x] = expf(-0.5f * (float)(x * x) / Pwer);
		total += 2.0f * gaussianWeights.at(x);
	}

	// �d�݂̍��v�ŏ��Z���邱�ƂŁA�d�݂̍��v��1�ɂ��Ă���B
	for (int i = 0; i < GAUSSIAN_WEIGHTS_COUNT; i++)
	{
		gaussianWeights[i] /= total;
	}

}
