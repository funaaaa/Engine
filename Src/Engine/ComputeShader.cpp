#include "ComputeShader.h"
#include "StructuredBuffer.h"
#include "RWStructuredBuffer.h"
#include "Engine.h"
#include <assert.h>

#include <d3dcompiler.h>

ComputeShader::ComputeShader()
{
	// 生成
	inputSB_ = std::make_shared<StructuredBuffer>();
	outputSB_ = std::make_shared<RWStructuredBuffer>();
}

ComputeShader::~ComputeShader()
{
}

void ComputeShader::Init(LPCWSTR CsPath,
	int SizeOfInput, int CountOfInputElement, void* InputFormatData,
	int SizeOfOutput, int CountOfOutputElement, void* OutputFormatData)
{

	/*-- コンピュートシェーダーをロード --*/

	Microsoft::WRL::ComPtr<ID3DBlob> csBlob = nullptr;		//コンピュートシェーダーオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;	//エラーオブジェクト

	//コンピュートシェーダーェーダの読み込みとコンパイル
	csBlob = LoadShader(CsPath, "main", "cs_5_0", csBlob.Get(), errorBlob.Get());


	/*-- ルートシグネチャを生成 --*/

	GenerateRootSignature();


	/*-- パイプラインを生成 --*/

	GeneratePipline(csBlob);


	/*-- 入力用データ構造体を生成 --*/

	inputSB_->Init(SizeOfInput, CountOfInputElement, InputFormatData);


	/*-- 出力用データ構造体を生成 --*/

	outputSB_->Init(SizeOfOutput, CountOfOutputElement, OutputFormatData);


	/*-- ディスクリプタヒープに登録する --*/

	CommitDescHeap();

}

void ComputeShader::Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ)
{

	/*-- ルートシグネチャをセット --*/

	Engine::Ins()->mainGraphicsCmdList_->SetComputeRootSignature(rootSignature_.Get());


	/*-- パイプラインをセット --*/

	Engine::Ins()->mainGraphicsCmdList_->SetPipelineState(pipeline_.Get());


	/*-- ディスクリプタヒープをセット --*/

	ID3D12DescriptorHeap* descHeapBuff[1] = { descHeap_.Get() };
	Engine::Ins()->mainGraphicsCmdList_->SetDescriptorHeaps(1, descHeapBuff);

	//ディスクリプタテーブルに登録する。
	Engine::Ins()->mainGraphicsCmdList_->SetComputeRootDescriptorTable(1, srvGPUDescHeapHandle_);
	Engine::Ins()->mainGraphicsCmdList_->SetComputeRootDescriptorTable(2, uavGPUDescHeapHandle_);


	/*-- ディスパッチ --*/

	Engine::Ins()->mainGraphicsCmdList_->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);

}

LPD3DBLOB ComputeShader::LoadShader(LPCWSTR ShaderFileName, const char EntryPointName[], const char ShaderModel[], ID3DBlob* ShaderBlob, ID3DBlob* ErrorBlob)
{
	HRESULT result = D3DCompileFromFile(
		ShaderFileName,										// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					// インクルード可能にする
		EntryPointName, ShaderModel,						// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	// デバッグ用設定
		0,
		&ShaderBlob, &ErrorBlob);

	CheckRoadShaderError(result, ErrorBlob);

	return ShaderBlob;
}

void ComputeShader::CheckRoadShaderError(HRESULT Result, ID3DBlob* ErrorBlob)
{
	if (FAILED(Result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(ErrorBlob->GetBufferSize());

		std::copy_n((char*)ErrorBlob->GetBufferPointer(),
			ErrorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}
}

void ComputeShader::GenerateRootSignature()
{

	/*-- サンプラーの設定 --*/
	D3D12_STATIC_SAMPLER_DESC sampler_ = {};
	sampler_.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler_.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler_.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler_.MipLODBias = 0;
	sampler_.MaxAnisotropy = 0;
	sampler_.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler_.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	sampler_.MinLOD = 0.0f;
	sampler_.MaxLOD = D3D12_FLOAT32_MAX;
	sampler_.ShaderRegister = 0;
	sampler_.RegisterSpace = 0;
	sampler_.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	/*-- ルートシグネチャの設定 --*/

	CD3DX12_DESCRIPTOR_RANGE1 ranges[DESCRIPTORHEAP_NUM];
	CD3DX12_ROOT_PARAMETER1 rootParameters[DESCRIPTORHEAP_NUM];

	ranges[DESCRIPTORHEAP_CB].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 8, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	rootParameters[DESCRIPTORHEAP_CB].InitAsDescriptorTable(1, &ranges[DESCRIPTORHEAP_CB]);

	ranges[DESCRIPTORHEAP_SRV].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 32, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	rootParameters[DESCRIPTORHEAP_SRV].InitAsDescriptorTable(1, &ranges[DESCRIPTORHEAP_SRV]);

	ranges[DESCRIPTORHEAP_UAV].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 8, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	rootParameters[DESCRIPTORHEAP_UAV].InitAsDescriptorTable(1, &ranges[DESCRIPTORHEAP_UAV]);

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler_, rootSignatureFlags);
	Microsoft::WRL::ComPtr<ID3DBlob> signature;
	Microsoft::WRL::ComPtr<ID3DBlob> error;
	D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	auto hr = Engine::Ins()->device_.dev_->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	if (FAILED(hr)) {
		// ルートシグネチャの作成に失敗した。
		assert(0);
	}

}

void ComputeShader::GeneratePipline(Microsoft::WRL::ComPtr<ID3DBlob> csBlob)
{

	// パイプラインステートを作成
	D3D12_COMPUTE_PIPELINE_STATE_DESC  psoDesc = { 0 };
	psoDesc.pRootSignature = rootSignature_.Get();
	psoDesc.CS = CD3DX12_SHADER_BYTECODE(csBlob.Get());
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	psoDesc.NodeMask = 0;

	auto hr = Engine::Ins()->device_.dev_->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pipeline_));
	if (FAILED(hr)) {
		// 生成に失敗した
		assert(0);
	}

}

void ComputeShader::CommitDescHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

	// srvの設定
	srvHeapDesc.NumDescriptors = 5;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	// ディスクリプタヒープを生成
	auto hr = Engine::Ins()->device_.dev_->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&descHeap_));
	if (FAILED(hr)) {
		MessageBox(nullptr, L"DescriptorHeap::Commit ディスクリプタヒープの作成に失敗しました。", L"エラー", MB_OK);
		std::abort();
	}

	// 定数バッファやシェーダーリソースのディスクリプタをヒープに書き込んでいく。

	// ハンドルの先頭を取得
	auto cpuHandle = descHeap_->GetCPUDescriptorHandleForHeapStart();
	auto gpuHandle = descHeap_->GetGPUDescriptorHandleForHeapStart();

	// シェーダーリソースを登録する
	inputSB_->RegistShaderResourceView(cpuHandle);

	// 次に進める。
	cpuHandle.ptr += Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 続いてUAV。
	outputSB_->RegistUnorderAccessView(cpuHandle);

	// 次に進める。
	cpuHandle.ptr += Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// シェーダーリソースのディスクリプタヒープの開始ハンドルを計算
	srvGPUDescHeapHandle_ = gpuHandle;
	gpuHandle.ptr += (UINT64)Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// UAVリソースのディスクリプタヒープの開始ハンドルを計算
	uavGPUDescHeapHandle_ = gpuHandle;

}

void ComputeShader::UpdateInputSB(void* updateSB)
{
	inputSB_->Update(updateSB);
}
