#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Vec.h"

// ボリュームテクスチャクラス UAVとしてしか使えない
class VolumeTextureUAV {

private:

	/*===== メンバ変数 =====*/

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc_;					// UAVの設定
	Microsoft::WRL::ComPtr<ID3D12Resource> texture_;			// テクスチャ本体
	int uavDescriptorIndex_;									// ディスクリプタヒープ上のこのUAVのインデックス


public:

	/*===== メンバ関数 =====*/

	// セッティング
	void Setting(DXGI_FORMAT Format, LPCWSTR BufferName, Vec3 TextureSize, D3D12_RESOURCE_STATES ResourceState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_SOURCE);

	// ディスクリプタをセット。
	void SetComputeRootDescriptorTalbe(int RootParamIndex);

	// リソースバリア
	void SetResourceBarrier(D3D12_RESOURCE_STATES Dst, D3D12_RESOURCE_STATES Src, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> CmdList = nullptr);

	// ゲッタ
	Microsoft::WRL::ComPtr<ID3D12Resource> GetRaytracingOutput() { return texture_; }
	int GetUAVIndex() { return uavDescriptorIndex_; }

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTexture3D(UINT Width, UINT Height, UINT Depth, DXGI_FORMAT Format, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType);

};