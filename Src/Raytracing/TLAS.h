#pragma once
#include "Engine.h"
#include <array>

// TLAS�N���X
class TLAS {

private:

	/*===== �����o�ϐ� =====*/

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> tlasBuffer_;			// TLAS�p�o�b�t�@
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> scratchBuffer_;		// �X�N���b�`�o�b�t�@
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> instanceDescBuffer_;	// �C���X�^���X�o�b�t�@
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> tlasUpdateBuffer_;	// TLAS�X�V�p�o�b�t�@

	int descriptorHeapIndex_;


public:

	/*===== �����o�֐� =====*/

	// TLAS�̐���
	void GenerateTLAS();

	// �A�N�Z�b�T
	Microsoft::WRL::ComPtr<ID3D12Resource> GetTLASBuffer(int Index) { return tlasBuffer_[Index]; }

	// �f�B�X�N���v�^�q�[�v�̃C���f�b�N�X�̃A�N�Z�b�T
	inline int GetDescriptorHeapIndex() { return descriptorHeapIndex_; }

	// �C���X�^���X���X�V
	void Update();


private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* PData, size_t DataSize);

	// �����\���̂̐ݒ�p�֐�
	void SettingAccelerationStructure(int Index);

	// �����\���̂̍\�z�p�֐�
	void CreateAccelerationStructure(int Index);

};
