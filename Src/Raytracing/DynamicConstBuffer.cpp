#include "DynamicConstBuffer.h"
#include "FHelper.h"

DynamicConstBuffer::DynamicConstBuffer()
{

	/*===== �R���X�g���N�^ =====*/

	buffer_.clear();

}

void DynamicConstBuffer::Generate(UINT BufferSize, const wchar_t* Name)
{

	/*===== �o�b�t�@�������� =====*/

	// �o�b�N�o�b�t�@�̐����o�b�t�@�𐶐�����B
	BufferSize = RoundUp(BufferSize, 256);
	UINT count = 2;
	buffer_.resize(count);

	// �o�b�t�@�𐶐�����B
	for (UINT i = 0; i < count; ++i) {
		buffer_[i] = CreateBuffer(
			BufferSize,
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_HEAP_TYPE_UPLOAD);

		// ���O���Z�b�g����B
		if (buffer_[i]) {
			buffer_[i]->SetName(Name);
		}
	}

}

void DynamicConstBuffer::Write(UINT BufferIndex, const void* Data, UINT Size)
{

	/*===== �f�[�^�������ݏ��� =====*/

	auto& buff = buffer_[BufferIndex];
	void* dst = nullptr;
	buff->Map(0, nullptr, &dst);
	if (dst) {
		memcpy(dst, Data, Size);
		buff->Unmap(0, nullptr);
	}

}

Microsoft::WRL::ComPtr<ID3D12Resource> DynamicConstBuffer::CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType)
{

	/*===== �o�b�t�@�S�ʂ𐶐����鏈�� =====*/

	// �o�b�t�@�𐶐��B
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = FHelper::CreateBuffer(Size, Flags, InitialState, HeapType);

	// �f�B�X�N���v�^�q�[�v�̐���
	descHeapIndex_ = DescriptorHeapMgr::Ins()->GetHead();

	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), descHeapIndex_, Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)Size;
	Engine::Ins()->device_.dev_->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	DescriptorHeapMgr::Ins()->IncrementHead();

	return resource;

}
