#pragma once
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <DirectXTex/d3dx12.h>
#include "Singleton.h"
#include "Pipline.h"

// パイプライン管理クラス
// 以前オブジェクト等を描画するパイプラインを管理するために使用していたが、レイトレ用で新しくパイプラインを作ったのでこのクラスではスプライト関係のパイプラインを管理するのみとなっています。
class PiplineManager :public Singleton<PiplineManager> {
public:

	std::vector<Pipline> piplines_;	// パイプライン
	// コンストラクタ
	PiplineManager() {};

public:
	/// <summary>
	/// パイプラインを生成する処理
	/// </summary>
	/// <param name="PiplineID">生成するパイプラインのID</param>
	/// <param name="PSname">使用するピクセルシェーダーのファイル名</param>
	/// <param name="VSname">使用する頂点シェーダーのファイル名</param>
	/// <param name="InputLayoutCount">インプットレイアウトの数</param>
	/// <param name="InputLayout">インプットレイアウト</param>
	/// <param name="RootparamCount">ルートパラメーターの数</param>
	/// <param name="Rootparam">ルートパラメーター</param>
	/// <param name="Primitive">プリミティブ形状</param>
	/// <param name="BlendID">ブレンドのID</param>
	void GeneratePipline(Pipline::PIPLINE_ID PiplineID,
		std::string PSname,
		std::string VSname,
		int InputLayoutCount,
		D3D12_INPUT_ELEMENT_DESC* InputLayout,
		int RootparamCount,
		CD3DX12_ROOT_PARAMETER* Rootparam,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE Primitive,
		Pipline::BLEND_ID BlendID,
		D3D12_CULL_MODE CullMode = D3D12_CULL_MODE_BACK,
		DXGI_FORMAT DxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM);
	/// <summary>
	/// パイプラインを生成する処理
	/// </summary>
	/// <param name="PiplineID">生成するパイプラインのID</param>
	/// <param name="PSname">使用するピクセルシェーダーのファイル名</param>
	/// <param name="VSname">使用する頂点シェーダーのファイル名</param>
	/// <param name="GSname">使用するジオメトリシェーダーのファイル名</param>
	/// <param name="InputLayoutCount">インプットレイアウトの数</param>
	/// <param name="InputLayout">インプットレイアウト</param>
	/// <param name="RootparamCount">ルートパラメーターの数</param>
	/// <param name="Rootparam">ルートパラメーター</param>
	/// <param name="Primitive">プリミティブ形状</param>
	/// <param name="BlendID">ブレンドのID</param>
	void GeneratePipline(Pipline::PIPLINE_ID PiplineID,
		std::string PSname,
		std::string VSname,
		std::string GSname,
		int InputLayoutCount,
		D3D12_INPUT_ELEMENT_DESC* InputLayout,
		int RootparamCount,
		CD3DX12_ROOT_PARAMETER* Rootparam,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE Primitive,
		Pipline::BLEND_ID BlendID,
		D3D12_CULL_MODE CullMode = D3D12_CULL_MODE_BACK,
		DXGI_FORMAT DxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM);

	void GeneratePiplineDepth(Pipline::PIPLINE_ID PiplineID,
		std::string PSname,
		std::string VSname,
		int InputLayoutCount,
		D3D12_INPUT_ELEMENT_DESC* InputLayout,
		int RootparamCount,
		CD3DX12_ROOT_PARAMETER* Rootparam,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE Primitive,
		Pipline::BLEND_ID BlendID,
		D3D12_CULL_MODE CullMode = D3D12_CULL_MODE_BACK,
		DXGI_FORMAT DxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT DxgiFormat2 = DXGI_FORMAT_R8_UNORM);


	// パイプランをセットする
	void SetPipline(int PiplineID);

	// パイプライン関係を生成する。
	void Init();
};