#include "TextureManager.h"
#include "DirectXBase.h"
#include "DescriptorHeapMgr.h"
#include <array>

TextureManager::TextureManager() {
}

int TextureManager::LoadTexture(LPCWSTR FileName) {

	// ファイルがロード済みかをチェック
	if (0 < texture.size()) {

		int counter = 0;
		for (auto& index : texture) {

			// ロードしてあったら識別番号を返す
			if (index.fileName == FileName) {

				return descriptorHeadMgrIndex[counter];

			}
			++counter;
		}
	}

	// テクスチャデータを保存
	Texture proTexture{};
	proTexture.fileName = FileName;

	// ロードしていなかったらロードする
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImg;
	HRESULT result = LoadFromWICFile(
		FileName,
		DirectX::WIC_FLAGS_NONE,
		&metadata, scratchImg
	);
	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);

	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		static_cast<UINT>(metadata.width),
		static_cast<UINT>(metadata.height),
		static_cast<UINT16>(metadata.arraySize),
		static_cast<UINT16>(metadata.mipLevels));

	// テクスチャバッファの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff = nullptr;
	CD3DX12_HEAP_PROPERTIES texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	result = DirectXBase::Ins()->dev->CreateCommittedResource(&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));

	// データ転送
	result = texbuff->WriteToSubresource(
		0,
		nullptr,							// 全領域コピー
		img->pixels,						// 元データの先頭アドレス
		static_cast<UINT>(img->rowPitch),	// 一ラインのサイズ
		static_cast<UINT>(img->slicePitch)	// いちまいのサイズ
	);

	// テクスチャ配列の最後尾にロードしたテクスチャ情報を記録
	proTexture.metadata = metadata;
	proTexture.scratchImg = &scratchImg;
	proTexture.texBuff = texbuff;
	texture.emplace_back(proTexture);
	descriptorHeadMgrIndex.emplace_back(DescriptorHeapMgr::Ins()->GetHead());

	// ディスクリプタヒープのアドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	// シェーダーリソースビューの生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// ヒープにシェーダーリソースビュー生成
	DirectXBase::Ins()->dev->CreateShaderResourceView(
		texbuff.Get(),
		&srvDesc,
		basicHeapHandle
	);

	// ディスクリプタヒープをインクリメント
	DescriptorHeapMgr::Ins()->IncrementHead();

	return DescriptorHeapMgr::Ins()->GetHead() - 1;
}

int TextureManager::CreateTexture(DirectX::XMFLOAT4 Color)
{
	// 同じ色のテクスチャがすでに生成済みかをチェックする
	LPCWSTR selfTex = L"SelfTexture";

	int counter = 0;
	for (auto& index : texture) {

		if (index.fileName == selfTex && index.colorData.x == Color.x && index.colorData.y == Color.y &&
			index.colorData.z == Color.z && index.colorData.w == Color.w) {

			// すでに生成してあるテクスチャなのでSRVヒープの番号を返す
			return counter;

		}
		++counter;

	}

	// 画像データを作成する
	static const int texWidth = 256;
	const int imageDataCount = texWidth * texWidth;
	// 画像イメージデータ配列
	std::array<DirectX::XMFLOAT4, texWidth> imageData;

	// 全ピクセルの色を初期化
	for (auto& index : imageData) {
		index.x = Color.x;
		index.y = Color.y;
		index.z = Color.z;
		index.w = Color.w;
	}

	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		texWidth,
		static_cast<UINT>(texWidth),
		static_cast<UINT16>(1),
		static_cast<UINT16>(1));

	// テクスチャバッファの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff = nullptr;
	CD3DX12_HEAP_PROPERTIES texHeapPropBuff = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	HRESULT result = DirectXBase::Ins()->dev->CreateCommittedResource(&texHeapPropBuff,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));

	// データ転送
	result = texbuff->WriteToSubresource(
		0,
		nullptr,									//全領域コピー
		imageData.data(),							//元データの先頭アドレス
		sizeof(DirectX::XMFLOAT4) * texWidth,		//一ラインのサイズ
		sizeof(DirectX::XMFLOAT4) * imageDataCount	//いちまいのサイズ
	);

	// テクスチャ配列の最後尾にロードしたテクスチャ情報を記録
	Texture proTexture{};
	proTexture.fileName = L"SelfTexture";
	proTexture.metadata = {};
	proTexture.scratchImg = {};
	proTexture.texBuff = texbuff;
	proTexture.colorData = Color;
	texture.push_back(proTexture);
	descriptorHeadMgrIndex.emplace_back(DescriptorHeapMgr::Ins()->GetHead());

	// ディスクリプタヒープのアドレスを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), DescriptorHeapMgr::Ins()->GetHead(), DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	// シェーダーリソースビューの生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// ヒープにシェーダーリソースビュー生成
	DirectXBase::Ins()->dev->CreateShaderResourceView(
		texbuff.Get(),
		&srvDesc,
		basicHeapHandle
	);

	// ディスクリプタヒープをインクリメント
	DescriptorHeapMgr::Ins()->IncrementHead();

	return DescriptorHeapMgr::Ins()->GetHead() - 1;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSRV(int IDNum) {

	D3D12_GPU_DESCRIPTOR_HANDLE basicHeapHandle = DescriptorHeapMgr::Ins()->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

	// 消費した分だけアドレスをずらす。
	for (int i = 0; i < IDNum; ++i) {

		basicHeapHandle.ptr += DirectXBase::Ins()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	}

	return basicHeapHandle;
}

Texture TextureManager::GetTexture(int ID)
{
	return texture[ID];
}
