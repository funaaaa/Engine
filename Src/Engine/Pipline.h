#pragma once
#include "ShaderStorage.h"
#include "Enum.h"
#include <DirectXTex/d3dx12.h>

class Pipline {
private:
	ComPtr<ID3D12PipelineState> pipelinestate;		// パイプラインステート
	ComPtr<ID3D12RootSignature> rootsignature;		// ルートシグネチャ
	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive;		// プリミティブ
	PiplineID piplineID;							// パイプラインのID

public:
	Pipline() {};
	///<summary>
	///コンストラクタ
	///</summary>
	///<param name="PSname"><ロードするピクセルシェーダーのファイル名/param>
	///<param name="VSname"><ロードする頂点シェーダーのファイル名/param>
	///<param name="inputLayoutCount"><インプットレイアウトの数/param>
	///<param name="inputLayout"><インプットレイアウト/param>
	///<param name="rootparamCount"><ルートパラメーター数/param>
	///<param name="rootparam"><ルートパラメーター/param>
	///<param name="primitive"><プリミティブ形状param>
	///<param name="blendID"><どのブレンドをするかのID/param>
	Pipline(
		PiplineID piplineID,
		string PSname,
		string VSname,
		int inputLayoutCount,
		D3D12_INPUT_ELEMENT_DESC* inputLayout,
		int rootparamCount,
		CD3DX12_ROOT_PARAMETER* rootparam,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive,
		int blendID,
		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK,
		DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM);

	Pipline(
		PiplineID piplineID,
		string PSname,
		string VSname,
		int inputLayoutCount,
		D3D12_INPUT_ELEMENT_DESC* inputLayout,
		int rootparamCount,
		CD3DX12_ROOT_PARAMETER* rootparam,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive,
		int blendID,
		bool isMRT,
		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK,
		DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT dxgiFormat2 = DXGI_FORMAT_R8_UNORM);

	Pipline(
		PiplineID piplineID,
		string PSname,
		string VSname,
		string GSname,
		int inputLayoutCount,
		D3D12_INPUT_ELEMENT_DESC* inputLayout,
		int rootparamCount,
		CD3DX12_ROOT_PARAMETER* rootparam,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive,
		int blendID,
		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK,
		DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM);

	//パイプラインのセット
	void SetPipline();

	//パイプラインIDを取得
	PiplineID GetPiplineID() { return piplineID; }

	//パイプライン設定
	void SetPiplineDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpipelineDesc, D3D12_INPUT_ELEMENT_DESC* inputLayout, int inputLayoutCount, CD3DX12_ROOT_PARAMETER* rootparam, int rootparamCount, int blendID, D3D12_CULL_MODE cullMode, DXGI_FORMAT dxgiFormat);
	void SetPiplineDescDepth(D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpipelineDesc, D3D12_INPUT_ELEMENT_DESC* inputLayout, int inputLayoutCount, CD3DX12_ROOT_PARAMETER* rootparam, int rootparamCount, int blendID, D3D12_CULL_MODE cullMode, DXGI_FORMAT dxgiFormat, DXGI_FORMAT dxgiFormat2);
};