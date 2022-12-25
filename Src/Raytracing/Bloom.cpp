#include "Bloom.h"
#include "RaytracingOutput.h"
#include "RayComputeShader.h"
#include "DynamicConstBuffer.h"
#include "WindowsAPI.h"
#include "Engine.h"

void Bloom::Setting()
{

	/*===== �O�������� =====*/

	// �u���[�����ԃe�N�X�`���𐶐��B
	for (auto& index : emissiveIntermediateTexture_) {

		index = std::make_shared<RaytracingOutput>();
		index->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"EmissiveIntermediateTexture", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	}

	// �u���[�o�͗p�N���X�𐶐��B
	blurXOutput_ = std::make_shared<RaytracingOutput>();
	blurYOutput_ = std::make_shared<RaytracingOutput>();

	// �u���[�o�̓e�X�g�p�N���X���Z�b�g�B
	blurXOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"BloomBlurXOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	blurYOutput_->Setting(DXGI_FORMAT_R8G8B8A8_UNORM, L"BloomBlurYOutput", Vec2(1280, 720), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// �g�p����R���s���[�g�V�F�[�_�[�𐶐��B
	blurX_ = std::make_shared<RayComputeShader>();
	blurY_ = std::make_shared<RayComputeShader>();
	blurFinal_ = std::make_shared<RayComputeShader>();

	// �K�E�V�A���u���[�Ɏg�p����R���s���[�g�V�F�[�_�[���Z�b�g�B
	blurX_->Setting(L"Resource/ShaderFiles/RayTracing/BloomBlurX.hlsl", 0, 1, 1, { 0 });
	blurY_->Setting(L"Resource/ShaderFiles/RayTracing/BloomBlurY.hlsl", 0, 1, 1, { blurXOutput_->GetUAVIndex() });
	blurFinal_->Setting(L"Resource/ShaderFiles/RayTracing/BloomBlurFinal.hlsl", 0, 0, 1, { blurYOutput_->GetUAVIndex() });

	// �G�~�b�V�u����������R���s���[�g�V�F�[�_�[�𐶐��B
	mixEmissive_ = std::make_shared<RayComputeShader>();
	mixEmissive_->Setting(L"Resource/ShaderFiles/RayTracing/BloomMix.hlsl", 0, 0, 4, { 0 });

	// �K�E�V�A���u���[�̏d�݃e�[�u�����v�Z�B
	CalcBloomWeightsTableFromGaussian(10);

	// �萔�o�b�t�@�𐶐��B
	bloomWeightTableCBX_ = std::make_shared<DynamicConstBuffer>();
	bloomWeightTableCBX_->Generate(sizeof(float) * BLOOM_GAUSSIAN_WEIGHTS_COUNT, L"BloomGaussianWeightCBX");
	bloomWeightTableCBY_ = std::make_shared<DynamicConstBuffer>();
	bloomWeightTableCBY_->Generate(sizeof(float) * BLOOM_GAUSSIAN_WEIGHTS_COUNT, L"BloomGaussianWeightCBY");

}

void Bloom::ApplyBloom(int InOutImg)
{

	/*===== �u���[���������� =====*/

	for (int index = 0; index < EMISSIVE_COUNT; ++index) {

		// �ŏ��̈��ڂ�������
		if (index == 0) {
			ApplyBloomGaussianBlur(InOutImg, InOutImg, emissiveIntermediateTexture_[index]->GetUAVIndex());
		}
		else {
			ApplyBloomGaussianBlur(emissiveIntermediateTexture_[index - 1]->GetUAVIndex(), InOutImg, emissiveIntermediateTexture_[index]->GetUAVIndex());
		}

	}

	std::vector<int> inputUAV;
	for (auto& index : emissiveIntermediateTexture_) inputUAV.emplace_back(index->GetUAVIndex());
	mixEmissive_->ChangeInputUAVIndex(inputUAV);
	mixEmissive_->Dispatch(static_cast<UINT>(WINDOW_WIDTH / 32) + 1, static_cast<UINT>(WINDOW_HEIGHT / 32) + 1, static_cast<UINT>(1), InOutImg, { }, Engine::Ins()->mainGraphicsCmdList_);

}

void Bloom::ApplyBloomGaussianBlur(int InputUAVIndex, int SourceUAVIndex, int OutputUAVIndex)
{

	/*===== �u���[���p�̃K�E�V�A���u���[�����s =====*/

	// �d�݃e�[�u�����v�Z�B
	CalcBloomWeightsTableFromGaussian(static_cast<float>(BLUR_POWER));

	// �d�݃e�[�u�����������ށB
	bloomWeightTableCBX_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), bloomGaussianWeights_.data(), sizeof(float) * BLOOM_GAUSSIAN_WEIGHTS_COUNT);
	bloomWeightTableCBY_->Write(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex(), bloomGaussianWeights_.data(), sizeof(float) * BLOOM_GAUSSIAN_WEIGHTS_COUNT);

	// �R���s���[�g�V�F�[�_�[�����s�B
	blurX_->ChangeInputUAVIndex({ InputUAVIndex });
	blurY_->ChangeInputUAVIndex({ blurXOutput_->GetUAVIndex() });
	blurFinal_->ChangeInputUAVIndex({ blurYOutput_->GetUAVIndex() });
	blurX_->Dispatch(static_cast<UINT>((WINDOW_WIDTH / 2.0f) / 32) + 1, static_cast<UINT>(WINDOW_HEIGHT / 32) + 1, static_cast<UINT>(1), blurXOutput_->GetUAVIndex(), { bloomWeightTableCBX_->GetBuffer(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() }, Engine::Ins()->mainGraphicsCmdList_);
	blurY_->Dispatch(static_cast<UINT>((WINDOW_WIDTH / 2.0f) / 32) + 1, static_cast<UINT>((WINDOW_HEIGHT / 2.0f) / 32) + 1, static_cast<UINT>(1), blurYOutput_->GetUAVIndex(), { bloomWeightTableCBY_->GetBuffer(Engine::Ins()->swapchain_.swapchain_->GetCurrentBackBufferIndex())->GetGPUVirtualAddress() }, Engine::Ins()->mainGraphicsCmdList_);
	blurFinal_->Dispatch(static_cast<UINT>(WINDOW_WIDTH / 32) + 1, static_cast<UINT>(WINDOW_HEIGHT / 32) + 1, static_cast<UINT>(1), OutputUAVIndex, {}, Engine::Ins()->mainGraphicsCmdList_);

}

void Bloom::CalcBloomWeightsTableFromGaussian(float Power)
{

	/*===== �d�݂̃e�[�u�����v�Z =====*/

	// �d�݂̍��v���L�^����ϐ����`����B
	float total = 0;

	// ��������K�E�X�֐���p���ďd�݂��v�Z���Ă���B
	// ���[�v�ϐ���x����e�N�Z������̋����B
	for (int x = 0; x < BLOOM_GAUSSIAN_WEIGHTS_COUNT; ++x)
	{
		bloomGaussianWeights_[x] = expf(-0.5f * static_cast<float>(x * x) / Power);
		total += 2.0f * bloomGaussianWeights_.at(x);
	}

	// �d�݂̍��v�ŏ��Z���邱�ƂŁA�d�݂̍��v��1�ɂ��Ă���B
	for (int i = 0; i < BLOOM_GAUSSIAN_WEIGHTS_COUNT; ++i)
	{
		bloomGaussianWeights_[i] /= total;
	}

}
