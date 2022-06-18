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

// シェーダーデータクラスの前方宣言
class ShaderData;

// シェーダーデータ保存クラス
class ShaderStorage : public Singleton<ShaderStorage> {

private:

	/*-- メンバ変数 --*/

	std::vector<std::shared_ptr<ShaderData>> shaderData;


public:

	/*-- メンバ関数 --*/

	// シェーダーをロードする。
	Microsoft::WRL::ComPtr<ID3DBlob> LoadShader(const std::string& shaderPath, const std::string& shaderModel, const std::string& entryPoint);
	Microsoft::WRL::ComPtr<ID3DBlob> LoadShaderForDXC(const std::string& shaderPath, const std::string& shaderModel, const std::string& entryPoint);

	// シェーダーデータを返す。
	Microsoft::WRL::ComPtr<ID3DBlob> GetShaderData(const std::string& shaderPath);
	Microsoft::WRL::ComPtr<IDxcBlob> GetShaderDataForDXC(const std::string& shaderPath);
	std::vector<char>& GetShaderBin(const std::string& shaderPath);

};


