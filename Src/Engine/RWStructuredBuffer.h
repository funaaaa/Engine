#pragma once
#include <DirectXTex/d3dx12.h>

/// <summary>
/// �\�����o�b�t�@
/// </summary>
class RWStructuredBuffer{
public:

	/// <summary>
	/// �\�����o�b�t�@���������B
	/// </summary>
	/// <param name="sizeOfElement">�G�������g�̃T�C�Y�B</param>
	/// <param name="numElement">�G�������g�̐��B</param>
	/// <param name="initData">�����f�[�^�B</param>
	void Init(int sizeOfElement, int numElement, void* initData);

	/// <summary>
	/// UAV�ɓo�^�B
	/// </summary>
	/// <param name=""></param>
	/// <param name="bufferNo"></param>
	void RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo);

	/// <summary>
	/// SRV�ɓo�^�B
	/// </summary>
	/// <param name="descriptorHandle"></param>
	/// <param name="bufferNo"></param>
	void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo);

	/// <summary>
	/// ����������Ă��邩����B
	/// </summary>
	/// <returns></returns>
	bool IsInited() const
	{
		return isInited;
	}

	/// <summary>
	/// CPU����A�N�Z�X�\�ȃ��\�[�X���擾����B
	/// </summary>
	/// <returns></returns>
	void* GetResourceOnCPU();


	ID3D12Resource* GetD3DResoruce() ;


public:

	ID3D12Resource* buffersOnGPU = {nullptr};
	void* buffersOnCPU = { nullptr };			//CPU������A�N�Z�X�ł��邷��X�g���N�`���o�b�t�@�̃A�h���X�B
	int numElement = 0;								//�v�f���B
	int sizeOfElement = 0;							//�G�������g�̃T�C�Y�B
	bool isInited = false;							//�������ς݁H

};