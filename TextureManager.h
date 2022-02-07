#pragma once
#include "Singleton.h"
#include <wrl/client.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include <DirectXTex/DirectXTex.h>

/*
�e�N�X�`���}�l�[�W���[
�ESRV�f�B�X�N���v�^�q�[�v�ƃe�N�X�`���N���X�z�������
�E�X�v���C�g�}�l�[�W���[����摜�t�@�C�����������Ƃ��Ă������SRV���f�B�X�N���v�^�q�[�v��ɐ������A���Ԗڂɐ��������������ʔԍ��Ƃ��ĕۑ�����B
�E�w��̃e�N�X�`����SRV���擾�������ꍇ�́A�f�B�X�N���v�^�q�[�v�̐擪�A�h���X�Ɏ��ʔԍ����C���N�������g����Ηǂ��B
*/

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

/*
�e�N�X�`���N���X
�E�t�@�C�����A���ʔԍ��A�e�N�X�`���o�b�t�@�������Ă���\����
*/
struct Texture {
	LPCWSTR fileName;				//�t�@�C����
	int IDNum;						//���ʔԍ�
	ComPtr<ID3D12Resource> texBuff;	//�e�N�X�`���o�b�t�@
	//�ȉ��͕K���K�v�ł͂Ȃ�����������֗�
	TexMetadata metadata;			//�摜�̃T�C�Y�ȂǁA�e�N�X�`���Ƃ��Ă̊e��񂪓���
	ScratchImage* scratchImg;
	Image* image;					//�r�b�g�}�b�v�`���ł̉摜�f�[�^�̂��̂��̂̃A�h���X������
	XMFLOAT4 colorData;				//�F���w�肵�ăe�N�X�`���𐶐�����ہA���̃e�N�X�`�����ǂ̐F�Ȃ̂���n�����߂̕ϐ�
};

class TextureManager : public Singleton<TextureManager> {

private:

	ComPtr<ID3D12DescriptorHeap> srvDescHeap;
	vector<Texture> texture;
	//�R���X�g���N�^
	friend Singleton<TextureManager>;
	TextureManager();

public:
	//�摜�����[�h���Ď��ʔԍ���Ԃ��֐��A���[�h�ς݂������环�ʔԍ���Ԃ��B
	int LoadTexture(LPCWSTR fileName);
	//�摜�𐶐����Ď��ʔԍ���Ԃ��֐�
	int CreateTexture(XMFLOAT4 color);
	//�����_�[�^�[�Q�b�g�p�̃e�N�X�`���𐶐����Ď��ʔԍ���Ԃ��֐�
	int CreateRenderTargetTexture(int width, int height, int mipLevel, int arraySize, DXGI_FORMAT format, float clearColor[4]);
	//���ʔԍ��̏��Ԃ̃V�F�[�_�[���\�[�X�r���[��Ԃ��B
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRV(int IDNum);
	//�f�B�X�N���v�^�q�[�v��Ԃ��B
	ComPtr<ID3D12DescriptorHeap> GetDescHeap();
	//�w��̔ԍ��̃e�N�X�`����Ԃ��B
	Texture GetTexture(int ID);
};