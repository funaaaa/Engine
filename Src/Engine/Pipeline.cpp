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

	// �p�C�v���C��ID��ۑ�
	this->piplineID_ = piplineID_;

	//�V�F�[�_�[�ǂݍ���
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = nullptr;		//���_�V�F�[�_�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob = nullptr;		//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;	//�G���[�I�u�W�F�N�g
	//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	vsBlob = ShaderStorage::Ins()->LoadShader(VSname, "vs_5_0", "main");
	//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	psBlob = ShaderStorage::Ins()->LoadShader(PSname, "ps_5_0", "main");

	//�O���t�B�b�N�X�p�C�v���C������
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

	// �p�C�v���C��ID��ۑ�
	this->piplineID_ = piplineID_;

	//�V�F�[�_�[�ǂݍ���
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = nullptr;		//���_�V�F�[�_�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob = nullptr;		//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;	//�G���[�I�u�W�F�N�g
	vsBlob = ShaderStorage::Ins()->LoadShader(VSname, "vs_5_0", "main");
	//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	psBlob = ShaderStorage::Ins()->LoadShader(PSname, "ps_5_0", "main");

	//�O���t�B�b�N�X�p�C�v���C������
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

	// �p�C�v���C��ID��ۑ�
	this->piplineID_ = piplineID_;

	//�V�F�[�_�[�ǂݍ���
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = nullptr;		//���_�V�F�[�_�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob = nullptr;		//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3DBlob> gsBlob = nullptr;		//�W�I���g���V�F�[�_�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;	//�G���[�I�u�W�F�N�g
	//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	vsBlob = ShaderStorage::Ins()->LoadShader(VSname, "vs_5_0", "main");
	//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	psBlob = ShaderStorage::Ins()->LoadShader(PSname, "ps_5_0", "main");
	//�W�I���g���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	gsBlob = ShaderStorage::Ins()->LoadShader(GSname, "gs_5_0", "main");

	//�O���t�B�b�N�X�p�C�v���C������
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	SetPiplineDesc(gpipeline, inputLayout, inputLayoutCount, rootparam_, rootparamCount_, blendID, cullMode, dxgiFormat);

	Engine::Ins()->device_.dev_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate_));
}

void Pipeline::SetPipline()
{
	//�p�C�v���C���X�e�[�g�ݒ�R�}���h
	Engine::Ins()->copyResourceCmdList_->SetPipelineState(pipelinestate_.Get());

	//���[�g�V�O�l�`���ݒ�R�}���h
	Engine::Ins()->copyResourceCmdList_->SetGraphicsRootSignature(rootsignature_.Get());

	//�v���~�e�B�u�`��ݒ�R�}���h
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
	gpipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;						//�W���ݒ�
	gpipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		//�w�ʃJ�����O,�|���S�����h��Ԃ�,�[�x�N���b�s���O��L����
	gpipelineDesc.RasterizerState.CullMode = cullMode;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	gpipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = gpipelineDesc.BlendState.RenderTarget[0];
	gpipelineDesc.BlendState.AlphaToCoverageEnable = false;
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.BlendEnable = true;											//�u�����h��L���ɂ���
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;							//���Z
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;								//�\�[�X�̒l��100%�g��
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;							//�f�X�g�̒l��0%�g��
	//�����Ƃ��ēn���ꂽ�l�ɂ���ău�����h��ς���
	switch (blendID) {
	case BLEND_ID::BLENDMODE_ADD:
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;								//���Z
		blendDesc.SrcBlend = D3D12_BLEND_ONE;								//�\�[�X�̒l��100%�g��
		blendDesc.DestBlend = D3D12_BLEND_ONE;								//�\�[�X�̒l��100%�g��
		break;
	case BLEND_ID::BLENDMODE_ALPHA:
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;								//���Z
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;							//�\�[�X�̒l��100%�g��
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;					//�\�[�X�̒l��100%�g��
		break;
	default:
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;								//���Z
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;							//�\�[�X�̒l��100%�g��
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;					//�\�[�X�̒l��100%�g��
	}
	gpipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	gpipelineDesc.InputLayout.NumElements = inputLayoutCount;
	gpipelineDesc.PrimitiveTopologyType = primitive_;
	gpipelineDesc.NumRenderTargets = 1;											//�`��Ώۂ�1��
	gpipelineDesc.RTVFormats[0] = dxgiFormat;									//0�`255�w���RGBA
	gpipelineDesc.SampleDesc.Count = 1;											//1�s�N�Z���ɂ�1��T���v�����O

	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
	gpipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);//�[�x�e�X�g���s��,�������݋���,�[�x����������΍��i
	gpipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//�e�N�X�`���T���v���[�̃Z�b�g
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	//���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(rootparamCount_, rootparam_, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> rootSigBlob = {};
	//�o�[�W������������ł̃V���A���C�Y
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;	//�G���[�I�u�W�F�N�g
	HRESULT result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//���[�g�V�O�l�`���̐���
	result = Engine::Ins()->device_.dev_->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature_));

	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	gpipelineDesc.pRootSignature = rootsignature_.Get();
}

