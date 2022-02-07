#pragma once
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <DirectXTex/d3dx12.h>
#include "Singleton.h"
#include "Enum.h"
#include "Pipline.h"

using namespace std;

class PiplineManager :public Singleton<PiplineManager> {
private:
	//static map<string, Pipline> pipline;				//パイプライン
	vector<Pipline> piplines;			//パイプライン
	friend Singleton<PiplineManager>;
	//コンストラクタ
	PiplineManager() {};

public:
	/// <summary>
	/// パイプラインを生成する処理
	/// </summary>
	/// <param name="piplineID">生成するパイプラインのID</param>
	/// <param name="PSname">使用するピクセルシェーダーのファイル名</param>
	/// <param name="VSname">使用する頂点シェーダーのファイル名</param>
	/// <param name="inputLayoutCount">インプットレイアウトの数</param>
	/// <param name="inputLayout">インプットレイアウト</param>
	/// <param name="rootparamCount">ルートパラメーターの数</param>
	/// <param name="rootparam">ルートパラメーター</param>
	/// <param name="primitive">プリミティブ形状</param>
	/// <param name="blendID">ブレンドのID</param>
	void GeneratePipline(PiplineID piplineID,
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
	/// <summary>
	/// パイプラインを生成する処理
	/// </summary>
	/// <param name="piplineID">生成するパイプラインのID</param>
	/// <param name="PSname">使用するピクセルシェーダーのファイル名</param>
	/// <param name="VSname">使用する頂点シェーダーのファイル名</param>
	/// <param name="GSname">使用するジオメトリシェーダーのファイル名</param>
	/// <param name="inputLayoutCount">インプットレイアウトの数</param>
	/// <param name="inputLayout">インプットレイアウト</param>
	/// <param name="rootparamCount">ルートパラメーターの数</param>
	/// <param name="rootparam">ルートパラメーター</param>
	/// <param name="primitive">プリミティブ形状</param>
	/// <param name="blendID">ブレンドのID</param>
	void GeneratePipline(PiplineID piplineID,
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

	void GeneratePiplineDepth(PiplineID piplineID,
		string PSname,
		string VSname,
		int inputLayoutCount,
		D3D12_INPUT_ELEMENT_DESC* inputLayout,
		int rootparamCount,
		CD3DX12_ROOT_PARAMETER* rootparam,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive,
		int blendID,
		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK,
		DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT dxgiFormat2 = DXGI_FORMAT_R8_UNORM);


	//パイプランをセットする
	void SetPipline(int piplineID);

	//ルートパラメーターを生成
	void Init();
};