#include "TextureManager.h"
#include "Engine.h"
#include "DescriptorHeapMgr.h"
#include <array>
#include <DirectXTex/DirectXTexDDS.cpp>

TextureManager::TextureManager() {
}

void TextureManager::WriteTextureData(CD3DX12_RESOURCE_DESC& TexresDesc, DirectX::TexMetadata& MetaData, const DirectX::Image* Img, Microsoft::WRL::ComPtr<ID3D12Resource>& TexBuff,
	std::vector<D3D12_SUBRESOURCE_DATA> Subresource) {

	// Footprint(�R�s�[�\�ȃ��\�[�X�̃��C�A�E�g)���擾�B
	std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT, 16> footprint;
	UINT64 totalBytes = 0;
	UINT64 rowSizeInBytes = 0;
	Engine::Ins()->device_.dev_->GetCopyableFootprints(&TexresDesc, 0, MetaData.mipLevels, 0, footprint.data(), nullptr, &rowSizeInBytes, &totalBytes);
	totalBytes = rowSizeInBytes * TexresDesc.Height;

	// Upload�p�̃o�b�t�@���쐬����B
	D3D12_RESOURCE_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = totalBytes;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

	D3D12_HEAP_PROPERTIES heap = D3D12_HEAP_PROPERTIES();
	heap.Type = D3D12_HEAP_TYPE_UPLOAD;

	Microsoft::WRL::ComPtr<ID3D12Resource> iUploadBuffer = nullptr;
	Engine::Ins()->device_.dev_->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&iUploadBuffer));

	// UploadBuffer�ւ̏������݁B
	void* ptr = nullptr;
	iUploadBuffer->Map(0, nullptr, &ptr);

	// 1�s�N�Z���̃T�C�Y
	UINT pixelSize = rowSizeInBytes / MetaData.width;

	for (uint32_t mip = 0; mip < MetaData.mipLevels; ++mip) {

		uint8_t* uploadStart = reinterpret_cast<uint8_t*>(ptr) + footprint[mip].Offset;
		const void* sourceStart = Subresource[mip].pData;
		uint32_t sourceSlicePtich = Subresource[mip].SlicePitch;

		memcpy(uploadStart, sourceStart, sourceSlicePtich);

	}

	iUploadBuffer->Unmap(0, nullptr);

	// �R�s�[����B
	for (uint32_t mip = 0; mip < MetaData.mipLevels; ++mip) {

		D3D12_TEXTURE_COPY_LOCATION copyDestLocation;
		copyDestLocation.pResource = TexBuff.Get();
		copyDestLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		copyDestLocation.SubresourceIndex = mip;

		D3D12_TEXTURE_COPY_LOCATION copySrcLocation;
		copySrcLocation.pResource = iUploadBuffer.Get();
		copySrcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		copySrcLocation.PlacedFootprint = footprint[mip];

		Engine::Ins()->mainGraphicsCmdList_->CopyTextureRegion(
			&copyDestLocation,
			0, 0, 0,
			&copySrcLocation,
			nullptr
		);

	}


	// ���\�[�X�o���A���Z�b�g�B
	D3D12_RESOURCE_BARRIER resourceBarrier = D3D12_RESOURCE_BARRIER();
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Transition.pResource = TexBuff.Get();
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	Engine::Ins()->mainGraphicsCmdList_->ResourceBarrier(1, &resourceBarrier);

	// �R�s�[�R�}���h�̎��s�B
	Engine::Ins()->mainGraphicsCmdList_->Close();
	ID3D12CommandList* commandLists[] = { Engine::Ins()->mainGraphicsCmdList_.Get() };
	Engine::Ins()->graphicsCmdQueue_->ExecuteCommandLists(1, commandLists);
	Engine::Ins()->graphicsCmdQueue_->Signal(Engine::Ins()->asFence_.Get(), ++Engine::Ins()->asfenceValue_);

	// �O���t�B�b�N�R�}���h���X�g�̊����҂�
	UINT64 gpuFence = Engine::Ins()->asFence_->GetCompletedValue();
	if (gpuFence != Engine::Ins()->asfenceValue_) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		Engine::Ins()->asFence_->SetEventOnCompletion(Engine::Ins()->asfenceValue_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// �R�}���h�A���P�[�^�̃��Z�b�g
	Engine::Ins()->mainGraphicsCmdAllocator_->Reset();	//�L���[���N���A

	// �R�}���h���X�g�̃��Z�b�g
	Engine::Ins()->mainGraphicsCmdList_->Reset(Engine::Ins()->mainGraphicsCmdAllocator_.Get(), nullptr);//�ĂуR�}���h���X�g�𒙂߂鏀��



}


