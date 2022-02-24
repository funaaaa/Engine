#pragma once
#include "DirectXBase.h"

// TLAS�N���X
class TLAS {

private:

	/*===== �����o�ϐ� =====*/

	ComPtr<ID3D12Resource> tlasBuffer;		// TLAS�p�o�b�t�@
	ComPtr<ID3D12Resource> scratchBuffer;	// �X�N���b�`�o�b�t�@

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;


public:

	/*===== �����o�֐� =====*/

	// TLAS�̐���
	void GenerateTLAS();

	// �A�N�Z�b�^
	ComPtr<ID3D12Resource> GetTLASBuffer() { return tlasBuffer; }
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& GetDescriptorHeap() { return descriptorHeap; }


private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize);

	// �o�b�t�@�S�ʂ𐶐����鏈��
	ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);

	// �����\���̂̐ݒ�p�֐�
	void SettingAccelerationStructure();

	// �����\���̂̍\�z�p�֐�
	void CreateAccelerationStructure();

};
