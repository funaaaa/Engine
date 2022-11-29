#include "DynamicConstBuffer.h"

DynamicConstBuffer::DynamicConstBuffer()
{

	/*===== �R���X�g���N�^ =====*/

	buffer_.clear();
	uploadBuffer_.clear();

}

void DynamicConstBuffer::Generate(UINT BufferSize, const wchar_t* Name)
{

	/*===== �o�b�t�@�������� =====*/

	// �o�b�N�o�b�t�@�̐����o�b�t�@�𐶐�����B
	BufferSize = RoundUp(BufferSize, 256);
	UINT count = 2;
	buffer_.resize(count);
	uploadBuffer_.resize(count);

	// �o�b�t�@�𐶐�����B
	for (UINT i = 0; i < count; ++i) {
		buffer_[i] = CreateBuffer(
			BufferSize,
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
			D3D12_HEAP_TYPE_DEFAULT);

		// ���O���Z�b�g����B
		if (buffer_[i]) {
			buffer_[i]->SetName(Name);
		}

		uploadBuffer_[i] = CreateBuffer(
			BufferSize,
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_HEAP_TYPE_UPLOAD);

		// ���O���Z�b�g����B
		if (uploadBuffer_[i]) {
			uploadBuffer_[i]->SetName(Name);
		}
	}

}

void DynamicConstBuffer::Write(UINT BufferIndex, const void* Data, UINT Size)
{

	/*===== �f�[�^�������ݏ��� =====*/

	auto& buff = uploadBuffer_[BufferIndex];
	void* dst = nullptr;
	buff->Map(0, nullptr, &dst);
	if (dst) {
		memcpy(dst, Data, Size);
		buff->Unmap(0, nullptr);
	}

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(buffer_[BufferIndex].Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);
	Engine::Ins()->mainGraphicsCmdList_->CopyResource(buffer_[BufferIndex].Get(), uploadBuffer_[BufferIndex].Get());
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(buffer_[BufferIndex].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &barrier);

}

Microsoft::WRL::ComPtr<ID3D12Resource> DynamicConstBuffer::CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType)
{

	/*===== �o�b�t�@�S�ʂ𐶐����鏈�� =====*/

	// ��������ݒ�p�\���̂�ݒ肷��B
	D3D12_HEAP_PROPERTIES heapProps{};
	if (HeapType == D3D12_HEAP_TYPE_DEFAULT) {
		heapProps = D3D12_HEAP_PROPERTIES{
		D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
		};
	}
	if (HeapType == D3D12_HEAP_TYPE_UPLOAD) {
		heapProps = D3D12_HEAP_PROPERTIES{
		D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
		};
	}

	// ���ۂɃo�b�t�@�𐶐�����B
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = 0;
	resDesc.Width = Size;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc = { 1, 0 };
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = Flags;

	// �o�b�t�@�������߂��o���B
	hr = Engine::Ins()->device_.dev_->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		InitialState,
		nullptr,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
	);

	// �f�B�X�N���v�^�q�[�v�̐���
	descHeapIndex_ = DescriptorHeapMgr::Ins()->GetHead();

	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), descHeapIndex_, Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)Size;
	Engine::Ins()->device_.dev_->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	DescriptorHeapMgr::Ins()->IncrementHead();

	// �����Ɏ��s������B
	if (FAILED(hr)) {
		OutputDebugStringA("CreateBuffer failed.\n");
	}

	return resource;

}
