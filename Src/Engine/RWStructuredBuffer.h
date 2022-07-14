#pragma once
#include <DirectXTex/d3dx12.h>
#include <wrl.h>

/// <summary>
/// �\�����o�b�t�@
/// </summary>
class RWStructuredBuffer {
public:

	/// <summary>
	/// �\�����o�b�t�@���������B
	/// </summary>
	/// <param name="sizeOfElement">�G�������g�̃T�C�Y�B</param>
	/// <param name="numElement">�G�������g�̐��B</param>
	/// <param name="initData">�����f�[�^�B</param>
	void Init(int SizeOfElement, int NumElement, void* InitData);

	/// <summary>
	/// UAV�ɓo�^�B
	/// </summary>
	/// <param name=""></param>
	/// <param name="bufferNo"></param>
	void RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle);

	/// <summary>
	/// SRV�ɓo�^�B
	/// </summary>
	/// <param name="descriptorHandle"></param>
	/// <param name="bufferNo"></param>
	void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle);

	/// <summary>
	/// ����������Ă��邩����B
	/// </summary>
	/// <returns></returns>
	bool IsInited() const
	{
		return isInited_;
	}

	/// <summary>
	/// CPU����A�N�Z�X�\�ȃ��\�[�X���擾����B
	/// </summary>
	/// <returns></returns>
	void* GetResourceOnCPU();


	Microsoft::WRL::ComPtr<ID3D12Resource> GetD3DResoruce();


public:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffersOnGPU_;
	void* buffersOnCPU_;			// CPU������A�N�Z�X�ł��邷��X�g���N�`���o�b�t�@�̃A�h���X�B
	int numElement_;				// �v�f���B
	int sizeOfElement_;			// �G�������g�̃T�C�Y�B
	bool isInited_;				// �������ς�

};