#pragma once
#include <string>
#include <wtypes.h>
#include <vector>
#include <d3d12.h>
#include <wrl.h>
#include <memory>

class RayRootsignature;

struct RayPiplineShaderData {

	std::string shaderPath;				// �V�F�[�_�[�p�X
	std::vector<LPCWSTR> entryPoint;	// �G���g���|�C���g
	RayPiplineShaderData() {};
	RayPiplineShaderData(std::string ShaderPath, std::vector<LPCWSTR> EntryPoint) :shaderPath(ShaderPath), entryPoint(EntryPoint) {};

};

// ���C�g���[�V���O�Ŏg�p����p�C�v���C��
class RaytracingPipline {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<RayPiplineShaderData> shaderData;			// �g�p����V�F�[�_�[��Z�߂��\����
	std::vector<D3D12_SHADER_BYTECODE> shaderCode;			// �g�p����V�F�[�_�[�̃o�C�g�R�[�h

	Microsoft::WRL::ComPtr<ID3D12StateObject> stateObject;	// �X�e�[�g�I�u�W�F�N�g

	std::shared_ptr<RayRootsignature> globalRootSig;		// �O���[�o�����[�g�V�O�l�`��


public:

	/*===== �����o�ϐ� =====*/

	// �Z�b�e�B���O����
	void Setting(const std::vector<RayPiplineShaderData>& InputData);

	// �Q�b�^
	Microsoft::WRL::ComPtr<ID3D12StateObject> GetStateObject() { return stateObject; }

};