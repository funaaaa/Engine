#pragma once
#include "Singleton.h"
#include <memory>
#include <vector>
#include <string>
#include <wtypes.h>

class DynamicConstBuffer;
class RayPipeline;
class TLAS;
class RaytracingOutput;
struct RayPipelineShaderData;
struct RayConstBufferData;

class RayEngine : public Singleton<RayEngine> {

private:

	/*===== �����o�ϐ� =====*/

	// �萔�o�b�t�@�B
	std::shared_ptr<RayConstBufferData> constBufferData_;
	std::shared_ptr<DynamicConstBuffer> constBuffer_;

	// ���C�g���p�̃p�C�v���C����ݒ�B
	std::vector<std::shared_ptr<RayPipelineShaderData>> pipelineShaders_;
	std::shared_ptr<RayPipeline> pipeline_;

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

	std::string shaderName = "Resource/ShaderFiles/RayTracing/DenoiseAOShader.hlsl";
	std::vector<LPCWSTR> rgEntry = { L"mainRayGen" };
	std::vector<LPCWSTR> msEntry = { L"mainMS", L"shadowMS" };
	std::vector<LPCWSTR> hgEntry = { L"mainCHS", L"mainAnyHit" };


public:

	/*===== �����o�֐� =====*/

	// ���O��������
	void Setting();

	// TLAS��ݒ�B
	void SettingTLAS();

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// ���C�g�����[���h���N���A
	void ClearRayWorld();

	// �萔�o�b�t�@���擾�B
	std::weak_ptr<RayConstBufferData> GetConstBufferPtr() { return constBufferData_; }

};