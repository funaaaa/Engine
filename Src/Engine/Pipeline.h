#pragma once
#include "ShaderStorage.h"
#include <DirectXTex/include/d3dx12.h>
#include <wrl.h>

// �p�C�v���C���N���X
// �ȑO�̓I�u�W�F�N�g����`�悷�邽�߂Ɏg�p���Ă��܂������A�V���Ƀ��C�g���p�̃p�C�v���C���N���X��������̂Ō��݂̓X�v���C�g�֌W�̏������s���݂̂ƂȂ��Ă��܂��B
// PiplineMgr�����̃N���X���Ǘ����Ă��܂��B
class Pipeline {

public:

	// �g�p����p�C�v���C����ID
	enum PIPLINE_ID {

		PIPLINE_SPRITE_ALPHA

	};
	// �g�p����u�����h���[�h
	enum BLEND_ID {

		BLENDMODE_ALPHA,
		BLENDMODE_ADD,

	};
	// �g�p����J�����萔�o�b�t�@�̎��
	enum PROJECTIONID {

		UI,
		OBJECT,
		BILLBOARD,

	};

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelinestate_;		// �p�C�v���C���X�e�[�g
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootsignature_;		// ���[�g�V�O�l�`��
	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_;		// �v���~�e�B�u
	PIPLINE_ID piplineID_;							// �p�C�v���C����ID

	using string = std::string;

public:
	Pipeline() {};
	///<summary>
	/// �R���X�g���N�^
	///</summary>
	///<param name="PSname"><���[�h����s�N�Z���V�F�[�_�[�̃t�@�C����/param>
	///<param name="VSname"><���[�h���钸�_�V�F�[�_�[�̃t�@�C����/param>
	///<param name="inputLayoutCount"><�C���v�b�g���C�A�E�g�̐�/param>
	///<param name="inputLayout"><�C���v�b�g���C�A�E�g/param>
	///<param name="rootparamCount"><���[�g�p�����[�^�[��/param>
	///<param name="rootparam"><���[�g�p�����[�^�[/param>
	///<param name="primitive"><�v���~�e�B�u�`��param>
	///<param name="blendID"><�ǂ̃u�����h�����邩��ID/param>
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

	// �p�C�v���C���̃Z�b�g
	void SetPipline();

	// �p�C�v���C��ID���擾
	PIPLINE_ID GetPiplineID() { return piplineID_; }

	// �p�C�v���C���ݒ�
	void SetPiplineDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpipelineDesc, D3D12_INPUT_ELEMENT_DESC* inputLayout, int inputLayoutCount, CD3DX12_ROOT_PARAMETER* rootparam_, int rootparamCount_, BLEND_ID blendID, D3D12_CULL_MODE cullMode, DXGI_FORMAT dxgiFormat);
	void SetPiplineDescDepth(D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpipelineDesc, D3D12_INPUT_ELEMENT_DESC* inputLayout, int inputLayoutCount, CD3DX12_ROOT_PARAMETER* rootparam_, int rootparamCount_, BLEND_ID blendID, D3D12_CULL_MODE cullMode, DXGI_FORMAT dxgiFormat, DXGI_FORMAT dxgiFormat2);
};