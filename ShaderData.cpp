#include "ShaderData.h"
#include "FString.h"
#include <d3dcompiler.h>
#include <system_error>

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
