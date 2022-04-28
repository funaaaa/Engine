#include "RayRootsignature.h"
#include "DirectXBase.h"

void RayRootsignature::AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT shaderRegister, UINT registerSpace)
{

	/*===== ���[�g�p�����[�^�[�ǉ����� =====*/


	if (type == D3D12_DESCRIPTOR_RANGE_TYPE_SRV) {

		descRange[rootparamCount] = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, shaderRegister, registerSpace);
		rootparam[rootparamCount].InitAsDescriptorTable(1, &descRange[rootparamCount]);
		//rootparam[rootparamCount].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		rootparam[rootparamCount].Descriptor.ShaderRegister = shaderRegister;
		rootparam[rootparamCount].Descriptor.RegisterSpace = registerSpace;
		rootparam[rootparamCount].DescriptorTable.NumDescriptorRanges = 1;

	}
	else if (type == D3D12_DESCRIPTOR_RANGE_TYPE_CBV) {

		rootparam[rootparamCount].InitAsConstantBufferView(shaderRegister, registerSpace);
		rootparam[rootparamCount].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;

	}
	else if (type == D3D12_DESCRIPTOR_RANGE_TYPE_UAV) {

		descRange[rootparamCount] = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, shaderRegister, registerSpace);
		rootparam[rootparamCount].InitAsDescriptorTable(1, &descRange[rootparamCount]);
		//rootparam[rootparamCount].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
		rootparam[rootparamCount].Descriptor.ShaderRegister = shaderRegister;
		rootparam[rootparamCount].Descriptor.RegisterSpace = registerSpace;
		rootparam[rootparamCount].DescriptorTable.NumDescriptorRanges = 1;

	}



	// ���[�g�p�����[�^�[�̐����X�V
	++rootparamCount;

}

void RayRootsignature::AddStaticSampler(const int& registerSpace) {

	/*===== �X�^�e�B�b�N�T���v���[�ǉ����� =====*/

	CD3DX12_STATIC_SAMPLER_DESC buff;
	buff.Init(0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR
	);
	buff.RegisterSpace = registerSpace;
	buff.ShaderRegister = 0;
	sampler[samplerCount] = buff;

	++samplerCount;

}

void RayRootsignature::Create(const bool& isLocal, const wchar_t* name)
{

	/*===== ���[�g�V�O�l�`���������� =====*/

	// ���[�g�V�O�l�`���ݒ�p�\���̂�ݒ�B
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.NumParameters = rootparamCount;
	rootSigDesc.pParameters = rootparam.data();
	rootSigDesc.NumStaticSamplers = samplerCount;
	rootSigDesc.pStaticSamplers = sampler.data();

	// ���[�J�����[�g�V�O�l�`���̃t���O�������Ă�����A���[�J�����[�g�V�O�l�`���̃t���O��ݒ肷��B
	if (isLocal) {

		rootSigDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

	}

	// ���[�g�V�O�l�`�������B
	Microsoft::WRL::ComPtr<ID3DBlob> blob, errBlob;
	D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &blob, &errBlob);
	if (errBlob.Get() != nullptr) {
		std::string a = static_cast<char*>(errBlob->GetBufferPointer());
		_RPTF0(_CRT_WARN, a.c_str());
		int b = 0;
	}
	HRESULT result = DirectXBase::Instance()->dev->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature));

	// ���O��ݒ�
	if (name != nullptr) {
		rootsignature->SetName(name);
	}

}
