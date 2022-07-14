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
	//���łɐ������Ă��郊�X�g�̒�����p�C�v���C��ID���d�����Ă��Ȃ������琶������B
	for (int i = 0; i < piplines_.size(); ++i) {
		if (piplines_[i].GetPiplineID() == PiplineID) {

			OutputDebugString(L"\nError ���łɐ������Ă���p�C�v���C���𐶐����悤�Ƃ��܂����B\n");
			exit(true);

			return;
		}
	}

	//�w�肳�ꂽ�p�C�v���C��ID���Ƃɐ������čŌ���ɒǉ�
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
	//���łɐ������Ă��郊�X�g�̒�����p�C�v���C��ID���d�����Ă��Ȃ������琶������
	for (int i = 0; i < piplines_.size(); ++i) {
		if (piplines_[i].GetPiplineID() == PiplineID) {

			OutputDebugString(L"\nError ���łɐ������Ă���p�C�v���C���𐶐����悤�Ƃ��܂����B\n");
			exit(true);

			return;
		}
	}

	//�w�肳�ꂽ�p�C�v���C��ID���Ƃɐ������čŌ���ɒǉ�
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
	//���łɐ������Ă��郊�X�g�̒�����p�C�v���C��ID���d�����Ă��Ȃ������琶������
	for (int i = 0; i < piplines_.size(); ++i) {
		if (piplines_[i].GetPiplineID() == PiplineID) {

			OutputDebugString(L"\nError ���łɐ������Ă���p�C�v���C���𐶐����悤�Ƃ��܂����B\n");
			exit(true);

			return;
		}
	}
	DxgiFormat2;
	//�w�肳�ꂽ�p�C�v���C��ID���Ƃɐ������čŌ���ɒǉ�
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

	OutputDebugString(L"\nError ���݂��Ă��Ȃ��p�C�v���C�����Z�b�g����悤�Ƃ��܂����B\n");
	assert(0);
}

