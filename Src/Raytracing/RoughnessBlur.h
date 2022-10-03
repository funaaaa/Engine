#pragma once
#include "Singleton.h"
#include <memory>
#include <array>

class RaytracingOutput;
class RayComputeShader;
class DynamicConstBuffer;

class RoughnessBlur : public Singleton<RoughnessBlur> {

private:

	/*===== �����o�ϐ� =====*/

	// �u���[�o�͗p
	std::shared_ptr<RaytracingOutput> blurXOutput_;
	std::shared_ptr<RaytracingOutput> blurYOutput_;

	// �u���[��������R���s���[�g�V�F�[�_�[
	std::shared_ptr<RayComputeShader> blurX_;
	std::shared_ptr<RayComputeShader> blurY_;

	// �f�m�C�Y���Ɏg�p����ꎞ�ۑ��v�e�N�X�`��
	std::shared_ptr<RaytracingOutput> denoiseOutput_;

	// �d�݃e�[�u��
	static const int GAUSSIAN_WEIGHTS_COUNT = 8;
	std::array<float, GAUSSIAN_WEIGHTS_COUNT> gaussianWeights_;

	// �d�ݒ萔�o�b�t�@
	std::shared_ptr<DynamicConstBuffer> weightTableCBX_;
	std::shared_ptr<DynamicConstBuffer> weightTableCBY_;


public:

	/*===== �����o�֐� =====*/

	// �Z�b�e�B���O�����B
	void Setting();

	// �K�E�V�A���u���[��������B
	void ApplyGaussianBlur(const int& InputReflectionIndex, const int& InputRoughnessIndex, const int& DenoiseMaskIndex, const int& OutputUAVIndex, const int& BlurPower);

	// �f�m�C�Y
	void Denoise(const int& InputReflectionIndex, const int& InputRoughnessIndex, const int& DenoiseMaskIndex, const int& OutputUAVIndex, const int& DenoisePower, const int& DenoiseCount);

private:

	// �K�E�V�A���u���[�̏d�݂��v�Z����B
	void CalcWeightsTableFromGaussian(float Power);

};
