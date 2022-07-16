#include "TextureManager.h"
#include "DirectXBase.h"
#include "DescriptorHeapMgr.h"
#include <array>

TextureManager::TextureManager() {
}

int TextureManager::LoadTexture(LPCWSTR FileName) {

	// �t�@�C�������[�h�ς݂����`�F�b�N
	if (0 < texture_.size()) {

		int counter = 0;
		for (auto& index_ : texture_) {

			// ���[�h���Ă������环�ʔԍ���Ԃ�
			if (index_.fileName_ == FileName) {

				return descriptorHeadMgrIndex_[counter];

			}
			++counter;
		}
	}

	// �e�N�X�`���f�[�^��ۑ�
	Texture proTexture{};
	proTexture.fileName_ = FileName;

	// ���[�h���Ă��Ȃ������烍�[�h����
	DirectX::TexMetadata metadata_;
	DirectX::ScratchImage scratchImg_;
	HRESULT result = LoadFromWICFile(
		FileName,
		DirectX::WIC_FLAGS_NONE,
		&metadata_, scratchImg_
	);
	const DirectX::Image* img = scratchImg_.GetImage(0, 0, 0);

	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata_.format,
		static_cast<UINT>(metadata_.width),
		static_cast<UINT>(metadata_.height),
		static_cast<UINT16>(metadata_.arraySize),
		static_cast<UINT16>(metadata_.mipLevels));

	// �e�N�X�`���o�b�t�@�̐���
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff = nullptr;
	CD3DX12_HEAP_PROPERTIES texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	result = DirectXBase::Ins()->dev_->CreateCommittedResource(&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));

	// �f�[�^�]��
	result = texbuff->WriteToSubresource(
		0,
		nullptr,							// �S�̈�R�s�[
		img->pixels,						// ���f�[�^�̐擪�A�h���X
		static_cast<UINT>(img->rowPitch),	// �ꃉ�C���̃T�C�Y
		static_cast<UINT>(img->slicePitch)	// �����܂��̃T�C�Y
	);

	// �e�N�X�`���z��̍Ō���Ƀ��[�h�����e�N�X�`�������L�^
	proTexture.metadata_ = metadata_;
	proTexture.scratchImg_ = &scratchImg_;
	proTexture.texBuff_ = texbuff;
	texture_.emplace_back(proTexture);
	descriptorHeadMgrIndex_.emplace_back(DescriptorHeapMgr::Ins()->GetHead());

	// �f�B�X�N���v�^�q�[�v�̃A�h���X���擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), DirectXBase::Ins()->dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	// �V�F�[�_�[���\�[�X�r���[�̐���
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata_.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// �q�[�v�ɃV�F�[�_�[���\�[�X�r���[����
	DirectXBase::Ins()->dev_->CreateShaderResourceView(
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

		if (index_.fileName_ == selfTex && index_.colorData_.x == Color.x && index_.colorData_.y == Color.y &&
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
	HRESULT result = DirectXBase::Ins()->dev_->CreateCommittedResource(&texHeapPropBuff,
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
	proTexture.fileName_ = L"SelfTexture";
	proTexture.metadata_ = {};
	proTexture.scratchImg_ = {};
	proTexture.texBuff_ = texbuff;
	proTexture.colorData_ = Color;
	texture_.push_back(proTexture);
	descriptorHeadMgrIndex_.emplace_back(DescriptorHeapMgr::Ins()->GetHead());

	// �f�B�X�N���v�^�q�[�v�̃A�h���X���擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), DirectXBase::Ins()->dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	// �V�F�[�_�[���\�[�X�r���[�̐���
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// �q�[�v�ɃV�F�[�_�[���\�[�X�r���[����
	DirectXBase::Ins()->dev_->CreateShaderResourceView(
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

		basicHeapHandle.ptr += DirectXBase::Ins()->dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	}

	return basicHeapHandle;
}

Texture TextureManager::GetTexture(int ID)
{
	return texture_[ID];
}
