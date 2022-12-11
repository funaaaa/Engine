#include "Pipeline.h"
#include "ShaderStorage.h"
#include "Engine.h"
#include <DirectXTex/include/d3dx12.h>

Pipeline::Pipeline(
	PIPLINE_ID piplineID_,
	string PSname,
	string VSname,
	int inputLayoutCount,
	D3D12_INPUT_ELEMENT_DESC* inputLayout,
	int rootparamCount_,
	CD3DX12_ROOT_PARAMETER* rootparam_,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_,
	BLEND_ID blendID,
	D3D12_CULL_MODE cullMode,
	DXGI_FORMAT dxgiFormat) : primitive_(primitive_)
{

	// パイプラインIDを保存
	this->piplineID_ = piplineID_;

	//シェーダー読み込み
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = nullptr;		//頂点シェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob = nullptr;		//ピクセルシェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;	//エラーオブジェクト
	//頂点シェーダの読み込みとコンパイル
	vsBlob = ShaderStorage::Ins()->LoadShader(VSname, "vs_5_0", "main");
	//ピクセルシェーダの読み込みとコンパイル
	psBlob = ShaderStorage::Ins()->LoadShader(PSname, "ps_5_0", "main");

	//グラフィックスパイプライン生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
	SetPiplineDesc(gpipeline, inputLayout, inputLayoutCount, rootparam_, rootparamCount_, blendID, cullMode, dxgiFormat);

	Engine::Ins()->device_.dev_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate_));
	pipelinestate_->SetName(L"Pipline");

}

Pipeline::Pipeline(
	PIPLINE_ID piplineID_,
	string PSname,
	string VSname,
	int inputLayoutCount,
	D3D12_INPUT_ELEMENT_DESC* inputLayout,
	int rootparamCount_,
	CD3DX12_ROOT_PARAMETER* rootparam_,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_,
	BLEND_ID blendID,
	bool isMRT,
	D3D12_CULL_MODE cullMode,
	DXGI_FORMAT dxgiFormat,
	DXGI_FORMAT dxgiFormat2) : primitive_(primitive_)
{

	// パイプラインIDを保存
	this->piplineID_ = piplineID_;

	//シェーダー読み込み
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = nullptr;		//頂点シェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob = nullptr;		//ピクセルシェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;	//エラーオブジェクト
	vsBlob = ShaderStorage::Ins()->LoadShader(VSname, "vs_5_0", "main");
	//ピクセルシェーダの読み込みとコンパイル
	psBlob = ShaderStorage::Ins()->LoadShader(PSname, "ps_5_0", "main");

	//グラフィックスパイプライン生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
	SetPiplineDescDepth(gpipeline, inputLayout, inputLayoutCount, rootparam_, rootparamCount_, blendID, cullMode, dxgiFormat, dxgiFormat2);
	isMRT;
	Engine::Ins()->device_.dev_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate_));
}

Pipeline::Pipeline(
	PIPLINE_ID piplineID_,
	string PSname,
	string VSname,
	string GSname,
	int inputLayoutCount,
	D3D12_INPUT_ELEMENT_DESC* inputLayout,
	int rootparamCount_,
	CD3DX12_ROOT_PARAMETER* rootparam_,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_,
	BLEND_ID blendID,
	D3D12_CULL_MODE cullMode,
	DXGI_FORMAT dxgiFormat) : primitive_(primitive_)
{

	// パイプラインIDを保存
	this->piplineID_ = piplineID_;

	//シェーダー読み込み
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = nullptr;		//頂点シェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob = nullptr;		//ピクセルシェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> gsBlob = nullptr;		//ジオメトリシェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;	//エラーオブジェクト
	//頂点シェーダの読み込みとコンパイル
	vsBlob = ShaderStorage::Ins()->LoadShader(VSname, "vs_5_0", "main");
	//ピクセルシェーダの読み込みとコンパイル
	psBlob = ShaderStorage::Ins()->LoadShader(PSname, "ps_5_0", "main");
	//ジオメトリシェーダの読み込みとコンパイル
	gsBlob = ShaderStorage::Ins()->LoadShader(GSname, "gs_5_0", "main");

	//グラフィックスパイプライン生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	SetPiplineDesc(gpipeline, inputLayout, inputLayoutCount, rootparam_, rootparamCount_, blendID, cullMode, dxgiFormat);

	Engine::Ins()->device_.dev_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate_));
}

