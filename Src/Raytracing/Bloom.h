#pragma once
#include "Singleton.h"
#include "Engine.h"
#include <memory>
#include <array>

class RaytracingOutput;
class RayComputeShader;
class DynamicConstBuffer;

// �u���[���N���X �P�x�̃e�N�X�`����n�����ƂŃu���[���������ĕԂ��B
class Bloom : public Singleton<Bloom> {

private:

	/*===== �����o�ϐ� =====*/

	// �u���[��������R���s���[�g�V�F�[�_�[
	std::shared_ptr<RayComputeShader> blurX_;
	std::shared_ptr<RayComputeShader> blurY_;
	std::shared_ptr<RayComputeShader> blurFinal_;

	// �u���[�o�͗p
	std::shared_ptr<RaytracingOutput> blurXOutput_;
	std::shared_ptr<RaytracingOutput> blurYOutput_;

	// �G�~�b�V�u����������B
	std::shared_ptr<RayComputeShader> mixEmissive_;

	// �u���[���p���ԃe�N�X�`��
	static const int BLOOM_GAUSSIAN_WEIGHTS_COUNT = 4;
	std::array<std::shared_ptr<RaytracingOutput>, BLOOM_GAUSSIAN_WEIGHTS_COUNT> emissiveIntermediateTexture_;

	// �d�݃e�[�u��
	std::array<float, BLOOM_GAUSSIAN_WEIGHTS_COUNT> bloomGaussianWeights_;

	// �d�ݒ萔�o�b�t�@
	std::shared_ptr<DynamicConstBuffer> bloomWeightTableCBX_;
	std::shared_ptr<DynamicConstBuffer> bloomWeightTableCBY_;

	// �K�E�V�A���u���[�̋���
	const float BLUR_POWER = 100.0f;


public:

	/*===== �����o�֐� =====*/

	// �O����
	void Setting();

	// �u���[����������B
	void ApplyBloom(int InOutImg);

private:

	// �K�E�V�A���u���[��������B
	void ApplyBloomGaussianBlur(int InputUAVIndex, int OutputUAVIndex);

	// �K�E�V�A���u���[�̏d�݂��v�Z����B
	void CalcBloomWeightsTableFromGaussian(float Power);


};