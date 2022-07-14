#pragma once
#include <DirectXTex/d3dx12.h>
#include <wrl.h>

/// <summary>
/// 構造化バッファ
/// </summary>
class RWStructuredBuffer {
public:

	/// <summary>
	/// 構造化バッファを初期化。
	/// </summary>
	/// <param name="sizeOfElement">エレメントのサイズ。</param>
	/// <param name="numElement">エレメントの数。</param>
	/// <param name="initData">初期データ。</param>
	void Init(int SizeOfElement, int NumElement, void* InitData);

	/// <summary>
	/// UAVに登録。
	/// </summary>
	/// <param name=""></param>
	/// <param name="bufferNo"></param>
	void RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle);

	/// <summary>
	/// SRVに登録。
	/// </summary>
	/// <param name="descriptorHandle"></param>
	/// <param name="bufferNo"></param>
	void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle);

	/// <summary>
	/// 初期化されているか判定。
	/// </summary>
	/// <returns></returns>
	bool IsInited() const
	{
		return isInited_;
	}

	/// <summary>
	/// CPUからアクセス可能なリソースを取得する。
	/// </summary>
	/// <returns></returns>
	void* GetResourceOnCPU();


	Microsoft::WRL::ComPtr<ID3D12Resource> GetD3DResoruce();


public:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffersOnGPU_;
	void* buffersOnCPU_;			// CPU側からアクセスできるするストラクチャバッファのアドレス。
	int numElement_;				// 要素数。
	int sizeOfElement_;			// エレメントのサイズ。
	bool isInited_;				// 初期化済み

};