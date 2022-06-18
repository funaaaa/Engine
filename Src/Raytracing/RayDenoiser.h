#pragma once
#include "Singleton.h"
#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)
#include <array>

class RaytracingOutput;
class RayComputeShader;
class DynamicConstBuffer;

class Denoiser : public Singleton<Denoiser> {

private:

	/*===== �����o�ϐ� =====*/

	// �u���[�o�͗p
	std::shared_ptr<RaytracingOutput> blurXOutput;
	std::shared_ptr<RaytracingOutput> blurYOutput;

	// �u���[��������R���s���[�g�V�F�[�_�[
	std::shared_ptr<RayComputeShader> blurX;
	std::shared_ptr<RayComputeShader> blurY;
	std::shared_ptr<RayComputeShader> blurFinal;
	
	// �f�m�C�Y���Ɏg�p����ꎞ�ۑ��v�e�N�X�`��
	std::shared_ptr<RaytracingOutput> denoiseOutput;

	// �e�e�N�X�`���ƐF�e�N�X�`�����������킹��V�F�[�_�[
	std::shared_ptr<RayComputeShader> mixColorAndLuminance;

	// �d�݃e�[�u��
	static const int GAUSSIAN_WEIGHTS_COUNT = 8;
	std::array<float, GAUSSIAN_WEIGHTS_COUNT> gaussianWeights;

	// �d�ݒ萔�o�b�t�@
	std::shared_ptr<DynamicConstBuffer> weightTableCBX;
	std::shared_ptr<DynamicConstBuffer> weightTableCBY;


public:

	/*===== �����o�֐� =====*/

	// �Z�b�e�B���O�����B
	void Setting();

	// �K�E�V�A���u���[��������B
	void ApplyGaussianBlur(const int& InputUAVIndex, const int& OutputUAVIndex, const int& BlurPower);

	// �F���Ɩ��邳����������B
	void MixColorAndLuminance(const int& InputColorIndex, const int& InputLuminanceIndex, const int& InputLightLuminanceIndex, const int& InputGIIndex, const int& OutputUAVIndex);

	// �f�m�C�Y
	void Denoise(const int& InOutImg, const int& DenoisePower, const int& DenoiseCount);

private:

	// �K�E�V�A���u���[�̏d�݂��v�Z����B
	void CalcWeightsTableFromGaussian(float Power);

};