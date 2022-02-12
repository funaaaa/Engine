#pragma once
#include "DirectXBase.h"
#include <d3d12.h>
#include <dxcapi.h>
#include <string>

// �V�F�[�_�[�f�[�^��ۑ����邽�߂̃f�[�^�N���X
class ShaderData {

private:

	/*-- �����o�ϐ� --*/

	ComPtr<ID3DBlob> shaderBlob;		// �V�F�[�_�[�̃f�[�^
	ComPtr<IDxcBlob> shaderBlobDxc;		// �V�F�[�_�[�̃f�[�^ dxc�p
	vector<char> shaderBin;				// dxc�ŃR���p�C�������ۂɏo�͂�����̕�����
	string entryPoint;					// �V�F�[�_�[�̃G���g���[�|�C���g
	string shaderModel;					// �V�F�[�_�[���f��
	string shaderPath;					// �V�F�[�_�[�̃t�@�C���p�X ������g���ăV�F�[�_�[�𔻒f����B


public:

	/*-- �����o�֐� --*/

	// �R���X�g���N�^
	ShaderData(const string& shaderPath, const string& entryPoint, const string& shaderModel);
	ShaderData(const string& shaderPath, const string& entryPoint, const string& shaderModel, const bool& isDXC);

	// ���[�h����
	void LoadShader();
	void LoadShaderDXC();

	// �V�F�[�_�[�̃t�@�C���p�X���擾���鏈��
	const string& GetShaderPath() { return shaderPath; }
	// �V�F�[�_�[�̃f�[�^���擾���鏈��
	const ComPtr<ID3DBlob>& GetShaderBlob() { return shaderBlob; }
	const ComPtr<IDxcBlob>& GetShaderBlobDXC() { return shaderBlobDxc; }
	const vector<char>& GetShaderBin() { return shaderBin; }


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