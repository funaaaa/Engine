#pragma once
#include <array>
#include <d3d12.h>
#include <wrl.h>

// D3D12�̃��\�[�X�N���X�����b�v�����N���X
class DoubleResourceWrapper {

private:

	/*===== �����o�ϐ� =====*/

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> resource_;		// ���\�[�X�{��
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> uploadResource_;	// �A�b�v���[�h�p���\�[�X
	std::array<void*, 2> mapAddress_;										// GPU�Ƀf�[�^��]������ۂ̃A�h���X
	size_t bufferSize_;														// �o�b�t�@�̃T�C�Y


public:

	/*===== �����o�֐� =====*/

	// �o�b�t�@�𐶐�
	void CreateBuffer(size_t Size, void* Data, const wchar_t* Name);

	// �f�[�^����������
	void WriteData(int Index, size_t Size, void* Data);

	// �o�b�t�@���擾
	Microsoft::WRL::ComPtr<ID3D12Resource> GetBuffer(int Index) { return resource_[Index]; }

};