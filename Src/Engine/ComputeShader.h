#pragma once
#include <wrl.h>
#include <DirectXTex/d3dx12.h>
#include <memory>

class StructuredBuffer;
class RWStructuredBuffer;

// コンピュートシェーダークラス 
// このクラスは最初に作成したコンピュートシェーダークラスで、テクスチャ等は扱えず構造化バッファを送ることしか出来ません。
// いずれRayComputeShader側にこの機能を追加し、このクラスは削除する予定です。
class ComputeShader {

private:

	/*-- メンバ変数 --*/

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipline;		// パイプライン
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;		// ディスクリプタヒープ 情報とかが保存されているのはinputSBとoutputSBの中
	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUDescHeapHandle;			// SRV開始ハンドル GPU
	D3D12_GPU_DESCRIPTOR_HANDLE uavGPUDescHeapHandle;			// UAV開始ハンドル GPU

public:

	std::shared_ptr<StructuredBuffer> inputSB;					// 入力用構造体バッファ
	std::shared_ptr<RWStructuredBuffer> outputSB;				// 書き込み用構造体バッファ


public:

	/*-- メンバ関数 --*/

	//コンストラクタ
	ComputeShader();
	~ComputeShader();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="csPath">コンピュートシェーダーのディレクトリーパス</param>
	/// <param name="sizeOfInput">入力用データ構造体の1要素のサイズ sizeOfを使う</param>
	/// <param name="countOfInputElement">入力用データ構造体の要素数</param>
	/// <param name="inputFormatData">入力用データ構造体のポインタ</param>
	/// <param name="sizeOfOutput">書き込み用データ構造体の1要素のサイズ sizeOfを使う</param>
	/// <param name="countOfOutputElement">書き込み用データ構造体の要素数</param>
	/// <param name="outputFormatData">書き込み用データ構造体のポインタ</param>
	void Init(LPCWSTR CsPath,
		int SizeOfInput, int CountOfInputElement, void* InputFormatData,
		int SizeOfOutput, int CountOfOutputElement, void* OutputFormatData);

	//ディスパッチコール
	void Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ);


	/*-- 機能関数 --*/

	//入力用構造化バッファを更新
	void UpdateInputSB(void* UpdateSB);


private:

	/*-- シェーダー読み込み関係 --*/

	// シェーダー読み込み
	LPD3DBLOB LoadShader(LPCWSTR ShaderFileName, const char EntryPointName[], const char ShaderModel[], ID3DBlob* ShaderBlob, ID3DBlob* ErrorBlob);
	// シェーダーのロードエラーをチェック
	void CheckRoadShaderError(HRESULT Result, ID3DBlob* ErrorBlob);

	/*-- ルートシグネチャ --*/

	// ルートシグネチャの生成
	void GenerateRootSignature();

	// ディスクリプタヒープの要素
	enum {
		DESCRIPTORHEAP_CB,
		DESCRIPTORHEAP_SRV,
		DESCRIPTORHEAP_UAV,
		DESCRIPTORHEAP_NUM
	};


	/*-- パイプライン --*/

	// パイプラインの生成
	void GeneratePipline(Microsoft::WRL::ComPtr<ID3DBlob> csBlob);


	/*-- ディスクリプタヒープ --*/

	// ディスクリプタヒープに各ディスクリプタをセットする
	void CommitDescHeap();

};