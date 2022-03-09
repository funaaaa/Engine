#pragma once
#include "DirectXBase.h"

// 動的な定数バッファクラス
class DynamicConstBuffer {

private:

	/*===== メンバ変数 =====*/

	vector<ComPtr<ID3D12Resource>> buffer;
	ComPtr<ID3D12DescriptorHeap> descHeap;


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	DynamicConstBuffer();

	// 生成処理
	void Generate(UINT bufferSize, const wchar_t* name = L"");

	// データ書き込み処理
	void Write(UINT bufferIndex, const void* data, UINT size);

	// アクセッタ
	ComPtr<ID3D12Resource> GetBuffer(UINT bufferIndex) const { return buffer[bufferIndex]; }


private:

	// バッファ全般を生成する処理
	ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);

	// アラインメント用
	inline UINT RoundUp(size_t size, UINT align) { return UINT(size + align - 1) & ~(align - 1); }

};