#pragma once
#include "Engine.h"

// TLASクラス
class TLAS {

private:

	/*===== メンバ変数 =====*/

	Microsoft::WRL::ComPtr<ID3D12Resource> tlasBuffer_;			// TLAS用バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer_;		// スクラッチバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> instanceDescBuffer_;	// インスタンスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> tlasUpdateBuffer_;	// TLAS更新用バッファ

	int descriptorHeapIndex_;


public:

	/*===== メンバ関数 =====*/

	// TLASの生成
	void GenerateTLAS();

	// アクセッサ
	Microsoft::WRL::ComPtr<ID3D12Resource> GetTLASBuffer() { return tlasBuffer_; }

	// ディスクリプタヒープのインデックスのアクセッサ
	inline int GetDescriptorHeapIndex() { return descriptorHeapIndex_; }

	// インスタンスを更新
	void Update();


private:

	// アドレスに情報を書き込む処理
	void WriteToMemory(Microsoft::WRL::ComPtr<ID3D12Resource>& Resource, const void* PData, size_t DataSize);

	// 加速構造体の設定用関数
	void SettingAccelerationStructure();

	// 加速構造体の構築用関数
	void CreateAccelerationStructure();

};
