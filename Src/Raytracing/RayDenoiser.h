#pragma once
#include "Singleton.h"
#include "Engine.h"
#include <memory>
#include <array>

class RaytracingOutput;
class RayComputeShader;
class DynamicConstBuffer;

class Denoiser : public Singleton<Denoiser> {

private:

	/*===== �����o�ϐ� =====*/

	// �u���[�o�͗p
	std::shared_ptr<RaytracingOutput> blurXOutput_;
	std::shared_ptr<RaytracingOutput> blurYOutput_;

	// �u���[��������R���s���[�g�V�F�[�_�[
	std::shared_ptr<RayComputeShader> blurX_;
	std::shared_ptr<RayComputeShader> blurY_;
	std::shared_ptr<RayComputeShader> blurFinal_;

	// �f�m�C�Y���Ɏg�p����ꎞ�ۑ��v�e�N�X�`��
	std::shared_ptr<RaytracingOutput> denoiseOutput_;

	// �e�e�N�X�`���ƐF�e�N�X�`�����������킹��V�F�[�_�[
	std::shared_ptr<RayComputeShader> mixColorAndLuminance_;

	// �d�݃e�[�u��
	static const int DENOISE_GAUSSIAN_WEIGHTS_COUNT = 4;
	std::array<float, DENOISE_GAUSSIAN_WEIGHTS_COUNT> denoiseGaussianWeights_;

	// �d�ݒ萔�o�b�t�@
	std::shared_ptr<DynamicConstBuffer> denoiseWeightTableCBX_;
	std::shared_ptr<DynamicConstBuffer> denoiseWeightTableCBY_;

public:

	/*===== �����o�֐� =====*/

	// �Z�b�e�B���O�����B
	void Setting();

	// �K�E�V�A���u���[��������B
	void ApplyDenoiseGaussianBlur(int InputUAVIndex, int DenoiseMaskIndex, int OutputUAVIndex, int BlurPower);

	// �F���Ɩ��邳����������B
	void MixColorAndLuminance(int InputColorIndex, int InputLuminanceIndex, int InputLightLuminanceIndex, int InputEmissiveIndex, int OutputUAVIndex);

	// �f�m�C�Y
	void Denoise(int InImg, int OutImg, int DenoiseMaskIndex, int DenoisePower, int DenoiseCount);

private:

	// �K�E�V�A���u���[�̏d�݂��v�Z����B
	void CalcDenoiseWeightsTableFromGaussian(float Power);

};