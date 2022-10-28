#pragma once
#include "Singleton.h"
#include "Engine.h"
#include <memory>
#include <array>

class RaytracingOutput;
class RayComputeShader;
class DynamicConstBuffer;

class RadialBlur : public Singleton<RadialBlur> {

private:

	/*===== �����o�ϐ� =====*/

	// �u���[��������R���s���[�g�V�F�[�_�[
	std::shared_ptr<RayComputeShader> blur_;

	// �e�e�N�X�`���ƐF�e�N�X�`�����������킹��V�F�[�_�[
	std::shared_ptr<RayComputeShader> mixColorAndLuminance_;

	// �d�݃e�[�u��
	static const int WEIGHTS_COUNT = 4;
	std::array<float, WEIGHTS_COUNT> blurWeight_;

	// �d�ݒ萔�o�b�t�@
	std::shared_ptr<DynamicConstBuffer> weight_;

	// �u���[�̋���
	float blurPower_;


public:

	/*===== �����o�֐� =====*/

	// �Z�b�e�B���O�����B
	void Setting();

	// �f�m�C�Y
	void Blur(int InImg, int OutImg);

	inline void SetBlurPower(float BlurPower) { blurPower_ = BlurPower; }

private:

	// ���W�A���u���[��������B
	void ApplyRadialBlur(int InImg, int OutImg);

};