#pragma once
#include <wrl.h>
#include <DirectXTex/d3dx12.h>
#pragma warning(push)
#pragma warning(disable:4267)
#include <memory>
#pragma warning(pop)
#include <vector>

class RayRootsignature;

// 新しくレイトレ用に作ったコンピュートシェーダー
// 現在はテクスチャ系のみ送ることが出来る。もともとあったComputeShaderクラスと結合する予定。
class RayComputeShader {

private:

	/*===== メンバ変数 =====*/

	std::shared_ptr<RayRootsignature> rootSignature_;			// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline_;		// パイプライン
	int inputCBVCount_;					// 入力するCBVの数
	int inputSRVCount_;					// 入力するSRVの数
	int inputUAVCount_;					// 入力するUAVの数
	std::vector<int> inputUAVIndex_;	// 入力UAVデータのディスクリプタヒープ上でのインデックス
	std::vector<int> inputSRVIndex_;	// 入力SRVデータのディスクリプタヒープ上でのインデックス
	int outputIndex_;					// 出力データのディスクリプタヒープ上でのインデックス


public:

	/*===== メンバ関数 =====*/

	// セッティング処理
	void Setting(LPCWSTR CsPath, int SRVCount, int CVBCount, int UAVCount, std::vector<int> SRVIndex, std::vector<int> UAVIndex);

	// 実行！
	void Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ, int OutputIndex, std::vector<D3D12_GPU_VIRTUAL_ADDRESS> InputCBV = {}, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> CmdList = nullptr);

	// 入力用UAVを変更。
	void ChangeInputUAVIndex(const std::vector<int>& NewInputUAV) { inputUAVIndex_ = NewInputUAV; }

private:

	// シェーダー読み込み
	LPD3DBLOB LoadShader(LPCWSTR ShaderFileName, const std::string& EntryPointName, const std::string& ShaderModel, ID3DBlob* ShaderBlob, ID3DBlob* ErrorBlob);
	// シェーダーのロードエラーをチェック
	void CheckRoadShaderError(HRESULT Result, ID3DBlob* ErrorBlob);


};