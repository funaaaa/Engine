#include "ShaderStorage.h"
#include "ShaderData.h"

ComPtr<ID3DBlob> ShaderStorage::LoadShader(const string& shaderPath, const string& shaderModel, const string& entryPoint)
{

	/*-- シェーダーのロード処理 --*/

	const int SHADER_COUNT = shaderData.size();

	// シェーダーの数分ループして、ロード済みのシェーダーかをチェックする。
	for (int index = 0; index < SHADER_COUNT; ++index) {

		// シェーダの名前が違っていたら次へ。
		if (shaderData[index]->GetShaderPath() != shaderPath) continue;

		// このindexのシェーダーをリターンする。
		return shaderData[index]->GetShaderBlob();

	}

	// シェーダーをロードして保存。
	shaderData.emplace_back(make_unique<ShaderData>(shaderPath, entryPoint, shaderModel));

	// 最後尾のデータをリターンする。
	return shaderData[shaderData.size() - 1]->GetShaderBlob();
}

ComPtr<ID3DBlob> ShaderStorage::LoadShaderForDXC(const string& shaderPath, const string& shaderModel, const string& entryPoint)
{

	/*-- シェーダーのロード処理 --*/

	const int SHADER_COUNT = shaderData.size();

	// シェーダーの数分ループして、ロード済みのシェーダーかをチェックする。
	for (int index = 0; index < SHADER_COUNT; ++index) {

		// シェーダの名前が違っていたら次へ。
		if (shaderData[index]->GetShaderPath() != shaderPath) continue;

		// このindexのシェーダーをリターンする。
		return shaderData[index]->GetShaderBlob();

	}

	// シェーダーをロードして保存。
	shaderData.emplace_back(make_unique<ShaderData>(shaderPath, entryPoint, shaderModel, true));

	// 最後尾のデータをリターンする。
	return shaderData[shaderData.size() - 1]->GetShaderBlob();

}

ComPtr<ID3DBlob> ShaderStorage::GetShaderData(const string& shaderPath)
{

	/*-- シェーダーデータを返す処理 --*/

	const int SHADER_COUNT = shaderData.size();

	// 全てのシェーダーデータを検索する。
	for (int index = 0; index < SHADER_COUNT; ++index) {

		// 引数の要素が合っているかをチェックする。
		if (!(shaderData[index]->GetShaderPath() == shaderPath)) continue;

		return shaderData[index]->GetShaderBlob();

	}

	return ComPtr<ID3DBlob>();
}

ComPtr<IDxcBlob> ShaderStorage::GetShaderDataForDXC(const string& shaderPath)
{
	/*-- シェーダーデータを返す処理 --*/

	const int SHADER_COUNT = shaderData.size();

	// 全てのシェーダーデータを検索する。
	for (int index = 0; index < SHADER_COUNT; ++index) {

		// 引数の要素が合っているかをチェックする。
		if (!(shaderData[index]->GetShaderPath() == shaderPath)) continue;

		return shaderData[index]->GetShaderBlobDXC();

	}

	return ComPtr<IDxcBlob>();
}

vector<char>& ShaderStorage::GetShaderBin(const string& shaderPath)
{
	/*-- シェーダーデータを返す処理 --*/

	const int SHADER_COUNT = shaderData.size();

	// 全てのシェーダーデータを検索する。
	for (int index = 0; index < SHADER_COUNT; ++index) {

		// 引数の要素が合っているかをチェックする。
		if (!(shaderData[index]->GetShaderPath() == shaderPath)) continue;

		return shaderData[index]->GetShaderBin();

	}

	return vector<char>{};
}
