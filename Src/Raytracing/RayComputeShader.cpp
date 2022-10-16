#include "RayComputeShader.h"
#include <d3dcompiler.h>
#include "Engine.h"
#include "DescriptorHeapMgr.h"
#include "RayRootsignature.h"
#include <assert.h>

void RayComputeShader::Setting(LPCWSTR CsPath, int SRVCount, int CBVCount, int UAVCount, std::vector<int> UAVIndex)
{

	/*===== セッティング処理 =====*/

	Microsoft::WRL::ComPtr<ID3DBlob> csBlob = nullptr;		//コンピュートシェーダーオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;	//エラーオブジェクト

	//コンピュートシェーダーェーダの読み込みとコンパイル
	csBlob = LoadShader(CsPath, "main", "cs_5_0", csBlob.Get(), errorBlob.Get());

	// 引数で渡された情報を保存。
	inputSRVCount_ = SRVCount;
	inputCBVCount_ = CBVCount;
	inputUAVCount_ = UAVCount;
	inputUAVIndex_ = UAVIndex;

	// ルートシグネチャを生成。
	rootSignature_ = std::make_shared<RayRootsignature>();

	// ルートシグネチャパラメーターを設定。
	for (int index = 0; index < SRVCount; ++index) {
		rootSignature_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, index);
	}
	for (int index = 0; index < CBVCount; ++index) {
		rootSignature_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, index);
	}
	for (int index = 0; index < UAVCount + 1; ++index) {	// +1しているのは出力用のデータがUAVだから
		rootSignature_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, index);
	}

	// ルートシグネチャを生成。
	rootSignature_->Create(false, L"RayComputeShader");

	// パイプラインを設定。
	D3D12_COMPUTE_PIPELINE_STATE_DESC  psoDesc = { 0 };
	psoDesc.pRootSignature = rootSignature_->GetRootSig().Get();
	psoDesc.CS = CD3DX12_SHADER_BYTECODE(csBlob.Get());
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	psoDesc.NodeMask = 0;

	auto hr = Engine::Ins()->dev_->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pipeline_));
	if (FAILED(hr)) {
		//生成に失敗した
		assert(0);
	}

}

void RayComputeShader::Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ, int OutputIndex, std::vector<D3D12_GPU_VIRTUAL_ADDRESS> InputCBV)
{

	/*===== 実行処理 =====*/

	// ルートシグネチャをセット。
	Engine::Ins()->cmdList_->SetComputeRootSignature(rootSignature_->GetRootSig().Get());

	// パイプラインをセット。
	Engine::Ins()->cmdList_->SetPipelineState(pipeline_.Get());

	// 一応UAVをセット。
	for (auto& index_ : inputUAVIndex_) {

		Engine::Ins()->cmdList_->SetComputeRootDescriptorTable(static_cast<int>(&index_ - &inputUAVIndex_[0]) + inputCBVCount_, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(index_));

	}

	// バックバッファのフレームを取得。
	//auto frameIndex = Engine::Ins()->swapchain->GetCurrentBackBufferIndex();

	// CBVをセット。
	for (auto& index_ : InputCBV) {

		Engine::Ins()->cmdList_->SetComputeRootConstantBufferView(static_cast<int>(&index_ - &InputCBV[0]), index_);

	}

	// 出力用UAVをセット。
	Engine::Ins()->cmdList_->SetComputeRootDescriptorTable(inputUAVCount_ + inputCBVCount_, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(OutputIndex));

	// ディスパッチ。
	Engine::Ins()->cmdList_->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);

}

LPD3DBLOB RayComputeShader::LoadShader(LPCWSTR ShaderFileName, const std::string& EntryPointName, const std::string& ShaderModel, ID3DBlob* ShaderBlob, ID3DBlob* ErrorBlob)
{

	/*===== シェーダー読み込み =====*/

	HRESULT result = D3DCompileFromFile(
		ShaderFileName,										// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					// インクルード可能にする
		EntryPointName.c_str(), ShaderModel.c_str(),		// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	// デバッグ用設定
		0,
		&ShaderBlob, &ErrorBlob);

	// エラーチェック
	CheckRoadShaderError(result, ErrorBlob);

	return ShaderBlob;
}

void RayComputeShader::CheckRoadShaderError(HRESULT Result, ID3DBlob* ErrorBlob)
{

	/*===== シェーダー読み込み時のエラーをチェック =====*/

	if (FAILED(Result)) {

		//errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(ErrorBlob->GetBufferSize());

		std::copy_n((char*)ErrorBlob->GetBufferPointer(),
			ErrorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

}
