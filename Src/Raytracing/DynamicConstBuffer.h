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
	void Generate(UINT bufferSize, const wchar_t* name = L"");

	// �f�[�^�������ݏ���
	void Write(UINT bufferIndex, const void* data, UINT size);

	// �A�N�Z�b�^
	Microsoft::WRL::ComPtr<ID3D12Resource> GetBuffer(UINT bufferIndex) const { return buffer[bufferIndex]; }


private:

	// �o�b�t�@�S�ʂ𐶐����鏈��
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);

	// �A���C�������g�p
	inline UINT RoundUp(size_t size, UINT align) { return UINT(size + align - 1) & ~(align - 1); }

};