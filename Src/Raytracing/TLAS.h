#pragma once
#include "DirectXBase.h"

// TLAS�N���X
class TLAS {

private:

	/*===== �����o�ϐ� =====*/

	Microsoft::WRL::ComPtr<ID3D12Resource> tlasBuffer;			// TLAS�p�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer;		// �X�N���b�`�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> instanceDescBuffer;	// �C���X�^���X�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> tlasUpdateBuffer;	// TLAS�X�V�p�o�b�t�@

	int descriptorHeapIndex;


public:

	/*===== �����o�֐� =====*/

	// TLAS�̐���
	void GenerateTLAS(const wchar_t* name = nullptr);

	// �A�N�Z�b�^
	Microsoft::WRL::ComPtr<ID3D12Resource> GetTLASBuffer() { return tlasBuffer; }

	// �f�B�X�N���v�^�q�[�v�̃C���f�b�N�X�̃A�N�Z�b�^
	inline const int& GetDescriptorHeapIndex() { return descriptorHeapIndex; }

	// �C���X�^���X���X�V
	void Update();


private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize);

	// �o�b�t�@�S�ʂ𐶐����鏈��
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);

	// �����\���̂̐ݒ�p�֐�
	void SettingAccelerationStructure();

	// �����\���̂̍\�z�p�֐�
	void CreateAccelerationStructure();

};
