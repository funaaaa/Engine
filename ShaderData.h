#pragma once
#include "DirectXBase.h"
#include <d3d12.h>
#include <string>

// シェーダーデータを保存するためのデータクラス
class ShaderData {

private:

	/*-- メンバ変数 --*/

	ComPtr<ID3DBlob> shaderBlob;		// シェーダーのデータ
	string entryPoint;					// シェーダーのエントリーポイント
	string shaderModel;					// シェーダーモデル
	string shaderPath;					// シェーダーのファイルパス これを使ってシェーダーを判断する。


public:

	/*-- メンバ関数 --*/

	// コンストラクタ
	ShaderData(const string& shaderPath, const string& entryPoint, const string& shaderModel);

	// ロード処理
	void LoadShader();

	// シェーダーのファイルパスを取得する処理
	const string& GetShaderPath() { return shaderPath; }
	// シェーダーのデータを取得する処理
	const ComPtr<ID3DBlob>& GetShaderBlob() { return shaderBlob; }

};