#include "ShaderStorage.h"
#include "ShaderData.h"

Microsoft::WRL::ComPtr<ID3DBlob> ShaderStorage::LoadShader(const std::string& shaderPath, const std::string& shaderModel, const std::string& entryPoint)
{

	/*-- シェーダーのロード処理 --*/

	const int SHADER_COUNT = static_cast<int>(shaderData.size());

	// シェーダーの数分ループして、ロード済みのシェーダーかをチェックする。
	for (int index = 0; index < SHADER_COUNT; ++index) {

		// シェーダの名前が違っていたら次へ。
		if (shaderData[index]->GetShaderPath() != shaderPath) continue;

		// このindexのシェーダーをリターンする。
		return shaderData[index]->GetShaderBlob();

	}

	// シェーダーをロードして保存。
	shaderData.emplace_back(std::make_unique<ShaderData>(shaderPath, entryPoint, shaderModel));

	// 最後尾のデータをリターンする。
	return shaderData[shaderData.size() - 1]->GetShaderBlob();
}

Microsoft::WRL::ComPtr<ID3DBlob> ShaderStorage::LoadShaderForDXC(const std::string& shaderPath, const std::string& shaderModel, const std::string& entryPoint)
{

	/*-- シェーダーのロード処理 --*/

	const int SHADER_COUNT = static_cast<int>(shaderData.size());

	// シェーダーの数分ループして、ロード済みのシェーダーかをチェックする。
	for (int index = 0; index < SHADER_COUNT; ++index) {

		// シェーダの名前が違っていたら次へ。
		if (shaderData[index]->GetShaderPath() != shaderPath) continue;

		// このindexのシェーダーをリターンする。
		return shaderData[index]->GetShaderBlob();

	}

	// シェーダーをロードして保存。
	shaderData.emplace_back(std::make_unique<ShaderData>(shaderPath, entryPoint, shaderModel, true));

	// 最後尾のデータをリターンする。
	return shaderData[shaderData.size() - 1]->GetShaderBlob();

}

Microsoft::WRL::ComPtr<ID3DBlob> ShaderStorage::GetShaderData(const std::string& shaderPath)
{

	/*-- シェーダーデータを返す処理 --*/

	const int SHADER_COUNT = static_cast<int>(shaderData.size());

	// 全てのシェーダーデータを検索する。
	for (int index = 0; index < SHADER_COUNT; ++index) {

		// 引数の要素が合っているかをチェックする。
		if (!(shaderData[index]->GetShaderPath() == shaderPath)) continue;

		return shaderData[index]->GetShaderBlob();

	}

	return Microsoft::WRL::ComPtr<ID3DBlob>();
}

Microsoft::WRL::ComPtr<IDxcBlob> ShaderStorage::GetShaderDataForDXC(const std::string& shaderPath)
{
	/*-- シェーダーデータを返す処理 --*/

	const int SHADER_COUNT = static_cast<int>(shaderData.size());

	// 全てのシェーダーデータを検索する。
	for (int index = 0; index < SHADER_COUNT; ++index) {

		// 引数の要素が合っているかをチェックする。
		if (!(shaderData[index]->GetShaderPath() == shaderPath)) continue;

		return shaderData[index]->GetShaderBlobDXC();

	}

	return Microsoft::WRL::ComPtr<IDxcBlob>();
}

std::vector<char>& ShaderStorage::GetShaderBin(const std::string& shaderPath)
{
	/*-- シェーダーデータを返す処理 --*/

	const int SHADER_COUNT = static_cast<int>(shaderData.size());

	// 全てのシェーダーデータを検索する。
	for (int index = 0; index < SHADER_COUNT; ++index) {

		// 引数の要素が合っているかをチェックする。
		if (!(shaderData[index]->GetShaderPath() == shaderPath)) continue;

		return shaderData[index]->GetShaderBin();

	}

	// シェーダーがロードされていない。
	assert(0);
	return shaderData[0]->GetShaderBin();
}
