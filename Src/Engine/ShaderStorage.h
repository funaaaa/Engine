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
#include <dxcapi.h>

#pragma comment(lib, "Lib/Dxcompiler/dxcompiler.lib")

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
	Microsoft::WRL::ComPtr<ID3DBlob> LoadShader(const std::string& shaderPath, const std::string& shaderModel, const std::string& entryPoint);
	Microsoft::WRL::ComPtr<ID3DBlob> LoadShaderForDXC(const std::string& shaderPath, const std::string& shaderModel, const std::string& entryPoint);

	// �V�F�[�_�[�f�[�^��Ԃ��B
	Microsoft::WRL::ComPtr<ID3DBlob> GetShaderData(const std::string& shaderPath);
	Microsoft::WRL::ComPtr<IDxcBlob> GetShaderDataForDXC(const std::string& shaderPath);
	std::vector<char>& GetShaderBin(const std::string& shaderPath);

};


