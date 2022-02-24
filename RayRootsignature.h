#pragma once
#include "DirectXBase.h"
#include <array>

using namespace std;

// ���C�g���[�V���O�Ŏg�p����O���[�o�����[�g�V�O�l�`���A���[�J�����[�g�V�O�l�`���N���X
class RayRootsignature {

private:

	/*===== �萔 =====*/

	static const int MAX_ROOTPARAM = 16;


private:

	/*===== �����o�ϐ� =====*/

	array<CD3DX12_ROOT_PARAMETER, MAX_ROOTPARAM> rootparam;		// ���[�g�p�����[�^�[
	array<CD3DX12_DESCRIPTOR_RANGE, MAX_ROOTPARAM> descRange;	// �f�B�X�N���v�^�e�[�u��
	ComPtr<ID3D12RootSignature> rootsignature;					// ���[�g�V�O�l�`��
	UINT rootparamCount;										// ���[�g�p�����[�^�[��


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	RayRootsignature() { rootparamCount = 0; }

	// ���[�g�p�����[�^�[�ǉ�����
	void AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT shaderRegister, UINT registerSpace = 0);

	// ���[�g�V�O�l�`���̐���
	void Create(const bool& isLocal, const wchar_t* name = nullptr);

	// �A�N�Z�b�^
	inline ComPtr<ID3D12RootSignature>& GetRootSig() { return rootsignature; }

};