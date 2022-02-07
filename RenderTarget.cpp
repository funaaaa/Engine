#include "RenderTarget.h"
#include "TextureManager.h"

bool RenderTarget::Create(int w, int h, int mipLevel, int arraySize, DXGI_FORMAT colorFormat, DXGI_FORMAT depthStencilFormat, float clearColor[4])
{
	this->width = w;
	this->height = h;
	//レンダリングターゲットとなるテクスチャを作成する。
	if (!CreateRenderTargetTexture(w, h, mipLevel, arraySize, colorFormat, clearColor)) {
	//	TK_ASSERT(false, "レンダリングターゲットとなるテクスチャの作成に失敗しました。");
		MessageBoxA(nullptr, "レンダリングターゲットとなるテクスチャの作成に失敗しました。", "エラー", MB_OK);
		return false;
	}
	//深度ステンシルバッファとなるテクスチャを作成する。
	if (depthStencilFormat != DXGI_FORMAT_UNKNOWN) {
		if (!CreateDepthStencilTexture(w, h, depthStencilFormat)) {
			MessageBoxA(nullptr, "レンダリングターゲットとなるテクスチャの作成に失敗しました。", "エラー", MB_OK);
			return false;
		}
	}
	if (!CreateDescriptorHeap()) {
		//ディスクリプタヒープの作成に失敗した。
		MessageBoxA(nullptr, "レンダリングターゲットとなるテクスチャの作成に失敗しました。", "エラー", MB_OK);
		return false;
	}
	//ディスクリプタを作成する。
	CreateDescriptor();
	if (clearColor) {
		memcpy(rtvClearColor, clearColor, sizeof(rtvClearColor));
	}
	return true;
}

bool RenderTarget::CreateDescriptorHeap()
{
	//RTV用のディスクリプタヒープを作成する。
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = 1;					//MiniEngineだとよくわからない値を入れていたので、とりあえず1を入れる。バグったら要確認 https://github.com/shoeisha-books/hlsl-grimoire-sample/blob/main/MiniEngine/RenderTarget.cpp
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DirectXBase::dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvHeap));
	if (rtvHeap == nullptr) {
		//RTV用のディスクリプタヒープの作成に失敗した。
		return false;
	}
	//ディスクリプタのサイズを取得。
	rtvDescriptorSize = DirectXBase::dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	if (depthStencilTexture) {
		//DSV用のディスクリプタヒープを作成する。
		desc.NumDescriptors = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		DirectXBase::dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&dsvHeap));
		if (dsvHeap == nullptr) {
			//DSV用のディスクリプタヒープの作成に失敗した。
			return false;
		}
		//ディスクリプタのサイズを取得。
		dsvDescriptorSize = DirectXBase::dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}
	return true;
}

bool RenderTarget::CreateRenderTargetTexture(int w, int h, int mipLevel, int arraySize, DXGI_FORMAT format, float clearColor[4])
{
	textureID = TextureManager::Instance()->CreateRenderTargetTexture(w,h,mipLevel,arraySize,format,clearColor);
	renderTargetTexture = TextureManager::Instance()->GetTexture(textureID).texBuff;

	return true;
}

bool RenderTarget::CreateDepthStencilTexture(int w, int h, DXGI_FORMAT format)
{
	D3D12_CLEAR_VALUE dsvClearValue;
	dsvClearValue.Format = format;
	dsvClearValue.DepthStencil.Depth = 1.0f;
	dsvClearValue.DepthStencil.Stencil = 0;

	CD3DX12_RESOURCE_DESC desc(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		0,
		static_cast<UINT>(w),
		static_cast<UINT>(h),
		1,
		1,
		format,
		1,
		0,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);

	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto hr = DirectXBase::dev->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&dsvClearValue,
		IID_PPV_ARGS(&depthStencilTexture)
	);
	if (FAILED(hr)) {
		//深度ステンシルバッファの作成に失敗。
		return false;
	}
	return true;
}

void RenderTarget::CreateDescriptor()
{
	//カラーテクスチャのディスクリプタを作成。
	auto rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
	DirectXBase::dev->CreateRenderTargetView(TextureManager::Instance()->GetTexture(textureID).texBuff.Get(), nullptr, rtvHandle);
	if (depthStencilTexture) {
		//深度テクスチャのディスクリプタを作成
		auto dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		DirectXBase::dev->CreateDepthStencilView(depthStencilTexture.Get(), nullptr, dsvHandle);
	}
}
