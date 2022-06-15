#pragma once
#include <wrl.h>
#include <DirectXTex/d3dx12.h>
#include <memory>
#include <vector>

class RayRootsignature;

// �V�������C�g���p�ɍ�����R���s���[�g�V�F�[�_�[
class RayComputeShader {

private:

	/*===== �����o�ϐ� =====*/

	std::shared_ptr<RayRootsignature> rootSignature;			// ���[�g�V�O�l�`��
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipline;		// �p�C�v���C��
	int inputCBVCount;					// ���͂���CBV�̐�
	int inputSRVCount;					// ���͂���SRV�̐�
	int inputUAVCount;					// ���͂���UAV�̐�
	std::vector<int> inputUAVIndex;		// ����UAV�f�[�^�̃f�B�X�N���v�^�q�[�v��ł̃C���f�b�N�X
	int outputIndex;					// �o�̓f�[�^�̃f�B�X�N���v�^�q�[�v��ł̃C���f�b�N�X


public:

	/*===== �����o�֐� =====*/

	// �Z�b�e�B���O����
	void Setting(LPCWSTR CsPath, const int& SRVCount, const int& CVBCount, const int& UAVCount, std::vector<int> SRVIndex);

	// ���s�I
	void Dispatch(const UINT& ThreadGroupCountX, const UINT& ThreadGroupCountY, const UINT& ThreadGroupCountZ, const int& OutputIndex, std::vector<D3D12_GPU_VIRTUAL_ADDRESS> InputCBV);

	// ���͗pUAV��ύX�B
	void ChangeInputUAVIndex(const std::vector<int>& NewInputUAV) { inputUAVIndex = NewInputUAV; }

private:

	//�V�F�[�_�[�ǂݍ���
	LPD3DBLOB LoadShader(LPCWSTR ShaderFileName, const std::string& EntryPointName, const std::string& ShaderModel, ID3DBlob* ShaderBlob, ID3DBlob* ErrorBlob);
	//�V�F�[�_�[�̃��[�h�G���[���`�F�b�N
	void CheckRoadShaderError(HRESULT Result, ID3DBlob* ErrorBlob);


};