#pragma once
#include "DirectXBase.h"
#include "DescriptorHeapMgr.h"

// 動的な定数バッファクラス
class DynamicConstBuffer {

private:

	/*===== メンバ変数 =====*/

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> buffer;
	int descHeapIndex;


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	DynamicConstBuffer();

	// 生成処理
	void Generate(UINT BufferSize, const wchar_t* Name = L"");

	// データ書き込み処理
	void Write(UINT BufferIndex, const void* Data, UINT Size);

	// アクセッタ
	Microsoft::WRL::ComPtr<ID3D12Resource> GetBuffer(UINT BufferIndex) const { return buffer[BufferIndex]; }
	const int& GetDescHeapIndex() { return descHeapIndex; }


private:

	// バッファ全般を生成する処理
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType);

	// アラインメント用
	inline UINT RoundUp(size_t Size, UINT Align) { return UINT(Size + Align - 1) & ~(Align - 1); }

};