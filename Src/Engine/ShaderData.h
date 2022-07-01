#pragma once
#include "DirectXBase.h"
#include <d3d12.h>
#include "dxcapi.h"
#include <string>

// �V�F�[�_�[�f�[�^��ۑ����邽�߂̃f�[�^�N���X
class ShaderData {

private:

	/*-- �����o�ϐ� --*/

	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;	// �V�F�[�_�[�̃f�[�^
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlobDxc;	// �V�F�[�_�[�̃f�[�^ dxc�p
	std::vector<char> shaderBin;					// dxc�ŃR���p�C�������ۂɏo�͂�����̕�����
	std::string entryPoint;							// �V�F�[�_�[�̃G���g���[�|�C���g
	std::string shaderModel;						// �V�F�[�_�[���f��
	std::string shaderPath;							// �V�F�[�_�[�̃t�@�C���p�X ������g���ăV�F�[�_�[�𔻒f����B


public:

	/*-- �����o�֐� --*/

	// �R���X�g���N�^
	ShaderData(const std::string& ShaderPath, const std::string& EntryPoint, const std::string& ShaderModel, const bool& IsDXC = false);

	// ���[�h����
	void LoadShader();
	void LoadShaderDXC();

	// �V�F�[�_�[�̃t�@�C���p�X���擾���鏈��
	const std::string& GetShaderPath() { return shaderPath; }
	// �V�F�[�_�[�̃f�[�^���擾���鏈��
	const Microsoft::WRL::ComPtr<ID3DBlob>& GetShaderBlob() { return shaderBlob; }
	const Microsoft::WRL::ComPtr<IDxcBlob>& GetShaderBlobDXC() { return shaderBlobDxc; }
	std::vector<char>& GetShaderBin() { return shaderBin; }


private:

	std::wstring StringToWString( std::string OString );

};