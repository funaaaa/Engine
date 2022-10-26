#pragma once
#include "Singleton.h"
#include "ConstBuffers.h"
#include <memory>
#include <array>

#include "ConstBuffers.h"
#include "DynamicConstBuffer.h"
#include "RaytracingPipeline.h"
#include "RayRootsignature.h"

class TLAS;
class RaytracingOutput;
class RaytracingPipeline;
class DynamicConstBuffer;

class RayEngine : public Singleton<RayEngine> {

public:

	/*===== �����o�ϐ� =====*/

	// TLAS�𐶐��B
	std::shared_ptr<TLAS> tlas_;

	// AO�o�͗p�B
	std::array<std::shared_ptr<RaytracingOutput>, 2> aoOutput_;
	std::array<std::shared_ptr<RaytracingOutput>, 2> denoiseAOOutput_;

	// �F�o�͗p�N���X�B
	std::array<std::shared_ptr<RaytracingOutput>, 2> colorOutput_;

	// ���C�g�o�͗p�B
	std::array<std::shared_ptr<RaytracingOutput>, 2> lightOutput_;
	std::array<std::shared_ptr<RaytracingOutput>, 2> denoiseLightOutput_;

	// GI�o�͗p�B
	std::array<std::shared_ptr<RaytracingOutput>, 2> giOutput_;
	std::array<std::shared_ptr<RaytracingOutput>, 2> denoiseGiOutput_;

	// �f�m�C�Y�}�X�N�p�B
	std::array<std::shared_ptr<RaytracingOutput>, 2> denoiseMaskOutput_;

	// �f�m�C�Y�̌��ʏo�͗p�N���X���Z�b�g�B
	std::array<std::shared_ptr<RaytracingOutput>, 2> denoiseMixTextureOutput_;

	// �f�m�C�YAO�p�̃p�C�v���C����ݒ�B
	std::vector<RayPipelineShaderData> pipelineShaders_;
	std::shared_ptr<RaytracingPipeline> pipeline_;

	RayConstBufferData constBufferData_;
	std::shared_ptr<DynamicConstBuffer> constBuffer_;


public:

	/*===== �����o�ϐ� =====*/

	void Setting();

	void SettingTLAS();

	void Update();

	void Draw();
	void NoiseDraw();

	RayConstBufferData& GetConstBufferData() { return constBufferData_; }

};