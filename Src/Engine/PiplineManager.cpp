#include "PiplineManager.h"
#include "Pipline.h"
#include <cassert>

//map<string, Pipline> PiplineManager::pipline{};

void PiplineManager::GeneratePipline(Pipline::PIPLINE_ID PiplineID,
	std::string PSname,
	std::string VSname,
	int InputLayoutCount,
	D3D12_INPUT_ELEMENT_DESC* InputLayout,
	int RootparamCount,
	CD3DX12_ROOT_PARAMETER* Rootparam,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE Primitive,
	Pipline::BLEND_ID BlendID,
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
	piplines_.emplace_back(Pipline(PiplineID, PSname, VSname, InputLayoutCount, InputLayout, RootparamCount, Rootparam, Primitive, BlendID, CullMode, DxgiFormat));

}

void PiplineManager::GeneratePipline(Pipline::PIPLINE_ID PiplineID,
	std::string PSname,
	std::string VSname,
	std::string GSname,
	int InputLayoutCount,
	D3D12_INPUT_ELEMENT_DESC* InputLayout,
	int RootparamCount,
	CD3DX12_ROOT_PARAMETER* Rootparam,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE Primitive,
	Pipline::BLEND_ID BlendID,
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
	piplines_.push_back(Pipline(PiplineID, PSname, VSname, InputLayoutCount, InputLayout, RootparamCount, Rootparam, Primitive, BlendID, CullMode, DxgiFormat));

}

void PiplineManager::GeneratePiplineDepth(Pipline::PIPLINE_ID PiplineID,
	std::string PSname,
	std::string VSname,
	int InputLayoutCount,
	D3D12_INPUT_ELEMENT_DESC* InputLayout,
	int RootparamCount,
	CD3DX12_ROOT_PARAMETER* Rootparam,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE Primitive,
	Pipline::BLEND_ID BlendID,
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
	piplines_.push_back(Pipline(PiplineID, PSname, VSname, InputLayoutCount, InputLayout, RootparamCount, Rootparam, Primitive, BlendID, CullMode, DxgiFormat));

}

void PiplineManager::SetPipline(int PiplineID)
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

void PiplineManager::Init()
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
	/*CD3DX12_DESCRIPTOR_RANGE descRangeSRV1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV2(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV3(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV4(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV5(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV6(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV7(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6);
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV8(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 7);*/
#pragma endregion

