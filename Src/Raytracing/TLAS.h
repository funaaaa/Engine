#pragma once
#include "Engine.h"

// TLAS�N���X
class TLAS {

private:

	/*===== �����o�ϐ� =====*/

	Microsoft::WRL::ComPtr<ID3D12Resource> tlasBuffer_;			// TLAS�p�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer_;		// �X�N���b�`�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> instanceDescBuffer_;	// �C���X�^���X�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> tlasUpdateBuffer_;	// TLAS�X�V�p�o�b�t�@

	int descriptorHeapIndex_;


public:

	/*===== �����o�֐� =====*/

	// TLAS�̐���
	void GenerateTLAS();

	// �A�N�Z�b�T
	Microsoft::WRL::ComPtr<ID3D12Resource> GetTLASBuffer() { return tlasBuffer_; }

	// �f�B�X�N���v�^�q�[�v�̃C���f�b�N�X�̃A�N�Z�b�T
	inline int GetDescriptorHeapIndex() { return descriptorHeapIndex_; }

	// �C���X�^���X���X�V
	void Update();


private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* PData, size_t DataSize);

	// �����\���̂̐ݒ�p�֐�
	void SettingAccelerationStructure();

	// �����\���̂̍\�z�p�֐�
	void CreateAccelerationStructure();

};
