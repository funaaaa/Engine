#include "DynamicConstBuffer.h"

DynamicConstBuffer::DynamicConstBuffer()
{

	/*===== �R���X�g���N�^ =====*/

	buffer.clear();

}

void DynamicConstBuffer::Generate(UINT bufferSize, const wchar_t* name)
{

	/*===== �o�b�t�@�������� =====*/

	// �o�b�N�o�b�t�@�̐����o�b�t�@�𐶐�����B
	bufferSize = RoundUp(bufferSize, 256);
	UINT count = 2;
	buffer.resize(count);

	// �o�b�t�@�𐶐�����B
	for (UINT i = 0; i < count; ++i) {
		buffer[i] = CreateBuffer(
			bufferSize,
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_HEAP_TYPE_UPLOAD);

		// ���O���Z�b�g����B
		if (buffer[i]) {
			buffer[i]->SetName(name);
		}
	}

}

void DynamicConstBuffer::Write(UINT bufferIndex, const void* data, UINT size)
{

	/*===== �f�[�^�������ݏ��� =====*/

	auto& buff = buffer[bufferIndex];
	void* dst = nullptr;
	buff->Map(0, nullptr, &dst);
	if (dst) {
		memcpy(dst, data, size);
		buff->Unmap(0, nullptr);
	}

}

Microsoft::WRL::ComPtr<ID3D12Resource> DynamicConstBuffer::CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType)
{

	/*===== �o�b�t�@�S�ʂ𐶐����鏈�� =====*/

	// ��������ݒ�p�\���̂�ݒ肷��B
	D3D12_HEAP_PROPERTIES heapProps{};
	if (heapType == D3D12_HEAP_TYPE_DEFAULT) {
		heapProps = D3D12_HEAP_PROPERTIES{
		D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
		};
	}
	if (heapType == D3D12_HEAP_TYPE_UPLOAD) {
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
	resDesc.Width = size;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc = { 1, 0 };
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = flags;

	// �o�b�t�@�������߂��o���B
	hr = DirectXBase::Ins()->dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		initialState,
		nullptr,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
	);

	// �f�B�X�N���v�^�q�[�v�̐���
	descHeapIndex = DescriptorHeapMgr::Ins()->GetHead();

	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), descHeapIndex, DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)size;
	DirectXBase::Ins()->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	DescriptorHeapMgr::Ins()->IncrementHead();

	// �����Ɏ��s������B
	if (FAILED(hr)) {
		OutputDebugStringA("CreateBuffer failed.\n");
	}

	return resource;

}