int TextureManager::LoadTexture(LPCWSTR FileName) {

	// �t�@�C�������[�h�ς݂����`�F�b�N
	if (0 < texture_.size()) {

		int counter = 0;
		for (auto& index_ : texture_) {

			// ���[�h���Ă������环�ʔԍ���Ԃ�
			if (index_.filePath_ == FileName) {

				return descriptorHeadMgrIndex_[counter];

			}
			++counter;
		}
	}

	// �e�N�X�`���f�[�^��ۑ�
	Texture proTexture{};
	proTexture.filePath_ = FileName;

	//// ���[�h���Ă��Ȃ������烍�[�h����
	//DirectX::TexMetadata metadata;
	//DirectX::ScratchImage scratchImg;
	//HRESULT result = LoadFromWICFile(
	//	FileName,
	//	DirectX::WIC_FLAGS_NONE,
	//	&metadata, scratchImg
	//);

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImg;
	HRESULT result = LoadFromDDSFile(FileName, DDS_FLAGS_NONE, &metadata, scratchImg);
	if (FAILED(result)) {
		result = LoadFromWICFile(FileName, WIC_FLAGS_NONE/*WIC_FLAGS_FORCE_RGB*/, &metadata, scratchImg);
	}
	if (FAILED(result)) {
		assert(0);
	}

	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);

	// MipMap���擾�B
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	result = PrepareUpload(
		Engine::Ins()->device_.dev_.Get(), img, scratchImg.GetImageCount(), metadata, subresources);

	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		static_cast<UINT>(metadata.width),
		static_cast<UINT>(metadata.height),
		static_cast<UINT16>(metadata.arraySize),
		static_cast<UINT16>(metadata.mipLevels));
	texresDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

	// �e�N�X�`���o�b�t�@�̐���
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff = nullptr;
	CD3DX12_HEAP_PROPERTIES texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	result = Engine::Ins()->device_.dev_->CreateCommittedResource(&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&texbuff));


	// ��������ǋL�B

	WriteTextureData(texresDesc, metadata, img, texbuff, subresources);

	// �����܂ŒǋL�B

	// �e�N�X�`���z��̍Ō���Ƀ��[�h�����e�N�X�`�������L�^
	proTexture.metadata_ = metadata;
	proTexture.scratchImg_ = &scratchImg;
	proTexture.texBuff_ = texbuff;
	texture_.emplace_back(proTexture);
	descriptorHeadMgrIndex_.emplace_back(DescriptorHeapMgr::Ins()->GetHead());

	// �f�B�X�N���v�^�q�[�v�̃A�h���X���擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	// �V�F�[�_�[���\�[�X�r���[�̐���
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = metadata.mipLevels;
	// �q�[�v�ɃV�F�[�_�[���\�[�X�r���[����
	Engine::Ins()->device_.dev_->CreateShaderResourceView(
		texbuff.Get(),
		&srvDesc,
		basicHeapHandle
	);

	// �f�B�X�N���v�^�q�[�v���C���N�������g
	DescriptorHeapMgr::Ins()->IncrementHead();

	return DescriptorHeapMgr::Ins()->GetHead() - 1;
}

