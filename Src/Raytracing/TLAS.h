#pragma once
#include "Engine.h"
#include <array>

// TLAS�N���X
class TLAS {

private:

	/*===== �����o�ϐ� =====*/

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> tlasBuffer_;			// TLAS�p�o�b�t�@
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> scratchBuffer_;		// �X�N���b�`�o�b�t�@
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> tlasUpdateBuffer_;	// TLAS�X�V�p�o�b�t�@
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> instanceDescBuffer_;	// �C���X�^���X�o�b�t�@
	std::array<void*, 2> instanceDescMapAddress_;

	std::array<int, 2> descriptorHeapIndex_;


public:

	/*===== �����o�֐� =====*/

	// TLAS�̐���
	void GenerateTLAS();

	// �f�B�X�N���v�^�q�[�v�̃C���f�b�N�X�̃A�N�Z�b�T
	inline int GetDescriptorHeapIndex(int Index) { return descriptorHeapIndex_[Index]; }

	// �C���X�^���X���X�V
	void Update();


private:

	// �A�h���X�ɏ����������ޏ���
	void WriteToMemory(void* MapAddress, const void* PData, size_t DataSize);

	// �����\���̂̐ݒ�p�֐�
	void SettingAccelerationStructure(int Index);

	// �����\���̂̍\�z�p�֐�
	void CreateAccelerationStructure(int Index);

};
