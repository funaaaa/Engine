#include "ShaderData.h"
#include "FString.h"
#include <d3dcompiler.h>
#include <system_error>
#include <fstream>
#include <iostream>
#include <sstream>

ShaderData::ShaderData(const string& shaderPath, const string& entryPoint, const string& shaderModel)
{

	/*-- コンストラクタ --*/

	// 引数を保存。
	this->shaderPath = shaderPath;
	this->entryPoint = entryPoint;
	this->shaderModel = shaderModel;
	this->shaderBlob = nullptr;

	//シェーダーをロード。
	LoadShader();

}

ShaderData::ShaderData(const string& shaderPath, const string& entryPoint, const string& shaderModel, const bool& isDXC)
{

	/*-- コンストラクタ --*/

	// 引数を保存。
	this->shaderPath = shaderPath;
	this->entryPoint = entryPoint;
	this->shaderModel = shaderModel;
	this->shaderBlob = nullptr;

	//シェーダーをロード。
	LoadShaderDXC();

}

void ShaderData::LoadShader()
{

	/*-- シェーダーをロードする処理 --*/

	wchar_t shaderPathBuff[128];
	FString::ConvertStringToWchar_t(shaderPath, shaderPathBuff, 128);

	ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT result = D3DCompileFromFile(
		shaderPathBuff,										//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード可能にする
		entryPoint.c_str(), shaderModel.c_str(),			//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバッグ用設定
		0,
		&shaderBlob, &errorBlob);

	// シェーダーのエラーチェック
	if (FAILED(result)) {

		// hresultからエラーメッセージを取得
		string errorMsg = system_category().message(result);

		//errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		//エラー内容を出力ウィンドウに表示
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
	std::wstring fileName = StringToWString(shaderPath);
	std::stringstream strstream;
	strstream << infile.rdbuf();
	std::string shaderCode = strstream.str();
	ComPtr<IDxcLibrary> library;
	DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
	ComPtr<IDxcCompiler> compiler;
	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	ComPtr<IDxcBlobEncoding> source;
	library->CreateBlobWithEncodingFromPinned(
		(LPBYTE)shaderCode.c_str(), (UINT32)shaderCode.size(), CP_UTF8, &source);
	ComPtr<IDxcIncludeHandler> includeHandler;
	// インクルードを使う場合には適切に設定すること.
	library->CreateIncludeHandler(&includeHandler);
	// コンパイルオプションの指定.
	std::vector<LPCWSTR> arguments;
	arguments.emplace_back(L"/Od");
	const auto target = L"lib_6_4";
	ComPtr<IDxcOperationResult> dxcResult;
	hr = compiler->Compile(source.Get(), fileName.c_str(),
		L"", target, arguments.data(), UINT(arguments.size()),
		nullptr, 0, includeHandler.Get(), &dxcResult);
	if (FAILED(hr)) {
		throw std::runtime_error("failed shader compile.");
	}
	dxcResult->GetStatus(&hr);
	if (FAILED(hr)) {
		ComPtr<IDxcBlobEncoding> errBlob;
		dxcResult->GetErrorBuffer(&errBlob);
		// ... errBlob の内容をエラーメッセージとして表示 (省略)
		throw std::runtime_error("failed shader compile");
	}
	ComPtr<IDxcBlob> blob;
	dxcResult->GetResult(&blob);

	shaderBlobDxc = blob;

	std::vector<char> result;
	auto size = blob->GetBufferSize();
	result.resize(size);
	memcpy(result.data(), blob->GetBufferPointer(), size);
	shaderBin = result;

}
