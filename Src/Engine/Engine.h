#pragma once

#include <DirectXTex/d3dx12.h>
#include <dxgi1_6.h>
#include <dinput.h>
#include <xstring>
#include <vector>
#include <wrl.h>
#include "Singleton.h"
#include <dxgi.h>
#include <Xinput.h>
#include <d3d12.h>
#include <memory>

class WindowsAPI;

class Engine : public Singleton<Engine> {

public:

	std::shared_ptr<WindowsAPI> windowsAPI_;						//WindowsAPIクラス
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController_;
	Microsoft::WRL::ComPtr<ID3D12Debug1> shaderDebugController_;
	Microsoft::WRL::ComPtr<ID3D12Device5> dev_;
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory_;
	std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapters_;//アダプターの列挙用
	Microsoft::WRL::ComPtr<IDXGIAdapter1> tmpAdapter_;			//特定の名前を持つアダプターオブジェクトを入れる
	std::vector<D3D_FEATURE_LEVEL> levels_;						//対応レベルの配列
	D3D_FEATURE_LEVEL featureLevel_;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> cmdList_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeaps_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc_;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer_;			//深度バッファ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;		//深度バッファ用ディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	UINT64 fenceVal_;
	IDirectInput8* dinput_;										//DirectInputオブジェクト
	IDirectInputDevice8* devkeybord_;							//キーボードオブジェクト
	IDirectInputDevice8* devmouse_;								//マウスオブジェクト

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heapForImgui_;	//imgui用ディスクリプタヒープ


public:
	//コンストラクタ
	friend Singleton<Engine>;
	Engine();
	//初期化処理
	void Init();
	//毎フレーム処理(描画前処理)
	void ProcessBeforeDrawing();
	//毎フレーム処理(描画後処理)
	void ProcessAfterDrawing();

	//レンダーターゲットをセット
	void SetRenderTarget();
	//描画後のリソースバリア
	void ResourceBarrierAfter();

	//imgui用ヒープ生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeaoForImgui();
};