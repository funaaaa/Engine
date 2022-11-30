#pragma once
#include "Engine.h"
#include "Singleton.h"

class DescriptorHeapMgr : public Singleton<DescriptorHeapMgr> {

private:

	/*===== �����o�ϐ� =====*/

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;	// CBV,SRV,UAV�p�f�B�X�N���v�^�q�[�v
	int head_;										// �擪�̃C���f�b�N�X


private:

	/*===== �萔 =====*/

	const int CBV_SRV_UAV_COUNT = 4096 * 2;	// CBV,SRV,UAV�p�f�B�X�N���v�^�̐�

public:

	/*===== �����o�֐� =====*/

	// �f�B�X�N���v�^�q�[�v�̐����B
	void GenerateDescriptorHeap() {

		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;			// �V�F�[�_�[���猩����
		descHeapDesc.NumDescriptors = CBV_SRV_UAV_COUNT;
		// �f�B�X�N���v�^�q�[�v�̐����B
		Engine::Ins()->device_.dev_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descriptorHeap_));
		descriptorHeap_->SetName(L"DescriptorHeapMgr");

		// �擪��������
		head_ = 0;

	}

	// �f�B�X�N���v�^�q�[�v�̃Q�b�^�B
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return descriptorHeap_; }

	// �擪�̃Q�b�^
	int GetHead() { return head_; }

	// �擪���C���N�������g
	inline void IncrementHead() { ++head_; }

	// �w��̃C���f�b�N�X��CPU�n���h�����擾
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandleIncrement(int Index) {

		return CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descriptorHeap_.Get()->GetGPUDescriptorHandleForHeapStart(), Index, Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	}


};