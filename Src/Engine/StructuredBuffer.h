#pragma once

#include <d3d12.h>

//�ǂݎ���p�\�����o�b�t�@

class StructuredBuffer{

public:

	/// <summary>
	/// �\�����o�b�t�@���������B
	/// </summary>
	/// <param name="sizeOfElement">�G�������g�̃T�C�Y�B</param>
	/// <param name="numElement">�G�������g�̐��B</param>
	/// <param name="initData">�����f�[�^�B</param>
	void Init(int SizeOfElement, int NumElement, void* InitData);

	/// <summary>
	/// SRV�ɓo�^�B
	/// </summary>
	/// <param name="descriptorHandle"></param>
	void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle);

	/// <summary>
	/// �\�����o�b�t�@�̓��e���X�V�B
	/// </summary>
	/// <param name="data"></param>
	void Update(void* Data);

	/// <summary>
	/// ����������Ă��邩����B
	/// </summary>
	/// <returns></returns>
	bool IsInited() const
	{
		return isInited;
	}

	ID3D12Resource* GetD3DResoruce() ;


public:

	ID3D12Resource* buffersOnGPU = {nullptr};
	void* buffersOnCPU = { nullptr };			//CPU������A�N�Z�X�ł��邷��X�g���N�`���o�b�t�@�̃A�h���X�B
	int numElement = 0;								//�v�f���B
	int sizeOfElement = 0;							//�G�������g�̃T�C�Y�B
	bool isInited = false;							//�������ς݁H

};