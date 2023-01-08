#pragma once
#include "Engine.h"
#include <array>

// TLAS�N���X
class TLAS {

private:

	/*===== �����o�ϐ� =====*/

	Microsoft::WRL::ComPtr<ID3D12Resource> tlasBuffer_;			// TLAS�p�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer_;		// �X�N���b�`�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> instanceDescBuffer_;	// �C���X�^���X�o�b�t�@
	void* instanceDescMapAddress_;

	size_t instanceDescSize_;

	int descriptorHeapIndex_;


public:

	/*===== �����o�֐� =====*/

	// TLAS�̐���
	void GenerateTLAS();

	// �f�B�X�N���v�^�q�[�v�̃C���f�b�N�X�̃A�N�Z�b�T
	inline int GetDescriptorHeapIndex() { return descriptorHeapIndex_; }

	// �C���X�^���X���X�V
	void Update();


private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(void* MapAddress, const void* PData, size_t DataSize);

	// �����\���̂̐ݒ�p�֐�
	void SettingAccelerationStructure();

	// �����\���̂̍\�z�p�֐�
	void CreateAccelerationStructure();

};
