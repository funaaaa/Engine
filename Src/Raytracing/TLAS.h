#pragma once
#include "DirectXBase.h"

// TLASクラス
class TLAS {

private:

	/*===== メンバ変数 =====*/

	Microsoft::WRL::ComPtr<ID3D12Resource> tlasBuffer;			// TLAS用バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer;		// スクラッチバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> instanceDescBuffer;	// インスタンスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> tlasUpdateBuffer;	// TLAS更新用バッファ

	int descriptorHeapIndex;


public:

	/*===== メンバ関数 =====*/

	// TLASの生成
	void GenerateTLAS(const wchar_t* name = nullptr);

	// アクセッタ
	Microsoft::WRL::ComPtr<ID3D12Resource> GetTLASBuffer() { return tlasBuffer; }

	// ディスクリプタヒープのインデックスのアクセッタ
	inline const int& GetDescriptorHeapIndex() { return descriptorHeapIndex; }

	// インスタンスを更新
	void Update();


private:

	// アドレスに情報を書き込む処理
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& resource, const void* pData, size_t dataSize);

	// バッファ全般を生成する処理
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);

	// 加速構造体の設定用関数
	void SettingAccelerationStructure();

	// 加速構造体の構築用関数
	void CreateAccelerationStructure();

};
