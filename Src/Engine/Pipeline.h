#pragma once
#include "ShaderStorage.h"
#include <DirectXTex/include/d3dx12.h>
#include <wrl.h>

// パイプラインクラス
// 以前はオブジェクト等を描画するために使用していましたが、新たにレイトレ用のパイプラインクラスを作ったので現在はスプライト関係の処理を行うのみとなっています。
// PiplineMgrがこのクラスを管理しています。
class Pipeline {

public:

	// 使用するパイプラインのID
	enum PIPLINE_ID {

		PIPLINE_SPRITE_ALPHA

	};
	// 使用するブレンドモード
	enum BLEND_ID {

		BLENDMODE_ALPHA,
		BLENDMODE_ADD,

	};
	// 使用するカメラ定数バッファの種類
	enum PROJECTIONID {

		UI,
		OBJECT,
		BILLBOARD,

	};

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelinestate_;		// パイプラインステート
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootsignature_;		// ルートシグネチャ
	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_;		// プリミティブ
	PIPLINE_ID piplineID_;							// パイプラインのID

	using string = std::string;

public:
	Pipeline() {};
	///<summary>
	/// コンストラクタ
	///</summary>
	///<param name="PSname"><ロードするピクセルシェーダーのファイル名/param>
	///<param name="VSname"><ロードする頂点シェーダーのファイル名/param>
	///<param name="inputLayoutCount"><インプットレイアウトの数/param>
	///<param name="inputLayout"><インプットレイアウト/param>
	///<param name="rootparamCount"><ルートパラメーター数/param>
	///<param name="rootparam"><ルートパラメーター/param>
	///<param name="primitive"><プリミティブ形状param>
	///<param name="blendID"><どのブレンドをするかのID/param>
	Pipeline(
		PIPLINE_ID piplineID_,
		string PSname,
		string VSname,
		int inputLayoutCount,
		D3D12_INPUT_ELEMENT_DESC* inputLayout,
		int rootparamCount_,
		CD3DX12_ROOT_PARAMETER* rootparam_,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_,
		BLEND_ID blendID,
		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK,
		DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM);

	Pipeline(
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
		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK,
		DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT dxgiFormat2 = DXGI_FORMAT_R8_UNORM);

	Pipeline(
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
		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK,
		DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM);

	// パイプラインのセット
	void SetPipline();

	// パイプラインIDを取得
	PIPLINE_ID GetPiplineID() { return piplineID_; }

	// パイプライン設定
	void SetPiplineDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpipelineDesc, D3D12_INPUT_ELEMENT_DESC* inputLayout, int inputLayoutCount, CD3DX12_ROOT_PARAMETER* rootparam_, int rootparamCount_, BLEND_ID blendID, D3D12_CULL_MODE cullMode, DXGI_FORMAT dxgiFormat);
	void SetPiplineDescDepth(D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpipelineDesc, D3D12_INPUT_ELEMENT_DESC* inputLayout, int inputLayoutCount, CD3DX12_ROOT_PARAMETER* rootparam_, int rootparamCount_, BLEND_ID blendID, D3D12_CULL_MODE cullMode, DXGI_FORMAT dxgiFormat, DXGI_FORMAT dxgiFormat2);
};