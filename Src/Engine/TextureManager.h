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
	LPCWSTR filePath_;								// ファイル名
	std::array<wchar_t, 128> filePathP_;			// ファイルパスのポインタ
	Microsoft::WRL::ComPtr<ID3D12Resource> texBuff_;	// テクスチャバッファ
	//以下は必ず必要ではないがあったら便利
	DirectX::TexMetadata metadata_;					// 画像のサイズなど、テクスチャとしての各情報が入る
	DirectX::ScratchImage* scratchImg_;
	DirectX::XMFLOAT4 colorData_;					// 色を指定してテクスチャを生成する際、このテクスチャがどの色なのかを渡すための変数
};

class TextureManager : public Singleton<TextureManager> {

private:

	std::vector<Texture> texture_;
	std::vector<int> descriptorHeadMgrIndex_;		// DescriptorHeapMgr上での保存先インデックス。
	// コンストラクタ
	friend Singleton<TextureManager>;
	TextureManager();

public:
	// 画像をロードして識別番号を返す関数、ロード済みだったら識別番号を返す。
	int LoadTexture(LPCWSTR FileName);
	int LoadTexture(std::array<wchar_t, 128> FileName);
	int LoadTexture(std::wstring FileName, const void* Src, const UINT64& Size);
	// 画像を生成して識別番号を返す関数
	int CreateTexture(DirectX::XMFLOAT4 Color);
	// 識別番号の順番のシェーダーリソースビューを返す。
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRV(int IDNum);
	// 指定の番号のテクスチャを返す。
	Texture GetTexture(int ID);
private:
	void WriteTextureData(CD3DX12_RESOURCE_DESC& TexresDesc, DirectX::TexMetadata& MetaData, const DirectX::Image* Img, Microsoft::WRL::ComPtr<ID3D12Resource>& TexBuff, std::vector<D3D12_SUBRESOURCE_DATA> Subresource);
};