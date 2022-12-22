#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "DescriptorHeapMgr.h"

// 動的な定数バッファクラス
class DynamicConstBuffer {

private:

	/*===== メンバ変数 =====*/

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> buffer_;
	int descHeapIndex_;


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	DynamicConstBuffer();

	// 生成処理
	void Generate(UINT BufferSize, const wchar_t* Name = L"");

	// データ書き込み処理
	void Write(UINT BufferIndex, const void* Data, UINT Size);

	// アクセッサ
	Microsoft::WRL::ComPtr<ID3D12Resource> GetBuffer(UINT BufferIndex) const { return buffer_[BufferIndex]; }
	int GetDescHeapIndex() { return descHeapIndex_; }


private:

	// バッファ全般を生成する処理
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType);

	// アラインメント用
	inline UINT RoundUp(size_t Size, UINT Align) { return UINT(Size + Align - 1) & ~(Align - 1); }

};