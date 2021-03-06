#pragma once

#include <d3d12.h>
#include <wrl.h>

//読み取り専用構造化バッファ

class StructuredBuffer{

public:

	/// <summary>
	/// 構造化バッファを初期化。
	/// </summary>
	/// <param name="sizeOfElement">エレメントのサイズ。</param>
	/// <param name="numElement">エレメントの数。</param>
	/// <param name="initData">初期データ。</param>
	void Init(int SizeOfElement, int NumElement, void* InitData);

	/// <summary>
	/// SRVに登録。
	/// </summary>
	/// <param name="descriptorHandle"></param>
	void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle);

	/// <summary>
	/// 構造化バッファの内容を更新。
	/// </summary>
	/// <param name="data"></param>
	void Update(void* Data);

	/// <summary>
	/// 初期化されているか判定。
	/// </summary>
	/// <returns></returns>
	bool IsInited() const
	{
		return isInited_;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> GetD3DResoruce();


public:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffersOnGPU_;
	void* buffersOnCPU_;	// CPU側からアクセスできるするストラクチャバッファのアドレス。
	int numElement_;		// 要素数。
	int sizeOfElement_;	// エレメントのサイズ。
	bool isInited_;		// 初期化済み？

};