#pragma once
#include "DirectXBase.h"
#include <d3d12.h>
#include <dxcapi.h>
#include <string>

// シェーダーデータを保存するためのデータクラス
class ShaderData {

private:

	/*-- メンバ変数 --*/

	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;		// シェーダーのデータ
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlobDxc;		// シェーダーのデータ dxc用
	std::vector<char> shaderBin;				// dxcでコンパイルした際に出力される謎の文字列
	std::string entryPoint;					// シェーダーのエントリーポイント
	std::string shaderModel;					// シェーダーモデル
	std::string shaderPath;					// シェーダーのファイルパス これを使ってシェーダーを判断する。


public:

	/*-- メンバ関数 --*/

	// コンストラクタ
	ShaderData(const std::string& shaderPath, const std::string& entryPoint, const std::string& shaderModel, const bool& isDXC = false);

	// ロード処理
	void LoadShader();
	void LoadShaderDXC();

	// シェーダーのファイルパスを取得する処理
	const std::string& GetShaderPath() { return shaderPath; }
	// シェーダーのデータを取得する処理
	const Microsoft::WRL::ComPtr<ID3DBlob>& GetShaderBlob() { return shaderBlob; }
	const Microsoft::WRL::ComPtr<IDxcBlob>& GetShaderBlobDXC() { return shaderBlobDxc; }
	std::vector<char>& GetShaderBin() { return shaderBin; }


private:

	std::wstring StringToWString( std::string oString )
	{
		// SJIS → wstring
		int iBufferSize = MultiByteToWideChar(CP_ACP, 0, oString.c_str()
			, -1, (wchar_t*)NULL, 0);

		// バッファの取得
		wchar_t* cpUCS2 = new wchar_t[iBufferSize];

		// SJIS → wstring
		MultiByteToWideChar(CP_ACP, 0, oString.c_str(), -1, cpUCS2
			, iBufferSize);

		// stringの生成
		std::wstring oRet(cpUCS2, cpUCS2 + iBufferSize - 1);

		// バッファの破棄
		delete[] cpUCS2;

		// 変換結果を返す
		return(oRet);
	}

};