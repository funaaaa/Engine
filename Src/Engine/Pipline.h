#pragma once
#include "ShaderStorage.h"
#include <DirectXTex/d3dx12.h>
#include <wrl.h>

class Pipline {

public:

	enum PIPLINE_ID {

		PIPLINE_SPRITE_ALPHA

	};
	enum BLEND_ID {

		BLENDMODE_ALPHA,
		BLENDMODE_ADD,

	};
	enum PROJECTIONID {

		UI,
		OBJECT,
		BILLBOARD,

	};

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelinestate;		// �p�C�v���C���X�e�[�g
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootsignature;		// ���[�g�V�O�l�`��
	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive;		// �v���~�e�B�u
	PIPLINE_ID piplineID;							// �p�C�v���C����ID

	using string = std::string;

public:
	Pipline() {};
	///<summary>
	///�R���X�g���N�^
	///</summary>
	///<param name="PSname"><���[�h����s�N�Z���V�F�[�_�[�̃t�@�C����/param>
	///<param name="VSname"><���[�h���钸�_�V�F�[�_�[�̃t�@�C����/param>
	///<param name="inputLayoutCount"><�C���v�b�g���C�A�E�g�̐�/param>
	///<param name="inputLayout"><�C���v�b�g���C�A�E�g/param>
	///<param name="rootparamCount"><���[�g�p�����[�^�[��/param>
	///<param name="rootparam"><���[�g�p�����[�^�[/param>
	///<param name="primitive"><�v���~�e�B�u�`��param>
	///<param name="blendID"><�ǂ̃u�����h�����邩��ID/param>
	Pipline(
		PIPLINE_ID piplineID,
		string PSname,
		string VSname,
		int inputLayoutCount,
		D3D12_INPUT_ELEMENT_DESC* inputLayout,
		int rootparamCount,
		CD3DX12_ROOT_PARAMETER* rootparam,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive,
		BLEND_ID blendID,
		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK,
		DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM);

	Pipline(
		PIPLINE_ID piplineID,
		string PSname,
		string VSname,
		int inputLayoutCount,
		D3D12_INPUT_ELEMENT_DESC* inputLayout,
		int rootparamCount,
		CD3DX12_ROOT_PARAMETER* rootparam,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive,
		BLEND_ID blendID,
		bool isMRT,
		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK,
		DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT dxgiFormat2 = DXGI_FORMAT_R8_UNORM);

	Pipline(
		PIPLINE_ID piplineID,
		string PSname,
		string VSname,
		string GSname,
		int inputLayoutCount,
		D3D12_INPUT_ELEMENT_DESC* inputLayout,
		int rootparamCount,
		CD3DX12_ROOT_PARAMETER* rootparam,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive,
		BLEND_ID blendID,
		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK,
		DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM);

	//�p�C�v���C���̃Z�b�g
	void SetPipline();

	//�p�C�v���C��ID���擾
	PIPLINE_ID GetPiplineID() { return piplineID; }

	//�p�C�v���C���ݒ�
	void SetPiplineDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpipelineDesc, D3D12_INPUT_ELEMENT_DESC* inputLayout, int inputLayoutCount, CD3DX12_ROOT_PARAMETER* rootparam, int rootparamCount, BLEND_ID blendID, D3D12_CULL_MODE cullMode, DXGI_FORMAT dxgiFormat);
	void SetPiplineDescDepth(D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpipelineDesc, D3D12_INPUT_ELEMENT_DESC* inputLayout, int inputLayoutCount, CD3DX12_ROOT_PARAMETER* rootparam, int rootparamCount, BLEND_ID blendID, D3D12_CULL_MODE cullMode, DXGI_FORMAT dxgiFormat, DXGI_FORMAT dxgiFormat2);
};