void Pipeline::SetPipline()
{
	//パイプラインステート設定コマンド
	Engine::Ins()->copyResourceCmdList_->SetPipelineState(pipelinestate_.Get());

	//ルートシグネチャ設定コマンド
	Engine::Ins()->copyResourceCmdList_->SetGraphicsRootSignature(rootsignature_.Get());

	//プリミティブ形状設定コマンド
	if (primitive_ == D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE) {
		Engine::Ins()->copyResourceCmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else if (primitive_ == D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE) {
		Engine::Ins()->copyResourceCmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	}
	else if (primitive_ == D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT) {
		Engine::Ins()->copyResourceCmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	}
}

void Pipeline::SetPiplineDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpipelineDesc, D3D12_INPUT_ELEMENT_DESC* inputLayout, int inputLayoutCount, CD3DX12_ROOT_PARAMETER* rootparam_, int rootparamCount_, BLEND_ID blendID, D3D12_CULL_MODE cullMode, DXGI_FORMAT dxgiFormat)
{
	gpipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;						//標準設定
	gpipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		//背面カリング,ポリゴン内塗りつぶし,深度クリッピングを有効に
	gpipelineDesc.RasterizerState.CullMode = cullMode;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	gpipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = gpipelineDesc.BlendState.RenderTarget[0];
	gpipelineDesc.BlendState.AlphaToCoverageEnable = false;
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.BlendEnable = true;											//ブレンドを有効にする
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;							//加算
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;								//ソースの値を100%使う
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;							//デストの値を0%使う
	//引数として渡された値によってブレンドを変える
	switch (blendID) {
	case BLEND_ID::BLENDMODE_ADD:
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;								//加算
		blendDesc.SrcBlend = D3D12_BLEND_ONE;								//ソースの値を100%使う
		blendDesc.DestBlend = D3D12_BLEND_ONE;								//ソースの値を100%使う
		break;
	case BLEND_ID::BLENDMODE_ALPHA:
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;								//加算
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;							//ソースの値を100%使う
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;					//ソースの値を100%使う
		break;
	default:
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;								//加算
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;							//ソースの値を100%使う
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;					//ソースの値を100%使う
	}
	gpipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	gpipelineDesc.InputLayout.NumElements = inputLayoutCount;
	gpipelineDesc.PrimitiveTopologyType = primitive_;
	gpipelineDesc.NumRenderTargets = 1;											//描画対象は1つ
	gpipelineDesc.RTVFormats[0] = dxgiFormat;									//0～255指定のRGBA
	gpipelineDesc.SampleDesc.Count = 1;											//1ピクセルにつき1回サンプリング

	//デプスステンシルステートの設定
	gpipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);//深度テストを行う,書き込み許可,深度が小さければ合格
	gpipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//テクスチャサンプラーのセット
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(rootparamCount_, rootparam_, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> rootSigBlob = {};
	//バージョン自動判定でのシリアライズ
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;	//エラーオブジェクト
	HRESULT result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//ルートシグネチャの生成
	result = Engine::Ins()->device_.dev_->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature_));

	//パイプラインにルートシグネチャをセット
	gpipelineDesc.pRootSignature = rootsignature_.Get();
}

void Pipeline::SetPiplineDescDepth(D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpipelineDesc, D3D12_INPUT_ELEMENT_DESC* inputLayout, int inputLayoutCount, CD3DX12_ROOT_PARAMETER* rootparam_, int rootparamCount_, BLEND_ID blendID, D3D12_CULL_MODE cullMode, DXGI_FORMAT dxgiFormat, DXGI_FORMAT dxgiFormat2)
{
	gpipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;						//標準設定
	gpipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		//背面カリング,ポリゴン内塗りつぶし,深度クリッピングを有効に
	gpipelineDesc.RasterizerState.CullMode = cullMode;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	gpipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = gpipelineDesc.BlendState.RenderTarget[0];
	gpipelineDesc.BlendState.AlphaToCoverageEnable = false;
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.BlendEnable = true;											//ブレンドを有効にする
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;							//加算
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;								//ソースの値を100%使う
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;							//デストの値を0%使う
	//引数として渡された値によってブレンドを変える
	switch (blendID) {
	case BLENDMODE_ADD:
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;								//加算
		blendDesc.SrcBlend = D3D12_BLEND_ONE;								//ソースの値を100%使う
		blendDesc.DestBlend = D3D12_BLEND_ONE;								//ソースの値を100%使う
		break;
	case BLENDMODE_ALPHA:
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;								//加算
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;							//ソースの値を100%使う
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;					//ソースの値を100%使う
		break;
	default:
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;								//加算
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;							//ソースの値を100%使う
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;					//ソースの値を100%使う
	}
	gpipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	gpipelineDesc.InputLayout.NumElements = inputLayoutCount;
	gpipelineDesc.PrimitiveTopologyType = primitive_;
	gpipelineDesc.NumRenderTargets = 2;											//描画対象は1つ
	gpipelineDesc.RTVFormats[0] = dxgiFormat;									//0～255指定のRGBA
	gpipelineDesc.RTVFormats[1] = dxgiFormat2;									//0～255指定のRGBA
	gpipelineDesc.SampleDesc.Count = 1;											//1ピクセルにつき1回サンプリング

	//デプスステンシルステートの設定
	gpipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);//深度テストを行う,書き込み許可,深度が小さければ合格
	gpipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//テクスチャサンプラーのセット
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(rootparamCount_, rootparam_, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> rootSigBlob = {};
	//バージョン自動判定でのシリアライズ
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;	//エラーオブジェクト
	HRESULT result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//ルートシグネチャの生成
	result = Engine::Ins()->device_.dev_->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature_));

	//パイプラインにルートシグネチャをセット
	gpipelineDesc.pRootSignature = rootsignature_.Get();
}