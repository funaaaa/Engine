#pragma once
#include <wrl.h>
#include <DirectXTex/d3dx12.h>

class StructuredBuffer;
class RWStructuredBuffer;

//コンピュートシェーダークラス
class ComputeShader {

private:

	/*-- メンバ変数 --*/

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipline;		//パイプライン
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;		//ディスクリプタヒープ 情報とかが保存されているのはinputSBとoutputSBの中
	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUDescHeapHandle;			//SRV開始ハンドル GPU
	D3D12_GPU_DESCRIPTOR_HANDLE uavGPUDescHeapHandle;			//UAV開始ハンドル GPU

public:

	StructuredBuffer* inputSB;									//入力用構造体バッファ
	RWStructuredBuffer* outputSB;								//書き込み用構造体バッファ


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
	void Init(LPCWSTR csPath,
		int sizeOfInput, int countOfInputElement, void* inputFormatData,
		int sizeOfOutput, int countOfOutputElement, void* outputFormatData);

	//ディスパッチコール
	void Dispatch(UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ);


	/*-- シェーダー読み込み関係 --*/

	//シェーダー読み込み
	LPD3DBLOB LoadShader(LPCWSTR shaderFileName, const char entryPointName[], const char shaderModel[], ID3DBlob* shaderBlob, ID3DBlob* errorBlob);
	//シェーダーのロードエラーをチェック
	void CheckRoadShaderError(HRESULT result, ID3DBlob* errorBlob);


	/*-- ルートシグネチャ --*/

	//ルートシグネチャの生成
	void GenerateRootSignature();

	//ディスクリプタヒープの要素
	enum {
		enDescriptorHeap_CB,
		enDescriptorHeap_SRV,
		enDescriptorHeap_UAV,
		enNumDescriptorHeap
	};


	/*-- パイプライン --*/

	//パイプラインの生成
	void GeneratePipline(Microsoft::WRL::ComPtr<ID3DBlob> csBlob);


	/*-- ディスクリプタヒープ --*/

	//ディスクリプタヒープに各ディスクリプタをセットする
	void CommitDescHeap();


	/*-- 機能関数 --*/

	//入力用構造化バッファを更新
	void UpdateInputSB(void* updateSB);

};