int TextureManager::LoadTexture(std::array<wchar_t, 128> FileName)
{
	// �t�@�C�������[�h�ς݂����`�F�b�N
	if (0 < texture_.size()) {

		int counter = 0;
		for (auto& index_ : texture_) {

			// ���[�h���Ă������环�ʔԍ���Ԃ�
			if (index_.filePath_ == FileName.data()) {

				return descriptorHeadMgrIndex_[counter];

			}
			++counter;
		}
	}

	// �e�N�X�`���f�[�^��ۑ�
	Texture proTexture{};
	proTexture.filePathP_ = FileName;
	proTexture.filePath_ = proTexture.filePathP_.data();

	// ���[�h���Ă��Ȃ������烍�[�h����
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImg;
	bool isDDS = false;
	HRESULT result = LoadFromDDSFile(proTexture.filePath_, DDS_FLAGS_NONE, &metadata, scratchImg);
	isDDS = result == S_OK;
	if (FAILED(result)) {
		result = LoadFromWICFile(proTexture.filePath_, WIC_FLAGS_NONE/*WIC_FLAGS_FORCE_RGB*/, &metadata, scratchImg);
	}
	if (FAILED(result)) {
		assert(0);
	}
	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);

	// DDS��������MipMap���擾�B
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	if (isDDS) {

		result = PrepareUpload(
			Engine::Ins()->device_.dev_.Get(), img, scratchImg.GetImageCount(), metadata, subresources);


	}

	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		static_cast<UINT>(metadata.width),
		static_cast<UINT>(metadata.height),
		static_cast<UINT16>(metadata.arraySize),
		static_cast<UINT16>(metadata.mipLevels));

	// �e�N�X�`���o�b�t�@�̐���
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff = nullptr;
	CD3DX12_HEAP_PROPERTIES texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	result = Engine::Ins()->device_.dev_->CreateCommittedResource(&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&texbuff));


	// ��������

	WriteTextureData(texresDesc, metadata, img, texbuff, subresources);

	// �����܂ŒǋL�B

	// �e�N�X�`���z��̍Ō���Ƀ��[�h�����e�N�X�`�������L�^
	proTexture.metadata_ = metadata;
	proTexture.scratchImg_ = &scratchImg;
	proTexture.texBuff_ = texbuff;
	texture_.emplace_back(proTexture);
	descriptorHeadMgrIndex_.emplace_back(DescriptorHeapMgr::Ins()->GetHead());

	// �f�B�X�N���v�^�q�[�v�̃A�h���X���擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	// �V�F�[�_�[���\�[�X�r���[�̐���
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = metadata.mipLevels;
	// �q�[�v�ɃV�F�[�_�[���\�[�X�r���[����
	Engine::Ins()->device_.dev_->CreateShaderResourceView(
		texbuff.Get(),
		&srvDesc,
		basicHeapHandle
	);

	// �f�B�X�N���v�^�q�[�v���C���N�������g
	DescriptorHeapMgr::Ins()->IncrementHead();

	return DescriptorHeapMgr::Ins()->GetHead() - 1;
}

int TextureManager::LoadTexture(std::wstring FileName, const void* Src, const UINT64& Size)
{

	// �f�[�^��null���ǂ������`�F�b�N�B
	if (Src == nullptr) {

		assert(0);

	}

	// �t�@�C�������[�h�ς݂����`�F�b�N
	if (0 < texture_.size()) {

		int counter = 0;
		for (auto& index_ : texture_) {

			// ���[�h���Ă������环�ʔԍ���Ԃ�
			if (index_.filePath_ == FileName.data()) {

				return descriptorHeadMgrIndex_[counter];

			}
			++counter;
		}
	}

	// �e�N�X�`���f�[�^��ۑ�
	Texture proTexture{};
	proTexture.filePath_ = FileName.data();

	// ���[�h���Ă��Ȃ������烍�[�h����
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImg;

	HRESULT hr = E_FAIL;
	hr = LoadFromDDSMemory(Src, Size, DDS_FLAGS_NONE, &metadata, scratchImg);
	if (FAILED(hr)) {
		hr = LoadFromWICMemory(Src, Size, WIC_FLAGS_NONE/*WIC_FLAGS_FORCE_RGB*/, &metadata, scratchImg);
	}
	if (FAILED(hr)) {
		assert(0);
	}

	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		static_cast<UINT>(metadata.width),
		static_cast<UINT>(metadata.height),
		static_cast<UINT16>(metadata.arraySize),
		static_cast<UINT16>(metadata.mipLevels));

	// �e�N�X�`���o�b�t�@�̐���
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff = nullptr;
	CD3DX12_HEAP_PROPERTIES texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	HRESULT result = Engine::Ins()->device_.dev_->CreateCommittedResource(&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));

	// �f�[�^�]��
	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);
	result = texbuff->WriteToSubresource(
		0,
		nullptr,							// �S�̈�R�s�[
		img->pixels,						// ���f�[�^�̐擪�A�h���X
		static_cast<UINT>(img->rowPitch),	// �ꃉ�C���̃T�C�Y
		static_cast<UINT>(img->slicePitch)	// �����܂��̃T�C�Y
	);

	// �e�N�X�`���z��̍Ō���Ƀ��[�h�����e�N�X�`�������L�^
	proTexture.metadata_ = metadata;
	proTexture.scratchImg_ = &scratchImg;
	proTexture.texBuff_ = texbuff;
	texture_.emplace_back(proTexture);
	descriptorHeadMgrIndex_.emplace_back(DescriptorHeapMgr::Ins()->GetHead());

	// �f�B�X�N���v�^�q�[�v�̃A�h���X���擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	// �V�F�[�_�[���\�[�X�r���[�̐���
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// �q�[�v�ɃV�F�[�_�[���\�[�X�r���[����
	Engine::Ins()->device_.dev_->CreateShaderResourceView(
		texbuff.Get(),
		&srvDesc,
		basicHeapHandle
	);

	// �f�B�X�N���v�^�q�[�v���C���N�������g
	DescriptorHeapMgr::Ins()->IncrementHead();

	return DescriptorHeapMgr::Ins()->GetHead() - 1;
}


