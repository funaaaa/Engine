#pragma once

#include <DirectXTex/d3dx12.h>
#include <dxgi1_6.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <xstring>
#include <vector>
#include <wrl.h>
#include "WindowsAPI.h"
#include "Singleton.h"
#include <dxgi.h>
#include <Xinput.h>
#include <d3d12.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

class DirectXBase : public Singleton<DirectXBase> {

public:

	WindowsAPI windowsAPI;						//WindowsAPIクラス
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	Microsoft::WRL::ComPtr<ID3D12Device5> dev;
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
	std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapters;		//アダプターの列挙用
	Microsoft::WRL::ComPtr<IDXGIAdapter1> tmpAdapter;			//特定の名前を持つアダプターオブジェクトを入れる
	std::vector<D3D_FEATURE_LEVEL> levels;			//対応レベルの配列
	D3D_FEATURE_LEVEL featureLevel;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> cmdList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;			//深度バッファ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;		//深度バッファ用ディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal;
	IDirectInput8* dinput;						//DirectInputオブジェクト
	IDirectInputDevice8* devkeybord;				//キーボードオブジェクト
	IDirectInputDevice8* devmouse;				//マウスオブジェクト
	
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heapForImgui;	//imgui用ディスクリプタヒープ


public:
	//コンストラクタ
	friend Singleton<DirectXBase>;
	DirectXBase();
	//初期化処理
	void Init();
	//毎フレーム処理(描画前処理)
	void processBeforeDrawing();
	//毎フレーム処理(描画後処理)
	void processAfterDrawing();

	//レンダーターゲットをセット
	void SetRenderTarget();
	//描画後のリソースバリア
	void ResourceBarrierAfter();

	//imgui用ヒープ生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeaoForImgui();
};