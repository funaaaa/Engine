#pragma once
#include "Singleton.h"
#include "ConstBuffers.h"
#include <memory>

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
	std::shared_ptr<RaytracingOutput> aoOutput_;
	std::shared_ptr<RaytracingOutput> denoiseAOOutput_;

	// �F�o�͗p�N���X�B
	std::shared_ptr<RaytracingOutput> colorOutput_;

	// ���C�g�o�͗p�B
	std::shared_ptr<RaytracingOutput> lightOutput_;
	std::shared_ptr<RaytracingOutput> denoiseLightOutput_;

	// GI�o�͗p�B
	std::shared_ptr<RaytracingOutput> giOutput_;
	std::shared_ptr<RaytracingOutput> denoiseGiOutput_;

	// �f�m�C�Y�}�X�N�p�B
	std::shared_ptr<RaytracingOutput> denoiseMaskOutput_;

	// �f�m�C�Y�̌��ʏo�͗p�N���X���Z�b�g�B
	std::shared_ptr<RaytracingOutput> denoiseMixTextureOutput_;

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

	RayConstBufferData& GetConstBufferData() { return constBufferData_; }

};