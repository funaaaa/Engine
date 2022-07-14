#pragma once
#include "DirectXBase.h"
#include <d3d12.h>
#include "dxcapi.h"
#include <string>

// シェーダーデータを保存するためのデータクラス
class ShaderData {

private:

	/*-- メンバ変数 --*/

	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob_;	// シェーダーのデータ
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlobDxc_;	// シェーダーのデータ dxc用
	std::vector<char> shaderBin_;					// dxcでコンパイルした際に出力される謎の文字列
	std::string entryPoint_;							// シェーダーのエントリーポイント
	std::string shaderModel_;						// シェーダーモデル
	std::string shaderPath_;							// シェーダーのファイルパス これを使ってシェーダーを判断する。


public:

	/*-- メンバ関数 --*/

	// コンストラクタ
	ShaderData(const std::string& ShaderPath, const std::string& EntryPoint, const std::string& ShaderModel, const bool& IsDXC = false);

	// ロード処理
	void LoadShader();
	void LoadShaderDXC();

	// シェーダーのファイルパスを取得する処理
	const std::string& GetShaderPath() { return shaderPath_; }
	// シェーダーのデータを取得する処理
	const Microsoft::WRL::ComPtr<ID3DBlob>& GetShaderBlob() { return shaderBlob_; }
	const Microsoft::WRL::ComPtr<IDxcBlob>& GetShaderBlobDXC() { return shaderBlobDxc_; }
	std::vector<char>& GetShaderBin() { return shaderBin_; }


private:

	std::wstring StringToWString( std::string OString );

};