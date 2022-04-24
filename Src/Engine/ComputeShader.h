#pragma once
#include <wrl.h>
#include <DirectXTex/d3dx12.h>

class StructuredBuffer;
class RWStructuredBuffer;

//�R���s���[�g�V�F�[�_�[�N���X
class ComputeShader {

private:

	/*-- �����o�ϐ� --*/

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;	//���[�g�V�O�l�`��
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipline;		//�p�C�v���C��
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;		//�f�B�X�N���v�^�q�[�v ���Ƃ����ۑ�����Ă���̂�inputSB��outputSB�̒�
	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUDescHeapHandle;			//SRV�J�n�n���h�� GPU
	D3D12_GPU_DESCRIPTOR_HANDLE uavGPUDescHeapHandle;			//UAV�J�n�n���h�� GPU

public:

	StructuredBuffer* inputSB;									//���͗p�\���̃o�b�t�@
	RWStructuredBuffer* outputSB;								//�������ݗp�\���̃o�b�t�@


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
	void Init(LPCWSTR csPath,
		int sizeOfInput, int countOfInputElement, void* inputFormatData,
		int sizeOfOutput, int countOfOutputElement, void* outputFormatData);

	//�f�B�X�p�b�`�R�[��
	void Dispatch(UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ);


	/*-- �V�F�[�_�[�ǂݍ��݊֌W --*/

	//�V�F�[�_�[�ǂݍ���
	LPD3DBLOB LoadShader(LPCWSTR shaderFileName, const char entryPointName[], const char shaderModel[], ID3DBlob* shaderBlob, ID3DBlob* errorBlob);
	//�V�F�[�_�[�̃��[�h�G���[���`�F�b�N
	void CheckRoadShaderError(HRESULT result, ID3DBlob* errorBlob);


	/*-- ���[�g�V�O�l�`�� --*/

	//���[�g�V�O�l�`���̐���
	void GenerateRootSignature();

	//�f�B�X�N���v�^�q�[�v�̗v�f
	enum {
		enDescriptorHeap_CB,
		enDescriptorHeap_SRV,
		enDescriptorHeap_UAV,
		enNumDescriptorHeap
	};


	/*-- �p�C�v���C�� --*/

	//�p�C�v���C���̐���
	void GeneratePipline(Microsoft::WRL::ComPtr<ID3DBlob> csBlob);


	/*-- �f�B�X�N���v�^�q�[�v --*/

	//�f�B�X�N���v�^�q�[�v�Ɋe�f�B�X�N���v�^���Z�b�g����
	void CommitDescHeap();


	/*-- �@�\�֐� --*/

	//���͗p�\�����o�b�t�@���X�V
	void UpdateInputSB(void* updateSB);

};