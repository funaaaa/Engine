#pragma once
#include "ShaderStorage.h"
#include "Enum.h"
#include <DirectXTex/d3dx12.h>

class Pipline {
private:
	ComPtr<ID3D12PipelineState> pipelinestate;		// �p�C�v���C���X�e�[�g
	ComPtr<ID3D12RootSignature> rootsignature;		// ���[�g�V�O�l�`��
	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive;		// �v���~�e�B�u
	PiplineID piplineID;							// �p�C�v���C����ID

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

	//�p�C�v���C���̃Z�b�g
	void SetPipline();

	//�p�C�v���C��ID���擾
	PiplineID GetPiplineID() { return piplineID; }

	//�p�C�v���C���ݒ�
	void SetPiplineDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpipelineDesc, D3D12_INPUT_ELEMENT_DESC* inputLayout, int inputLayoutCount, CD3DX12_ROOT_PARAMETER* rootparam, int rootparamCount, int blendID, D3D12_CULL_MODE cullMode, DXGI_FORMAT dxgiFormat);
	void SetPiplineDescDepth(D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpipelineDesc, D3D12_INPUT_ELEMENT_DESC* inputLayout, int inputLayoutCount, CD3DX12_ROOT_PARAMETER* rootparam, int rootparamCount, int blendID, D3D12_CULL_MODE cullMode, DXGI_FORMAT dxgiFormat, DXGI_FORMAT dxgiFormat2);
};