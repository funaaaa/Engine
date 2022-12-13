#include "PipelineManager.h"
#include "Pipeline.h"
#include <cassert>

//map<string, Pipeline> PipelineManager::pipline{};

void PipelineManager::GeneratePipline(Pipeline::PIPLINE_ID PiplineID,
	std::string PSname,
	std::string VSname,
	int InputLayoutCount,
	D3D12_INPUT_ELEMENT_DESC* InputLayout,
	int RootparamCount,
	CD3DX12_ROOT_PARAMETER* Rootparam,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE Primitive,
	Pipeline::BLEND_ID BlendID,
	D3D12_CULL_MODE CullMode,
	DXGI_FORMAT DxgiFormat)
{
	//すでに生成してあるリストの中からパイプラインIDが重複していなかったら生成する。
	for (int i = 0; i < piplines_.size(); ++i) {
		if (piplines_[i].GetPiplineID() == PiplineID) {

			OutputDebugString(L"\nError すでに生成してあるパイプラインを生成しようとしました。\n");
			exit(true);

			return;
		}
	}

	//指定されたパイプラインIDごとに生成して最後尾に追加
	piplines_.emplace_back(Pipeline(PiplineID, PSname, VSname, InputLayoutCount, InputLayout, RootparamCount, Rootparam, Primitive, BlendID, CullMode, DxgiFormat));

}

void PipelineManager::GeneratePipline(Pipeline::PIPLINE_ID PiplineID,
	std::string PSname,
	std::string VSname,
	std::string GSname,
	int InputLayoutCount,
	D3D12_INPUT_ELEMENT_DESC* InputLayout,
	int RootparamCount,
	CD3DX12_ROOT_PARAMETER* Rootparam,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE Primitive,
	Pipeline::BLEND_ID BlendID,
	D3D12_CULL_MODE CullMode,
	DXGI_FORMAT DxgiFormat)
{
	//すでに生成してあるリストの中からパイプラインIDが重複していなかったら生成する
	for (int i = 0; i < piplines_.size(); ++i) {
		if (piplines_[i].GetPiplineID() == PiplineID) {

			OutputDebugString(L"\nError すでに生成してあるパイプラインを生成しようとしました。\n");
			exit(true);

			return;
		}
	}

	//指定されたパイプラインIDごとに生成して最後尾に追加
	piplines_.push_back(Pipeline(PiplineID, PSname, VSname, InputLayoutCount, InputLayout, RootparamCount, Rootparam, Primitive, BlendID, CullMode, DxgiFormat));

}

void PipelineManager::GeneratePiplineDepth(Pipeline::PIPLINE_ID PiplineID,
	std::string PSname,
	std::string VSname,
	int InputLayoutCount,
	D3D12_INPUT_ELEMENT_DESC* InputLayout,
	int RootparamCount,
	CD3DX12_ROOT_PARAMETER* Rootparam,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE Primitive,
	Pipeline::BLEND_ID BlendID,
	D3D12_CULL_MODE CullMode,
	DXGI_FORMAT DxgiFormat,
	DXGI_FORMAT DxgiFormat2)
{
	//すでに生成してあるリストの中からパイプラインIDが重複していなかったら生成する
	for (int i = 0; i < piplines_.size(); ++i) {
		if (piplines_[i].GetPiplineID() == PiplineID) {

			OutputDebugString(L"\nError すでに生成してあるパイプラインを生成しようとしました。\n");
			exit(true);

			return;
		}
	}
	DxgiFormat2;
	//指定されたパイプラインIDごとに生成して最後尾に追加
	piplines_.push_back(Pipeline(PiplineID, PSname, VSname, InputLayoutCount, InputLayout, RootparamCount, Rootparam, Primitive, BlendID, CullMode, DxgiFormat));

}

void PipelineManager::SetPipline(int PiplineID)
{
	for (int i = 0; i < piplines_.size(); ++i) {
		if (piplines_.at(i).GetPiplineID() == PiplineID) {
			piplines_.at(i).SetPipline();
			return;
		}
	}

	OutputDebugString(L"\nError 存在していないパイプラインがセットされようとしました。\n");
	assert(0);
}

void PipelineManager::Init()
{
	/*パイプライン用の各要素を生成*/
#pragma region inputlayout
	/*InputLayoutを生成*/
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{	//頂点座標
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	//法線ベクトル
			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	//uv座標
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		}
	};
	D3D12_INPUT_ELEMENT_DESC inputLayoutPos[] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	};
#pragma endregion

#pragma region descrange
	/*ディスクリプタテーブルの生成*/
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);					//t0レジスタ

#pragma region Rootparam
	/*ルートパラメータの生成*/
	CD3DX12_ROOT_PARAMETER const1Tex1[2]{};


	//定数用
	const1Tex1[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	//テクスチャ用
	const1Tex1[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

#pragma endregion

	/*スプライト用のパイプライン*/
	GeneratePipline(Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, "Resource/ShaderFiles/SpritePS.hlsl", "Resource/ShaderFiles/SpriteVS.hlsl", 3, inputLayout, 2, const1Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, Pipeline::BLEND_ID::BLENDMODE_ALPHA);
}
