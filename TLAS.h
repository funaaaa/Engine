#pragma once
#include "DirectXBase.h"

// TLASクラス
class TLAS {

private:

	/*===== メンバ変数 =====*/

	ComPtr<ID3D12Resource> tlasBuffer;		// TLAS用バッファ
	ComPtr<ID3D12Resource> scratchBuffer;	// スクラッチバッファ

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;


public:

	/*===== メンバ関数 =====*/

	// TLASの生成
	void GenerateTLAS();

	// アクセッタ
	ComPtr<ID3D12Resource> GetTLASBuffer() { return tlasBuffer; }
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& GetDescriptorHeap() { return descriptorHeap; }


private:

	// アドレスに情報を書き込む処理
	void WriteToMemory(ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize);

	// バッファ全般を生成する処理
	ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);

	// 加速構造体の設定用関数
	void SettingAccelerationStructure();

	// 加速構造体の構築用関数
	void CreateAccelerationStructure();

};
