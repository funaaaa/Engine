#include "TextureManager.h"
#include "DirectXBase.h"
#include "DescriptorHeapMgr.h"
#include <array>

TextureManager::TextureManager() {
}

int TextureManager::LoadTexture(LPCWSTR FileName) {

	// �t�@�C�������[�h�ς݂����`�F�b�N
	if (0 < texture.size()) {

		int counter = 0;
		for (auto& index : texture) {

			// ���[�h���Ă������环�ʔԍ���Ԃ�
			if (index.fileName == FileName) {

				return descriptorHeadMgrIndex[counter];

			}
			++counter;
		}
	}

	// �e�N�X�`���f�[�^��ۑ�
	Texture proTexture{};
	proTexture.fileName = FileName;

	// ���[�h���Ă��Ȃ������烍�[�h����
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImg;
	HRESULT result = LoadFromWICFile(
		FileName,
		DirectX::WIC_FLAGS_NONE,
		&metadata, scratchImg
	);
	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);

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
	result = DirectXBase::Ins()->dev->CreateCommittedResource(&texHeapProp,
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
	proTexture.metadata = metadata;
	proTexture.scratchImg = &scratchImg;
	proTexture.texBuff = texbuff;
	texture.emplace_back(proTexture);
	descriptorHeadMgrIndex.emplace_back(DescriptorHeapMgr::Ins()->GetHead());

	// �f�B�X�N���v�^�q�[�v�̃A�h���X���擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	// �V�F�[�_�[���\�[�X�r���[�̐���
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// �q�[�v�ɃV�F�[�_�[���\�[�X�r���[����
	DirectXBase::Ins()->dev->CreateShaderResourceView(
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
	for (auto& index : texture) {

		if (index.fileName == selfTex && index.colorData.x == Color.x && index.colorData.y == Color.y &&
			index.colorData.z == Color.z && index.colorData.w == Color.w) {

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
	for (auto& index : imageData) {
		index.x = Color.x;
		index.y = Color.y;
		index.z = Color.z;
		index.w = Color.w;
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
	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(&texHeapPropBuff,
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
	proTexture.fileName = L"SelfTexture";
	proTexture.metadata = {};
	proTexture.scratchImg = {};
	proTexture.texBuff = texbuff;
	proTexture.colorData = Color;
	texture.push_back(proTexture);
	descriptorHeadMgrIndex.emplace_back(DescriptorHeapMgr::Ins()->GetHead());

	// �f�B�X�N���v�^�q�[�v�̃A�h���X���擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	// �V�F�[�_�[���\�[�X�r���[�̐���
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// �q�[�v�ɃV�F�[�_�[���\�[�X�r���[����
	DirectXBase::Ins()->dev->CreateShaderResourceView(
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

		basicHeapHandle.ptr += DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	}

	return basicHeapHandle;
}

Texture TextureManager::GetTexture(int ID)
{
	return texture[ID];
}
