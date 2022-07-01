#pragma once
#include "DirectXBase.h"
#include "DescriptorHeapMgr.h"

// ���I�Ȓ萔�o�b�t�@�N���X
class DynamicConstBuffer {

private:

	/*===== �����o�ϐ� =====*/

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> buffer;
	int descHeapIndex;


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	DynamicConstBuffer();

	// ��������
	void Generate(UINT BufferSize, const wchar_t* Name = L"");

	// �f�[�^�������ݏ���
	void Write(UINT BufferIndex, const void* Data, UINT Size);

	// �A�N�Z�b�^
	Microsoft::WRL::ComPtr<ID3D12Resource> GetBuffer(UINT BufferIndex) const { return buffer[BufferIndex]; }
	const int& GetDescHeapIndex() { return descHeapIndex; }


private:

	// �o�b�t�@�S�ʂ𐶐����鏈��
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType);

	// �A���C�������g�p
	inline UINT RoundUp(size_t Size, UINT Align) { return UINT(Size + Align - 1) & ~(Align - 1); }

};