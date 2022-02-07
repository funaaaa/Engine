#pragma once
#include "DirectXBase.h"
#include <d3d12.h>
#include <string>

// �V�F�[�_�[�f�[�^��ۑ����邽�߂̃f�[�^�N���X
class ShaderData {

private:

	/*-- �����o�ϐ� --*/

	ComPtr<ID3DBlob> shaderBlob;		// �V�F�[�_�[�̃f�[�^
	string entryPoint;					// �V�F�[�_�[�̃G���g���[�|�C���g
	string shaderModel;					// �V�F�[�_�[���f��
	string shaderPath;					// �V�F�[�_�[�̃t�@�C���p�X ������g���ăV�F�[�_�[�𔻒f����B


public:

	/*-- �����o�֐� --*/

	// �R���X�g���N�^
	ShaderData(const string& shaderPath, const string& entryPoint, const string& shaderModel);

	// ���[�h����
	void LoadShader();

	// �V�F�[�_�[�̃t�@�C���p�X���擾���鏈��
	const string& GetShaderPath() { return shaderPath; }
	// �V�F�[�_�[�̃f�[�^���擾���鏈��
	const ComPtr<ID3DBlob>& GetShaderBlob() { return shaderBlob; }

};