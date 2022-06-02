#pragma once
#include "Singleton.h"
#include "RaytracingOutput.h"
#include "RayComputeShader.h"

class Denoiser : public Singleton<Denoiser>{

private:

	/*===== �����o�ϐ� =====*/

	// �u���[�o�͗p
	RaytracingOutput blurXOutput;
	RaytracingOutput blurYOutput;

	// �u���[��������R���s���[�g�V�F�[�_�[
	RayComputeShader blurX;
	RayComputeShader blurY;
	RayComputeShader blurFinal;


public:

	/*===== �����o�ւ� =====*/

	// �Z�b�e�B���O�����B
	void Setting();

	// �K�E�V�A���u���[��������B
	void ApplyDenoise(const int& InputUAVIndex, const int& OutputUAVIndex);

};