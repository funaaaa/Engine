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
	//���łɐ������Ă��郊�X�g�̒�����p�C�v���C��ID���d�����Ă��Ȃ������琶������B
	for (int i = 0; i < piplines_.size(); ++i) {
		if (piplines_[i].GetPiplineID() == PiplineID) {

			OutputDebugString(L"\nError ���łɐ������Ă���p�C�v���C���𐶐����悤�Ƃ��܂����B\n");
			exit(true);

			return;
		}
	}

	//�w�肳�ꂽ�p�C�v���C��ID���Ƃɐ������čŌ���ɒǉ�
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
	//���łɐ������Ă��郊�X�g�̒�����p�C�v���C��ID���d�����Ă��Ȃ������琶������
	for (int i = 0; i < piplines_.size(); ++i) {
		if (piplines_[i].GetPiplineID() == PiplineID) {

			OutputDebugString(L"\nError ���łɐ������Ă���p�C�v���C���𐶐����悤�Ƃ��܂����B\n");
			exit(true);

			return;
		}
	}

	//�w�肳�ꂽ�p�C�v���C��ID���Ƃɐ������čŌ���ɒǉ�
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

	OutputDebugString(L"\nError ���݂��Ă��Ȃ��p�C�v���C�����Z�b�g����悤�Ƃ��܂����B\n");
	assert(0);
}

void PipelineManager::Init()
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

#pragma region Rootparam
	/*���[�g�p�����[�^�̐���*/
	CD3DX12_ROOT_PARAMETER const1Tex1[2]{};


	//�萔�p
	const1Tex1[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	//�e�N�X�`���p
	const1Tex1[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

#pragma endregion

	/*�X�v���C�g�p�̃p�C�v���C��*/
	GeneratePipline(Pipeline::PIPLINE_ID::PIPLINE_SPRITE_ALPHA, "Resource/ShaderFiles/SpritePS.hlsl", "Resource/ShaderFiles/SpriteVS.hlsl", 3, inputLayout, 2, const1Tex1, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, Pipeline::BLEND_ID::BLENDMODE_ALPHA);
}
