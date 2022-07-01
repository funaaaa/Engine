#pragma once
#include <wrl.h>
#include <DirectXTex/d3dx12.h>
#include <memory>

class StructuredBuffer;
class RWStructuredBuffer;

// �R���s���[�g�V�F�[�_�[�N���X 
// ���̃N���X�͍ŏ��ɍ쐬�����R���s���[�g�V�F�[�_�[�N���X�ŁA�e�N�X�`�����͈������\�����o�b�t�@�𑗂邱�Ƃ����o���܂���B
// ������RayComputeShader���ɂ��̋@�\��ǉ����A���̃N���X�͍폜����\��ł��B
class ComputeShader {

private:

	/*-- �����o�ϐ� --*/

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;	// ���[�g�V�O�l�`��
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipline;		// �p�C�v���C��
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;		// �f�B�X�N���v�^�q�[�v ���Ƃ����ۑ�����Ă���̂�inputSB��outputSB�̒�
	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUDescHeapHandle;			// SRV�J�n�n���h�� GPU
	D3D12_GPU_DESCRIPTOR_HANDLE uavGPUDescHeapHandle;			// UAV�J�n�n���h�� GPU

public:

	std::shared_ptr<StructuredBuffer> inputSB;					// ���͗p�\���̃o�b�t�@
	std::shared_ptr<RWStructuredBuffer> outputSB;				// �������ݗp�\���̃o�b�t�@


public:

	/*-- �����o�֐� --*/

	//�R���X�g���N�^
	ComputeShader();
	~ComputeShader();

	/// <summary>
	/// ����������
	/// </summary>
	/// <param name="csPath">�R���s���[�g�V�F�[�_�[�̃f�B���N�g���[�p�X</param>
	/// <param name="sizeOfInput">���͗p�f�[�^�\���̂�1�v�f�̃T�C�Y sizeOf���g��</param>
	/// <param name="countOfInputElement">���͗p�f�[�^�\���̗̂v�f��</param>
	/// <param name="inputFormatData">���͗p�f�[�^�\���̂̃|�C���^</param>
	/// <param name="sizeOfOutput">�������ݗp�f�[�^�\���̂�1�v�f�̃T�C�Y sizeOf���g��</param>
	/// <param name="countOfOutputElement">�������ݗp�f�[�^�\���̗̂v�f��</param>
	/// <param name="outputFormatData">�������ݗp�f�[�^�\���̂̃|�C���^</param>
	void Init(LPCWSTR CsPath,
		int SizeOfInput, int CountOfInputElement, void* InputFormatData,
		int SizeOfOutput, int CountOfOutputElement, void* OutputFormatData);

	//�f�B�X�p�b�`�R�[��
	void Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ);


	/*-- �@�\�֐� --*/

	//���͗p�\�����o�b�t�@���X�V
	void UpdateInputSB(void* UpdateSB);


private:

	/*-- �V�F�[�_�[�ǂݍ��݊֌W --*/

	// �V�F�[�_�[�ǂݍ���
	LPD3DBLOB LoadShader(LPCWSTR ShaderFileName, const char EntryPointName[], const char ShaderModel[], ID3DBlob* ShaderBlob, ID3DBlob* ErrorBlob);
	// �V�F�[�_�[�̃��[�h�G���[���`�F�b�N
	void CheckRoadShaderError(HRESULT Result, ID3DBlob* ErrorBlob);

	/*-- ���[�g�V�O�l�`�� --*/

	// ���[�g�V�O�l�`���̐���
	void GenerateRootSignature();

	// �f�B�X�N���v�^�q�[�v�̗v�f
	enum {
		DESCRIPTORHEAP_CB,
		DESCRIPTORHEAP_SRV,
		DESCRIPTORHEAP_UAV,
		DESCRIPTORHEAP_NUM
	};


	/*-- �p�C�v���C�� --*/

	// �p�C�v���C���̐���
	void GeneratePipline(Microsoft::WRL::ComPtr<ID3DBlob> csBlob);


	/*-- �f�B�X�N���v�^�q�[�v --*/

	// �f�B�X�N���v�^�q�[�v�Ɋe�f�B�X�N���v�^���Z�b�g����
	void CommitDescHeap();

};