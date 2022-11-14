#pragma once
#include "Singleton.h"
#include "Vec.h"
#include <wrl/client.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include <array>
#include <DirectXTex/DirectXTex.h>
#include <string>
#include <DirectXTex/d3dx12.h>

struct Texture {
	LPCWSTR filePath_;								// �t�@�C����
	std::array<wchar_t, 128> filePathP_;			// �t�@�C���p�X�̃|�C���^
	Microsoft::WRL::ComPtr<ID3D12Resource> texBuff_;	// �e�N�X�`���o�b�t�@
	//�ȉ��͕K���K�v�ł͂Ȃ�����������֗�
	DirectX::TexMetadata metadata_;					// �摜�̃T�C�Y�ȂǁA�e�N�X�`���Ƃ��Ă̊e��񂪓���
	DirectX::ScratchImage* scratchImg_;
	DirectX::XMFLOAT4 colorData_;					// �F���w�肵�ăe�N�X�`���𐶐�����ہA���̃e�N�X�`�����ǂ̐F�Ȃ̂���n�����߂̕ϐ�
};

class TextureManager : public Singleton<TextureManager> {

private:

	std::vector<Texture> texture_;
	std::vector<int> descriptorHeadMgrIndex_;		// DescriptorHeapMgr��ł̕ۑ���C���f�b�N�X�B
	// �R���X�g���N�^
	friend Singleton<TextureManager>;
	TextureManager();

public:
	// �摜�����[�h���Ď��ʔԍ���Ԃ��֐��A���[�h�ς݂������环�ʔԍ���Ԃ��B
	int LoadTexture(LPCWSTR FileName);
	int LoadTexture(std::array<wchar_t, 128> FileName);
	int LoadTexture(std::wstring FileName, const void* Src, const UINT64& Size);
	// �摜�𐶐����Ď��ʔԍ���Ԃ��֐�
	int CreateTexture(DirectX::XMFLOAT4 Color);
	// ���ʔԍ��̏��Ԃ̃V�F�[�_�[���\�[�X�r���[��Ԃ��B
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRV(int IDNum);
	// �w��̔ԍ��̃e�N�X�`����Ԃ��B
	Texture GetTexture(int ID);
private:
	void WriteTextureData(CD3DX12_RESOURCE_DESC& TexresDesc, DirectX::TexMetadata& MetaData, const DirectX::Image* Img, Microsoft::WRL::ComPtr<ID3D12Resource>& TexBuff, std::vector<D3D12_SUBRESOURCE_DATA> Subresource);
};