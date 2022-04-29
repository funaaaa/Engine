#include "ComputeShader.h"
#include "StructuredBuffer.h"
#include "RWStructuredBuffer.h"
#include "DirectXBase.h"

#include <d3dcompiler.h>

using namespace Microsoft::WRL;

#pragma comment(lib, "d3dcompiler.lib")

ComputeShader::ComputeShader()
{
	//生成
	inputSB = new StructuredBuffer();
	outputSB = new RWStructuredBuffer();
}

ComputeShader::~ComputeShader()
{
	//解放
	delete inputSB;
	delete outputSB;
}

void ComputeShader::Init(LPCWSTR csPath,
	int sizeOfInput, int countOfInputElement, void* inputFormatData,
	int sizeOfOutput, int countOfOutputElement, void* outputFormatData)
{

	/*-- コンピュートシェーダーをロード --*/

	ComPtr<ID3DBlob> csBlob = nullptr;		//コンピュートシェーダーオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr;	//エラーオブジェクト

	//コンピュートシェーダーェーダの読み込みとコンパイル
	csBlob = LoadShader(csPath, "main", "cs_5_0", csBlob.Get(), errorBlob.Get());


	/*-- ルートシグネチャを生成 --*/

	GenerateRootSignature();


	/*-- パイプラインを生成 --*/

	GeneratePipline(csBlob);


	/*-- 入力用データ構造体を生成 --*/

	inputSB->Init(sizeOfInput, countOfInputElement, inputFormatData);


	/*-- 出力用データ構造体を生成 --*/

	outputSB->Init(sizeOfOutput, countOfOutputElement, outputFormatData);


	/*-- ディスクリプタヒープに登録する --*/

	CommitDescHeap();

}

void ComputeShader::Dispatch(UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ)
{

	/*-- ルートシグネチャをセット --*/

	DirectXBase::Ins()->cmdList->SetComputeRootSignature(rootSignature.Get());


	/*-- パイプラインをセット --*/

	DirectXBase::Ins()->cmdList->SetPipelineState(pipline.Get());


	/*-- ディスクリプタヒープをセット --*/

	ID3D12DescriptorHeap* descHeapBuff[1] = { descHeap.Get() };
	DirectXBase::Ins()->cmdList->SetDescriptorHeaps(1, descHeapBuff);

	//ディスクリプタテーブルに登録する。
	DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(1, srvGPUDescHeapHandle);
	DirectXBase::Ins()->cmdList->SetComputeRootDescriptorTable(2, uavGPUDescHeapHandle);


	/*-- ディスパッチ --*/

	DirectXBase::Ins()->cmdList->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);

}

LPD3DBLOB ComputeShader::LoadShader(LPCWSTR shaderFileName, const char entryPointName[], const char shaderModel[], ID3DBlob* shaderBlob, ID3DBlob* errorBlob)
{
	HRESULT result = D3DCompileFromFile(
		shaderFileName,										//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード可能にする
		entryPointName, shaderModel,						//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバッグ用設定
		0,
		&shaderBlob, &errorBlob);

	CheckRoadShaderError(result, errorBlob);

	return shaderBlob;
}

void ComputeShader::CheckRoadShaderError(HRESULT result, ID3DBlob* errorBlob)
{
	if (FAILED(result)) {
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

void ComputeShader::GenerateRootSignature()
{

	/*-- サンプラーの設定 --*/
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	/*-- ルートシグネチャの設定 --*/

	CD3DX12_DESCRIPTOR_RANGE1 ranges[enNumDescriptorHeap];
	CD3DX12_ROOT_PARAMETER1 rootParameters[enNumDescriptorHeap];

	ranges[enDescriptorHeap_CB].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 8, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	rootParameters[enDescriptorHeap_CB].InitAsDescriptorTable(1, &ranges[enDescriptorHeap_CB]);

	ranges[enDescriptorHeap_SRV].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 32, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	rootParameters[enDescriptorHeap_SRV].InitAsDescriptorTable(1, &ranges[enDescriptorHeap_SRV]);

	ranges[enDescriptorHeap_UAV].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 8, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	rootParameters[enDescriptorHeap_UAV].InitAsDescriptorTable(1, &ranges[enDescriptorHeap_UAV]);

	// Allow input layout and deny uneccessary access to certain pipeline stages.
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);
	Microsoft::WRL::ComPtr<ID3DBlob> signature;
	Microsoft::WRL::ComPtr<ID3DBlob> error;
	D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	auto hr = DirectXBase::Ins()->dev->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	if (FAILED(hr)) {
		//ルートシグネチャの作成に失敗した。
		assert(0);
	}

}

void ComputeShader::GeneratePipline(Microsoft::WRL::ComPtr<ID3DBlob> csBlob)
{

	//パイプラインステートを作成
	D3D12_COMPUTE_PIPELINE_STATE_DESC  psoDesc = { 0 };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.CS = CD3DX12_SHADER_BYTECODE(csBlob.Get());
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	psoDesc.NodeMask = 0;

	auto hr = DirectXBase::Ins()->dev->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pipline));
	if (FAILED(hr)) {
		//生成に失敗した
		assert(0);
	}

}

void ComputeShader::CommitDescHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

	//srvの設定
	srvHeapDesc.NumDescriptors = 5;			//srv(入力用)とuav(書き込み用)の2つ
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	//ディスクリプタヒープを生成
	auto hr = DirectXBase::Ins()->dev->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&descHeap));
	if (FAILED(hr)) {
		MessageBox(nullptr, L"DescriptorHeap::Commit ディスクリプタヒープの作成に失敗しました。", L"エラー", MB_OK);
		std::abort();
	}

	//定数バッファやシェーダーリソースのディスクリプタをヒープに書き込んでいく。

	//ハンドルの先頭を取得
	auto cpuHandle = descHeap->GetCPUDescriptorHandleForHeapStart();
	auto gpuHandle = descHeap->GetGPUDescriptorHandleForHeapStart();

	//シェーダーリソースを登録する
	inputSB->RegistShaderResourceView(cpuHandle, 0);

	//次に進める。
	cpuHandle.ptr += DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//続いてUAV。
	outputSB->RegistUnorderAccessView(cpuHandle, 0);

	//次に進める。
	cpuHandle.ptr += DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//シェーダーリソースのディスクリプタヒープの開始ハンドルを計算
	srvGPUDescHeapHandle = gpuHandle;
	gpuHandle.ptr += (UINT64)DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//UAVリソースのディスクリプタヒープの開始ハンドルを計算
	uavGPUDescHeapHandle = gpuHandle;

}

void ComputeShader::UpdateInputSB(void* updateSB)
{
	inputSB->Update(updateSB);
}
