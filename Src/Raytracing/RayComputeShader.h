#pragma once
#include <wrl.h>
#include <DirectXTex/d3dx12.h>
#include <memory>
#include <vector>

class RayRootsignature;

// 新しくレイトレ用に作ったコンピュートシェーダー
class RayComputeShader {

private:

	/*===== メンバ変数 =====*/

	std::shared_ptr<RayRootsignature> rootSignature;			// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipline;		// パイプライン
	int inputCBVCount;					// 入力するCBVの数
	int inputSRVCount;					// 入力するSRVの数
	int inputUAVCount;					// 入力するUAVの数
	std::vector<int> inputUAVIndex;		// 入力UAVデータのディスクリプタヒープ上でのインデックス
	int outputIndex;					// 出力データのディスクリプタヒープ上でのインデックス


public:

	/*===== メンバ関数 =====*/

	// セッティング処理
	void Setting(LPCWSTR CsPath, const int& SRVCount, const int& CVBCount, const int& UAVCount, std::vector<int> SRVIndex);

	// 実行！
	void Dispatch(const UINT& ThreadGroupCountX, const UINT& ThreadGroupCountY, const UINT& ThreadGroupCountZ, const int& OutputIndex, std::vector<D3D12_GPU_VIRTUAL_ADDRESS> InputCBV);

	// 入力用UAVを変更。
	void ChangeInputUAVIndex(const std::vector<int>& NewInputUAV) { inputUAVIndex = NewInputUAV; }

private:

	//シェーダー読み込み
	LPD3DBLOB LoadShader(LPCWSTR ShaderFileName, const std::string& EntryPointName, const std::string& ShaderModel, ID3DBlob* ShaderBlob, ID3DBlob* ErrorBlob);
	//シェーダーのロードエラーをチェック
	void CheckRoadShaderError(HRESULT Result, ID3DBlob* ErrorBlob);


};