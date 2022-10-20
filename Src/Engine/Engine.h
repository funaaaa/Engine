#pragma once

#include <DirectXTex/d3dx12.h>
#include <dxgi1_6.h>
#include <dinput.h>
#include <xstring>
#include <vector>
#include <array>
#include <wrl.h>
#include "Singleton.h"
#include <dxgi.h>
#include <Xinput.h>
#include <d3d12.h>
#include <memory>

class WindowsAPI;

class Engine : public Singleton<Engine> {

public:

	// デバッグに使用する各クラス
	struct Debug {
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController_;
		Microsoft::WRL::ComPtr<ID3D12Debug1> shaderDebugController_;
	};

	// DirectXのデバイス関連
	struct Device {
		Microsoft::WRL::ComPtr<ID3D12Device5> dev_;
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory_;
		std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapters_;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> tmpAdapter_;
		std::vector<D3D_FEATURE_LEVEL> levels_;
		D3D_FEATURE_LEVEL featureLevel_;
	};

	// SwapChain関連
	struct SwapChain {
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeaps_;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc_;
		Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer_;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain_;
	};

	// Input関連
	struct DevInput {
		IDirectInput8* dinput_;				// DirectInputオブジェクト
		IDirectInputDevice8* devkeybord_;	// キーボードオブジェクト
		IDirectInputDevice8* devmouse_;		// マウスオブジェクト
	};


public:

	/*===== メンバ変数 =====*/

	std::shared_ptr<WindowsAPI> windowsAPI_;	// WindowsAPIクラス
	Debug debug_;								// デバッグ用のクラス
	Device device_;								// デバイス関連
	SwapChain swapchain_;						// SwapChain関連
	DevInput input_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heapForImgui_;	//imgui用ディスクリプタヒープ


	// GPUの非同期処理に必要な変数。
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mainGraphicsCmdAllocator_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> copyResourceCmdAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> mainGraphicsCmdList_;	// メインで使用するグラフィックスコマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> copyResourceCmdList;		// CopyResourceで使用するグラフィックスコマンドリスト
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> graphicsCmdQueue_;				// グラフィックスキュー
	std::array<Microsoft::WRL::ComPtr<ID3D12Fence>, 2> graphicsToDenoiseFence_;	// mainGraphicsとdenoiseの同期をとるためのフェンス
	std::array<UINT64, 2> graphicsToDenoiseFenceVal_;
	Microsoft::WRL::ComPtr<ID3D12Fence> GPUtoCPUFence_;				// mainGraphicsとCPUの同期をとるためのフェンス
	UINT64 GPUtoCPUFenceVal_;
	std::array<Microsoft::WRL::ComPtr<ID3D12Fence>, 2> denoiseToCopyFence_;		// denoiseとCopyの同期をとるためのフェンス
	std::array<UINT64, 2> denoiseToCopyFenceVal_;
	Microsoft::WRL::ComPtr<ID3D12Fence> finishCopyFence_;			// copy終了監視用フェンス
	UINT64 finishCopyFenceVal_;

	// コンピュートキュー
	std::array < Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, 2> computeCmdAllocator_;
	std::array < Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>, 2> computeCmdList_;
	std::array<Microsoft::WRL::ComPtr<ID3D12CommandQueue>, 2> computeCmdQueue_;

	// 現在Queueのインデックス。
	int currentQueueIndex_;
	int frameIndex_;

	bool canUseMainGraphicsQueue_;			// MainGraphicsCmdListにコマンドを詰むことができる状態かのフラグ
	std::array<bool, 2> canUseDenoiseQueue_;				// DenoiseCmdListにコマンドを詰むことができる状態かのフラグ
	bool canUseCopyQueue_;					// CopyCmdListにコマンドを詰むことができる状態かのフラグ
	bool hasFinishedMainGraphicsProcess_;	// そのフレームのMainGraphicsCmdListの処理が終わったかどうかのフラグ


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