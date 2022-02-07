#pragma once
#include "DirectXBase.h"

class RenderTarget {
public:
	int textureID = 0;
	ComPtr<ID3D12Resource> renderTargetTexture = nullptr;		//レンダリングターゲットとなるテクスチャ。
	ComPtr<ID3D12Resource> depthStencilTexture = nullptr;		//深度ステンシルバッファとなるテクスチャ。
	ComPtr<ID3D12DescriptorHeap> rtvHeap = nullptr;				//RTV用のディスクリプタヒープ。
	ComPtr<ID3D12DescriptorHeap> dsvHeap = nullptr;				//深度ステンシルバッファビューのディスクリプタヒープ。
	UINT rtvDescriptorSize = 0;									//フレームバッファのディスクリプタのサイズ。
	UINT dsvDescriptorSize = 0;									//深度ステンシルバッファのディスクリプタのサイズ。
	int width = 0;												//レンダリングターゲットの幅。
	int height = 0;												//レンダリングターゲットの高さ。
	float rtvClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };		//レンダリングターゲットビューのクリアカラー。
	float dsvClearColor = 1.0f;									//DSVのクリアカラー。

	/// <summary>
	/// デストラクタ。
	/// </summary>
	~RenderTarget() {};

	/// <summary>
	/// レンダリングターゲットの作成。
	/// </summary>
	/// <param name="w">レンダリングターゲットの幅</param>
	/// <param name="h">レンダリングターゲットの高さ</param>
	/// <param name="mipLevel">ミップマップレベル。0を指定した場合はミップマップがサポートされているGPUでは1*1ピクセルまでのミップマップが作成される。</param>
	/// <param name="arraySize">テクスチャ配列のサイズ</param>
	/// <param name="colorFormat">カラーバッファのフォーマット。</param>
	/// <param name="depthStencilFormat">深度ステンシルバッファのフォーマット。</param>
	/// <returns>trueが返ってきたら作成成功</returns>
	bool Create(
		int w,
		int h,
		int mipLevel,
		int arraySize,
		DXGI_FORMAT colorFormat,
		DXGI_FORMAT depthStencilFormat,
		float clearColor[4] = nullptr
	);

	/// <summary>
	/// ディスクリプタヒープを作成。
	/// </summary>
	/// <returns></returns>
	bool CreateDescriptorHeap();

	/// <summary>
	/// レンダリングターゲットとなるテクスチャを作成。
	/// </summary>
	/// <param name="w">テクスチャの幅</param>
	/// <param name="h">テクスチャの高さ</param>
	/// <param name="mipLevel">ミップマップレベル</param>
	/// <param name="arraySize">テクスチャ配列のサイズ</param>
	/// <param name="format">テクスチャのフォーマット</param>
	/// <returns>trueが返ってきたら成功。</returns>
	bool CreateRenderTargetTexture(
		int w,
		int h,
		int mipLevel,
		int arraySize,
		DXGI_FORMAT format,
		float clearColor[4]
	);

	/// <summary>
	/// 深度ステンシルバッファとなるテクスチャを作成。
	/// </summary>
	/// <param name="w">テクスチャの幅</param>
	/// <param name="h">テクスチャの高さ</param>
	/// <param name="format">テクスチャのフォーマット</param>
	/// <returns>trueが返ってきたら成功</returns>
	bool CreateDepthStencilTexture(
		int w,
		int h,
		DXGI_FORMAT format);

	/// <summary>
	/// ディスクリプタの作成。
	/// </summary>
	/// <param name="d3dDevice">D3Dデバイス</param>
	/// <returns>trueが返ってきたら成功。</returns>
	void CreateDescriptor();

	//深度バッファを持っているか 持っていたらtrue
	bool HasDepthStencil() { return depthStencilTexture != nullptr; }
};
