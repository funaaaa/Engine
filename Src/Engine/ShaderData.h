#pragma once
#include "DirectXBase.h"
#include <d3d12.h>
#include <dxcapi.h>
#include <string>

// �V�F�[�_�[�f�[�^��ۑ����邽�߂̃f�[�^�N���X
class ShaderData {

private:

	/*-- �����o�ϐ� --*/

	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;		// �V�F�[�_�[�̃f�[�^
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlobDxc;		// �V�F�[�_�[�̃f�[�^ dxc�p
	std::vector<char> shaderBin;				// dxc�ŃR���p�C�������ۂɏo�͂�����̕�����
	std::string entryPoint;					// �V�F�[�_�[�̃G���g���[�|�C���g
	std::string shaderModel;					// �V�F�[�_�[���f��
	std::string shaderPath;					// �V�F�[�_�[�̃t�@�C���p�X ������g���ăV�F�[�_�[�𔻒f����B


public:

	/*-- �����o�֐� --*/

	// �R���X�g���N�^
	ShaderData(const std::string& shaderPath, const std::string& entryPoint, const std::string& shaderModel, const bool& isDXC = false);

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

	std::wstring StringToWString( std::string oString )
	{
		// SJIS �� wstring
		int iBufferSize = MultiByteToWideChar(CP_ACP, 0, oString.c_str()
			, -1, (wchar_t*)NULL, 0);

		// �o�b�t�@�̎擾
		wchar_t* cpUCS2 = new wchar_t[iBufferSize];

		// SJIS �� wstring
		MultiByteToWideChar(CP_ACP, 0, oString.c_str(), -1, cpUCS2
			, iBufferSize);

		// string�̐���
		std::wstring oRet(cpUCS2, cpUCS2 + iBufferSize - 1);

		// �o�b�t�@�̔j��
		delete[] cpUCS2;

		// �ϊ����ʂ�Ԃ�
		return(oRet);
	}

};