#pragma once
#include <DirectXTex/d3dx12.h>
#include <array>
#include <wrl.h>

// ���C�g���[�V���O�Ŏg�p����O���[�o�����[�g�V�O�l�`���A���[�J�����[�g�V�O�l�`���N���X
class RayRootsignature {

private:

	/*===== �萔 =====*/

	static const int MAX_ROOTPARAM = 16;
	static const int MAX_SAMPLER = 26;


private:

	/*===== �����o�ϐ� =====*/

	std::array<CD3DX12_ROOT_PARAMETER, MAX_ROOTPARAM> rootparam;	// ���[�g�p�����[�^�[
	std::array<CD3DX12_DESCRIPTOR_RANGE, MAX_ROOTPARAM> descRange;	// �f�B�X�N���v�^�e�[�u��
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootsignature_;		// ���[�g�V�O�l�`��
	std::array<CD3DX12_STATIC_SAMPLER_DESC, MAX_SAMPLER> sampler;	// �X�^�e�B�b�N�T���v���[
	UINT rootparamCount;											// ���[�g�p�����[�^�[��
	UINT samplerCount;												// �T���v���[�̐�


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	RayRootsignature() {
		rootparamCount = 0;
		samplerCount = 0;
	}

	// ���[�g�p�����[�^�[�ǉ�����
	void AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT ShaderRegister, UINT RegisterSpace = 0);

	// �X�^�e�B�b�N�T���v���[�ǉ�����
	void AddStaticSampler(const int& RegisterSpace = 0);

	// ���[�g�V�O�l�`���̐���
	void Create(const bool& IsLocal, const wchar_t* Name = nullptr);

	// �A�N�Z�b�^
	inline Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSig() { return rootsignature_; }

};