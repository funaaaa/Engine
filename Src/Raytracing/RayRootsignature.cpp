#include "RayRootsignature.h"
#include "DirectXBase.h"

void RayRootsignature::AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT ShaderRegister, UINT RegisterSpace)
{

	/*===== ���[�g�p�����[�^�[�ǉ����� =====*/


	if (Type == D3D12_DESCRIPTOR_RANGE_TYPE_SRV) {

		descRange[rootparamCount] = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, ShaderRegister, RegisterSpace);
		rootparam[rootparamCount].InitAsDescriptorTable(1, &descRange[rootparamCount]);
		//rootparam[rootparamCount].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		rootparam[rootparamCount].Descriptor.ShaderRegister = ShaderRegister;
		rootparam[rootparamCount].Descriptor.RegisterSpace = RegisterSpace;
		rootparam[rootparamCount].DescriptorTable.NumDescriptorRanges = 1;

	}
	else if (Type == D3D12_DESCRIPTOR_RANGE_TYPE_CBV) {
		
		rootparam[rootparamCount].InitAsConstantBufferView(ShaderRegister, RegisterSpace);
		rootparam[rootparamCount].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;

	}
	else if (Type == D3D12_DESCRIPTOR_RANGE_TYPE_UAV) {

		descRange[rootparamCount] = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, ShaderRegister, RegisterSpace);
		rootparam[rootparamCount].InitAsDescriptorTable(1, &descRange[rootparamCount]);
		//rootparam[rootparamCount].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
		rootparam[rootparamCount].Descriptor.ShaderRegister = ShaderRegister;
		rootparam[rootparamCount].Descriptor.RegisterSpace = RegisterSpace;
		rootparam[rootparamCount].DescriptorTable.NumDescriptorRanges = 1;

	}



	// ���[�g�p�����[�^�[�̐����X�V
	++rootparamCount;

}

void RayRootsignature::AddStaticSampler(const int& RegisterSpace) {

	/*===== �X�^�e�B�b�N�T���v���[�ǉ����� =====*/

	CD3DX12_STATIC_SAMPLER_DESC buff;
	buff.Init(0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR
	);
	buff.RegisterSpace = RegisterSpace;
	buff.ShaderRegister = 0;
	sampler[samplerCount] = buff;

	++samplerCount;

}

void RayRootsignature::Create(const bool& IsLocal, const wchar_t* Name)
{

	/*===== ���[�g�V�O�l�`���������� =====*/

	// ���[�g�V�O�l�`���ݒ�p�\���̂�ݒ�B
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.NumParameters = rootparamCount;
	rootSigDesc.pParameters = rootparam.data();
	rootSigDesc.NumStaticSamplers = samplerCount;
	rootSigDesc.pStaticSamplers = sampler.data();

	// ���[�J�����[�g�V�O�l�`���̃t���O�������Ă�����A���[�J�����[�g�V�O�l�`���̃t���O��ݒ肷��B
	if (IsLocal) {

		rootSigDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

	}

	// ���[�g�V�O�l�`�������B
	Microsoft::WRL::ComPtr<ID3DBlob> blob, errBlob;
	D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &blob, &errBlob);
	if (errBlob.Get() != nullptr) {
		std::string a = static_cast<char*>(errBlob->GetBufferPointer());
		_RPTF0(_CRT_WARN, a.c_str());
	}
	DirectXBase::Ins()->dev->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature));

	// ���O��ݒ�
	if (Name != nullptr) {
		rootsignature->SetName(Name);
	}

}
