#pragma once
#include "DirectXBase.h"

// TLAS�N���X
class TLAS {

private:

	/*===== �����o�ϐ� =====*/

	Microsoft::WRL::ComPtr<ID3D12Resource> tlasBuffer;			// TLAS�p�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer_;		// �X�N���b�`�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> instanceDescBuffer;	// �C���X�^���X�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> tlasUpdateBuffer;	// TLAS�X�V�p�o�b�t�@

	int descriptorHeapIndex;


public:

	/*===== �����o�֐� =====*/

	// TLAS�̐���
	void GenerateTLAS();

	// �A�N�Z�b�T
	Microsoft::WRL::ComPtr<ID3D12Resource> GetTLASBuffer() { return tlasBuffer; }

	// �f�B�X�N���v�^�q�[�v�̃C���f�b�N�X�̃A�N�Z�b�T
	inline const int& GetDescriptorHeapIndex() { return descriptorHeapIndex; }

	// �C���X�^���X���X�V
	void Update();


private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* PData, size_t DataSize);

	// �o�b�t�@�S�ʂ𐶐����鏈��
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType);

	// �����\���̂̐ݒ�p�֐�
	void SettingAccelerationStructure();

	// �����\���̂̍\�z�p�֐�
	void CreateAccelerationStructure();

};
