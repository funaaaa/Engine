#pragma once
#include "Singleton.h"
#include <memory>
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
	void ApplyDenoise(const int& InputUAVIndex, const int& OutputUAVIndex, const int& BlurPower);

private:

	// �K�E�V�A���u���[�̏d�݂��v�Z����B
	void CalcWeightsTableFromGaussian(float Power);

};