void PiplineManager::Init()
{
	/*�p�C�v���C���p�̊e�v�f�𐶐�*/
#pragma region inputlayout
	/*InputLayout�𐶐�*/
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{	//���_���W
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	//�@���x�N�g��
			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	//uv���W
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
	/*�f�B�X�N���v�^�e�[�u���̐���*/
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);					//t0���W�X�^
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
	/*���[�g�p�����[�^�̐���*/
	CD3DX12_ROOT_PARAMETER const1Tex1[2]{};
	/*CD3DX12_ROOT_PARAMETER const2tex1[3]{};
	CD3DX12_ROOT_PARAMETER const3Tex1[4]{};
	CD3DX12_ROOT_PARAMETER const1Tex8[9]{};
	CD3DX12_ROOT_PARAMETER const1Tex2[3]{};
	CD3DX12_ROOT_PARAMETER const3Tex2[5]{};
	CD3DX12_ROOT_PARAMETER const4Tex2[6]{};
	CD3DX12_ROOT_PARAMETER const1[1]{};
	CD3DX12_ROOT_PARAMETER const2[2]{};*/


	//�萔�p
	const1Tex1[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	//�e�N�X�`���p
	const1Tex1[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	////�萔�p
	//const2tex1[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	////�萔�p
	//const2tex1[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const2tex1[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	////�萔�p
	//const3Tex1[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	////�萔�p
	//const3Tex1[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	////�萔�p
	//const3Tex1[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const3Tex1[3].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	////�萔�p
	//const1Tex8[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const1Tex8[1].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const1Tex8[2].InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const1Tex8[3].InitAsDescriptorTable(1, &descRangeSRV3, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const1Tex8[4].InitAsDescriptorTable(1, &descRangeSRV4, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const1Tex8[5].InitAsDescriptorTable(1, &descRangeSRV5, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const1Tex8[6].InitAsDescriptorTable(1, &descRangeSRV6, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const1Tex8[7].InitAsDescriptorTable(1, &descRangeSRV7, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const1Tex8[8].InitAsDescriptorTable(1, &descRangeSRV8, D3D12_SHADER_VISIBILITY_ALL);

	////�萔�p
	//const1Tex2[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const1Tex2[1].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const1Tex2[2].InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL);

	////�萔�p
	//const3Tex2[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	////�萔�p
	//const3Tex2[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	////�萔�p
	//const3Tex2[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const3Tex2[3].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const3Tex2[4].InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL);

	////�萔�p
	//const4Tex2[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	////�萔�p
	//const4Tex2[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	////�萔�p
	//const4Tex2[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	////�萔�p
	//const4Tex2[3].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const4Tex2[4].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);
	////�e�N�X�`���p
	//const4Tex2[5].InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL);

	////�萔�p
	//const1[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

	////�萔�p
	//const2[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	////�萔�p
	//const2[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

#pragma endregion

	/*���C���p�̃p�C�v���C��*/
	//GeneratePipline(PIPLINE_LINE_ALPHA, L"Resource/ShaderFiles/LinePS.hlsl", L"Resource/ShaderFiles/LineVS.hlsl", 3, inputLayout, 2, const1Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, BLENDMODE_ALPHA);

	/*�X�v���C�g�p�̃p�C�v���C��*/
	//GeneratePipline(PIPLINE_SPRITE_ADD, "Resource/ShaderFiles/SpritePS.hlsl", "Resource/ShaderFiles/SpriteVS.hlsl", 3, inputLayout, 2, const1Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD);
	GeneratePipline(Pipline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, "Resource/ShaderFiles/SpritePS.hlsl", "Resource/ShaderFiles/SpriteVS.hlsl", 3, inputLayout, 2, const1Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, Pipline::BLEND_ID::BLENDMODE_ALPHA);

	/*�I�u�W�F�N�g�p�̃p�C�v���C��*/
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

	/*�I�u�W�F�N�g�C���X�^���V���O�`��p�̃p�C�v���C��*/
	//GeneratePipline(PIPLINE_OBJECT_INSTANCE_LIGHT_ALPHA, L"Resource/ShaderFiles/ObjectInstanceLightPS.hlsl", L"Resource/ShaderFiles/ObjectInstanceVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_OBJECT_INSTANCE_NOLIGHT_ALPHA, L"Resource/ShaderFiles/ObjectInstanceNoLightPS.hlsl", L"Resource/ShaderFiles/ObjectInstanceVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_OBJECT_INSTANCE_OUTLINE_ALPHA, L"Resource/ShaderFiles/ObjectInstanceNoLightPS.hlsl", L"Resource/ShaderFiles/ObjectInstanceOutlineVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA, D3D12_CULL_MODE_FRONT);
	//GeneratePipline(PIPLINE_OBJECT_INSTANCE_TOONSHADER_ALPHA, L"Resource/ShaderFiles/ObjectInstanceToonShaderPS.hlsl", L"Resource/ShaderFiles/ObjectInstanceVS.hlsl", 3, inputLayout, 4, const3Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);

	/*GS�p�[�e�B�N���p�̃p�C�v���C��*/
	//GeneratePipline(PIPLINE_GSPARTICLE_NOLIGHT_ALPHA, L"Resource/ShaderFiles/GSParticlePS.hlsl", L"Resource/ShaderFiles/GSParticleVS.hlsl", L"Resource/ShaderFiles/GSParticleGS.hlsl", 1, inputLayoutPos, 2, const1Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_GSPARTICLE_INSTANCE_NOLIGHT_ALPHA, L"Resource/ShaderFiles/GSInstancePS.hlsl", L"Resource/ShaderFiles/GSInstanceVS.hlsl", L"Resource/ShaderFiles/GSInstanceGS.hlsl", 1, inputLayoutPos, 2, const1Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, BLENDMODE_ALPHA);

	/*�}���`�p�X�����_�����O�p�̃p�C�v���C��*/
	//GeneratePipline(PIPLINE_MULTIPATH_NORMAL_ADD, L"Resource/ShaderFiles/MultiPathNormalPS.hlsl", L"Resource/ShaderFiles/MultiPathNormalVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD);
	//GeneratePipline(PIPLINE_MULTIPATH_NORMAL_ALPHA, L"Resource/ShaderFiles/MultiPathNormalPS.hlsl", L"Resource/ShaderFiles/MultiPathNormalVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_MULTIPATH_MONOCHROME_ADD, L"Resource/ShaderFiles/MultiPathMonochromePS.hlsl", L"Resource/ShaderFiles/MultiPathNormalVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD);
	//GeneratePipline(PIPLINE_MULTIPATH_MONOCHROME_ALPHA, L"Resource/ShaderFiles/MultiPathMonochromePS.hlsl", L"Resource/ShaderFiles/MultiPathNormalVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_MULTIPATH_BLUR_ADD, L"Resource/ShaderFiles/MultiPathBlurPS.hlsl", L"Resource/ShaderFiles/MultiPathNormalVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD);
	//GeneratePipline(PIPLINE_MULTIPATH_BLUR_ALPHA, L"Resource/ShaderFiles/MultiPathBlurPS.hlsl", L"Resource/ShaderFiles/MultiPathNormalVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_MULTIPATH_GAUSSIAN_X_ALPHA, "Resource/ShaderFiles/GaussXPS.hlsl", "Resource/ShaderFiles/GaussXVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_MULTIPATH_GAUSSIAN_Y_ALPHA, "Resource/ShaderFiles/GaussYPS.hlsl", "Resource/ShaderFiles/GaussYVS.hlsl", 3, inputLayout, 3, const2tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);
	//GeneratePipline(PIPLINE_MULTIPATH_FOG_ALPHA, L"Resource/ShaderFiles/MultiPathFogPS.hlsl", L"Resource/ShaderFiles/MultiPathNormalVS.hlsl", 3, inputLayout, 3, const1Tex2, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ALPHA);

	/*�֗��n�p�p�C�v���C��*/
	//GeneratePipline(PIPLINE_SPRITE_SAMPLING_LUMINANCE_ADD, "Resource/ShaderFiles/SamplingLuminancePS.hlsl", "Resource/ShaderFiles/SpriteVS.hlsl", 3, inputLayout, 2, const1Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD, D3D12_CULL_MODE_BACK, DXGI_FORMAT_R32G32B32A32_FLOAT);
	//GeneratePipline(PIPLINE_SPRITE_MIXE_8TEXTURE_ADD, "Resource/ShaderFiles/Sampling8TexturePS.hlsl", "Resource/ShaderFiles/SpriteVS.hlsl", 3, inputLayout, 9, const1Tex8, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD);
	//GeneratePipline(PIPLINE_SPRITE_MIXE_8TEXTURE_ADD_TITLE, "Resource/ShaderFiles/Sampling8TexturePSTitle.hlsl", "Resource/ShaderFiles/SpriteVS.hlsl", 3, inputLayout, 9, const1Tex8, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD);
	//GeneratePipline(PIPLINE_SPRITE_MIXE_2TEXTURE_ADD, "Resource/ShaderFiles/Sampling2TexturePS.hlsl", "Resource/ShaderFiles/SpriteVS.hlsl", 3, inputLayout, 3, const1Tex2, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, BLENDMODE_ADD);
}
