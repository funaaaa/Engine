#include "ShaderData.h"
#include "FString.h"
#include <d3dcompiler.h>
#include <system_error>
#include <fstream>
#include <iostream>
#include <sstream>
#include <array>

#pragma comment(lib, "dxcompiler.lib")

ShaderData::ShaderData(const std::string& shaderPath, const std::string& entryPoint, const std::string& shaderModel, const bool& isDXC)
{

	/*-- コンストラクタ --*/

	// 引数を保存。
	this->shaderPath = shaderPath;
	this->entryPoint = entryPoint;
	this->shaderModel = shaderModel;
	this->shaderBlob = nullptr;

	if (isDXC) {

		//シェーダーをロード。
		LoadShaderDXC();

	}
	else {

		//シェーダーをロード。
		LoadShader();

	}

}

void ShaderData::LoadShader()
{

	/*-- シェーダーをロードする処理 --*/

	std::array<wchar_t, 128> shaderPathBuff;
	FString::ConvertStringToWchar_t(shaderPath, shaderPathBuff.data(), 128);

	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT result = D3DCompileFromFile(
		shaderPathBuff.data(),								//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード可能にする
		entryPoint.c_str(), shaderModel.c_str(),			//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバッグ用設定
		0,
		&shaderBlob, &errorBlob);

	// シェーダーのエラーチェック
	if (FAILED(result)) {

		// hresultからエラーメッセージを取得
		std::string errorMsg = std::system_category().message(result);

		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

}

void ShaderData::LoadShaderDXC()
{
	HRESULT hr;
	std::ifstream infile(shaderPath, std::ifstream::binary);
	if (!infile) {
		throw std::runtime_error("failed shader compile.");
	}

	// 型変換。
	std::wstring fileName = StringToWString(shaderPath);
	std::stringstream strstream;

	strstream << infile.rdbuf();

	std::string shaderCode = strstream.str();
	Microsoft::WRL::ComPtr<IDxcLibrary> library;
	DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
	Microsoft::WRL::ComPtr<IDxcCompiler> compiler;
	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> source;

	library->CreateBlobWithEncodingFromPinned(
		(LPBYTE)shaderCode.c_str(), static_cast<UINT32>(shaderCode.size()), CP_UTF8, &source);
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

	// インクルードを使う場合には適切に設定すること
	library->CreateIncludeHandler(&includeHandler);
	// コンパイルオプションの指定
	std::vector<LPCWSTR> arguments;

	arguments.emplace_back(L"/Od");		// 最適化を無効にする。
	arguments.emplace_back(L"/Zi");		// デバッグ情報を有効にする。
	arguments.emplace_back(L"-Qembed_debug");	// PDBをシェーダーコンテナーに埋め込む。これを指定する場合はZiは必須らしい。

	// シェーダーモデルは一旦これで固定する。
	const auto target = L"lib_6_4";

	Microsoft::WRL::ComPtr<IDxcOperationResult> dxcResult;
	hr = compiler->Compile(source.Get(), fileName.c_str(),
		L"", target, arguments.data(), UINT(arguments.size()),
		nullptr, 0, includeHandler.Get(), &dxcResult);

	if (FAILED(hr)) {
		throw std::runtime_error("failed shader compile.");
	}

	dxcResult->GetStatus(&hr);
	if (FAILED(hr)) {
		Microsoft::WRL::ComPtr<IDxcBlobEncoding> errBlob;
		dxcResult->GetErrorBuffer(&errBlob);
		throw std::runtime_error("failed shader compile");
	}

	Microsoft::WRL::ComPtr<IDxcBlob> blob;
	dxcResult->GetResult(&blob);

	shaderBlobDxc = blob;

	std::vector<char> result;
	auto size = blob->GetBufferSize();
	result.resize(size);
	memcpy(result.data(), blob->GetBufferPointer(), size);
	shaderBin = result;

}

std::wstring ShaderData::StringToWString(std::string OString)
{
	// SJIS → wstring
	int iBufferSize = MultiByteToWideChar(CP_ACP, 0, OString.c_str()
		, -1, (wchar_t*)NULL, 0);

	// バッファの取得
	wchar_t* cpUCS2 = new wchar_t[iBufferSize];

	// SJIS → wstring
	MultiByteToWideChar(CP_ACP, 0, OString.c_str(), -1, cpUCS2
		, iBufferSize);

	// stringの生成
	std::wstring oRet(cpUCS2, cpUCS2 + iBufferSize - 1);

	// バッファの破棄
	delete[] cpUCS2;

	// 変換結果を返す
	return(oRet);
}