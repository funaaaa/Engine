#include "RayComputeShader.h"
#include <d3dcompiler.h>
#include "DirectXBase.h"
#include "DescriptorHeapMgr.h"
#include "RayRootsignature.h"
#include <assert.h>

void RayComputeShader::Setting(LPCWSTR CsPath, const int& SRVCount, const int& CBVCount, const int& UAVCount, std::vector<int> UAVIndex)
{

	/*===== �Z�b�e�B���O���� =====*/

	Microsoft::WRL::ComPtr<ID3DBlob> csBlob = nullptr;		//�R���s���[�g�V�F�[�_�[�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;	//�G���[�I�u�W�F�N�g

	//�R���s���[�g�V�F�[�_�[�F�[�_�̓ǂݍ��݂ƃR���p�C��
	csBlob = LoadShader(CsPath, "main", "cs_5_0", csBlob.Get(), errorBlob.Get());

	// �����œn���ꂽ����ۑ��B
	inputSRVCount = SRVCount;
	inputCBVCount = CBVCount;
	inputUAVCount = UAVCount;
	inputUAVIndex = UAVIndex;

	// ���[�g�V�O�l�`���𐶐��B
	rootSignature_ = std::make_shared<RayRootsignature>();

	// ���[�g�V�O�l�`���p�����[�^�[��ݒ�B
	for (int index_ = 0; index_ < SRVCount; ++index_) {
		rootSignature_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, index_);
	}
	for (int index_ = 0; index_ < CBVCount; ++index_) {
		rootSignature_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, index_);
	}
	for (int index_ = 0; index_ < UAVCount + 1; ++index_) {	// +1���Ă���̂͏o�͗p�̃f�[�^��UAV������
		rootSignature_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, index_);
	}

	// ���[�g�V�O�l�`���𐶐��B
	rootSignature_->Create(false, L"RayComputeShader");

	// �p�C�v���C����ݒ�B
	D3D12_COMPUTE_PIPELINE_STATE_DESC  psoDesc = { 0 };
	psoDesc.pRootSignature = rootSignature_->GetRootSig().Get();
	psoDesc.CS = CD3DX12_SHADER_BYTECODE(csBlob.Get());
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	psoDesc.NodeMask = 0;

	auto hr = DirectXBase::Ins()->dev_->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pipline_));
	if (FAILED(hr)) {
		//�����Ɏ��s����
		assert(0);
	}

}

void RayComputeShader::Dispatch(const UINT& ThreadGroupCountX, const UINT& ThreadGroupCountY, const UINT& ThreadGroupCountZ, const int& OutputIndex, std::vector<D3D12_GPU_VIRTUAL_ADDRESS> InputCBV)
{

	/*===== ���s���� =====*/

	// ���[�g�V�O�l�`�����Z�b�g�B
	DirectXBase::Ins()->cmdList_->SetComputeRootSignature(rootSignature_->GetRootSig().Get());

	// �p�C�v���C�����Z�b�g�B
	DirectXBase::Ins()->cmdList_->SetPipelineState(pipline_.Get());

	// �ꉞUAV���Z�b�g�B
	int counter = 0;
	for (auto& index_ : inputUAVIndex) {

		DirectXBase::Ins()->cmdList_->SetComputeRootDescriptorTable(counter + inputCBVCount, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(index_));

		++counter;

	}

	// �o�b�N�o�b�t�@�̃t���[�����擾�B
	//auto frameIndex = DirectXBase::Ins()->swapchain->GetCurrentBackBufferIndex();

	// CBV���Z�b�g�B
	counter = 0;
	for (auto& index_ : InputCBV) {

		DirectXBase::Ins()->cmdList_->SetComputeRootConstantBufferView(counter, index_);

		++counter;

	}

	// �o�͗pUAV���Z�b�g�B
	DirectXBase::Ins()->cmdList_->SetComputeRootDescriptorTable(inputUAVCount + inputCBVCount, DescriptorHeapMgr::Ins()->GetGPUHandleIncrement(OutputIndex));

	// �f�B�X�p�b�`�B
	DirectXBase::Ins()->cmdList_->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);

}

LPD3DBLOB RayComputeShader::LoadShader(LPCWSTR ShaderFileName, const std::string& EntryPointName, const std::string& ShaderModel, ID3DBlob* ShaderBlob, ID3DBlob* ErrorBlob)
{

	/*===== �V�F�[�_�[�ǂݍ��� =====*/

	HRESULT result = D3DCompileFromFile(
		ShaderFileName,										// �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					// �C���N���[�h�\�ɂ���
		EntryPointName.c_str(), ShaderModel.c_str(),		// �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	// �f�o�b�O�p�ݒ�
		0,
		&ShaderBlob, &ErrorBlob);

	// �G���[�`�F�b�N
	CheckRoadShaderError(result, ErrorBlob);

	return ShaderBlob;
}

void RayComputeShader::CheckRoadShaderError(HRESULT Result, ID3DBlob* ErrorBlob)
{

	/*===== �V�F�[�_�[�ǂݍ��ݎ��̃G���[���`�F�b�N =====*/

	if (FAILED(Result)) {

		//errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(ErrorBlob->GetBufferSize());

		std::copy_n((char*)ErrorBlob->GetBufferPointer(),
			ErrorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

}
