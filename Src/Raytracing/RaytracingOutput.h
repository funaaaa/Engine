#pragma once
#include <d3d12.h>
#include <wrl.h>

// レイトレ出力UAVクラス
class RaytracingOutput {

private:

	/*===== メンバ変数 =====*/

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;					// UAVの設定
	Microsoft::WRL::ComPtr<ID3D12Resource> rayTracingOutput;	// UAV
	int uavDescriptorIndex;										// ディスクリプタヒープ上のこのUAVのインデックス


public:

	/*===== メンバ関数 =====*/

	// セッティング
	void Setting();

	// ディスクリプタをセット。
	void SetComputeRootDescriptorTalbe(const int& RootParamIndex);

	// ゲッタ
	Microsoft::WRL::ComPtr<ID3D12Resource> GetRaytracingOutput() { return rayTracingOutput; }

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTexture2D(UINT width, UINT height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);

};