int TextureManager::CreateTexture(DirectX::XMFLOAT4 Color)
{
	// �����F�̃e�N�X�`�������łɐ����ς݂����`�F�b�N����
	LPCWSTR selfTex = L"SelfTexture";

	int counter = 0;
	for (auto& index_ : texture_) {

		if (index_.filePath_ == selfTex && index_.colorData_.x == Color.x && index_.colorData_.y == Color.y &&
			index_.colorData_.z == Color.z && index_.colorData_.w == Color.w) {

			// ���łɐ������Ă���e�N�X�`���Ȃ̂�SRV�q�[�v�̔ԍ���Ԃ�
			return counter;

		}
		++counter;

	}

	// �摜�f�[�^���쐬����
	static const int texWidth = 256;
	const int imageDataCount = texWidth * texWidth;
	// �摜�C���[�W�f�[�^�z��
	std::array<DirectX::XMFLOAT4, texWidth> imageData;

	// �S�s�N�Z���̐F��������
	for (auto& index_ : imageData) {
		index_.x = Color.x;
		index_.y = Color.y;
		index_.z = Color.z;
		index_.w = Color.w;
	}

	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		texWidth,
		static_cast<UINT>(texWidth),
		static_cast<UINT16>(1),
		static_cast<UINT16>(1));

	// �e�N�X�`���o�b�t�@�̐���
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff = nullptr;
	CD3DX12_HEAP_PROPERTIES texHeapPropBuff = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	HRESULT result = Engine::Ins()->device_.dev_->CreateCommittedResource(&texHeapPropBuff,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));

	// �f�[�^�]��
	result = texbuff->WriteToSubresource(
		0,
		nullptr,									//�S�̈�R�s�[
		imageData.data(),							//���f�[�^�̐擪�A�h���X
		sizeof(DirectX::XMFLOAT4) * texWidth,		//�ꃉ�C���̃T�C�Y
		sizeof(DirectX::XMFLOAT4) * imageDataCount	//�����܂��̃T�C�Y
	);

	// �e�N�X�`���z��̍Ō���Ƀ��[�h�����e�N�X�`�������L�^
	Texture proTexture{};
	proTexture.filePath_ = L"SelfTexture";
	proTexture.metadata_ = {};
	proTexture.scratchImg_ = {};
	proTexture.texBuff_ = texbuff;
	proTexture.colorData_ = Color;
	texture_.push_back(proTexture);
	descriptorHeadMgrIndex_.emplace_back(DescriptorHeapMgr::Ins()->GetHead());

	// �f�B�X�N���v�^�q�[�v�̃A�h���X���擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	// �V�F�[�_�[���\�[�X�r���[�̐���
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// �q�[�v�ɃV�F�[�_�[���\�[�X�r���[����
	Engine::Ins()->device_.dev_->CreateShaderResourceView(
		texbuff.Get(),
		&srvDesc,
		basicHeapHandle
	);

	// �f�B�X�N���v�^�q�[�v���C���N�������g
	DescriptorHeapMgr::Ins()->IncrementHead();

	return DescriptorHeapMgr::Ins()->GetHead() - 1;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSRV(int IDNum) {

	D3D12_GPU_DESCRIPTOR_HANDLE basicHeapHandle = DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

	// ������������A�h���X�����炷�B
	for (int i = 0; i < IDNum; ++i) {

		basicHeapHandle.ptr += Engine::Ins()->device_.dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	}

	return basicHeapHandle;
}

Texture TextureManager::GetTexture(int ID)
{
	return texture_[ID];
}
