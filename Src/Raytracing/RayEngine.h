#pragma once
#include "Singleton.h"
#include "ConstBuffers.h"
#include <memory>
#include <array>

#include "ConstBuffers.h"
#include "DynamicConstBuffer.h"
#include "RaytracingPipeline.h"
#include "RayRootsignature.h"

#include "VolumeTexture.h"

class TLAS;
class RaytracingOutput;
class RaytracingPipeline;
class RayComputeShader;
class DynamicConstBuffer;

// ���C�g���[�V���O�p�̏������܂Ƃ߂��N���X�BEngine�ɕt������`�Ŏg�p����B
class RayEngine : public Singleton<RayEngine> {

public:

	/*===== �����o�ϐ� =====*/

	// TLAS�𐶐��B
	std::shared_ptr<TLAS> tlas_;

	// �F�o�͗p�N���X�B
	std::array<std::shared_ptr<RaytracingOutput>, 2> colorOutput_;

	// ���C�g�o�͗p�B
	std::array<std::shared_ptr<RaytracingOutput>, 2> lightOutput_;
	std::array<std::shared_ptr<RaytracingOutput>, 2> denoiseLightOutput_;

	// �f�m�C�Y�}�X�N�p�B
	std::array<std::shared_ptr<RaytracingOutput>, 2> denoiseMaskOutput_;

	// �f�m�C�Y�̌��ʏo�͗p�N���X���Z�b�g�B
	std::array<std::shared_ptr<RaytracingOutput>, 2> denoiseMixTextureOutput_;

	// �f�m�C�Y�������ăG�t�F�N�g�������ŏI�o�͗p�N���X�B
	std::array<std::shared_ptr<RaytracingOutput>, 2> finalOutputTexture_;

	// �u���[���p�G�~�b�V�u�o��
	std::array<std::shared_ptr<RaytracingOutput>, 2> emissiveOutput_;

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	std::vector<RayPipelineShaderData> pipelineShaders_;
	std::shared_ptr<RaytracingPipeline> pipeline_;

	RayConstBufferData constBufferData_;
	std::shared_ptr<DynamicConstBuffer> constBuffer_;
	int debugMesnInfoHandle_;

	// �t�H�O�p�{�����[���e�N�X�`��
	VolumeTextureUAV volumeTexture_;

	// 3D�m�C�Y�������ݗp�R���s���[�g�V�F�[�_�[
	std::shared_ptr<RayComputeShader> write3DNoiseCS_;


public:

	/*===== �����o�ϐ� =====*/

	void Setting();

	void SettingTLAS();

	void Update();

	void Draw();

	RayConstBufferData& GetConstBufferData() { return constBufferData_; }

private:

	// ���C�g���[�V���O�����s�B
	void TraceRay();

	// �f�m�C�Y�R�}���h��ςށB
	void DenoiseCommand();

	// �R�s�[�R�}���h��ςށB
	void CopyCommand();

};