void Pipeline::SetPiplineDescDepth(D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpipelineDesc, D3D12_INPUT_ELEMENT_DESC* inputLayout, int inputLayoutCount, CD3DX12_ROOT_PARAMETER* rootparam_, int rootparamCount_, BLEND_ID blendID, D3D12_CULL_MODE cullMode, DXGI_FORMAT dxgiFormat, DXGI_FORMAT dxgiFormat2)
{
	gpipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;						//�W���ݒ�
	gpipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		//�w�ʃJ�����O,�|���S�����h��Ԃ�,�[�x�N���b�s���O��L����
	gpipelineDesc.RasterizerState.CullMode = cullMode;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	gpipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = gpipelineDesc.BlendState.RenderTarget[0];
	gpipelineDesc.BlendState.AlphaToCoverageEnable = false;
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.BlendEnable = true;											//�u�����h��L���ɂ���
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;							//���Z
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;								//�\�[�X�̒l��100%�g��
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;							//�f�X�g�̒l��0%�g��
	//�����Ƃ��ēn���ꂽ�l�ɂ���ău�����h��ς���
	switch (blendID) {
	case BLENDMODE_ADD:
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;								//���Z
		blendDesc.SrcBlend = D3D12_BLEND_ONE;								//�\�[�X�̒l��100%�g��
		blendDesc.DestBlend = D3D12_BLEND_ONE;								//�\�[�X�̒l��100%�g��
		break;
	case BLENDMODE_ALPHA:
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;								//���Z
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;							//�\�[�X�̒l��100%�g��
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;					//�\�[�X�̒l��100%�g��
		break;
	default:
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;								//���Z
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;							//�\�[�X�̒l��100%�g��
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;					//�\�[�X�̒l��100%�g��
	}
	gpipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	gpipelineDesc.InputLayout.NumElements = inputLayoutCount;
	gpipelineDesc.PrimitiveTopologyType = primitive_;
	gpipelineDesc.NumRenderTargets = 2;											//�`��Ώۂ�1��
	gpipelineDesc.RTVFormats[0] = dxgiFormat;									//0�`255�w���RGBA
	gpipelineDesc.RTVFormats[1] = dxgiFormat2;									//0�`255�w���RGBA
	gpipelineDesc.SampleDesc.Count = 1;											//1�s�N�Z���ɂ�1��T���v�����O

	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
	gpipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);//�[�x�e�X�g���s��,�������݋���,�[�x����������΍��i
	gpipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//�e�N�X�`���T���v���[�̃Z�b�g
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	//���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(rootparamCount_, rootparam_, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> rootSigBlob = {};
	//�o�[�W������������ł̃V���A���C�Y
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;	//�G���[�I�u�W�F�N�g
	HRESULT result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//���[�g�V�O�l�`���̐���
	result = Engine::Ins()->device_.dev_->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature_));

	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	gpipelineDesc.pRootSignature = rootsignature_.Get();
}