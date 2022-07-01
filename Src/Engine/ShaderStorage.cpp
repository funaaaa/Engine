#include "ShaderStorage.h"
#include "ShaderData.h"

#pragma comment(lib, "dxcompiler.lib")

Microsoft::WRL::ComPtr<ID3DBlob> ShaderStorage::LoadShader(const std::string& ShaderPath, const std::string& ShaderModel, const std::string& EntryPoint)
{

	/*-- シェーダーのロード処理 --*/

	const int SHADER_COUNT = static_cast<int>(shaderData.size());

	// シェーダーの数分ループして、ロード済みのシェーダーかをチェックする。
	for (int index = 0; index < SHADER_COUNT; ++index) {

		// シェーダの名前が違っていたら次へ。
		if (shaderData[index]->GetShaderPath() != ShaderPath) continue;

		// このindexのシェーダーをリターンする。
		return shaderData[index]->GetShaderBlob();

	}

	// シェーダーをロードして保存。
	shaderData.emplace_back(std::make_unique<ShaderData>(ShaderPath, EntryPoint, ShaderModel));

	// 最後尾のデータをリターンする。
	return shaderData[shaderData.size() - 1]->GetShaderBlob();
}

Microsoft::WRL::ComPtr<ID3DBlob> ShaderStorage::LoadShaderForDXC(const std::string& ShaderPath, const std::string& ShaderModel, const std::string& EntryPoint)
{

	/*-- シェーダーのロード処理 --*/

	const int SHADER_COUNT = static_cast<int>(shaderData.size());

	// シェーダーの数分ループして、ロード済みのシェーダーかをチェックする。
	for (int index = 0; index < SHADER_COUNT; ++index) {

		// シェーダの名前が違っていたら次へ。
		if (shaderData[index]->GetShaderPath() != ShaderPath) continue;

		// このindexのシェーダーをリターンする。
		return shaderData[index]->GetShaderBlob();

	}

	// シェーダーをロードして保存。
	shaderData.emplace_back(std::make_unique<ShaderData>(ShaderPath, EntryPoint, ShaderModel, true));

	// 最後尾のデータをリターンする。
	return shaderData[shaderData.size() - 1]->GetShaderBlob();

}

Microsoft::WRL::ComPtr<ID3DBlob> ShaderStorage::GetShaderData(const std::string& ShaderPath)
{

	/*-- シェーダーデータを返す処理 --*/

	const int SHADER_COUNT = static_cast<int>(shaderData.size());

	// 全てのシェーダーデータを検索する。
	for (int index = 0; index < SHADER_COUNT; ++index) {

		// 引数の要素が合っているかをチェックする。
		if (!(shaderData[index]->GetShaderPath() == ShaderPath)) continue;

		return shaderData[index]->GetShaderBlob();

	}

	return Microsoft::WRL::ComPtr<ID3DBlob>();
}

Microsoft::WRL::ComPtr<IDxcBlob> ShaderStorage::GetShaderDataForDXC(const std::string& ShaderPath)
{
	/*-- シェーダーデータを返す処理 --*/

	const int SHADER_COUNT = static_cast<int>(shaderData.size());

	// 全てのシェーダーデータを検索する。
	for (int index = 0; index < SHADER_COUNT; ++index) {

		// 引数の要素が合っているかをチェックする。
		if (!(shaderData[index]->GetShaderPath() == ShaderPath)) continue;

		return shaderData[index]->GetShaderBlobDXC();

	}

	return Microsoft::WRL::ComPtr<IDxcBlob>();
}

std::vector<char>& ShaderStorage::GetShaderBin(const std::string& ShaderPath)
{
	/*-- シェーダーデータを返す処理 --*/

	const int SHADER_COUNT = static_cast<int>(shaderData.size());

	// 全てのシェーダーデータを検索する。
	for (int index = 0; index < SHADER_COUNT; ++index) {

		// 引数の要素が合っているかをチェックする。
		if (!(shaderData[index]->GetShaderPath() == ShaderPath)) continue;

		return shaderData[index]->GetShaderBin();

	}

	// シェーダーがロードされていない。
	assert(0);
	return shaderData[0]->GetShaderBin();
}