#pragma region Rootparam
	/*ルートパラメータの生成*/
	CD3DX12_ROOT_PARAMETER const1Tex1[2]{};
	/*CD3DX12_ROOT_PARAMETER const2tex1[3]{};
	CD3DX12_ROOT_PARAMETER const3Tex1[4]{};
	CD3DX12_ROOT_PARAMETER const1Tex8[9]{};
	CD3DX12_ROOT_PARAMETER const1Tex2[3]{};
	CD3DX12_ROOT_PARAMETER const3Tex2[5]{};
	CD3DX12_ROOT_PARAMETER const4Tex2[6]{};
	CD3DX12_ROOT_PARAMETER const1[1]{};
	CD3DX12_ROOT_PARAMETER const2[2]{};*/


	//定数用
	const1Tex1[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	//テクスチャ用
	const1Tex1[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	////定数用
	//const2tex1[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	////定数用
	//const2tex1[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const2tex1[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	////定数用
	//const3Tex1[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	////定数用
	//const3Tex1[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	////定数用
	//const3Tex1[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const3Tex1[3].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	////定数用
	//const1Tex8[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const1Tex8[1].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const1Tex8[2].InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const1Tex8[3].InitAsDescriptorTable(1, &descRangeSRV3, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const1Tex8[4].InitAsDescriptorTable(1, &descRangeSRV4, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const1Tex8[5].InitAsDescriptorTable(1, &descRangeSRV5, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const1Tex8[6].InitAsDescriptorTable(1, &descRangeSRV6, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const1Tex8[7].InitAsDescriptorTable(1, &descRangeSRV7, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const1Tex8[8].InitAsDescriptorTable(1, &descRangeSRV8, D3D12_SHADER_VISIBILITY_ALL);

	////定数用
	//const1Tex2[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const1Tex2[1].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const1Tex2[2].InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL);

	////定数用
	//const3Tex2[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	////定数用
	//const3Tex2[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	////定数用
	//const3Tex2[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const3Tex2[3].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const3Tex2[4].InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL);

	////定数用
	//const4Tex2[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	////定数用
	//const4Tex2[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	////定数用
	//const4Tex2[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	////定数用
	//const4Tex2[3].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const4Tex2[4].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);
	////テクスチャ用
	//const4Tex2[5].InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL);

	////定数用
	//const1[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

	////定数用
	//const2[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	////定数用
	//const2[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

#pragma endregion

	/*ライン用のパイプライン*/
	//GeneratePipline(PIPLINE_LINE_ALPHA, L"Resource/ShaderFiles/LinePS.hlsl", L"Resource/ShaderFiles/LineVS.hlsl", 3, inputLayout, 2, const1Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, BLENDMODE_ALPHA);

	/*スプライト用のパイプライン*/
	//GeneratePipline(PIPLINE_SPRITE_ADD, "Resource/ShaderFiles/SpritePS.hlsl", "Resource/ShaderFiles/SpriteVS.hlsl", 3, inputLayout, 2, const1Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD);
	GeneratePipline(Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, "Resource/ShaderFiles/SpritePS.hlsl", "Resource/ShaderFiles/SpriteVS.hlsl", 3, inputLayout, 2, const1Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, Pipline::BLEND_ID::BLENDMODE_ALPHA);

	/*オブジェクト用のパイプライン*/
	//GeneratePipline(PIPLINE_OBJECT_LIGHT_ALPHA, "Resource/ShaderFiles/ObjectLightPS.hlsl", "Resource/ShaderFiles/ObjectVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA, D3D12_CULL_MODE_BACK);
	//GeneratePipline(PIPLINE_OBJECT_LIGHT_FOG_ALPHA, L"Resource/ShaderFiles/ObjectLightFogPS.hlsl", L"Resource/ShaderFiles/ObjectVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA, D3D12_CULL_MODE_NONE);
	//GeneratePipline(PIPLINE_OBJECT_LIGHT_FOG_BUMP_ALPHA, L"Resource/ShaderFiles/ObjectLightFogBumpPS.hlsl", L"Resource/ShaderFiles/ObjectVS.hlsl", 3, inputLayout, 5, const3Tex2, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA, D3D12_CULL_MODE_NONE);
	//GeneratePipline(PIPLINE_OBJECT_NOLIGHT_ALPHA, "Resource/ShaderFiles/ObjectNoLightPS.hlsl", "Resource/ShaderFiles/ObjectVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA, D3D12_CULL_MODE_NONE);
	//GeneratePipline(PIPLINE_OBJECT_NOLIGHT_ALPHA_F, L"Resource/ShaderFiles/ObjectNoLightPS.hlsl", L"Resource/ShaderFiles/ObjectVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA, D3D12_CULL_MODE_FRONT);
	//GeneratePipline(PIPLINE_OBJECT_OUTLINE_ALPHA, L"Resource/ShaderFiles/ObjectNoLightPS.hlsl", L"Resource/ShaderFiles/ObjectVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA, D3D12_CULL_MODE_FRONT);
	//GeneratePipline(PIPLINE_OBJECT_TOONSHADER_ALPHA, "Resource/ShaderFiles/ObjectToonShaderPS.hlsl", "Resource/ShaderFiles/ObjectToonShaderVS.hlsl", 3, inputLayout, 5, const3Tex2, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_OBJECT_TOONSHADER_SHADOW_ALPHA, L"Resource/ShaderFiles/ObjectToonShaderShadowPS.hlsl", L"Resource/ShaderFiles/ObjectToonShaderVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_OBJECT_TOONSHADER_SHADOW_FOG_ALPHA, L"Resource/ShaderFiles/ObjectToonShaderShadowFogPS.hlsl", L"Resource/ShaderFiles/ObjectToonShaderVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_OBJECT_TOONSHADER_SHADOW_FOG_ALPHA_F, L"Resource/ShaderFiles/ObjectToonShaderShadowFogPS.hlsl", L"Resource/ShaderFiles/ObjectToonShaderVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA, D3D12_CULL_MODE_NONE);
	//GeneratePiplineDepth(PIPLINE_OBJECT_TOONSHADER_SHADOW_MRT_D_ALPHA, L"Resource/ShaderFiles/ObjectToonShaderShadowMRTDepthPS.hlsl", L"Resource/ShaderFiles/ObjectMRTDepthVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA, D3D12_CULL_MODE_BACK, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT);
	//GeneratePiplineDepth(PIPLINE_OBJECT_OUTLINE_MRT_D_ALPHA, L"Resource/ShaderFiles/ObjectToonShaderShadowMRTDepthPS.hlsl", L"Resource/ShaderFiles/ObjectMRTDepthVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA, D3D12_CULL_MODE_FRONT, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT);
	//GeneratePipline(PIPLINE_OBJECT_BUMP_ALPHA, "Resource/ShaderFiles/ObjectBumpPS.hlsl", "Resource/ShaderFiles/ObjectVS.hlsl", 3, inputLayout, 5, const3Tex2, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_OBJECT_GET_SHADOWMAP, "Resource/ShaderFiles/ObjectGetShadowMapPS.hlsl", "Resource/ShaderFiles/ObjectGetShadowMapVS.hlsl", 3, inputLayout, 2, const2, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA, D3D12_CULL_MODE_NONE, DXGI_FORMAT_R32_FLOAT);
	//GeneratePipline(PIPLINE_OBJECT_LIGHT_SHADOWMAP_ALPHA, "Resource/ShaderFiles/ObjectLightShadowMapPS.hlsl", "Resource/ShaderFiles/ObjectShadowMapVS.hlsl", 3, inputLayout, 6, const4Tex2, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);

	/*オブジェクトインスタンシング描画用のパイプライン*/
	//GeneratePipline(PIPLINE_OBJECT_INSTANCE_LIGHT_ALPHA, L"Resource/ShaderFiles/ObjectInstanceLightPS.hlsl", L"Resource/ShaderFiles/ObjectInstanceVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_OBJECT_INSTANCE_NOLIGHT_ALPHA, L"Resource/ShaderFiles/ObjectInstanceNoLightPS.hlsl", L"Resource/ShaderFiles/ObjectInstanceVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_OBJECT_INSTANCE_OUTLINE_ALPHA, L"Resource/ShaderFiles/ObjectInstanceNoLightPS.hlsl", L"Resource/ShaderFiles/ObjectInstanceOutlineVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA, D3D12_CULL_MODE_FRONT);
	//GeneratePipline(PIPLINE_OBJECT_INSTANCE_TOONSHADER_ALPHA, L"Resource/ShaderFiles/ObjectInstanceToonShaderPS.hlsl", L"Resource/ShaderFiles/ObjectInstanceVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);

	/*GSパーティクル用のパイプライン*/
	//GeneratePipline(PIPLINE_GSPARTICLE_NOLIGHT_ALPHA, L"Resource/ShaderFiles/GSParticlePS.hlsl", L"Resource/ShaderFiles/GSParticleVS.hlsl", L"Resource/ShaderFiles/GSParticleGS.hlsl", 1, inputLayoutPos, 2, const1Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_GSPARTICLE_INSTANCE_NOLIGHT_ALPHA, L"Resource/ShaderFiles/GSInstancePS.hlsl", L"Resource/ShaderFiles/GSInstanceVS.hlsl", L"Resource/ShaderFiles/GSInstanceGS.hlsl", 1, inputLayoutPos, 2, const1Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, BLENDMODE_ALPHA);

	/*マルチパスレンダリング用のパイプライン*/
	//GeneratePipline(PIPLINE_MULTIPATH_NORMAL_ADD, L"Resource/ShaderFiles/MultiPathNormalPS.hlsl", L"Resource/ShaderFiles/MultiPathNormalVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD);
	//GeneratePipline(PIPLINE_MULTIPATH_NORMAL_ALPHA, L"Resource/ShaderFiles/MultiPathNormalPS.hlsl", L"Resource/ShaderFiles/MultiPathNormalVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_MULTIPATH_MONOCHROME_ADD, L"Resource/ShaderFiles/MultiPathMonochromePS.hlsl", L"Resource/ShaderFiles/MultiPathNormalVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD);
	//GeneratePipline(PIPLINE_MULTIPATH_MONOCHROME_ALPHA, L"Resource/ShaderFiles/MultiPathMonochromePS.hlsl", L"Resource/ShaderFiles/MultiPathNormalVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_MULTIPATH_BLUR_ADD, L"Resource/ShaderFiles/MultiPathBlurPS.hlsl", L"Resource/ShaderFiles/MultiPathNormalVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD);
	//GeneratePipline(PIPLINE_MULTIPATH_BLUR_ALPHA, L"Resource/ShaderFiles/MultiPathBlurPS.hlsl", L"Resource/ShaderFiles/MultiPathNormalVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_MULTIPATH_GAUSSIAN_X_ALPHA, "Resource/ShaderFiles/GaussXPS.hlsl", "Resource/ShaderFiles/GaussXVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_MULTIPATH_GAUSSIAN_Y_ALPHA, "Resource/ShaderFiles/GaussYPS.hlsl", "Resource/ShaderFiles/GaussYVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_MULTIPATH_FOG_ALPHA, L"Resource/ShaderFiles/MultiPathFogPS.hlsl", L"Resource/ShaderFiles/MultiPathNormalVS.hlsl", 3, inputLayout, 3, const1Tex2, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);

	/*便利系用パイプライン*/
	//GeneratePipline(PIPLINE_SPRITE_SAMPLING_LUMINANCE_ADD, "Resource/ShaderFiles/SamplingLuminancePS.hlsl", "Resource/ShaderFiles/SpriteVS.hlsl", 3, inputLayout, 2, const1Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD, D3D12_CULL_MODE_BACK, DXGI_FORMAT_R32G32B32A32_FLOAT);
	//GeneratePipline(PIPLINE_SPRITE_MIXE_8TEXTURE_ADD, "Resource/ShaderFiles/Sampling8TexturePS.hlsl", "Resource/ShaderFiles/SpriteVS.hlsl", 3, inputLayout, 9, const1Tex8, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD);
	//GeneratePipline(PIPLINE_SPRITE_MIXE_8TEXTURE_ADD_TITLE, "Resource/ShaderFiles/Sampling8TexturePSTitle.hlsl", "Resource/ShaderFiles/SpriteVS.hlsl", 3, inputLayout, 9, const1Tex8, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD);
	//GeneratePipline(PIPLINE_SPRITE_MIXE_2TEXTURE_ADD, "Resource/ShaderFiles/Sampling2TexturePS.hlsl", "Resource/ShaderFiles/SpriteVS.hlsl", 3, inputLayout, 3, const1Tex2, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD);
}
