#pragma once
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <DirectXTex/d3dx12.h>
#include "Singleton.h"
#include "Pipline.h"

// �p�C�v���C���Ǘ��N���X
// �ȑO�I�u�W�F�N�g����`�悷��p�C�v���C�����Ǘ����邽�߂Ɏg�p���Ă������A���C�g���p�ŐV�����p�C�v���C����������̂ł��̃N���X�ł̓X�v���C�g�֌W�̃p�C�v���C�����Ǘ�����݂̂ƂȂ��Ă��܂��B
class PiplineManager :public Singleton<PiplineManager> {
public:

	std::vector<Pipline> piplines;	// �p�C�v���C��
	// �R���X�g���N�^
	PiplineManager() {};

public:
	/// <summary>
	/// �p�C�v���C���𐶐����鏈��
	/// </summary>
	/// <param name="PiplineID">��������p�C�v���C����ID</param>
	/// <param name="PSname">�g�p����s�N�Z���V�F�[�_�[�̃t�@�C����</param>
	/// <param name="VSname">�g�p���钸�_�V�F�[�_�[�̃t�@�C����</param>
	/// <param name="InputLayoutCount">�C���v�b�g���C�A�E�g�̐�</param>
	/// <param name="InputLayout">�C���v�b�g���C�A�E�g</param>
	/// <param name="RootparamCount">���[�g�p�����[�^�[�̐�</param>
	/// <param name="Rootparam">���[�g�p�����[�^�[</param>
	/// <param name="Primitive">�v���~�e�B�u�`��</param>
	/// <param name="BlendID">�u�����h��ID</param>
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
	/// �p�C�v���C���𐶐����鏈��
	/// </summary>
	/// <param name="PiplineID">��������p�C�v���C����ID</param>
	/// <param name="PSname">�g�p����s�N�Z���V�F�[�_�[�̃t�@�C����</param>
	/// <param name="VSname">�g�p���钸�_�V�F�[�_�[�̃t�@�C����</param>
	/// <param name="GSname">�g�p����W�I���g���V�F�[�_�[�̃t�@�C����</param>
	/// <param name="InputLayoutCount">�C���v�b�g���C�A�E�g�̐�</param>
	/// <param name="InputLayout">�C���v�b�g���C�A�E�g</param>
	/// <param name="RootparamCount">���[�g�p�����[�^�[�̐�</param>
	/// <param name="Rootparam">���[�g�p�����[�^�[</param>
	/// <param name="Primitive">�v���~�e�B�u�`��</param>
	/// <param name="BlendID">�u�����h��ID</param>
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


	// �p�C�v�������Z�b�g����
	void SetPipline(int PiplineID);

	// �p�C�v���C���֌W�𐶐�����B
	void Init();
};