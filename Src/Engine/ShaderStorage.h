#pragma once
#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)

#include <vector>
#include <d3d12.h>
#include <string>
#include <wrl/client.h>
#include "Singleton.h"
#include "dxcapi.h"

// �V�F�[�_�[�f�[�^�N���X�̑O���錾
class ShaderData;

// �V�F�[�_�[�f�[�^�ۑ��N���X
class ShaderStorage : public Singleton<ShaderStorage> {

private:

	/*-- �����o�ϐ� --*/

	std::vector<std::shared_ptr<ShaderData>> shaderData;


public:

	/*-- �����o�֐� --*/

	// �V�F�[�_�[�����[�h����B
	Microsoft::WRL::ComPtr<ID3DBlob> LoadShader(const std::string& ShaderPath, const std::string& ShaderModel, const std::string& EntryPoint);
	Microsoft::WRL::ComPtr<ID3DBlob> LoadShaderForDXC(const std::string& ShaderPath, const std::string& ShaderModel, const std::string& EntryPoint);

	// �V�F�[�_�[�f�[�^��Ԃ��B
	Microsoft::WRL::ComPtr<ID3DBlob> GetShaderData(const std::string& ShaderPath);
	Microsoft::WRL::ComPtr<IDxcBlob> GetShaderDataForDXC(const std::string& ShaderPath);
	std::vector<char>& GetShaderBin(const std::string& ShaderPath);

};


